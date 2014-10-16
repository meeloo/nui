/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

nuiZoomBar::nuiZoomBar(nuiOrientation Orientation, const nuiRange& rRange, float SideSize, float MiddleSize, nuiSize PageSizeMin, nuiWidgetPtr pThumb)
: nuiScrollBar(Orientation, rRange, pThumb),
mPageSizeMin(PageSizeMin),
mUnzoomAfterBound(false)
{
  SetObjectClass(_T("nuiZoomBar")); 

  
  SetThumbSideSize(SideSize);
  SetThumbMiddleSize(MiddleSize);
}

nuiZoomBar::~nuiZoomBar()
{
  
}


bool nuiZoomBar::Draw(nuiDrawContext* pContext)
{
  nuiScrollBar::Draw(pContext);
  
  pContext->DrawLine(mThumbRect.Left() + mThumbSideSize, mThumbRect.mBottom, mThumbRect.Left() + mThumbSideSize, mThumbRect.mTop);
  pContext->DrawLine(mThumbRect.Right() - mThumbSideSize, mThumbRect.mBottom, mThumbRect.Right() - mThumbSideSize, mThumbRect.mTop);

  
  return true;
}


bool nuiZoomBar::MouseClicked(const nglMouseInfo& rInfo)
{
  mClickX = rInfo.X;
  mClickY = rInfo.Y;
  
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mClicked = true;
    Invalidate();
    
    mThumbClicked = false;
    mPageUpClicked = false;
    mPageDownClicked = false;
    if (mOrientation == nuiHorizontal)
    {
      if (rInfo.X < mThumbRect.Left()) // PageUp
      {
        mTimer.Start(true,true);
        mPageUpClicked = true;
      }
      else if (rInfo.X > mThumbRect.Right()) // PageDown
      {
        mTimer.Start(true,true);
        mPageDownClicked = true;
      }
      else if (rInfo.X < mThumbRect.Left() + mThumbSideSize ) // Change Left
      {
        mClickValue = mpRange->GetValue();
        mLeftSideClicked = true;
      }
      else if ( rInfo.X > mThumbRect.Right() - mThumbSideSize) // Change Right
      {
        mClickValue = mpRange->GetValue();
        mRightSideClicked = true;
      }
      else
      {
        mClickValue = mpRange->GetValue();
        mThumbClicked = true;
      }
    }
    else
    {
      if (rInfo.Y < mThumbRect.Top()) // PageUp
      {
        mTimer.Start(true,true);
        mPageUpClicked = true;
      }
      else if (rInfo.Y > mThumbRect.Bottom()) // PageDown
      {
        mTimer.Start(true,true);
        mPageDownClicked = true;
      }
      else if (rInfo.Y < mThumbRect.Top() + mThumbSideSize ) // Change Top
      {
        mClickValue = mpRange->GetValue();
        mRightSideClicked = true;
      }
      else if ( rInfo.Y > mThumbRect.Bottom() - mThumbSideSize) // Change Bottom
      {
        mClickValue = mpRange->GetValue();
        mLeftSideClicked = true;
      }
      else
      {
        mClickValue = mpRange->GetValue();
        mThumbClicked = true;
        ThumbPressed();
      }
    }
    return true;
  }
  else if (rInfo.Buttons & nglMouseInfo::ButtonWheelUp)
  {
    mpRange->Decrement();
    return true;
  }
  else if (rInfo.Buttons & nglMouseInfo::ButtonWheelDown)
  {
    mpRange->Increment();
    return true;
  }
  return false;
}


bool nuiZoomBar::MouseUnclicked(const nglMouseInfo& rInfo)
{
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mClicked = false;
    
    if (mPageUpClicked || mPageDownClicked)
    {      
      mTimer.Stop();
    }
    else
    {
      ThumbDepressed();
    }
    
    mThumbClicked = false;
    mPageUpClicked = false;
    mPageDownClicked = false;
    mRightSideClicked = false;
    mLeftSideClicked = false;
    
    Invalidate();
    return true;
  }
  return false;
  
}


bool nuiZoomBar::MouseMoved(const nglMouseInfo& rInfo)
{
  if (!mThumbClicked && !mLeftSideClicked && !mRightSideClicked)
  {
    if (mOrientation == nuiHorizontal)
    {
      ChangeZoomCursor(rInfo.X, mThumbRect.Left(), mThumbRect.Right());
    }
    else if (mOrientation == nuiVertical)
    {
      ChangeZoomCursor(rInfo.Y, mThumbRect.Bottom(), mThumbRect.Top());
    }    
  }  
  
  
  
  nuiSize x,y;
  x = rInfo.X - mClickX;
  y = rInfo.Y - mClickY;
  
  nuiSize length = mpRange->GetRange();
  nuiSize start=(mClickValue/length);
  nuiSize movement;
  
  if (mThumbClicked)
  {
    if (mOrientation == nuiHorizontal)
    {
      movement = x;
      start +=movement/mRect.GetWidth();
    }
    else
    {
      movement = y;
      start +=movement/mRect.GetHeight();
    }
    mpRange->SetValue(start*length);
    return true;
  }
  else if (mLeftSideClicked)
  {
    if (mOrientation == nuiHorizontal)
    {
      UpdateDownBound(x);
      
      mClickX = rInfo.X;
      mClickY = rInfo.Y;
      return true;
    }
    
    if (mOrientation == nuiVertical)
    {
      UpdateDownBound(y);
      
      mClickX = rInfo.X;
      mClickY = rInfo.Y;
      return true;
    }
  }
  else if (mRightSideClicked)
  {
    if (mOrientation == nuiHorizontal)
    {
      UpdateUpBound(x);
      
      mClickX = rInfo.X;
      mClickY = rInfo.Y;
      return true;
    }
    
    if (mOrientation == nuiVertical)
    {
      UpdateUpBound(y);
      
      mClickX = rInfo.X;
      mClickY = rInfo.Y;
      return true;
    }
  }
  
  return true;  
}


