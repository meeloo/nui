/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
*/

#include <unordered_map>

class nuiLexer
{
public:
  enum TokenType
  {
    EndOfFile,
    Symbol,

    QuotedString,
    SimpleQuotedString,

    Number,

    Dot,
    Comma,
    SemiColon,
    Colon,
    LessThan,
    LessThanOrEqual,
    MoreThan,
    MoreThanOrEqual,
    ExclamationMark,
    QuestionMark,
    Slash,
    Pipe,
    Ampersand,
    Circumflex,
    Percent,
    Arobase,
    Tilde,
    BackQuote,
    BackSlash,

    Multiply,
    Divide,
    Plus,
    Minus,

    MultiplyEqual,
    DivideEqual,
    PlusEqual,
    MinusEqual,

    Equal,
    IsEqual,

    OpenParent,
    CloseParent,
    OpenBracket,
    CloseBracket,
    OpenSBracket,
    CloseSBracket,
    Comments,
    Blank,
    Hash,
    NewLine
  };
  
  class Token
  {
  public:
    Token(const nglString& str = nglString(), nuiLexer::TokenType type = EndOfFile)
    : mString(str), mType(type)
    {
    }

    const char* GetTypeName() const;
    const char* c_str() const;

    nglString mString;
    nuiLexer::TokenType mType;
  };
  

  nuiLexer(const nglString& str);
  nuiLexer(nglIStream* pStream, const nglPath& rSourcePath);

  const nuiLexer::Token& NextNonBlankToken(bool SkipNewLinesToo);
  const nuiLexer::Token& NextToken();
  const nuiLexer::Token& GetToken() const;

  bool SkipBlank();
  bool NextChar();
  nglUChar LookAhead() const;

  const nuiLexer::Token& CaptureToken(nuiLexer::TokenType type);
  const nuiLexer::Token& ParseSymbol();
  const nuiLexer::Token& ParseNumber();
  const nuiLexer::Token& ParseString();
  const nuiLexer::Token& ParseMultiLineComment();
  const nuiLexer::Token& ParseLineComment();

  bool IsDone() const; // True if we are past the last token in the input string
  bool IsStarved() const; // True if we can no longer add to the current token because we are past the end of the input string
  int GetLine() const;
  int GetColumn() const;

  // Config:
  void SetValidInSymbolStart(const nglString& rValidChars); ///< Set the characters that are valid as the first character of a symbol (usualy all latin letters + underscore).
  void SetValidInSymbol(const nglString& rValidChars); ///< Set the characters that are valid in a symbol (usualy all latin characters + underscore + latin numbers).
  void SetValidInBlank(const nglString& rValidChars); ///< Set the characters that are considered blanks.

  bool IsValidInSymbolStart(nglUChar c) const; ///< Returns true if the given char is valid as a symbol start
  bool IsValidInSymbol(nglUChar c) const; ///< Returns true if the given char is valid in a symbol
  bool IsBlank(nglUChar c) const; ///< Returns true if the given char is a blank.
  bool IsNumberDigit(nglUChar c, uint32 Base = 10) const; ///< Returns true if the given char is a valid number digit for the given base.
  bool GetNumberDigit(uint8& res, nglUChar c, uint32 Base) const; ///< Returns true if the given char is a valid number digit for the given base. In this case res contains the converted digit as a number.


private:
  void Init();
  nglIStream* mpStream = nullptr;
  nglPath mSourcePath;

  nglString mInput;
  size_t mStart = 0;
  size_t mEnd = 0;
  nuiLexer::Token mToken;
  nglUChar mLastChar;
  nglUChar mChar;

  int mLine = 0;
  int mColumn = 0;

  class TokenPattern
  {
  public:
    TokenPattern(TokenType type = EndOfFile) : mType(type) {}

    TokenType mType = EndOfFile;
    std::unordered_map<nglUChar, TokenPattern> mChildren;

  };
  TokenPattern mPatterns;

  void AddTokenPattern(const nglString& rString, nuiLexer::TokenType type);

  std::set<nglUChar> mValidInSymbolStart;
  std::set<nglUChar> mValidInSymbol;
  std::set<nglUChar> mLineCommentStarters;
  std::set<nglUChar> mBlanks;

  bool StreamNextChar(nglUChar& ch);

};

