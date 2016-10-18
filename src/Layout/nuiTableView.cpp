//
//  nuiTableView.cpp
//  nui3
//
//  Created by vince on 12/7/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#include "nuiTableView.h"
#include "nuiWidgetMatcher.h"

#pragma mark nuiTableView

nuiTableView::nuiTableView()
: nuiScrollView(false, true),
  mpSource(nullptr),
  mOwnSource(false),
  mTableViewSink(this)
{
  if (SetObjectClass("nuiTableView"))
    InitAttributes();
}

nuiTableView::nuiTableView(nuiCellSource* pSource, bool OwnSource)
: nuiScrollView(false, true),
  mpSource(pSource),
  mOwnSource(OwnSource),
  mTableViewSink(this)
{
  NGL_ASSERT(pSource);
//  pSource->Acquire();
  mSelection.resize(pSource->GetNumberOfCells(), false);

  mpSource->SetTableView(this);
  mTableViewSink.Connect(pSource->DataChanged, &nuiTableView::OnSourceDataChanged);

  if (SetObjectClass("nuiTableView"))
    InitAttributes();
}

void nuiTableView::OnSourceDataChanged(const nuiEvent& rEvent)
{
  if (mpSource)
  {
    mNeedUpdateCells = true;
    mSelection.resize(mpSource->GetNumberOfCells());
    InvalidateLayout();
  }
}

nuiTableView::~nuiTableView()
{
  if (mpSource && mOwnSource)
    delete mpSource;
}


void nuiTableView::SetSource(nuiCellSource* pSource, bool OwnSource)
{
  if (mpSource)
  {
    mTableViewSink.Disconnect(pSource->DataChanged, &nuiTableView::OnSourceDataChanged);
    if (mOwnSource)
      delete mpSource;
  }
  mOwnSource = OwnSource;
  mpSource = pSource;
  if (pSource)
  {
    mpSource->SetTableView(this);
    mTableViewSink.Connect(pSource->DataChanged, &nuiTableView::OnSourceDataChanged);
  }

  InvalidateLayout();
}

void nuiTableView::InitAttributes()
{
  AddAttribute(new nuiAttribute<bool>(nglString("EnableSelection"), nuiUnitBoolean,
                                      nuiMakeDelegate(this, &nuiTableView::IsSelectionEnabled),
                                      nuiMakeDelegate(this, &nuiTableView::EnableSelection)));
//  AddAttribute(new nuiAttribute<bool>(nglString("EnableMultiSelection"), nuiUnitBoolean,
//                                      nuiMakeDelegate(this, &nuiTableView::IsMultiSelectionEnabled),
//                                      nuiMakeDelegate(this, &nuiTableView::EnableSelection)));
  AddAttribute(new nuiAttribute<nuiSize>(nglString("CellHeight"), nuiUnitPixels,
                                         nuiMakeDelegate(this, &nuiTableView::GetCellHeight),
                                         nuiMakeDelegate(this, &nuiTableView::SetCellHeight)));
  AddAttribute(new nuiAttribute<bool>(nglString("DrawSeparators"), nuiUnitBoolean,
                                         nuiMakeDelegate(this, &nuiTableView::GetDrawSeparators),
                                         nuiMakeDelegate(this, &nuiTableView::SetDrawSeparators)));
  AddAttribute(new nuiAttribute<const nuiColor&>(nglString("SeparatorColor"), nuiUnitColor,
                                         nuiMakeDelegate(this, &nuiTableView::GetSeparatorColor),
                                         nuiMakeDelegate(this, &nuiTableView::SetSeparatorColor)));

  AddAttribute(new nuiAttribute<nuiSize>(nglString("SeparatorOffset"), nuiUnitSize,
                                         nuiMakeDelegate(this, &nuiTableView::GetSeparatorOffset),
                                         nuiMakeDelegate(this, &nuiTableView::SetSeparatorOffset)));
  AddAttribute(new nuiAttribute<nuiSize>(nglString("SeparatorWidth"), nuiUnitSize,
                                         nuiMakeDelegate(this, &nuiTableView::GetSeparatorWidth),
                                         nuiMakeDelegate(this, &nuiTableView::SetSeparatorWidth)));
}

nuiRect nuiTableView::CalcIdealSize()
{
  mChildrenUnionRect.Set((nuiSize)0,(nuiSize)0,(nuiSize)0, (nuiSize)mpSource->GetNumberOfCells()*GetCellHeight());
//  printf("nuiTableView: ideal height: %d\n", (int)mChildrenUnionRect.GetHeight());
  return mChildrenUnionRect;
}

