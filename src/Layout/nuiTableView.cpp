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

nuiTableView::nuiTableView(nuiCellSource* pSource)
: nuiScrollView(false, true),
  mpSource(pSource),
  mCellHeight(48),
  mFirstVisibleCell(0),
  mLastVisibleCell(0)
{
  NGL_ASSERT(pSource);
  pSource->Acquire();
  mSelection.resize(pSource->GetNumberOfCells(), false);

  if (SetObjectClass("nuiTableView"))
    InitAttributes();
}

nuiTableView::~nuiTableView()
{
  mpSource->Release();
}

void nuiTableView::InitAttributes()
{
  AddAttribute(new nuiAttribute<bool>(nglString(_T("EnableSelection")), nuiUnitBoolean,
                                         nuiMakeDelegate(this, &nuiTableView::IsSelectionEnabled),
                                         nuiMakeDelegate(this, &nuiTableView::EnableSelection)));
  AddAttribute(new nuiAttribute<nuiSize>(nglString(_T("CellHeight")), nuiUnitPixels,
                                         nuiMakeDelegate(this, &nuiTableView::GetCellHeight),
                                         nuiMakeDelegate(this, &nuiTableView::SetCellHeight)));
  AddAttribute(new nuiAttribute<bool>(nglString(_T("DrawSeparators")), nuiUnitBoolean,
                                         nuiMakeDelegate(this, &nuiTableView::GetDrawSeparators),
                                         nuiMakeDelegate(this, &nuiTableView::SetDrawSeparators)));
  AddAttribute(new nuiAttribute<const nuiColor&>(nglString(_T("SeparatorColor")), nuiUnitColor,
                                         nuiMakeDelegate(this, &nuiTableView::GetSeparatorColor),
                                         nuiMakeDelegate(this, &nuiTableView::SetSeparatorColor)));
}

nuiRect nuiTableView::CalcIdealSize()
{
  mChildrenUnionRect.Set((nuiSize)0,(nuiSize)0,(nuiSize)0, (nuiSize)mpSource->GetNumberOfCells()*GetCellHeight());
//  printf("nuiTableView: ideal height: %d\n", (int)mChildrenUnionRect.GetHeight());
  return mChildrenUnionRect;
}

bool nuiTableView::SetRect(const nuiRect& rRect)
{
  if (rRect.GetHeight() != mRect.GetHeight())
  {
    CreateCells(rRect.GetHeight());
  }
  return nuiScrollView::SetRect(rRect);
}

bool nuiTableView::Draw(nuiDrawContext* pContext)
{
  NGL_ASSERT(mFirstVisibleCell >= 0);
  int32 i = mFirstVisibleCell;
  for (Cells::const_iterator it = mVisibleCells.begin(); it != mVisibleCells.end(); ++it)
  {
    nuiWidget* pCell = *it;
    pCell->SetSelected(mSelection[i++]);
  }
  nuiScrollView::Draw(pContext);

  if (mDrawSeparators && mVisibleCells.size() > 1)
  {
    nuiRenderArray* pLines = new nuiRenderArray(GL_LINES);
    pLines->Reserve(mVisibleCells.size()-1);

    nuiSize w = GetRect().GetWidth();
    auto cell = mVisibleCells.begin();
    for (++cell; cell != mVisibleCells.end(); ++cell)
    {
      nuiSize y = (*cell)->GetRect().Top();
      pLines->SetVertex(0,y);
      pLines->PushVertex();
      pLines->SetVertex(w,y);
      pLines->PushVertex();
    }
    nuiColor color = mSeparatorColor;
    color.SetAlpha(GetMixedAlpha());
    pContext->EnableBlending(true);
    pContext->SetBlendFunc(nuiBlendTransp);
    pContext->SetStrokeColor(color);
    pContext->DrawArray(pLines);
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
  int32 visibleCells = MIN(mpSource->GetNumberOfCells(), ::ToAbove(Height / GetCellHeight()) + 1);
//  mLastVisibleCell = mFirstVisibleCell + (visibleCells-1);
//
//  int32 diff = visibleCells - mVisibleCells.size();
//
//  if (diff != 0)
//  {
//    int32 cell = mLastVisibleCell-diff;
//    while (diff > 0)
//    {
//      nuiWidget* pWidget = mpSource->CreateCell();
//      NGL_ASSERT(pWidget);
//      mVisibleCells.push_back(pWidget);
//      mpSource->UpdateCell(cell++, pWidget);
//      AddChild(pWidget);
//      --diff;
//    }
//    while (diff < 0)
//    {
//      nuiWidget* pWidget = mVisibleCells.back();
//      DelChild(pWidget);
//      mVisibleCells.pop_back();
//      ++diff;
//    }

  mFirstVisibleCell = 0;
  mLastVisibleCell = (visibleCells-1);
  if (mVisibleCells.size() != visibleCells)
  {
//    printf("DelChild %d - AddChild %d\n", mVisibleCells.size(), visibleCells);
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
  return MIN(mpSource->GetNumberOfCells()-1, MAX(-1, index));
}


bool nuiTableView::MouseClicked(const nglMouseInfo& rInfo)
{
  if (mSpeedY == 0)
  {
    int32 i = GetCellIndex(rInfo);
    mClickedIndex = i;
    if (i >= 0 && mSelection.size() > i)
      SelectCell(i, !mSelection[i]);
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
  if (!mMoved && !mSelection.empty() && mClickedIndex >= 0)
  {
    SelectCell(mClickedIndex, !mSelection[mClickedIndex]);
    mClickedIndex = -1;
  }

  return nuiScrollView::MouseMoved(rInfo);
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

