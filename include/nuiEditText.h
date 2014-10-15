/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/
#ifndef __nuiEditText_h__
#define __nuiEditText_h__

#include "nuiWidget.h"
#include "nuiTheme.h"

#include "nglDragAndDropObjects.h"
#include "nglDataObjects.h"

#include "nuiFontBase.h"
#include "nuiTextLayout.h"

class nuiFont;

class nuiEditText : public nuiWidget
{
protected:
  class TextBlock;

public:
  nuiEditText(const nglString& rText = nglString::Empty);
  virtual ~nuiEditText();

  void InitProperties();

  virtual bool Draw(nuiDrawContext* pContext);

  virtual nuiRect CalcIdealSize();
  virtual bool SetRect(const nuiRect& rRect);

  virtual bool TextInput(const nglString& rUnicodeString);
  virtual bool KeyDown  (const nglKeyEvent& rEvent);
  virtual bool KeyUp    (const nglKeyEvent& rEvent);

  virtual void TextCompositionStarted();
  virtual void TextCompositionConfirmed();
  virtual void TextCompositionCanceled();
  virtual void TextCompositionUpdated(const nglString& rString, int32 CursorPosition);
  virtual nglString GetTextComposition() const;
  virtual void TextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const;
  
  virtual bool MouseClicked  (nuiSize X, nuiSize Y, nglMouseInfo::Flags Button);
  virtual bool MouseUnclicked(nuiSize X, nuiSize Y, nglMouseInfo::Flags Button);
  virtual bool MouseMoved    (nuiSize X, nuiSize Y);

  virtual nglDropEffect OnCanDrop(nglDragAndDrop* pDragObject, nuiSize X, nuiSize Y);
  virtual void OnDropped(nglDragAndDrop* pDragObject, nuiSize X, nuiSize Y, nglMouseInfo::Flags Button);
  
  virtual void OnDragRequestData(nglDragAndDrop* pDragObject, const nglString& rMimeType);
  virtual void OnDragStop(bool canceled);

  enum CommandId
  {
    eGoDocBegin = 0,
    eGoDocEnd,
    eGoLineBegin,
    eGoLineEnd,
    eGoParagraphBegin,
    eGoParagraphEnd,
    eGoUp,
    eGoDown,
    eGoLeft,
    eGoRight,
    eGoPageUp,
    eGoPageDown,
    eGoNextWord,
    eGoPreviousWord,
    eStartSelection,
    eStopSelection,
    eStartCommand,
    eStopCommand,
    eStartShift,
    eStopShift,
    eStartAlt,
    eStopAlt,
    eDisableSelection,
    eSelectAll,
    eDeleteSelection,
    eSelectParagraph,
    eSelectLine,
    eSelectWord,

    eCopy,
    eCut,
    ePaste,

    eDeleteForward,
    eDeleteBackward,

    eUndo,
    eRedo,

    eShowCursor,

    eInsertText,
    eNewLine,

    eLastCommand
  };

  bool Do(CommandId command, nuiObject* pParams);
  bool Undo();
  bool Redo();
  bool CanUndo() const;
  bool CanRedo() const;

  nglString GetSelection() const;
  const nglString& GetText() const;
  void SetText(const nglString& rText);

  void MoveDropCursorTo(nuiSize X, nuiSize Y);

  void MoveCursorTo(nuiSize X, nuiSize Y);
  void SetCursorPos(uint Pos);
  void SetCursorPos(uint Line, uint Column);
  uint GetCursorPos() const;
  void SetAnchorPos(uint Pos);
  uint GetAnchorPos() const;
  void MoveCursorTo(uint Pos); ///< Move the cursor to a position. Move the anchor too if we are not currently in selection mode.

  TextBlock* GetBlock(uint Pos) const;
  uint GetPosFromCoords(uint x, uint y, bool IgnoreWidth) const;
  uint GetPosFromCoords(nuiSize x, nuiSize y, bool IgnoreWidth) const;
  bool GetCoordsFromPos(uint Pos, uint& x, uint& y) const;
  bool GetCoordsFromPos(uint Pos, nuiSize& x, nuiSize& y) const;
  void GetCursorPos(uint& rX, uint& rY);
  void GetCursorPos(nuiSize& rX, nuiSize& rY);

  bool SetFont(nuiTheme::FontStyle FontStyle = nuiTheme::Fixed);
  bool SetFont(nuiFont* pFont, bool AlreadyAcquired = false);
  nuiFont* GetFont();

  void SetEditable(bool Set);
  bool IsEditable();

  void AddText(const nglString& rText); ///< Adds the given string to the currently held text
  void Print(const nglChar* pFormat, ...); ///< Adds the given string to the currently held text
  void SetFollowModifications(bool Set);
  bool GetFollowModifications() const;

  void SetTextColor(const nuiColor& Color);
  const nuiColor& GetTextColor() const;
  
  // events
  nuiSimpleEventSource<0> TextChanged;

  virtual void OnSetFocus(nuiWidgetPtr pWidget);
  
protected:
  
  void InitAttributes();
    
