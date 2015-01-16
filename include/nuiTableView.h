//
//  nuiTableView.h
//  nui3
//
//  Created by vince on 12/7/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#ifndef nui3_nuiTableView_h
#define nui3_nuiTableView_h


#pragma mark-
#pragma mark CellSource

class nuiCellSource : public nuiObject
{
public:
  typedef nuiSimpleEventSource<0> CellSourceEvent;
  
  nuiCellSource() : nuiObject(), mNeedsRefresh(false) {}
  
  virtual nuiWidget* CreateCell() = 0;
  virtual void UpdateCell(int32 index, nuiWidget* pItem) = 0;
  virtual uint32 GetNumberOfCells() = 0;
  
  bool GetNeedsRefresh() { return mNeedsRefresh; }
  virtual void SetNeedsRefresh(bool refresh) { mNeedsRefresh = refresh; }

  CellSourceEvent DataChanged;

protected:
  virtual ~nuiCellSource() {};

private:
  bool mNeedsRefresh;
};


#pragma mark-
#pragma mark nuiTableView

class nuiTableView : public nuiScrollView
{
public:
  nuiTableView();
  nuiTableView(nuiCellSource* pSource);

  virtual ~nuiTableView();
  nuiRect CalcIdealSize();
  bool SetRect(const nuiRect& rRect);
  bool Draw(nuiDrawContext* pContext);
  
  nuiCellSource* GetSource() { return mpSource; }

  nuiWidget* GetCell(int32 Index);

  void SetCellHeight(nuiSize Height) { mCellHeight = Height; InvalidateLayout(); }
  nuiSize GetCellHeight() const { return mCellHeight; }
  void SetDrawSeparators(bool DrawSeparators) { mDrawSeparators = DrawSeparators; Invalidate(); }
  bool GetDrawSeparators() const { return mDrawSeparators; }
  void SetSeparatorColor(const nuiColor& rColor) { mSeparatorColor = rColor; Invalidate(); }
  const nuiColor& GetSeparatorColor() const { return mSeparatorColor; }

  nuiSimpleEventSource<0> SelectionChanged;
  typedef nuiFastDelegate2<nuiCellSource, int32, bool> CellSelectedDelegate;
  void SetCellSelectedDelegate(const CellSelectedDelegate& rDelegate) { mSetCellSelectedDelegate = rDelegate; }

  typedef nuiFastDelegate2<nuiCellSource*, int32, nglDragAndDrop*> StartDragDelegate;
  void SetStartDragDelegate(const StartDragDelegate& rDelegate) { mStartDragDelegate = rDelegate; }
  void OnDragStop(bool canceled);

protected:
  void InitAttributes();
  bool SetChildrenRect(nuiSize x, nuiSize y, nuiSize xx, nuiSize yy, nuiSize scrollv, nuiSize scrollh);
  void OnHotRectChanged(const nuiEvent& rEvent);

private:
  nuiCellSource* mpSource;
  using Cells = std::list<nuiWidget*>;
  nuiSize mCellHeight;
  bool mDrawSeparators = true;
  nuiColor mSeparatorColor = nuiColor(0,0,0,192);
  Cells mVisibleCells;
  int32 mFirstVisibleCell;
  int32 mLastVisibleCell;
  CellSelectedDelegate mSetCellSelectedDelegate;
  StartDragDelegate mStartDragDelegate;
  void CreateCells(nuiSize Height);

///< Selection
public:
  using Selection = std::vector<uint32>;
  void EnableSelection(bool Enable);
  bool IsSelectionEnabled() const;
  void SelectCell(int32 Index, bool Select=true);
  void ClearSelection();
  const Selection& GetSelection();
  int32 GetSelectedCell() const noexcept { return mSelectedCell; }

protected:
  bool mSelectionEnabled=false;
  bool mMultiSelectionEnabled=false;
  int32 mSelectedCell=-1;;
  Selection mSelection;
  void UpdateSelectedStates();

///< Mousing
public:
  bool MouseClicked(const nglMouseInfo& rInfo);
  bool MouseUnclicked(const nglMouseInfo& rInfo);
  bool MouseMoved(const nglMouseInfo& rInfo);

protected:
  nuiWidget* GetCell(const nglMouseInfo& rInfo);
  int32 GetCellIndex(const nglMouseInfo& rInfo);
  int32 mClickedIndex = -1;
  

//  bool CallPreMouseMoved(const nglMouseInfo& rInfo);
};


#endif
