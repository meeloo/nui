/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -

/////////// nuiLexerizer
nuiLexer::nuiLexer(const nglString& str)
: mInput(str)
{
  Init();
}

nuiLexer::nuiLexer(nglIStream* pStream, const nglPath& rSourcePath)
{
  mpStream = pStream;
  mSourcePath = rSourcePath;
  if (mSourcePath.IsLeaf())
    mSourcePath = mSourcePath.GetParent();

  Init();
}

void nuiLexer::Init()
{
  AddTokenPattern(".", Dot);
  AddTokenPattern(",", Comma);
  AddTokenPattern(";", SemiColon);
  AddTokenPattern(":", Colon);
  AddTokenPattern("<", LessThan);
  AddTokenPattern("<=", LessThanOrEqual);
  AddTokenPattern(">", MoreThan);
  AddTokenPattern(">=", MoreThanOrEqual);
  AddTokenPattern("!", ExclamationMark);
  AddTokenPattern("?", QuestionMark);
  AddTokenPattern("|", Pipe);
  AddTokenPattern("&", Ampersand);
  AddTokenPattern("^", Circumflex);
  AddTokenPattern("%", Percent);
  AddTokenPattern("@", Arobase);
  AddTokenPattern("~", Tilde);
  AddTokenPattern("#", Hash);
  AddTokenPattern("`", BackQuote);
  AddTokenPattern("\\", BackSlash);

  AddTokenPattern("*", Multiply);
  AddTokenPattern("/", Divide);
  AddTokenPattern("+", Plus);
  AddTokenPattern("-", Minus);

  AddTokenPattern("*=", MultiplyEqual);
  AddTokenPattern("/=", DivideEqual);
  AddTokenPattern("+=", PlusEqual);
  AddTokenPattern("-=", MinusEqual);

  AddTokenPattern("[", OpenSBracket);
  AddTokenPattern("]", CloseSBracket);

  AddTokenPattern("(", OpenParent);
  AddTokenPattern(")", CloseParent);

  AddTokenPattern("{", OpenBracket);
  AddTokenPattern("}", CloseBracket);

  AddTokenPattern("=", Equal);
  AddTokenPattern("==", IsEqual);

  //  AddTokenPattern("'", SimpleQuote);
  //  AddTokenPattern("\"", DoubleQuote);

  SetValidInSymbolStart("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
  SetValidInSymbol("0123456789");
  SetValidInBlank(" \t\r\n");

  // Start streaming chars:
  NextChar();
}

const char* nuiLexer::Token::GetTypeName() const
{

#define TOK(X) #X
  static const char* names[] =
  {
    TOK(EndOfFile),
    TOK(Symbol),

    TOK(QuotedString),
    TOK(SimpleQuotedString),

    TOK(Number),

    TOK(Dot),
    TOK(Comma),
    TOK(SemiColon),
    TOK(Colon),
    TOK(LessThan),
    TOK(LessThanOrEqual),
    TOK(MoreThan),
    TOK(MoreThanOrEqual),
    TOK(ExclamationMark),
    TOK(QuestionMark),
    TOK(Slash),
    TOK(Pipe),
    TOK(Ampersand),
    TOK(Circumflex),
    TOK(Percent),
    TOK(Arobase),
    TOK(Tilde),
    TOK(BackQuote),
    TOK(BackSlash),

    TOK(Multiply),
    TOK(Divide),
    TOK(Plus),
    TOK(Minus),

    TOK(MultiplyEqual),
    TOK(DivideEqual),
    TOK(PlusEqual),
    TOK(MinusEqual),

    TOK(Equal),
    TOK(IsEqual),

    TOK(OpenParent),
    TOK(CloseParent),
    TOK(OpenBracket),
    TOK(CloseBracket),
    TOK(OpenSBracket),
    TOK(CloseSBracket),
    TOK(Comments),
    TOK(Blank),
    TOK(Hash),
    TOK(NewLine)
  };
#undef TOK

  return names[mType];
}

const char* nuiLexer::Token::c_str() const
{
  return mString.GetChars();
}

const nuiLexer::Token& nuiLexer::NextNonBlankToken()
{
  do {
    NextToken();
  } while (mToken.mType == Blank && !IsStarved());
  return mToken;
}

const nuiLexer::Token& nuiLexer::NextToken()
{
  mToken.mString.Nullify();
  mToken.mType = EndOfFile;

  if (IsDone())
    return CaptureToken(EndOfFile);

  if (mChar == 10)
  {
    NextChar();
    return CaptureToken(NewLine);
  }

  if (mChar <= ' ')
  {
    SkipBlank();
    if (mStart + 1 < mEnd)
      return CaptureToken(Blank);
  }

  // Specia case for floats beginning with a decimal dot:
  if (mChar == '.')
  {
    if (isnumber(LookAhead()))
      return ParseNumber();
  }

  // Special case for comments:
  if (mChar == '/')
  {
    //  Parse comment?
    if (LookAhead() == '/')
    {
      // Parse line comment
      return ParseLineComment();
    }
    else if (LookAhead() == '*')
    {
      // Parse multi line comment
      return ParseMultiLineComment();
    }
  }


  if (IsValidInSymbolStart(mChar))
  {
    // Parse a Symbol
    return ParseSymbol();
  }
  else if (mChar == '\"' || mChar == '\'')
  {
    return ParseString();
  }
  else if (IsNumberDigit(mChar))
  {
    return ParseNumber();
  }
  else
  {
    TokenPattern* pattern = &mPatterns;
    auto it = pattern->mChildren.find(mChar);
    if (it != pattern->mChildren.end())
    {

      while (it != pattern->mChildren.end())
      {
        NextChar();
        pattern = &it->second;
        it = pattern->mChildren.find(mChar);
      }

      return CaptureToken(pattern->mType);
    }
  }

  return mToken;
}

const nuiLexer::Token& nuiLexer::ParseString()
{
  // Skip the "
  nglUChar QuoteType = mChar;
  TokenType type = mChar == '\"' ? QuotedString : SimpleQuotedString;

  NextChar();

  while (mChar != QuoteType && !IsStarved())
  {
    if (!NextChar())
      return CaptureToken(type);
  }

  // Skip the "
  NextChar();

  return CaptureToken(type);
}


const nuiLexer::Token& nuiLexer::ParseMultiLineComment()
{
  // Skip the *
  NextChar();
  NextChar();

  while (mChar != '*' && LookAhead() != '/' && !IsStarved())
  {
    if (!NextChar())
      return CaptureToken(Comments);
  }

  // Skip the /
  NextChar();
  NextChar();

  return CaptureToken(Comments);
}

const nuiLexer::Token& nuiLexer::ParseLineComment()
{
  while (mChar != 10 && NextChar() && !IsStarved())
  {
    // Bleh!
  }
  return CaptureToken(Comments);
}


nglUChar nuiLexer::LookAhead() const
{
  if (IsDone())
    return 0;
  return mInput[mEnd];
}


const nuiLexer::Token& nuiLexer::ParseSymbol()
{
  if (!NextChar())
    return CaptureToken(Symbol);

  while (IsValidInSymbol(mChar) && !IsStarved())
  {
    if (!NextChar())
      return CaptureToken(Symbol);
  }

  return CaptureToken(Symbol);
}

const nuiLexer::Token& nuiLexer::ParseNumber()
{
  // Crude parsing: I don't check the validity so there can be 2 decimal dots or exponent sign...
  if (!NextChar())
    return CaptureToken(Number);

  while ((IsNumberDigit(mChar) || mChar == '.' || mChar == 'e' || mChar == 'E') && !IsStarved())
  {
    if (!NextChar())
      return CaptureToken(Number);
  }

  return CaptureToken(Number);
}


const nuiLexer::Token& nuiLexer::CaptureToken(nuiLexer::TokenType type)
{
  if (mpStream)
  {
    mToken.mString = (mEnd - mStart - 1) > 0 ? mInput.Extract(mStart, mEnd - mStart - 1) : "";
    mInput.DeleteLeft((mEnd - mStart - 1));
    mStart = 0;
    mEnd = 1;
    mToken.mType = type;

    return mToken;
  }
  else
  {
    mToken.mString = (mEnd - mStart - 1) > 0 ? mInput.Extract(mStart, mEnd - mStart - 1) : "";
    mStart = mEnd - 1;
    mToken.mType = type;

    return mToken;
  }
}

const nuiLexer::Token& nuiLexer::GetToken() const
{
  return mToken;
}

bool nuiLexer::SkipBlank()
{
  while (IsBlank(mChar) && NextChar() && !IsStarved())
  {
    // Woké
  }

  return !IsDone();
}

bool nuiLexer::IsBlank(nglUChar c) const
{
  return mBlanks.find(c) != mBlanks.end();
}

bool nuiLexer::IsDone() const
{
  return mpStream ? (mpStream->GetState() != eStreamReady) : (mStart >= mInput.GetLength());
}

bool nuiLexer::IsStarved() const
{
  return mpStream ? (mpStream->GetState() != eStreamReady) : (mEnd >= mInput.GetLength());
}


bool nuiLexer::NextChar()
{
  if (!mpStream)
  {
    if (IsDone())
      return false;

    bool lastCharOrOverflow = mEnd >= mInput.GetLength();

    mLastChar = mChar;
    mChar = lastCharOrOverflow ? '\0' : mInput[mEnd];
    mEnd++;
    mColumn++;

    if (mChar == '\n')
    {
      mLine++;
      mColumn = 0;
    }
    return !IsDone();
  }

  // We are handling a stream:
  mLastChar = mChar;
  nglUChar previous = mChar;
  // Parse an utf-8 char sequence:
  uint8 c = 0;
  if (1 != mpStream->ReadUInt8(&c, 1))
  {
    mChar = 0;
    return false;
  }
  if (!(c & 0x80))
  {
    mChar = c;
  }
  else
  {
    //  0xC0 // 2 bytes
    //  0xE0 // 3
    //  0xF0 // 4
    //  0xF8 // 5
    //  0xFC // 6
    uint32 count = 0;
    if ((c & 0xFC) == 0xFC)
    {
      mChar = c & ~0xFC;
      count = 5;
    }
    else if ((c & 0xF8) == 0xF8)
    {
      mChar = c & ~0xF8;
      count = 4;
    }
    else if ((c & 0xF0) == 0xF0)
    {
      mChar = c & ~0xF0;
      count = 3;
    }
    else if ((c & 0xE0) == 0xE0)
    {
      mChar = c & ~0xE0;
      count = 2;
    }
    else if ((c & 0xC0) == 0xC0)
    {
      mChar = c & ~0xC0;
      count = 1;
    }

    for (uint32 i = 0; i < count; i++)
    {
      if (1 != mpStream->ReadUInt8(&c, 1))
        return false;
      mChar <<= 6;
      mChar |= c & 0x3F;
    }
  }

  mInput += mChar;

  mEnd++;
  mColumn++;

  if (mChar == '\n')
  {
    mLine++;
    mColumn = 0;
  }

  if ((mChar == 0xa && previous != 0xd) || (mChar == 0xd && previous != 0xa) )
  {
    mColumn = 1;
    mLine++;
  }

  //wprintf("%lc", mChar);

  return true;
}


int nuiLexer::GetLine() const
{
  return mLine;
}

int nuiLexer::GetColumn() const
{
  return mColumn;
}


void nuiLexer::SetValidInSymbolStart(const nglString& rValidChars)
{
  for (uint32 i = 0; i < rValidChars.GetLength(); i++)
    mValidInSymbolStart.insert(rValidChars[i]);
}

void nuiLexer::SetValidInSymbol(const nglString& rValidChars)
{
  for (uint32 i = 0; i < rValidChars.GetLength(); i++)
    mValidInSymbol.insert(rValidChars[i]);
}

void nuiLexer::SetValidInBlank(const nglString& rValidChars)
{
  for (uint32 i = 0; i < rValidChars.GetLength(); i++)
    mBlanks.insert(rValidChars[i]);
}

bool nuiLexer::IsValidInSymbolStart(nglUChar c) const
{
  return (mValidInSymbolStart.find(c) != mValidInSymbolStart.end());
}

bool nuiLexer::IsValidInSymbol(nglUChar c) const
{
  if (mValidInSymbol.find(c) != mValidInSymbol.end())
    return true;
  return IsValidInSymbolStart(c);
}

bool nuiLexer::IsNumberDigit(nglUChar c, uint32 Base) const
{
  uint8 d = 0;
  return GetNumberDigit(d, c, Base);
}

bool nuiLexer::GetNumberDigit(uint8& res, nglUChar c, uint32 Base) const
{
  if (c >= '0' && c <= '9')
    c -= '0';
  else if (c >= 'a' && c <= 'z')
  {
    c -= 'a';
    c += 10;
  }
  else if (c >= 'A' && c <= 'Z')
  {
    c -= 'A';
    c += 10;
  }
  res = (uint8)c;
  return res < Base;
}

void nuiLexer::AddTokenPattern(const nglString& rString, nuiLexer::TokenType type)
{
  TokenPattern* pattern = &mPatterns;
  size_t i = 0;
  nglUChar ch = 0;

  while ((ch = rString.GetNextUChar(i)))
  {
    pattern = &(pattern->mChildren[ch]);
  }

  pattern->mType = type;
}


