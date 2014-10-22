//
//  nuiLayoutManager.h
//  nui3
//
//  Created by Sébastien Métrot on 18/10/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#pragma once

#include "nui.h"

#define LAYOUT_ANIM_NAME "LAYOUT_ANIM"


//class nuiLayoutBase : nuiObject
nuiLayoutBase::nuiLayoutBase()
{
  
}

nuiLayoutBase::nuiLayoutBase(const nglString& rObjectName)
: nuiObject(rObjectName)
{
  Init();
}

void nuiLayoutBase::Init()
{
  mpParentLayout = NULL;
  mPosition = nuiFill;
  mFillRule = nuiFill;
  mCSSPasses = 0;
  mInSetRect = false;
  mpLayoutAnimation = NULL;
  mFixedAspectRatio = false;
  mOverrideVisibleRect = false;
  mAutoUpdateLayout = false;

  mCanRespectConstraint = false; ///< By default the widgets don't care about the constraints imposed by their parents. Only few ones care about this.

  mAnimateLayout = false;
  mNeedSelfLayout = true;
  mNeedLayout = true;
  mNeedIdealRect = true;
  mHasUserPos = false;
  mHasUserSize = false;
  mHasUserWidth = false;
  mHasUserHeight = false;
  mForceIdealSize = false;

  mMinIdealWidth = mMaxIdealWidth = mMinIdealHeight = mMaxIdealHeight = -1.0f;
  mMinWidth = mMaxWidth = mMinHeight = mMaxHeight = -1.0f;
  
  mBorderRight = mBorderLeft = 0.f;
  mBorderTop = mBorderBottom = 0.f;
  
  mVisible = true;

  NUI_ADD_EVENT(Shown);
  NUI_ADD_EVENT(Hiden);
  NUI_ADD_EVENT(VisibilityChanged);
  NUI_ADD_EVENT(UserRectChanged);
  NUI_ADD_EVENT(HotRectChanged);

  

//  mLayoutRect;
//  mLayoutRectFromParent;
//  mVisibleRect;
//  mIdealRect;
//  mUserRect;
//  mHotRect;
//  mConstraint;
//  
//  mpMatrixNodes;
//  
//  mAnimateLayout;
//  mVisible;
//  mHasUserPos;
//  mHasUserSize;
//  mHasUserWidth;
//  mHasUserHeight;
//  mForceIdealSize;
//  mNeedSelfLayout;
//  mNeedLayout;
//  mNeedIdealRect;
//  mCanRespectConstraint;
//  mInSetRect;
//  mFixedAspectRatio;
//  mOverrideVisibleRect;
//  mAutoUpdateLayout;
//  
//   mMaxIdealWidth;
//   mMaxIdealHeight;
//   mMinIdealWidth;
//   mMinIdealHeight;
//  
//   mMaxWidth;
//   mMaxHeight;
//   mMinWidth;
//   mMinHeight;
//  
//  mPosition;
//  mFillRule;
//  mpLayoutAnimation;
//  
//  mCSSPasses;

  if (SetObjectClass("nuiLayoutBase"))
  {
    InitAttributes();
  }
}

bool nuiLayoutBase::SetObjectClass(const nglString& rName)
{
  CheckValid();
  
  bool res = nuiObject::SetObjectClass(rName);
  ResetCSSPass();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_ALL);
  return res;
}

// virtual from nuiObject
void nuiLayoutBase::SetObjectName(const nglString& rName)
{
  CheckValid();
  
  nuiObject::SetObjectName(rName);
  ResetCSSPass();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_ALL);
}



bool nuiLayoutBase::AttrIsVisible()
{
  return IsVisible();
}

void nuiLayoutBase::InitAttributes()
{
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("Visible"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiLayoutBase::AttrIsVisible),
                nuiMakeDelegate(this, &nuiLayoutBase::SetVisible)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("LayoutRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetLayoutRect),
                nuiMakeDelegate(this, &nuiLayoutBase::SetLayout)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("LayoutRectUnsafe"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetLayoutRect),
                nuiAttribute<const nuiRect&>::SetterDelegate(this, &nuiLayoutBase::InternalSetLayout)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("UserRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetUserRect),
                nuiMakeDelegate(this, &nuiLayoutBase::SetUserRect)));
  
  AddAttribute(new nuiAttribute<nuiPoint>
               (nglString("UserPos"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetUserPos),
                nuiAttribute<nuiPoint>::SetterDelegate(this, &nuiLayoutBase::SetUserPos)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("VisibleRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetVisibleRect),
                nuiMakeDelegate(this, &nuiLayoutBase::SetVisibleRect)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("UserWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetUserWidth),
                nuiMakeDelegate(this, &nuiLayoutBase::SetUserWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("UserHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetUserHeight),
                nuiMakeDelegate(this, &nuiLayoutBase::SetUserHeight)));
  
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinIdealWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMinIdealWidth),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMinIdealWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinIdealHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMinIdealHeight),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMinIdealHeight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxIdealWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMaxIdealWidth),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMaxIdealWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxIdealHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMaxIdealHeight),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMaxIdealHeight)));
  
  
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMinWidth),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMinWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMinHeight),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMinHeight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMaxWidth),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMaxWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetMaxHeight),
                nuiMakeDelegate(this, &nuiLayoutBase::SetMaxHeight)));
  
  
  // nuiAttribute<nuiSize> <=> nuiAttribute<double>
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("Borders"), nuiUnitSize,
                //nuiMakeDelegate(this, &nuiLayoutBase::GetBorderLeft),
                nuiMakeDelegate(this, &nuiLayoutBase::SetBorders)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderLeft"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetBorderLeft),
                nuiMakeDelegate(this, &nuiLayoutBase::SetBorderLeft)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderTop"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetBorderTop),
                nuiMakeDelegate(this, &nuiLayoutBase::SetBorderTop)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderRight"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetBorderRight),
                nuiMakeDelegate(this, &nuiLayoutBase::SetBorderRight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderBottom"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetBorderBottom),
                nuiMakeDelegate(this, &nuiLayoutBase::SetBorderBottom)));
  
  // nuiAttribute<nuiSize> <=> nuiAttribute<double>
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderLeft"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetActualBorderLeft)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderTop"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetActualBorderTop)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderRight"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetActualBorderRight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderBottom"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetActualBorderBottom)));
  
  nuiAttribute<nuiPosition>* AttributePosition = new nuiAttribute<nuiPosition>
  (nglString("Position"), nuiUnitPosition,
   nuiMakeDelegate(this, &nuiLayoutBase::GetPosition),
   nuiMakeDelegate(this, &nuiLayoutBase::SetPosition));
  AddAttribute("Position", AttributePosition);
  
  nuiAttribute<nuiPosition>* AttributeFillRule = new nuiAttribute<nuiPosition>
  (nglString("FillRule"), nuiUnitPosition,
   nuiMakeDelegate(this, &nuiLayoutBase::GetFillRule),
   nuiMakeDelegate(this, &nuiLayoutBase::SetFillRule));
  AddAttribute("FillRule", AttributeFillRule);
  
  AddAttribute(new nuiAttribute<float>
               (nglString("LayoutAnimationDuration"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiLayoutBase::GetLayoutAnimationDuration),
                nuiMakeDelegate(this, &nuiLayoutBase::SetLayoutAnimationDuration)
                ));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("FixedAspectRatio"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiLayoutBase::GetFixedAspectRatio),
                nuiMakeDelegate(this, &nuiLayoutBase::SetFixedAspectRatio)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("AutoUpdateLayout"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiLayoutBase::GetAutoUpdateLayout),
                nuiMakeDelegate(this, &nuiLayoutBase::SetAutoUpdateLayout)));
  

}