void nuiZoomBar::UpdateUpBound(nuiSize move)
{
  nuiSize length = mpRange->GetRange();
  nuiSize pageSize = mpRange->GetPageSize()/length;
  
  pageSize += move/mRect.GetWidth();
  
  if (pageSize < 0.0)
    pageSize = 0.0;
  if (pageSize > 1.0)
    pageSize = 1.0;
  
  double Page = pageSize*length;
  double Value = mpRange->GetValue();
  
  if (Value + Page > mpRange->GetMaximum())
  {
    if (IsUnZoomAfterBoundEnabled())
    {
      Value -= (Value + Page - mpRange->GetMaximum());
    }
    else
    {
      Page = mpRange->GetMaximum() - Value;
    }
  }
  
  if (Page <= mPageSizeMin)
  {
    Page = mPageSizeMin;
  }
  
  mpRange->SetValueAndSize(Value, Page);
}

void nuiZoomBar::UpdateDownBound(nuiSize move)
{
  nuiSize prevEnd = mpRange->GetValue() + mpRange->GetPageSize();
    
  nuiSize length = mpRange->GetRange();
  nuiSize start=(mClickValue/length);
  
  start += move/mRect.GetWidth();
  
  double Value = start * length;
  nuiSize Page = prevEnd - Value;
  
  nuiSize min = mpRange->GetMinimum();
  if (Value < min)
  {
    if (IsUnZoomAfterBoundEnabled())
    {
      Page += min - Value;
      Value = min;
      
    }
    else
    {
      Value = min;
      Page = prevEnd - min;
    }
  }
  
  mClickValue = Value;
  
  if (Page < 0.0)
    Page = 0.0;
  if (Page > mpRange->GetRange())
    Page = mpRange->GetRange();

  if (Page <= mPageSizeMin)
  {
    Page = mPageSizeMin;
    Value = prevEnd - Page;
  }
  
  mpRange->SetValueAndSize(Value, Page);
}


nuiSize nuiZoomBar::GetPageSizeMin() const
{
  return mPageSizeMin;
}

void nuiZoomBar::SetPageSizeMin(nuiSize min)
{
  mPageSizeMin = MAX(min, 0.0);
}

bool nuiZoomBar::IsUnZoomAfterBoundEnabled()
{
  return mUnzoomAfterBound;
}

void nuiZoomBar::EnableUnzoomAfterBound(bool set)
{
  mUnzoomAfterBound = set;
}


void nuiZoomBar::ChangeZoomCursor(nuiSize coord, nuiSize DownLimit, nuiSize UpLimit)
{
  if (coord > DownLimit + mThumbSideSize && coord < UpLimit - mThumbSideSize) // Hover Center
  {
    SetMouseCursor(eCursorHand);
  }
  
  else if (coord >= DownLimit && coord < DownLimit + mThumbSideSize ) //Hover Left Handler
  {
    if (mpRange->GetPageSize() == mPageSizeMin)
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeW);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeS);
    }
    else if (mpRange->GetValue() == mpRange->GetMinimum() && mpRange->GetPageSize() == mpRange->GetRange())
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeE);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeN);
    }
    else if (mpRange->GetValue() == mpRange->GetMinimum() && !IsUnZoomAfterBoundEnabled())
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeE);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeN);
    }
    else
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeWE);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeNS);
    }
  
  }
  else if (coord <= UpLimit && coord > UpLimit - mThumbSideSize) //Hover Right Handler
  {
    if (mpRange->GetPageSize() == mPageSizeMin)
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeE);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeN);
    }
    else if (mpRange->GetValue() == mpRange->GetMinimum() && mpRange->GetPageSize() == mpRange->GetRange())
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeW);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeS);
    }
    else if (mpRange->GetValue() + mpRange->GetPageSize() == mpRange->GetMaximum() && !IsUnZoomAfterBoundEnabled())
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeW);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeS);
    }
    else
    {
      if (mOrientation == nuiHorizontal)
        SetMouseCursor(eCursorResizeWE);
      else if (mOrientation == nuiVertical)
        SetMouseCursor(eCursorResizeNS);
    }
  }
    else
    {
      SetMouseCursor(eCursorArrow);
    }
  
}