  void _SetFont(const nglString& rFontSymbol);
  void SetFont(const nglString& rFontSymbol);
  const nglString& _GetFont() const;
  
  
  // Commands:
  bool GoDocBegin(nuiObject* pParams);
  bool GoDocEnd(nuiObject* pParams);
  bool GoLineBegin(nuiObject* pParams);
  bool GoLineEnd(nuiObject* pParams);
  bool GoParagraphBegin(nuiObject* pParams);
  bool GoParagraphEnd(nuiObject* pParams);
  bool GoUp(nuiObject* pParams);
  bool GoDown(nuiObject* pParams);
  bool GoLeft(nuiObject* pParams);
  bool GoRight(nuiObject* pParams);
  bool GoPageUp(nuiObject* pParams);
  bool GoPageDown(nuiObject* pParams);
  bool GoNextWord(nuiObject* pParams);
  bool GoPreviousWord(nuiObject* pParams);
  bool StartSelection(nuiObject* pParams);
  bool StopSelection(nuiObject* pParams);
  bool StartCommand(nuiObject* pParams);
  bool StopCommand(nuiObject* pParams);
  bool StartShift(nuiObject* pParams);
  bool StopShift(nuiObject* pParams);
  bool StartAlt(nuiObject* pParams);
  bool StopAlt(nuiObject* pParams);
  bool DisableSelection(nuiObject* pParams);
  bool SelectAll(nuiObject* pParams);
  bool DeleteSelection(nuiObject* pParams);
  bool SelectParagraph(nuiObject* pParams);
  bool SelectLine(nuiObject* pParams);
  bool SelectWord(nuiObject* pParams);

  bool Copy(nuiObject* pParams);
  bool Cut(nuiObject* pParams);
  virtual bool Paste(nuiObject* pParams);

  bool DeleteForward(nuiObject* pParams);
  bool DeleteBackward(nuiObject* pParams);

  bool Undo(nuiObject* pParams);
  bool Redo(nuiObject* pParams);

  bool ShowCursor(nuiObject* pParams);

  bool InsertText(nuiObject* pParams);
  bool NewLine(nuiObject* pParams);

  class TextBlock
  {
  public:
    TextBlock(nuiFont* pFont, const nglString& rString, uint begin, uint end);
    virtual ~TextBlock();

    void Draw(nuiDrawContext* pContext, nuiSize X, nuiSize Y, uint SelectionBegin, uint SelectionEnd, uint CompositionBegin, uint CompositionEnd, nuiSize WidgetWidth);
    const nuiRect& GetIdealSize();

    void SetRect(const nuiRect& rRect);
    const nuiRect& GetRect() const;

    uint GetPos() const;
    uint GetLength() const;
    uint GetEnd() const;
    void SetPos(uint Pos);
    void SetLength(uint Length);
    void SetEnd(uint End);

    uint GetLineHeight();

    nuiSize GetHeight();

    uint GetLineBeginFromPos(uint Pos);
    uint GetLineEndFromPos(uint Pos);
    bool GetCoordsFromPos(uint Pos, uint& rX, uint& rY);
    bool GetCoordsFromPos(uint Pos, nuiSize& rX, nuiSize& rY);
    uint GetPosFromCoords(uint X, uint Y);
    uint GetPosFromCoords(nuiSize X, nuiSize Y);

    bool ContainsPos(uint Pos);

    void Layout();
    void InvalidateLayout();

  protected:
    uint mBegin;
    uint mEnd;
    nuiTextLayout* mpLayout;
    nuiRect mRect;
    nuiRect mIdealRect;
    const nglString& mrString;
    nuiFont* mpFont;

    bool mLayoutOK;
  };

  std::vector<TextBlock*> mpBlocks;
  nglString mText;
  uint mCursorPos; // Position in the text string
  uint mAnchorPos; // Position in the text string
  int32 mDropCursorPos; // Position in the text string, -1 is disabled
  int mCompositionPos; // Position in the text string of the text composition start (for IME)
  int mCompositionLength; // size of the text composition (for IME)

  std::vector<std::pair<CommandId, nuiObject*> > mCommandStack;
  uint mCommandStackPos;
  void CropCommandStack();
  void ClearCommandStack();
  void PushCommand(CommandId, nuiObject*);

  void SaveCursorPos(nuiObject* pParams) const;
  void LoadCursorPos(nuiObject* pParams);

  void SaveAnchorPos(nuiObject* pParams) const;
  void LoadAnchorPos(nuiObject* pParams);

  void SavePos(nuiObject* pParams) const;
  void LoadPos(nuiObject* pParams);

  typedef bool (nuiEditText::*CommandFunction)(nuiObject* pParams);
  std::vector<CommandFunction> mCommands;

  nuiFont* mpFont;
  nuiSize mTextHeight;
  nuiSize mSelectGap;

  void ClearBlocks();
  void CreateBlocks(const nglString& rText, std::vector<TextBlock*>& rpBlocks);

  std::map<nglKeyCode, CommandId> mKeyBindings;
  std::map<nglKeyCode, CommandId> mCommandKeyBindings;
  std::map<nglKeyCode, CommandId> mAltKeyBindings;
  void InitKeyBindings();
  void InitCommands();

  bool mCommandDown;
  bool mShiftDown;
  bool mAltDown;
  bool mSelecting; 
  bool mSelectionActive; 
  bool mIsEditable;
  bool mFollowModifications;
  bool mStartDragging;
  bool mDragging;
  
  bool mTextColorSet;
  nuiColor mTextColor;
  
};

#endif // __nuiEditText_h__