void nuiLayoutBase::Built()
{
  // ...
}

const nuiRect& nuiLayoutBase::GetLayoutRect() const
{
  CheckValid();
  return mLayoutRect;
}

nuiRect nuiLayoutBase::GetBorderedLayoutRect() const
{
  CheckValid();
  return GetBorderedLayoutRect(GetLayoutRect());
}

nuiRect nuiLayoutBase::GetBorderedLayoutRect(const nuiRect& rRect) const
{
  CheckValid();
  nuiRect rect = rRect;
  rect.Bottom() += GetActualBorderBottom();
  rect.Top() -= GetActualBorderTop();
  rect.Left() -= GetActualBorderLeft();
  rect.Right() += GetActualBorderRight();
  return rect;
}

nuiRect nuiLayoutBase::GetBorderLessLayoutRect(const nuiRect& rRect) const
{
  CheckValid();
  nuiRect rect = rRect;
  rect.Bottom() -= GetActualBorderBottom();
  rect.Top() += GetActualBorderTop();
  rect.Left() += GetActualBorderLeft();
  rect.Right() -= GetActualBorderRight();
  return rect;
}


void nuiLayoutBase::SilentInvalidateLayout()
{
  CheckValid();
#ifdef _DEBUG_LAYOUT
  if (GetDebug())
  {
    NGL_OUT("  nuiLayoutBase::SilentInvalidateLayout '%s' [%s]\n", GetObjectClass().GetChars(), GetObjectName().GetChars());
  }
#endif
  mNeedSelfLayout = true;
  mNeedLayout = true;
  mNeedIdealRect = true;
  DebugRefreshInfo();
}

void nuiLayoutBase::InvalidateLayout()
{
  CheckValid();

  if (mAutoUpdateLayout)
  {
    UpdateLayout();
    //Invalidate();  // UpdateLayout already calls Invalidate when needed
    return;
  }

  if ((!mNeedSelfLayout && HasUserRect()))
  {
    UpdateLayout();
    return;
  }

  bool broadcast = !mNeedLayout;
  SilentInvalidateLayout();

  if (mpParentLayout && broadcast)
  {
    //NGL_OUT("InvalidateLayout + Broadcast from %s\n", GetObjectClass().GetChars());
    mpParentLayout->BroadcastInvalidateLayout(this, false);
  }
  DebugRefreshInfo();
}

void nuiLayoutBase::ForcedInvalidateLayout()
{
  CheckValid();
  bool broadcast = !mNeedLayout;
  SilentInvalidateLayout();

  if (mpParentLayout && broadcast)
  {
    //NGL_OUT("InvalidateLayout + Broadcast from %s\n", GetObjectClass().GetChars());
    mpParentLayout->BroadcastInvalidateLayout(this, false);
  }
  DebugRefreshInfo();
}

void nuiLayoutBase::BroadcastInvalidateLayout(nuiLayoutBase* pSender, bool BroadCastOnly)
{
  CheckValid();

  if ((!mNeedSelfLayout && HasUserSize())) // A child can't change the ideal position of its parent so we can stop broadcasting if the parent has a fixed ideal size.
  {
    UpdateLayout();
    return;
  }

  if (!BroadCastOnly)
  {
    mNeedSelfLayout = true;
    mNeedIdealRect = true;
  }

  if (mpParentLayout)
  {
    mpParentLayout->BroadcastInvalidateLayout(pSender, BroadCastOnly);
    //    NGL_OUT("nuiLayoutBase::BroadcastInvalidateLayout %s / %s / 0x%x\n", pSender->GetObjectClass().GetChars(), pSender->GetObjectName().GetChars(), pSender);
  }

#ifdef DEBUG
  if (GetDebug() || pSender->GetDebug())
  {
    //NGL_OUT("nuiLayoutBase::BroadcastInvalidateLayout SKIP %s / %s / 0x%x\n", pSender->GetObjectClass().GetChars(), pSender->GetObjectName().GetChars(), pSender);
  }
#endif

  mNeedLayout = true;

  DebugRefreshInfo();
}

bool nuiLayoutBase::HasLayoutChanged() const
{
  CheckValid();
  return mNeedSelfLayout;
}

void nuiLayoutBase::SetPosition(nuiPosition mode)
{
  CheckValid();
  if (mPosition == mode)
    return;

  mPosition = mode;
  InvalidateLayout();
}

nuiPosition nuiLayoutBase::GetPosition() const
{
  CheckValid();
  return mPosition;
}

void nuiLayoutBase::SetFillRule(nuiPosition mode)
{
  CheckValid();
  if (mFillRule == mode)
    return;
  mFillRule = mode;

  if (mPosition == nuiFill)
    InvalidateLayout();
}

nuiPosition nuiLayoutBase::GetFillRule() const
{
  CheckValid();
  return mFillRule;
}


void nuiLayoutBase::SilentSetVisible(bool Visible)
{
  CheckValid();
  if (mVisible == Visible)
    return;

  mVisible = Visible;
}


