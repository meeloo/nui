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
  char operators[] =
  {
    '.',
    '<',
    '>',
    '?',
    ':',
    '/',
    '|',
    '&',
    '^',
    '*',
    '/',
    '+',
    '-',
    '=',
    '%',
    '!',
    '~',
    0
  };

  for (int i = 0; operators[i]; i++)
    mOperators.insert(operators[i]);

  SetValidInSymbolStart("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
  SetValidInSymbol("0123456789");
  SetValidInBlank(" \t\r\n");

  // Start streaming chars:
  NextChar();
}

nuiLexer::nuiLexer(nglIStream* pStream, const nglPath& rSourcePath)
{
  mpStream = pStream;
  mSourcePath = rSourcePath;
  if (mSourcePath.IsLeaf())
    mSourcePath = mSourcePath.GetParent();
}

const char* nuiLexer::Token::GetTypeName() const
{
  switch (mType)
  {
    case EndOfFile:
      return "EOF";
    case Symbol:
      return "Symbol";
    case String:
      return "String";
    case Number:
      return "Number";
    case Operator:
      return "Operator";
    case AssignementOperator:
      return "AssignementOperator";

    case OpenParent:
      return "OpenParent";
    case CloseParent:
      return "CloseParent";

    case OpenBracket:
      return "OpenBracket";
    case CloseBracket:
      return "CloseBracket";

    case OpenSBracket:
      return "OpenSBracket";
    case CloseSBracket:
      return "CloseSBracket";

    case Comments:
      return "Comments";

    case Comma:
      return "Comma";

    case SemiColon:
      return "SemiColon";


    case Blank:
      return "Blank";
    case NewLine:
      return "NewLine";

    case Hash:
      return "Hash";
  }

  return "???";
}

const char* nuiLexer::Token::c_str() const
{
  return mString.GetChars();
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

  if (mChar == '.')
  {
    if (isnumber(LookAhead()))
      return ParseNumber();

    NextChar();
    return CaptureToken(Operator);
  }
  else if (mChar == '(')
  {
    NextChar();
    return CaptureToken(OpenParent);
  }
  else if (mChar == ')')
  {
    NextChar();
    return CaptureToken(CloseParent);
  }
  else if (mChar == '{')
  {
    NextChar();
    return CaptureToken(OpenBracket);
  }
  else if (mChar == '}')
  {
    NextChar();
    return CaptureToken(CloseBracket);
  }
  else if (mChar == '[')
  {
    NextChar();
    return CaptureToken(OpenSBracket);
  }
  else if (mChar == ']')
  {
    NextChar();
    return CaptureToken(CloseSBracket);
  }
  else if (mChar == ',')
  {
    NextChar();
    return CaptureToken(Comma);
  }
  else if (mChar == ';')
  {
    NextChar();
    return CaptureToken(SemiColon);
  }
  else if (mChar == '#')
  {
    NextChar();
    return CaptureToken(Hash);
  }
  else if (IsValidInSymbolStart(mChar))
  {
    // Parse a Symbol
    return ParseSymbol();
  }
  else if (IsNumberDigit(mChar))
  {
    return ParseNumber();
  }
  else if (mChar == '\"')
  {
    //  Parse string
    ParseString();
  }
  else if (mChar == '/')
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
    else if (LookAhead() == '=')
    {
      NextChar();
      NextChar();
      return CaptureToken(AssignementOperator);
    }

    NextChar();
    return CaptureToken(Operator);
  }
  else if (mChar == '=')
  {
    if (LookAhead() == '=')
    {
      NextChar();
      return CaptureToken(Operator);
    }

    NextChar();
    return CaptureToken(AssignementOperator);
  }
  else if (mChar == '*' || mChar == '+' || mChar == '-' || mChar == '&' || mChar == '|' || mChar == '^' || mChar == '%')
  {
    if (LookAhead() == '=')
    {
      NextChar();
      NextChar();
      return CaptureToken(AssignementOperator);
    }

    NextChar();
    return CaptureToken(Operator);
  }
  else if (mOperators.find(mChar) != mOperators.end())
  {
    NextChar();
    return CaptureToken(Operator);
  }

  return mToken;
}

const nuiLexer::Token& nuiLexer::ParseString()
{
  // Skip the "
  NextChar();

  while (mChar != '"' && !IsStarved())
  {
    if (!NextChar())
      return CaptureToken(String);
  }

  // Skip the "
  NextChar();

  return CaptureToken(String);
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


char nuiLexer::LookAhead() const
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
  mToken.mString = (mEnd - mStart - 1) > 0 ? mInput.Extract(mStart, mEnd - mStart - 1) : "";
  mStart = mEnd - 1;
  mToken.mType = type;

  return mToken;
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
  nglChar previous = mChar;
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

bool nuiLexer::IsValidInSymbolStart(nglChar c) const
{
  return (mValidInSymbolStart.find(c) != mValidInSymbolStart.end());
}

bool nuiLexer::IsValidInSymbol(nglChar c) const
{
  if (mValidInSymbol.find(c) != mValidInSymbol.end())
    return true;
  return IsValidInSymbolStart(c);
}

bool nuiLexer::IsNumberDigit(nglChar c, uint32 Base) const
{
  uint8 d = 0;
  return GetNumberDigit(d, c, Base);
}

bool nuiLexer::GetNumberDigit(uint8& res, nglChar c, uint32 Base) const
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
  res = c;
  return res < Base;
}

