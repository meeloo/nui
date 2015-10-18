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

class nuiTableView;

class nuiCellSource
{
public:
  typedef nuiSimpleEventSource<0> CellSourceEvent;
  
  nuiCellSource() : mNeedsRefresh(false) {}
  virtual ~nuiCellSource() {}

  virtual nuiWidget* CreateCell() = 0;
  virtual void UpdateCell(int32 index, nuiWidget* pItem) = 0;
  virtual uint32 GetNumberOfCells() = 0;
  
  bool GetNeedsRefresh() { return mNeedsRefresh; }
  virtual void SetNeedsRefresh(bool refresh) { mNeedsRefresh = refresh; }
  
  CellSourceEvent DataChanged;
  
  void SetTableView(nuiTableView* pView) { mpTableView = pView; }
protected:
  nuiTableView* mpTableView = nullptr;
private:
  bool mNeedsRefresh;
};


#pragma mark-
#pragma mark nuiTableView

class nuiTableView : public nuiScrollView
{
public:
  nuiTableView();
  nuiTableView(nuiCellSource* pSource, bool OwnSource=true);
  virtual ~nuiTableView();
  nuiCellSource* GetSource() { return mpSource; }
  void SetSource(nuiCellSource* pSource, bool OwnSource=true);

  
  nuiRect CalcIdealSize();
  bool SetRect(const nuiRect& rRect);
  bool Draw(nuiDrawContext* pContext);
  
  
  nuiWidget* GetCell(int32 Index);

  void SetCellHeight(nuiSize Height) { mCellHeight = Height; InvalidateLayout(); }
  nuiSize GetCellHeight() const { return mCellHeight; }
  void SetDrawSeparators(bool DrawSeparators) { mDrawSeparators = DrawSeparators; Invalidate(); }
  bool GetDrawSeparators() const { return mDrawSeparators; }
  void SetSeparatorColor(const nuiColor& rColor) { mSeparatorColor = rColor; Invalidate(); }
  const nuiColor& GetSeparatorColor() const { return mSeparatorColor; }
  void SetSeparatorOffset(nuiSize Offset) { mSeparatorOffset = Offset; Invalidate(); }
  nuiSize GetSeparatorOffset() const { return mSeparatorOffset; }
  void SetSeparatorWidth(nuiSize Width) { mSeparatorWidth = Width; Invalidate(); }
  nuiSize GetSeparatorWidth() const { return mSeparatorWidth; }

  nuiSimpleEventSource<0> SelectionChanged;
  typedef nuiFastDelegate2<nuiCellSource, int32, bool> CellSelectedDelegate;
  void SetCellSelectedDelegate(const CellSelectedDelegate& rDelegate) { mSetCellSelectedDelegate = rDelegate; }

  typedef nuiFastDelegate2<nuiCellSource*, int32, nglDragAndDrop*> StartDragDelegate;
  void SetStartDragDelegate(const StartDragDelegate& rDelegate) { mStartDragDelegate = rDelegate; }
  void OnDragStop(bool canceled);

  void OnSourceDataChanged(const nuiEvent& rEvent);

  int32 GetCellIndex(nuiWidget* pCell) const noexcept;

  void SetHotCell(int32 Index) { mHotCell = Index; InvalidateLayout(); }

protected:
  void InitAttributes();
  bool SetChildrenRect(nuiSize x, nuiSize y, nuiSize xx, nuiSize yy, nuiSize scrollv, nuiSize scrollh);
  void OnHotRectChanged(const nuiEvent& rEvent);

  nuiEventSink<nuiTableView> mTableViewSink;

private:
  nuiCellSource* mpSource = nullptr;
  bool mOwnSource = false;
  
  using Cells = std::list<nuiWidget*>;
  Cells mVisibleCells;
  int32 mFirstVisibleCell = 0;
  int32 mLastVisibleCell = 0;
  nuiSize mCellHeight = 48;
  bool mNeedUpdateCells = true;
  void CreateCells(nuiSize Height);

  int32 mHotCell = -1;

  bool mDrawSeparators = true;
  nuiColor mSeparatorColor = nuiColor(0,0,0,192);
  
  StartDragDelegate mStartDragDelegate;


  
///< Selection
public:
  using Selection = std::vector<uint32>;
  void EnableSelection(bool Enable);
  bool IsSelectionEnabled() const;
  void SelectCell(int32 Index, bool Select=true);
  void ClearSelection();
  const Selection& GetSelection();
  int32 GetSelectedCell() const noexcept { return mSelectedCell; }
  CellSelectedDelegate mSetCellSelectedDelegate;

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
  nuiSize mSeparatorOffset = 8;
  nuiSize mSeparatorWidth = 1;
  int32 GetClickedCell() { return mClickedIndex; }

//  bool CallPreMouseMoved(const nglMouseInfo& rInfo);
};


#endif