bool nuiLayoutBase::IsVisible(bool combined) const
{
  CheckValid();
  if (!combined || !mpParentLayout)
    return mVisible;
  if (mVisible)
    return mpParentLayout->IsVisible(true);
  return mVisible;
}

void nuiLayoutBase::StartTransition()
{
  CheckValid();
  mInTransition++;
}

void nuiLayoutBase::StopTransition()
{
  CheckValid();
  mInTransition--;
}

bool nuiLayoutBase::IsInTransition() const
{
  CheckValid();
  return mInTransition != 0;
}


// Layout stuff
const nuiRect& nuiLayoutBase::GetIdealRect()
{
  CheckValid();
  if (mNeedIdealRect)
  {
    mIdealRect = CalcIdealSize();

    if (mHasUserSize && mHasUserPos)
      mIdealRect = mUserRect;
    else
    {
      if (mHasUserWidth)
        mIdealRect.SetWidth(mUserRect.GetWidth());
      if (mHasUserHeight)
        mIdealRect.SetHeight(mUserRect.GetHeight());
    }

    if (mMaxIdealWidth > 0.0f)
      mIdealRect.Right() = MIN(mMaxIdealWidth, mIdealRect.GetWidth());
    if (mMaxIdealHeight > 0.0f)
      mIdealRect.Bottom() = MIN(mMaxIdealHeight, mIdealRect.GetHeight());
    if (mMinIdealWidth > 0.0f)
      mIdealRect.Right() = MAX(mMinIdealWidth, mIdealRect.GetWidth());
    if (mMinIdealHeight > 0.0f)
      mIdealRect.Bottom() = MAX(mMinIdealHeight, mIdealRect.GetHeight());

    if (mHasUserPos && !mHasUserSize)
      mIdealRect.MoveTo(mUserRect.mLeft, mUserRect.mTop);


    mIdealRect.Bottom() += GetActualBorderBottom();
    mIdealRect.Top() -= GetActualBorderTop();
    mIdealRect.Left() -= GetActualBorderLeft();
    mIdealRect.Right() += GetActualBorderRight();
    if (!HasUserPos())
      mIdealRect.MoveTo(0, 0);
    mIdealRect.RoundToBiggest();

  }

#ifdef NUI_LOG_GETIDEALRECT
  nglString _log;
  uint32 parentcount = GetParentCount(this);
  _log .Add("|  ", parentcount)
  .Add(mNeedIdealRect?">>> ":"")
  .Add(GetObjectClass())
  .Add(" / ")
  .Add(GetObjectName())
  .Add(" [GetIdealRect] => ")
  .Add(mIdealRect.GetValue())
  .AddNewLine();

  NGL_OUT(_log.GetChars());
#endif //NUI_LOG_GETIDEALRECT

  mNeedIdealRect = false;
  return mIdealRect;
}

bool nuiLayoutBase::IsInSetRect() const
{
  if (mInSetRect)
    return true;
  return mpParentLayout ? mpParentLayout->IsInSetRect() : false;
}

void nuiLayoutBase::SetBorders(nuiSize XY)
{
  CheckValid();
  SetBorder(XY, XY);
}

