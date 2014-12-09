//
//  nuiTableView.cpp
//  nui3
//
//  Created by vince on 12/7/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#include "nuiTableView.h"
#include "nuiWidgetMatcher.h"

#define LIST_VIEW_ANIMATION_OFFSET 0.f
#define LIST_VIEW_ANIMATION_SENSITIVITY 2
#define LIST_VIEW_ANIMATION_DURATION 1.f

#define LIST_VIEW_MOVE_THRESHOLD 10.f
#define LIST_VIEW_MOUSE_WHEEL_INCREMENT 20

#define LIST_VIEW_SCROLL_SIZE 8.0f
#define LIST_VIEW_SCROLL_TRIGGER_OFFSET (12.0f)

#define SCREEN_INV_SCALE_FACTOR nuiGetInvScaleFactor()

#define IAL_SMOOTH_SCROLL_RATIO 0.2f
#define IAL_AUTOSCROLL_MIN_SPEED 150.0
#define IAL_AUTOSCROLL_MAX_SPEED 800.0
#define IAL_AUTOSCROLL_OFFSET 48
#define IAL_AUTOSCROLL_DECELERATE_RATIO 0.95
#define IAL_OFFSET_DECELERATE_RATIO 0.7








#pragma mark nuiTableView

nuiTableView::nuiTableView(CellSource* pSource)
: nuiScrollView(false, true),
  mpSource(pSource),
  mCellHeight(48),
  mFirstVisibleCell(0),
  mLastVisibleCell(0)
{
  NGL_ASSERT(pSource);
  if (SetObjectClass("nuiTableView"))
    InitAttributes();
}

nuiTableView::~nuiTableView()
{
  mpSource->Release();
}

void nuiTableView::InitAttributes()
{
  AddAttribute(new nuiAttribute<nuiSize>(nglString(_T("CellHeight")), nuiUnitPixels,
                                         nuiMakeDelegate(this, &nuiTableView::GetCellHeight),
                                         nuiMakeDelegate(this, &nuiTableView::SetCellHeight)));
}

nuiRect nuiTableView::CalcIdealSize()
{
  mChildrenUnionRect.Set((nuiSize)0,(nuiSize)0,(nuiSize)0, (nuiSize)mpSource->GetNumberOfCells()*GetCellHeight());
  return mChildrenUnionRect;
}

bool nuiTableView::SetRect(const nuiRect& rRect)
{
  if (rRect.GetHeight() != mRect.GetHeight())
    CreateCells(rRect.GetHeight());
  return nuiScrollView::SetRect(rRect);
}

bool nuiTableView::Draw(nuiDrawContext* pContext)
{
  return nuiScrollView::Draw(pContext);
}

bool nuiTableView::SetChildrenRect(nuiSize x, nuiSize y, nuiSize xx, nuiSize yy, nuiSize scrollv, nuiSize scrollh)
{
  nuiRange& vrange = mpVertical->GetRange();
  nuiSize YOffset = vrange.GetValue();
  
  if (mSmoothScrolling)
  {
    if (!mVThumbPressed)
      YOffset = mYOffset;
  }
  YOffset = (nuiSize)ToNearest(YOffset);

  const nuiSize cellHeight = GetCellHeight();
  const nuiSize tableHeight = GetRect().GetHeight();
  const nuiSize tableWidth = GetRect().GetWidth();

  int32 firstVisibleCell = MAX(0, ::ToBelow(YOffset / cellHeight));
  int32 lastVisibleCell = MIN((int32)mpSource->GetNumberOfCells()-1, firstVisibleCell + mVisibleCells.size()-1);

  if (mFirstVisibleCell > firstVisibleCell)
  {
    while (mFirstVisibleCell > firstVisibleCell)
    {
      nuiWidget* pCell = mVisibleCells.back();
      mVisibleCells.pop_back();
      mVisibleCells.push_front(pCell);

//      for (int32 i = mVisibleCells.size()-1; i > 0; i--)
//      {
//        mVisibleCells[i] = mVisibleCells[i-1];
//      }
//      mVisibleCells[0] = pCell;

      --mFirstVisibleCell;
      --mLastVisibleCell;
      mpSource->UpdateCell(mFirstVisibleCell, pCell);
    }
  }
  else if (mLastVisibleCell < lastVisibleCell)
  {
    while (mLastVisibleCell < lastVisibleCell)
    {
      nuiWidget* pCell = mVisibleCells.front();
      mVisibleCells.pop_front();
      mVisibleCells.push_back(pCell);

//      for (int32 i = 1; i < mVisibleCells.size(); i++)
//      {
//        mVisibleCells[i-1] = mVisibleCells[i];
//      }
//      mVisibleCells[mVisibleCells.size()-1] = pCell;

      ++mLastVisibleCell;
      ++mFirstVisibleCell;
      mpSource->UpdateCell(mLastVisibleCell, pCell);
    }
  }


  if (YOffset >= 0)
    y = -(YOffset - cellHeight*mFirstVisibleCell);
  else
    y = -YOffset;

  int i = 0;
  for (Cells::const_iterator it = mVisibleCells.begin(); it != mVisibleCells.end(); ++it)
  {
    nuiWidget* pCell = *it;
    nuiRect rect = pCell->GetIdealRect();
    rect.Set((nuiSize)0, y, tableWidth, cellHeight);
    pCell->SetLayout(rect);
    y+= cellHeight;
  }

  return true;
}

void nuiTableView::CreateCells(nuiSize Height)
{
  NGL_ASSERT(mpSource);
  nuiMainWindow* pTop = dynamic_cast<nuiMainWindow*>(GetTopLevel());
  NGL_ASSERT(pTop);

  int32 visibleCells = MIN(mpSource->GetNumberOfCells(), ::ToAbove(Height / GetCellHeight()) + 1);
  if (mVisibleCells.size() != visibleCells)
  {
    while (!mVisibleCells.empty())
    {
      nuiWidget* pWidget = mVisibleCells.front();
      DelChild(pWidget);
      mVisibleCells.erase(mVisibleCells.begin());
    }

  for (int32 cell = 0; cell < visibleCells; ++cell)
  {
    nuiWidget* pWidget = mpSource->CreateCell();
    NGL_ASSERT(pWidget);
    mVisibleCells.push_back(pWidget);
    mpSource->UpdateCell(cell, pWidget);
    AddChild(pWidget);
//    pTop->ApplyWidgetCSS(pWidget, true, NUI_WIDGET_MATCHTAG_ALL);
  }
  mFirstVisibleCell = 0;
  mLastVisibleCell = visibleCells-1;
  }
}

void nuiTableView::OnHotRectChanged(const nuiEvent& rEvent)
{
}