bool nuiTableView::SetRect(const nuiRect& rRect)
{
  mNeedUpdateCells |= (rRect.GetHeight() != mRect.GetHeight());
  if (mNeedUpdateCells)
  {
    mNeedUpdateCells=false;
    CreateCells(rRect.GetHeight()+GetOverDrawBottom()+GetOverDrawTop());
  }
  nuiScrollView::SetRect(rRect);
  
  if (mHotCell >= 0)
  {
    nuiRect rect((nuiSize)0, mHotCell*GetCellHeight(), GetRect().GetWidth(), GetCellHeight());
    GetRange(nuiVertical)->MakeInRangeVisual(rect.Top(), rect.GetHeight()-1);
//    GetRange(nuiVertical)->SetValue(rect.Top());
    mHotCell = -1;
  }

  return true;
}

bool nuiTableView::Draw(nuiDrawContext* pContext)
{
  NGL_ASSERT(mFirstVisibleCell >= 0);
  int32 i = mFirstVisibleCell;
  if (IsSelectionEnabled())
  {
    for (Cells::const_iterator it = mVisibleCells.begin(); it != mVisibleCells.end(); ++it)
    {
      nuiWidget* pCell = *it;
      pCell->SetSelected(mSelection[i++]);
    }
  }
  nuiScrollView::Draw(pContext);

  if (mDrawSeparators && mVisibleCells.size() > 1)
  {
//    nuiRenderArray* pLines = new nuiRenderArray(GL_LINES);
//    pLines->Reserve(mVisibleCells.size()-1);

    nuiColor color = mSeparatorColor;
    color.SetAlpha(color.Alpha()*GetMixedAlpha());
    pContext->EnableBlending(true);
    pContext->SetBlendFunc(nuiBlendTransp);
    pContext->SetStrokeColor(color);
    pContext->SetLineWidth(mSeparatorWidth);

    nuiSize x0 = mSeparatorOffset;
    nuiSize x1 = GetRect().GetWidth()-mSeparatorOffset;
//    nuiSize w = GetRect().GetWidth();
    auto cell = mVisibleCells.begin();
    for (++cell; cell != mVisibleCells.end(); ++cell)
    {
      nuiSize y = (*cell)->GetRect().Top();
      pContext->DrawLine(x0, y, x1, y);
//      pLines->SetVertex(x0,y);
//      pLines->PushVertex();
//      pLines->SetVertex(x1,y);
//      pLines->PushVertex();
    }
//    nuiColor color = mSeparatorColor;
//    color.SetAlpha(GetMixedAlpha());
//    pContext->EnableBlending(true);
//    pContext->SetBlendFunc(nuiBlendTransp);
//    pContext->SetStrokeColor(color);
//    pContext->DrawArray(pLines);
  }

  return true;
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

  if (mVisibleCells.empty())
    return true;

  int32 firstVisibleCell = MAX(0, ::ToBelow((YOffset - GetOverDrawTop())/ cellHeight));
  int32 lastVisibleCell = MIN((int32)mpSource->GetNumberOfCells()-1, firstVisibleCell + mVisibleCells.size()-1);

  if (mFirstVisibleCell > firstVisibleCell)
  {
    while (mFirstVisibleCell > firstVisibleCell)
    {
      nuiWidget* pCell = mVisibleCells.back();
      mVisibleCells.pop_back();
      mVisibleCells.push_front(pCell);
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
  int32 currentVisibleCells = mVisibleCells.size();
  int32 visibleCells = MIN(mpSource->GetNumberOfCells(), ::ToAbove(Height / GetCellHeight()) + 1);

  if (currentVisibleCells == visibleCells)
  {
    if (visibleCells > 0)
    {
      if (mLastVisibleCell >= mpSource->GetNumberOfCells())
      {
        mLastVisibleCell = mpSource->GetNumberOfCells()-1;
        mFirstVisibleCell = MAX(0, mpSource->GetNumberOfCells()-visibleCells);
      }
      
      auto pWidgetIt = mVisibleCells.begin();
      for (int32 cell = mFirstVisibleCell; cell <= mLastVisibleCell; ++cell)
      {
        mpSource->UpdateCell(cell, *pWidgetIt++);
      }
    }
    return;
  }
  
  mFirstVisibleCell = 0;
  mLastVisibleCell = (visibleCells-1);
  
  if (mVisibleCells.size() != visibleCells)
  {
//    printf("DelChild %d - AddChild %d\n", mVisibleCells.size(), visibleCells);
    while (!mVisibleCells.empty())
    {
      nuiWidget* pWidget = mVisibleCells.front();
      DelChild(pWidget);
      mVisibleCells.pop_front();
    }

    for (int32 cell = 0; cell < visibleCells; ++cell)
    {
      nuiWidget* pWidget = mpSource->CreateCell();
      NGL_ASSERT(pWidget);
      mVisibleCells.push_back(pWidget);
      mpSource->UpdateCell(cell, pWidget);
      AddChild(pWidget);
    }
  }
}

void nuiTableView::OnHotRectChanged(const nuiEvent& rEvent)
{
}


#pragma mark Mousing

nuiWidget* nuiTableView::GetCell(const nglMouseInfo& rInfo)
{
  for (auto it = mVisibleCells.begin(); it != mVisibleCells.end(); ++it)
  {
    nuiWidget* pWidget = *it;
    if (pWidget->IsInsideFromParent(rInfo.X, rInfo.Y))
    {
      return pWidget;
    }
  }
  return nullptr;
}

int32 nuiTableView::GetCellIndex(const nglMouseInfo& rInfo)
{
  int32 index = (rInfo.Y + mYOffset) / mCellHeight;
  if (index >= mpSource->GetNumberOfCells())
    return -1;
  return MAX(-1, index);
}


bool nuiTableView::MouseClicked(const nglMouseInfo& rInfo)
{
  if (mSpeedY == 0)// && IsSelectionEnabled())
  {
    int32 i = GetCellIndex(rInfo);
    mClickedIndex = i;
    if (IsSelectionEnabled())
    {
      if (i >= 0 && mSelection.size() > i)
      {
        SelectCell(i, !mSelection[i]); 
      }
    }
  }
  return nuiScrollView::MouseClicked(rInfo);
}

bool nuiTableView::MouseUnclicked(const nglMouseInfo& rInfo)
{
  if (!mMoved && mClickedIndex >= 0)
  {
    SelectionChanged();
  }
  mClickedIndex = -1;
  return nuiScrollView::MouseUnclicked(rInfo);
}

bool nuiTableView::MouseMoved(const nglMouseInfo& rInfo)
{
  if (!mMoved /*&& IsSelectionEnabled()*/)
  {
    if (!mLeftClick)
      return false;
    
    if (!mSelection.empty() && mClickedIndex >= 0)
    {
      float x = mTouch.X - rInfo.X;
      float y = mTouch.Y - rInfo.Y;
      float dist = sqrt(x * x + y * y);
      if (dist > 10)
      {
        mTouched = false;
        mXOffset = GetXPos();
        mYOffset = GetYPos();
        mClickValueH = mXOffset;
        mClickValueV = mYOffset;
        NGL_ASSERT(rInfo.Counterpart);
        rInfo.Counterpart->X = rInfo.X;
        rInfo.Counterpart->Y = rInfo.Y;
        LocalToGlobal(rInfo.Counterpart->X, rInfo.Counterpart->Y);
        mLastTime = nglTime();

        if (mStartDragDelegate && std::fabs(x) > std::fabs(y))
        {
          nglDragAndDrop* pObject = mStartDragDelegate(mpSource, mClickedIndex);
          if (pObject) {
//            mIsDragging = true;
//            mClicked = false;
//            mDoubleClicked = false;
            Drag(pObject);
          }

        }

        SelectCell(mClickedIndex, !mSelection[mClickedIndex]);
        mClickedIndex = -1;
        
        return nuiScrollView::MouseMoved(rInfo);
      }
      else return true;
    }
  }


  return nuiScrollView::MouseMoved(rInfo);
}

void nuiTableView::OnDragStop(bool canceled)
{
  
}


#pragma mark Selection

void nuiTableView::EnableSelection(bool Enable)
{
  mSelectionEnabled = Enable;
  if (!Enable)
    ClearSelection();
}

bool nuiTableView::IsSelectionEnabled() const
{
  return mSelectionEnabled;
}

void nuiTableView::SelectCell(int32 Index, bool Select)
{
  if (!mpSource ||
      mpSource->GetNumberOfCells() <= Index)
  {
    return;
  }
  
  
  if (Select && mSelectedCell != Index)
  {
    if (mSelectedCell >= 0)
    {
      mSelection[mSelectedCell] = false;
      Invalidate();
    }
    mSelectedCell = Index;
  }
  else if (!Select && mSelectedCell == Index)
  {
    mSelectedCell = -1;
  }

  if (mSelection[Index] != Select)
  {
    mSelection[Index] = Select;
    Invalidate();
  }

}

//bool nuiTableView::IsSelected(int32 Index)
//{
//  if (mSelection.find(Index) != mSelection.end())
//    return true;
//  return false;
//}

void nuiTableView::ClearSelection()
{
  for (int i = 0; i < mSelection.size(); ++i)
    mSelection[i] = false;
  mSelectedCell = -1;
  Invalidate();
}

const nuiTableView::Selection& nuiTableView::GetSelection()
{
  return mSelection;
}


int32 nuiTableView::GetCellIndex(nuiWidget* pCell) const noexcept
{
  auto pWidgetIt = mVisibleCells.begin();
  for (int32 cell = mFirstVisibleCell; cell <= mLastVisibleCell; ++cell, pWidgetIt++)
  {
    if (pCell == *pWidgetIt)
      return cell;
  }
  return -1;
}