void nuiLayoutBase::SetBorder(nuiSize X, nuiSize Y)
{
  CheckValid();
  if (mBorderLeft == X && mBorderRight == X && mBorderTop == Y && mBorderBottom == Y)
    return;

  mBorderLeft = mBorderRight = X;
  mBorderTop = mBorderBottom = Y;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::SetBorderLeft(nuiSize border)
{
  CheckValid();
  if (mBorderLeft == border)
    return;

  mBorderLeft = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::SetBorderTop(nuiSize border)
{
  CheckValid();
  if (mBorderTop == border)
    return;

  mBorderTop = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::SetBorderRight(nuiSize border)
{
  CheckValid();
  if (mBorderRight == border)
    return;

  mBorderRight = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::SetBorderBottom(nuiSize border)
{
  CheckValid();
  if (mBorderBottom == border)
    return;

  mBorderBottom = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiLayoutBase::GetBorderLeft() const
{
  CheckValid();
  return mBorderLeft;
}

nuiSize nuiLayoutBase::GetBorderTop() const
{
  CheckValid();
  return mBorderTop;
}

nuiSize nuiLayoutBase::GetBorderRight() const
{
  CheckValid();
  return mBorderRight;
}

nuiSize nuiLayoutBase::GetBorderBottom() const
{
  CheckValid();
  return mBorderBottom;
}

nuiSize nuiLayoutBase::GetActualBorderLeft() const
{
  CheckValid();
  return GetBorderLeft();
}

nuiSize nuiLayoutBase::GetActualBorderTop() const
{
  CheckValid();
  return GetBorderTop();
}

nuiSize nuiLayoutBase::GetActualBorderRight() const
{
  CheckValid();
  return GetBorderRight();
}

nuiSize nuiLayoutBase::GetActualBorderBottom() const
{
  CheckValid();
  return GetBorderBottom();
}

void nuiLayoutBase::InternalSetLayout(const nuiRect& rect)
{
  CheckValid();
  bool PositionChanged = (rect.Left() != mLayoutRect.Left()) || (rect.Top() != mLayoutRect.Top());
  bool SizeChanged = !rect.Size().IsEqual(mLayoutRect.Size());
  mNeedSelfLayout = mNeedSelfLayout || SizeChanged;

  InternalSetLayout(rect, PositionChanged, SizeChanged);

  mNeedSelfLayout = false;
  mNeedLayout = false;
  DebugRefreshInfo();
}

void nuiLayoutBase::SetUserWidth(nuiSize s)
{
  CheckValid();
  if (mHasUserWidth && GetUserWidth() == s)
    return;

  mUserRect.SetWidth(s);
  mHasUserWidth = true;
  UserRectChanged();
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiLayoutBase::GetUserWidth()
{
  CheckValid();
  return mUserRect.GetWidth();
}

void nuiLayoutBase::SetUserHeight(nuiSize s)
{
  CheckValid();
  if (mHasUserHeight && GetUserHeight() == s)
    return;

  mUserRect.SetHeight(s);
  mHasUserHeight = true;
  UserRectChanged();
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiLayoutBase::GetUserHeight()
{
  CheckValid();
  return mUserRect.GetHeight();
}


// User size
void nuiLayoutBase::SetUserRect(const nuiRect& rRect)
{
  CheckValid();
  if (!(mUserRect == rRect) || !mHasUserPos || !mHasUserSize)
  {
    mUserRect = rRect;
    bool SizeChanged = !mUserRect.Size().IsEqual(mLayoutRect.Size());
    bool optim = HasUserRect() && !SizeChanged;
    mHasUserSize = true;
    mHasUserPos = true;
    mHasUserWidth = true;
    mHasUserHeight = true;
    UserRectChanged();

    mIdealRect = mUserRect;

    if (optim)
    {
      if (!IsInSetRect())
      {
        mInSetRect = true;
        SetRect(rRect);
        mInSetRect = false;
      }
    }
    else
    {
      ForcedInvalidateLayout();
    }
    DebugRefreshInfo();
  }
}

const nuiRect& nuiLayoutBase::GetUserRect() const
{
  CheckValid();
  return mUserRect;
}

nuiPoint nuiLayoutBase::GetUserPos() const
{
  CheckValid();
  return nuiPoint(mUserRect.Left(), mUserRect.Top());
}

void nuiLayoutBase::UnsetUserRect()
{
  CheckValid();
  if (mHasUserPos || mHasUserSize)
  {
    mHasUserSize = false;
    mHasUserPos = false;
    mHasUserWidth = false;
    mHasUserHeight = false;
    InvalidateLayout();
    DebugRefreshInfo();
  }
}

void nuiLayoutBase::SetUserSize(nuiSize X,nuiSize Y)
{
  CheckValid();
  if (mHasUserSize && GetUserWidth() == X && GetUserHeight() == Y)
    return;

  mUserRect.SetSize(X,Y);
  mHasUserSize = true;
  mHasUserWidth = true;
  mHasUserHeight = true;
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::UnsetUserSize()
{
  CheckValid();
  if (mHasUserSize)
  {
    mHasUserSize = false;
    mHasUserWidth = false;
    mHasUserHeight =false;
    InvalidateLayout();
    DebugRefreshInfo();
  }
}

void nuiLayoutBase::SetUserPos(nuiSize X, nuiSize Y)
{
  CheckValid();
  if (mHasUserPos && mUserRect.Left() == X && mUserRect.Top() == Y)
    return;

  mUserRect.MoveTo(X,Y);
  mHasUserPos = true;
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::SetUserPos(nuiPoint Pos)
{
  CheckValid();
  SetUserPos(Pos[0], Pos[1]);
}

void nuiLayoutBase::UnsetUserPos()
{
  CheckValid();
  mHasUserPos = false;
  InvalidateLayout();
  DebugRefreshInfo();
}

bool nuiLayoutBase::HasUserRect() const
{
  CheckValid();
  return mHasUserSize && mHasUserPos;
}

bool nuiLayoutBase::HasUserSize() const
{
  CheckValid();
  return mHasUserSize;
}

bool nuiLayoutBase::HasUserWidth() const
{
  CheckValid();
  return mHasUserWidth;
}

bool nuiLayoutBase::HasUserHeight() const
{
  CheckValid();
  return mHasUserHeight;
}



bool nuiLayoutBase::HasUserPos() const
{
  CheckValid();
  return mHasUserPos;
}

void nuiLayoutBase::ForceIdealRect(bool Force)
{
  CheckValid();
  if (mForceIdealSize == Force)
    return;

  mForceIdealSize = Force;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::GetHotRect(nuiRect& rRect) const
{
  CheckValid();
  rRect = mHotRect;
}

const nuiRect& nuiLayoutBase::GetHotRect() const
{
  CheckValid();
  return mHotRect;
}

void nuiLayoutBase::SetHotRect(const nuiRect& rRect)
{
  CheckValid();
  mHotRect = rRect;
  HotRectChanged();
  if (mpParentLayout)
  {
    nuiRect r(rRect);
    LocalToLocal(mpParentLayout, r);
    mpParentLayout->OnChildHotRectChanged(this, r);
  }
  DebugRefreshInfo();
}

void nuiLayoutBase::SetBorder(nuiSize Left, nuiSize Right, nuiSize Top, nuiSize Bottom)
{
  CheckValid();
  if (mBorderLeft == Left && mBorderRight == Right && mBorderTop == Top && mBorderBottom == Bottom)
    return;

  mBorderLeft = Left;
  mBorderRight = Right;
  mBorderTop = Top;
  mBorderBottom = Bottom;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiLayoutBase::GetBorder(nuiSize& rLeft, nuiSize& rRight, nuiSize& rTop, nuiSize& rBottom)
{
  CheckValid();
  rLeft = mBorderLeft;
  rRight = mBorderRight;
  rTop = mBorderTop;
  rBottom = mBorderBottom;
}

void nuiLayoutBase::SetVisibleRect(const nuiRect& rRect)
{
  CheckValid();
  nuiRect temp = mVisibleRect;
  if (mVisibleRect == rRect)
    return;

  mVisibleRect = rRect;
  mOverrideVisibleRect = true;
  Invalidate();
}

void nuiLayoutBase::SilentSetVisibleRect(const nuiRect& rRect)
{
  CheckValid();
  if (mVisibleRect == rRect)
    return;

  mVisibleRect = rRect;
  mOverrideVisibleRect = true;
  SilentInvalidate();
}

void nuiLayoutBase::ResetVisibleRect()
{
  if (!mOverrideVisibleRect)
    return;
  mOverrideVisibleRect = false;
  InvalidateLayout();
}

const nuiRect& nuiLayoutBase::GetVisibleRect() const
{
  CheckValid();
  return mVisibleRect;
}

void nuiLayoutBase::SetFixedAspectRatio(bool set)
{
  mFixedAspectRatio = set;
  InvalidateLayout();
}

bool nuiLayoutBase::GetFixedAspectRatio() const
{
  return mFixedAspectRatio;
}

nuiRect nuiLayoutBase::GetLayoutForRect(const nuiRect& rRect)
{
  CheckValid();
  nuiRect rect(GetIdealRect());
  nuiRect r(rRect);

  if (mMaxWidth >= 0)
    rect.SetWidth(MIN(r.GetWidth(), mMaxWidth));

  if (mMaxHeight >= 0)
    rect.SetHeight(MIN(r.GetHeight(), mMaxHeight));

  if (mMinWidth >= 0)
    rect.SetWidth(MAX(r.GetWidth(), mMinWidth));

  if (mMinHeight >= 0)
    rect.SetHeight(MAX(r.GetHeight(), mMinHeight));

  // Prevent the widget from being bigger than the size provided by its parent:
  float w = r.GetWidth();
  float ww = rect.GetWidth();
  float h = r.GetHeight();
  float hh = rect.GetHeight();

  if (ww > w)
  {
    ww = w;
  }
  if (hh > h)
  {
    hh = h;
  }

  if (GetFixedAspectRatio())
  {
    // Give good ratio to keep things in proportions
    float tw = mIdealRect.GetWidth();
    float th = mIdealRect.GetHeight();
    float r = 1.0f;

    if (hh < th)
    {
      r = hh / th;
      ww = tw * r;
    }

    if (ww < tw)
    {
      r = ww / tw;
      hh = th * r;
    }
  }

  rect.SetWidth(ww);
  rect.SetHeight(hh);

  if (mPosition == nuiNoPosition)
  {
    rect.Move(r.Left(), r.Top());
  }
  else if (mPosition != nuiFill)
  {
    rect.SetPosition(mPosition, r);
  }
  else
  {
    if (mFillRule == nuiFill)
    {
      rect.SetPosition(mPosition, r);
    }
    else if (mFillRule == nuiTile)
    {
      rect = r;
    }
    else
    {
      nuiRect rct = r;
      float ratio,rratio,rw,rh;
      ratio  = (float)rct.GetWidth()  / (float)rct.GetHeight();
      rratio = (float)rect.GetWidth() / (float)rect.GetHeight();

      if (ratio < rratio)
      {
        rw = (float)rct.GetWidth();
        rh = rw / rratio;
      }

      else
      {
        rh = (float)rct.GetHeight();
        rw = rratio * rh;
      }

      rect = nuiRect(0.0f, 0.0f, rw, rh);
      rect.SetPosition(mFillRule, r);
    }
  }

  rect.Left()   += GetActualBorderLeft();
  rect.Right()  -= GetActualBorderRight();
  rect.Top()    += GetActualBorderTop();
  rect.Bottom() -= GetActualBorderBottom();

  rect.RoundToNearest();

  return rect;
}

void nuiLayoutBase::SetLayout(const nuiRect& rRect)
{
  if (IsInTransition())
    return;

  mLayoutRectFromParent = rRect;

  CheckValid();
  nuiRect rect(GetLayoutForRect(rRect));

  if (GetLayoutAnimationDuration() > 0)
  {
    nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(true);
    bool value_already_set = pAnim->GetEndValue() == rect;

    if (pAnim->IsPlaying() && !value_already_set)
    {
      pAnim->Stop();
    }

    if (!(pAnim->IsPlaying() && value_already_set))
    {
      pAnim->SetEndValue(rect);
      nuiDelayedPlayAnim(pAnim, eAnimFromStart, 0.0, 1, eAnimLoopForward);
    }
  }
  else
  {
    InternalSetLayout(rect);
  }
}

nuiRectAttributeAnimation* nuiLayoutBase::GetLayoutAnimation(bool CreateIfNotAvailable)
{
  CheckValid();
  if (mpLayoutAnimation)
    return mpLayoutAnimation;

  nuiRectAttributeAnimation* pAnim = (nuiRectAttributeAnimation*)GetAnimation(LAYOUT_ANIM_NAME);
  if (!pAnim && CreateIfNotAvailable)
  {
    pAnim = new nuiRectAttributeAnimation();
    pAnim->SetDuration(0);
    pAnim->SetTargetObject(this);
    pAnim->SetTargetAttribute("LayoutRectUnsafe");
    pAnim->SetCaptureStartOnPlay(true);
    AddAnimation(LAYOUT_ANIM_NAME, pAnim);
  }
  mpLayoutAnimation = pAnim;
  return pAnim;
}

void nuiLayoutBase::SetLayoutAnimationDuration(float duration)
{
  CheckValid();
  nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(duration > 0);
  if (pAnim)
    pAnim->SetDuration(duration);
}

float nuiLayoutBase::GetLayoutAnimationDuration()
{
  CheckValid();
  nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(false);
  if (pAnim)
    return pAnim->GetDuration();
  return 0;
}

void nuiLayoutBase::SetLayoutAnimationEasing(const nuiEasingMethod& rMethod)
{
  CheckValid();
  nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(true);
  if (pAnim)
    pAnim->SetEasing(rMethod);
}


bool nuiLayoutBase::SetLayoutConstraint(const nuiLayoutBase::LayoutConstraint& rConstraint)
{
  CheckValid();
  if (!(mConstraint == rConstraint))
  {
    mConstraint = rConstraint;
    mNeedIdealRect = mNeedIdealRect || mCanRespectConstraint;
    //InvalidateLayout();
    return true;
  }
  return false;
}


const nuiLayoutBase::LayoutConstraint& nuiLayoutBase::GetLayoutConstraint() const
{
  CheckValid();
  return mConstraint;
}

void nuiLayoutBase::SetMaxIdealWidth(float MaxWidth)
{
  CheckValid();
  if (mMaxIdealWidth == MaxWidth)
    return;
  mMaxIdealWidth = MaxWidth;
  InvalidateLayout();
}

void nuiLayoutBase::SetMaxIdealHeight(float MaxHeight)
{
  CheckValid();
  if (mMaxIdealHeight == MaxHeight)
    return;
  mMaxIdealHeight = MaxHeight;
  InvalidateLayout();
}

void nuiLayoutBase::SetMinIdealWidth(float MinWidth)
{
  CheckValid();
  if (mMinIdealWidth == MinWidth)
    return;
  mMinIdealWidth = MinWidth;
  InvalidateLayout();
}

void nuiLayoutBase::SetMinIdealHeight(float MinHeight)
{
  CheckValid();
  if (mMinIdealHeight == MinHeight)
    return;
  mMinIdealHeight = MinHeight;
  InvalidateLayout();
}

float nuiLayoutBase::GetMaxIdealWidth() const
{
  CheckValid();
  return mMaxIdealWidth;
}

float nuiLayoutBase::GetMaxIdealHeight() const
{
  CheckValid();
  return mMaxIdealHeight;
}

float nuiLayoutBase::GetMinIdealWidth() const
{
  CheckValid();
  return mMinIdealWidth;
}

float nuiLayoutBase::GetMinIdealHeight() const
{
  CheckValid();
  return mMinIdealHeight;
}


void nuiLayoutBase::SetMaxWidth(float MaxWidth)
{
  CheckValid();
  if (mMaxWidth == MaxWidth)
    return;
  mMaxWidth = MaxWidth;
  InvalidateLayout();
}

void nuiLayoutBase::SetMaxHeight(float MaxHeight)
{
  CheckValid();
  if (mMaxHeight == MaxHeight)
    return;
  mMaxHeight = MaxHeight;
  InvalidateLayout();
}

void nuiLayoutBase::SetMinWidth(float MinWidth)
{
  CheckValid();
  if (mMinWidth == MinWidth)
    return;
  mMinWidth = MinWidth;
  InvalidateLayout();
}

void nuiLayoutBase::SetMinHeight(float MinHeight)
{
  CheckValid();
  if (mMinHeight == MinHeight)
    return;
  mMinHeight = MinHeight;
  InvalidateLayout();
}

float nuiLayoutBase::GetMaxWidth() const
{
  CheckValid();
  return mMaxWidth;
}

float nuiLayoutBase::GetMaxHeight() const
{
  CheckValid();
  return mMaxHeight;
}

float nuiLayoutBase::GetMinWidth() const
{
  CheckValid();
  return mMinWidth;
}

float nuiLayoutBase::GetMinHeight() const
{
  CheckValid();
  return mMinIdealHeight;
}


nuiEventSource* nuiLayoutBase::GetEvent(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiEventSource*, nglString::LessFunctor>::iterator it = mEventMap.find(rName);
  if (it == mEventMap.end())
    return NULL;
  return (*it).second;
}

void nuiLayoutBase::GetEvents(std::vector<nglString>& rNames)
{
  CheckValid();
  std::map<nglString, nuiEventSource*, nglString::LessFunctor>::iterator it;
  std::map<nglString, nuiEventSource*, nglString::LessFunctor>::iterator end = mEventMap.end();

  for (it = mEventMap.begin(); it != end; ++it)
    rNames.push_back((*it).first);
}

void nuiLayoutBase::AddEvent(const nglString& rName, nuiEventSource& rEvent)
{
  CheckValid();
  mEventMap[rName] = &rEvent;
}

bool nuiLayoutBase::AddEventAction(const nglString& rEventName, nuiEventActionHolder* pActions)
{
  nuiEventSource* pEvent = GetEvent(rEventName);
  if (!pEvent)
    return false;

  pActions->Connect(pEvent, this);
  mEventActions.push_back(pActions);
  return true;
}

void nuiLayoutBase::UpdateLayout()
{
  CheckValid();
  if (IsInSetRect())
    return;

  mNeedSelfLayout = true;
  mNeedIdealRect = true;

  GetIdealRect();
  SetLayout(mLayoutRectFromParent);
  Invalidate();
}

void nuiLayoutBase::ApplyCSSForStateChange(uint32 MatchersTag)
{
  CheckValid();
  // Apply CSS, do default stuff, etc...
  if (!IsTrashed(true))
  {
    nuiTopLevel* pTopLevel = GetTopLevel();
    if (pTopLevel)
      pTopLevel->PrepareWidgetCSS(this, true, NUI_WIDGET_MATCHTAG_ALL);
  }
}

void nuiLayoutBase::ResetCSSPass()
{
  CheckValid();
  InternalResetCSSPass();
}

void nuiLayoutBase::IncrementCSSPass()
{
  CheckValid();
  mCSSPasses++;
}

uint32 nuiLayoutBase::GetCSSPass() const
{
  CheckValid();
  return mCSSPasses;
}

void nuiLayoutBase::AutoInvalidateLayout(const nuiEvent& rEvent)
{
  InvalidateLayout();
}

void nuiLayoutBase::SetVisible(bool Visible)
{
  CheckValid();
  if (IsVisible(false) == Visible)
    return;

  if (IsVisible(false) == Visible)
    return;

  nuiAnimation* pHideAnim = GetAnimation("HIDE");
  nuiAnimation* pShowAnim = GetAnimation("SHOW");

  if (Visible)
  {
    // Show
    if (pHideAnim && pHideAnim->IsPlaying())
    {
      // Stop hiding anim
      pHideAnim->Stop();
      // Start Show Anim if there is one
      if (pShowAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //pShowAnim->SetTime(0, eAnimFromStart);
        StartAnimation("SHOW");
        //        pShowAnim->SilentSetTime(0, eAnimFromStart);
        //        pShowAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // otherwise set visible = true
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
    else if (pShowAnim && pShowAnim->IsPlaying())
    {
      //  Do nothing (return)
      NGL_ASSERT(mVisible == true);
    }
    else if (mVisible)
    {
      // Do nothing (return)
      NGL_ASSERT(mVisible == true);
    }
    else // !mVisible
    {
      // Start Show Anim if there is one
      if (pShowAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //pShowAnim->SetTime(0, eAnimFromStart);
        StartAnimation("SHOW");
        //        pShowAnim->SilentSetTime(0, eAnimFromStart);
        //        pShowAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // otherwise set visible = true
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
  }
  else // !Visible
  {
    if (pHideAnim && pHideAnim->IsPlaying())
    {
      // Do nothing
    }
    else if (pShowAnim && pShowAnim->IsPlaying())
    {
      // Stop Showing
      pShowAnim->Stop();
      // Start Hiding anim if there is one
      if (pHideAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //        pHideAnim->SetTime(0, eAnimFromStart);
        StartAnimation("HIDE");
        //        pHideAnim->SilentSetTime(0, eAnimFromStart);
        //        pHideAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // Otherwise set visible = false
      {
        Invalidate();
        mVisible = false;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
    else if (mVisible)
    {
      // Start Hiding anim if there is one
      if (pHideAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //        pHideAnim->SetTime(0, eAnimFromStart);
        StartAnimation("HIDE");
        //pHideAnim->SilentSetTime(0, eAnimFromStart);
        //       pHideAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // Otherwise set visible = false
      {
        Invalidate();
        mVisible = false;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
    else // !mVisible
    {
      // Do nothing
      NGL_ASSERT(mVisible == false);
    }
  }

  if (mVisible)
    BroadcastVisible();
}

void nuiLayoutBase::BroadcastVisible()
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    if (pItem->GetNeedSelfRedraw())
    {
      pItem->InvalidateRect(pItem->GetLayoutRect().Size());
    }
    nuiLayoutBase* pContainer = dynamic_cast<nuiLayoutBase*>(pItem);
    if (pContainer)
    {
      pContainer->BroadcastVisible();
    }
  }
  delete pIt;
}

nuiRect nuiLayoutBase::CalcIdealSize()
{
  CheckValid();
  nuiRect temp;

  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    nuiRect r(pItem->GetIdealRect()); // Dummy call. Only the side effect is important: the object recalculates its layout.
    temp.Union(temp, r.Size());
  }
  delete pIt;

  DebugRefreshInfo();
  return temp.Size();
}

bool nuiLayoutBase::SetSelfRect(const nuiRect& rRect)
{
  CheckValid();
#ifdef _DEBUG_LAYOUT
  if (GetDebug())
    NGL_OUT("nuiLayoutBase::SetRect on '%s' (%f, %f - %f, %f)\n", GetObjectClass().GetChars(), rRect.mLeft, rRect.mTop, rRect.GetWidth(), rRect.GetHeight());
#endif

  bool inval = mNeedInvalidateOnSetRect;
  if (!(mLayoutRect == rRect))
    inval = true;

  if (inval)
    Invalidate();
  if (mForceIdealSize)
    mLayoutRect.Set(rRect.Left(), rRect.Top(), mIdealRect.GetWidth(), mIdealRect.GetHeight());
  else
    mLayoutRect = rRect;

  if (!mOverrideVisibleRect)
    mVisibleRect = GetOverDrawRect(true, true);

  if (inval)
    Invalidate();

  DebugRefreshInfo();
}

bool nuiLayoutBase::SetRect(const nuiRect& rRect)
{
  SetSelfRect(rRect);
  nuiRect rect(rRect.Size());
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    pItem->GetIdealRect();
    pItem->SetLayout(rect);
  }
  delete pIt;

  DebugRefreshInfo();
  return true;
}

void nuiLayoutBase::InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged)
{
  CheckValid();

  if (mNeedSelfLayout || SizeChanged)
  {
    mInSetRect = true;
    SetRect(rect);
    mInSetRect = false;
    Invalidate();
  }
  else
  {
    // Is this case the widget have just been moved inside its parent. No need to re layout it, only change the rect...
    mLayoutRect = rect;

    if (mNeedLayout)
    {
      // The children need to be re layed out (at least one of them!).
      nuiLayoutBase::IteratorPtr pIt = GetFirstChild(false);
      do
      {
        nuiLayoutBase* pItem = pIt->GetWidget();
        if (pItem)
        {
          // The rect of each child doesn't change BUT we still ask for its ideal rect.
          nuiRect rect(pItem->GetBorderedRect());
          nuiRect ideal(pItem->GetIdealRect());

          if (pItem->HasUserPos())
          {
            rect = ideal;
          }
          else if (pItem->HasUserSize())
          {
            rect.SetSize(ideal.GetWidth(), ideal.GetHeight());
          }
          else
          {
            // Set the widget to the size of the parent
          }

          pItem->SetLayout(rect);
        }
      } while (GetNextChild(pIt));
      delete pIt;

    }
  }

  //#TEST:
#ifdef NUI_CHECK_LAYOUTS
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    if (pItem->IsVisible())
    {
      NGL_ASSERT(!pItem->GetNeedLayout());
    }
  }
  delete pIt;
  //#TEST end
#endif
}

void nuiLayoutBase::OnChildHotRectChanged(nuiLayoutBase* pChild, const nuiRect& rChildHotRect)
{
  CheckValid();
  SetHotRect(rChildHotRect);
}

void nuiLayoutBase::InternalResetCSSPass()
{
  CheckValid();
  mCSSPasses = 0;

  IteratorPtr pIt = GetFirstChild(false);
  for (; pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    pItem->ResetCSSPass();
  }
  delete pIt;
}

void nuiLayoutBase::SetChildrenLayoutAnimationDuration(float duration)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationDuration(duration);
  }
  delete pIt;
}

void nuiLayoutBase::SetChildrenLayoutAnimationEasing(const nuiEasingMethod& rMethod)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiLayoutBase* pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationEasing(rMethod);
  }
  delete pIt;
}

///////////////////////////
///
#FIXME En cours de transposition du code de nuiWidget
void nuiLayoutBase::LocalToGlobal(int& x, int& y) const
{
  CheckValid();
  if (!IsMatrixIdentity())
  {
    nuiVector vec((double)x,(double)y, 0);
    vec = GetMatrix() * vec;
    x = ToBelow(vec[0]);
    y = ToBelow(vec[1]);
  }
  
  x += (int)mRect.mLeft;
  y += (int)mRect.mTop;
  
  if (mpParent)
    mpParent->LocalToGlobal(x,y);
}

void nuiLayoutBase::LocalToGlobal(nuiSize& x, nuiSize& y) const
{
  CheckValid();
  if (!IsMatrixIdentity())
  {
    nuiVector vec(x, y, 0);
    vec = GetMatrix() * vec;
    x = vec[0];
    y = vec[1];
  }
  
  x += mRect.mLeft;
  y += mRect.mTop;
  
  if (mpParent)
    mpParent->LocalToGlobal(x,y);
}

void nuiLayoutBase::LocalToGlobal(nuiRect& rRect) const
{
  CheckValid();
  if (!IsMatrixIdentity())
  {
    nuiVector vec1(rRect.mLeft,rRect.mTop,0);
    nuiVector vec2(rRect.mRight,rRect.mBottom,0);
    vec1 = GetMatrix() * vec1;
    vec2 = GetMatrix() * vec2;
    rRect.mLeft   = vec1[0];
    rRect.mTop    = vec1[1];
    rRect.mRight  = vec2[0];
    rRect.mBottom = vec2[1];
  }
  
  rRect.Move(mRect.mLeft, mRect.mTop);
  
  if (mpParent)
  {
    mpParent->LocalToGlobal(rRect);
  }
}

void nuiLayoutBase::GlobalToLocal(int& x, int& y) const
{
  CheckValid();
  if (mpParent)
    mpParent->GlobalToLocal(x,y);
  x -= (int)mRect.mLeft;
  y -= (int)mRect.mTop;
  
  if (!IsMatrixIdentity())
  {
    nuiVector vec((double)x,(double)y,0);
    nuiMatrix mat;
    GetMatrix(mat);
    mat.InvertHomogenous();
    vec = mat * vec;
    x = ToBelow(vec[0]);
    y = ToBelow(vec[1]);
  }
}

void nuiLayoutBase::GlobalToLocal(nuiSize& x, nuiSize& y) const
{
  CheckValid();
  if (mpParent)
    mpParent->GlobalToLocal(x,y);
  x -= mRect.mLeft;
  y -= mRect.mTop;
  
  if (!IsMatrixIdentity())
  {
    nuiVector vec(x,y,0);
    nuiMatrix mat;
    GetMatrix(mat);
    mat.InvertHomogenous();
    vec = mat * vec;
    x = vec[0];
    y = vec[1];
  }
}

void nuiLayoutBase::GlobalToLocal(nuiRect& rRect) const
{
  CheckValid();
  if (mpParent)
  {
    mpParent->GlobalToLocal(rRect);
  }
  rRect.Move(-mRect.mLeft, -mRect.mTop);
  
  if (!IsMatrixIdentity())
  {
    nuiMatrix mat;
    GetMatrix(mat);
    mat.InvertHomogenous();
    nuiVector vec1(rRect.mLeft,rRect.mTop,0);
    nuiVector vec2(rRect.mRight,rRect.mBottom,0);
    vec1 = mat * vec1;
    vec2 = mat * vec2;
    rRect.mLeft   = vec1[0];
    rRect.mTop    = vec1[1];
    rRect.mRight  = vec2[0];
    rRect.mBottom = vec2[1];
  }
}

void nuiLayoutBase::LocalToLocal(nuiLayoutBase* pWidget,int& x, int& y) const
{
  CheckValid();
  if (GetRoot() != pWidget->GetRoot())
    return;
  
  LocalToGlobal(x,y);
  pWidget->GlobalToLocal(x,y);
}

void nuiLayoutBase::LocalToLocal(nuiLayoutBase* pWidget, nuiSize& x, nuiSize& y) const
{
  CheckValid();
  if (GetRoot() != pWidget->GetRoot())
    return;
  
  LocalToGlobal(x,y);
  pWidget->GlobalToLocal(x,y);
}

void nuiLayoutBase::LocalToLocal(nuiLayoutBase* pWidget,nuiRect& rRect) const
{
  CheckValid();
  if (GetRoot() != pWidget->GetRoot())
    return;
  
  LocalToGlobal(rRect);
  pWidget->GlobalToLocal(rRect);
}


///
nuiAnimation* nuiLayoutBase::GetAnimation(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.find(rName);
  
  if (it == mAnimations.end())
    return NULL;
  
  return (*it).second;
}

void nuiLayoutBase::GetAnimations(std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>& rAnims)
{
  CheckValid();
  rAnims = mAnimations;
}

uint32 nuiLayoutBase::GetAnimCount()
{
  CheckValid();
  return (uint32)mAnimations.size();
}

void nuiLayoutBase::AddAnimation(const nglString& rName, nuiAnimation* pAnim, bool TransitionAnimation)
{
  CheckValid();
  nuiAnimation* pOldAnim = GetAnimation(rName);
  if (pOldAnim)
    delete pOldAnim;
  
  mAnimations[rName] = pAnim;
  pAnim->SetDeleteOnStop(false); /// Cancel anim delete on stop or we'll crash as soon as the widget is destroyed or the user starts to play the anim once more.
  if (rName == "TRASH")
    mGenericWidgetSink.Connect(pAnim->AnimStop, &nuiLayoutBase::AutoDestroy);
  if (rName == "HIDE")
    mGenericWidgetSink.Connect(pAnim->AnimStop, &nuiLayoutBase::AutoHide);
  
  if (TransitionAnimation)
  {
    mGenericWidgetSink.Connect(pAnim->AnimStart, &nuiLayoutBase::AutoStartTransition);
    mGenericWidgetSink.Connect(pAnim->AnimStop, &nuiLayoutBase::AutoStopTransition);
  }
  
  DebugRefreshInfo();
}

void nuiLayoutBase::DelAnimation(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.find(rName);
  
  if (it == mAnimations.end())
    return;
  
  mAnimations.erase(it);
  DebugRefreshInfo();
}

void nuiLayoutBase::ClearAnimations()
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.begin();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator end = mAnimations.end();
  
  for ( ; it != end; ++it)
    delete (*it).second;
  
  mAnimations.clear();
  DebugRefreshInfo();
}

void nuiLayoutBase::AnimateAll(bool Set, bool Reset)
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.begin();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator end = mAnimations.end();
  
  for ( ; it != end; ++it)
  {
    if (Reset)
      (*it).second->SetTime(0);
    
    if (Set)
      (*it).second->Play();
    else
      (*it).second->Stop();
  }
  DebugRefreshInfo();
}


void nuiLayoutBase::ResetAnimation(const nglString& rName)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
    pAnim->SetTime(0);
  DebugRefreshInfo();
}

void nuiLayoutBase::StartAnimation(const nglString& rName, double Time)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
  {
    nuiDelayedPlayAnim(pAnim, eAnimFromStart, Time, 1, eAnimLoopForward);
  }
  DebugRefreshInfo();
}

void nuiLayoutBase::StartAnimation(const nglString& rName, int32 count, nuiAnimLoop loopmode , double Time)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
  {
    nuiDelayedPlayAnim(pAnim, eAnimFromStart, Time, count, loopmode);
  }
  DebugRefreshInfo();
}


void nuiLayoutBase::StopAnimation(const nglString& rName)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
    pAnim->Stop();
  DebugRefreshInfo();
}

void nuiLayoutBase::AutoDestroy(const nuiEvent& rEvent)
{
  CheckValid();
  NGL_ASSERT(mpParent!= nullptr);
  mpParent->DelChild(this);
}

void nuiLayoutBase::AutoHide(const nuiEvent& rEvent)
{
  CheckValid();
  SilentSetVisible(false);
}

void nuiLayoutBase::AutoStartTransition(const nuiEvent& rEvent)
{
  CheckValid();
  StartTransition();
}

void nuiLayoutBase::AutoStopTransition(const nuiEvent& rEvent)
{
  CheckValid();
  StopTransition();
}

bool nuiLayoutBase::IsTrashed(bool combined) const
{
  CheckValid();
  if (!combined || !mpParent)
    return mTrashed;
  if (!mTrashed)
    return mpParent->IsTrashed(true);
  return mTrashed;
}

