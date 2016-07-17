/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
*/

class nuiLexer
{
public:
  enum TokenType
  {
    EndOfFile,
    Symbol,
    String,
    Number,
    Operator,
    AssignementOperator,
    OpenParent,
    CloseParent,
    OpenBracket,
    CloseBracket,
    OpenSBracket,
    CloseSBracket,
    Comments,
    Blank,
    Comma,
    SemiColon,
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

  const nuiLexer::Token& NextToken();
  const nuiLexer::Token& GetToken() const;

  bool SkipBlank();
  bool NextChar();
  char LookAhead() const;

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

  bool IsValidInSymbolStart(nglChar c) const; ///< Returns true if the given char is valid as a symbol start
  bool IsValidInSymbol(nglChar c) const; ///< Returns true if the given char is valid in a symbol
  bool IsBlank(nglChar c) const; ///< Returns true if the given char is a blank.
  bool IsNumberDigit(nglChar c, uint32 Base = 10) const; ///< Returns true if the given char is a valid number digit for the given base.
  bool GetNumberDigit(uint8& res, nglChar c, uint32 Base) const; ///< Returns true if the given char is a valid number digit for the given base. In this case res contains the converted digit as a number.


private:
  nglIStream* mpStream = nullptr;
  nglPath mSourcePath;

  nglString mInput;
  size_t mStart = 0;
  size_t mEnd = 0;
  nuiLexer::Token mToken;
  char mLastChar;
  char mChar;

  int mLine = 0;
  int mColumn = 0;

  std::set<char> mOperators;

  std::set<nglChar> mValidInSymbolStart;
  std::set<nglChar> mValidInSymbol;
  std::set<nglChar> mLineCommentStarters;
  std::set<nglChar> mBlanks;

};

