//
//  nuiTableView.h
//  nui3
//
//  Created by vince on 12/7/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#ifndef nui3_nuiTableView_h
#define nui3_nuiTableView_h


class CellSource : public nuiObject
{
public:
  typedef nuiSimpleEventSource<0> CellSourceEvent;
  
  CellSource() : nuiObject(), mNeedsRefresh(false) {}
  
  virtual nuiWidget* CreateCell() = 0;
  virtual void UpdateCell(int32 index, nuiWidget* pItem) = 0;
  virtual uint32 GetNumberOfCells() = 0;
  
  bool GetNeedsRefresh() { return mNeedsRefresh; }
  virtual void SetNeedsRefresh(bool refresh) { mNeedsRefresh = refresh; }
  
  CellSourceEvent DataChanged;

protected:
  virtual ~CellSource() {};

private:
  bool mNeedsRefresh;
};

class nuiTableView : public nuiScrollView
{
public:
  nuiTableView(CellSource* pSource);
  virtual ~nuiTableView();
  nuiRect CalcIdealSize();
  bool SetRect(const nuiRect& rRect);
  bool Draw(nuiDrawContext* pContext);
  
  nuiWidget* GetCell(int32 Index);

protected:
  void InitAttributes();
  bool SetChildrenRect(nuiSize x, nuiSize y, nuiSize xx, nuiSize yy, nuiSize scrollv, nuiSize scrollh);
  void SetCellHeight(nuiSize Height) { mCellHeight = Height; InvalidateLayout(); }
  nuiSize GetCellHeight() const { return mCellHeight; }
  void OnHotRectChanged(const nuiEvent& rEvent);

private:
  CellSource* mpSource;
//  using Cells = std::vector<nuiWidget*>;
  using Cells = std::list<nuiWidget*>;
  nuiSize mCellHeight;
  Cells mVisibleCells;
  int32 mFirstVisibleCell;
  int32 mLastVisibleCell;
  
  void CreateCells(nuiSize Height);
};


#endif
