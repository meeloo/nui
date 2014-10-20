//
//  nuiLayoutManager.h
//  nui3
//
//  Created by Sébastien Métrot on 18/10/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#pragma once

#include "nui.h"

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
  mPosition = nuiFill;
  mFillRule = nuiFill;
  mCSSPasses = 0;
  mpMatrixNodes = NULL;
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

  LoadIdentityMatrix();
  
  NUI_ADD_EVENT(Shown);
  NUI_ADD_EVENT(Hiden);
  NUI_ADD_EVENT(VisibilityChanged);
  

  mRect;
  mLayoutRect;
  mVisibleRect;
  mIdealRect;
  mUserRect;
  mHotRect;
  mConstraint;
  
  mpMatrixNodes;
  
  mAnimateLayout;
  mVisible;
  mHasUserPos;
  mHasUserSize;
  mHasUserWidth;
  mHasUserHeight;
  mForceIdealSize;
  mNeedSelfLayout;
  mNeedLayout;
  mNeedIdealRect;
  mCanRespectConstraint;
  mInSetRect;
  mFixedAspectRatio;
  mOverrideVisibleRect;
  mAutoUpdateLayout;
  
   mMaxIdealWidth;
   mMaxIdealHeight;
   mMinIdealWidth;
   mMinIdealHeight;
  
   mMaxWidth;
   mMaxHeight;
   mMinWidth;
   mMinHeight;
  
  mPosition;
  mFillRule;
  mpLayoutAnimation;
  
  mCSSPasses;
  
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
                nuiMakeDelegate(this, &nuiLayoutBase::GetRect),
                nuiMakeDelegate(this, &nuiLayoutBase::SetLayout)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("LayoutRectUnsafe"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiLayoutBase::GetRect),
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
  
  AddAttribute(new nuiAttribute<nuiMatrix>
               (nglString("Matrix"), nuiUnitMatrix,
                nuiMakeDelegate(this, &nuiLayoutBase::_GetMatrix),
                nuiMakeDelegate(this, &nuiLayoutBase::_SetMatrix)));
  
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

const nuiRect& nuiLayoutBase::GetRect() const
{
  CheckValid();
  return mRect;
}

nuiRect nuiLayoutBase::GetBorderedRect() const
{
  CheckValid();
  return GetBorderedRect(GetRect());
}

nuiRect nuiLayoutBase::GetBorderedRect(const nuiRect& rRect) const
{
  CheckValid();
  nuiRect rect = rRect;
  rect.Bottom() += GetActualBorderBottom();
  rect.Top() -= GetActualBorderTop();
  rect.Left() -= GetActualBorderLeft();
  rect.Right() += GetActualBorderRight();
  return rect;
}

nuiRect nuiLayoutBase::GetBorderLessRect(const nuiRect& rRect) const
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
  mNeedRender = true;
  mNeedSelfRedraw = true;
  DebugRefreshInfo();
}

void nuiLayoutBase::InvalidateLayout()
{
  CheckValid();

  if (mAutoUpdateLayout)
  {
    UpdateLayout();
    Invalidate();
    return;
  }

  if ((!mNeedSelfLayout && HasUserRect()))
  {
    UpdateLayout();
    return;
  }

  bool broadcast = !mNeedLayout;
  SilentInvalidateLayout();

  if (mpParent && broadcast)
  {
    //NGL_OUT("InvalidateLayout + Broadcast from %s\n", GetObjectClass().GetChars());
    mpParent->BroadcastInvalidateLayout(this, false);
  }
  DebugRefreshInfo();
}

void nuiLayoutBase::ForcedInvalidateLayout()
{
  CheckValid();
  bool broadcast = !mNeedLayout;
  SilentInvalidateLayout();

  if (mpParent && broadcast)
  {
    //NGL_OUT("InvalidateLayout + Broadcast from %s\n", GetObjectClass().GetChars());
    mpParent->BroadcastInvalidateLayout(this, false);
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

  mNeedRender = true;
  if (mpSurface)
    mNeedSelfRedraw = true;

  if (!BroadCastOnly)
  {
    mNeedSelfLayout = true;
    mNeedIdealRect = true;
  }

  if (mpParent)
  {
    mpParent->BroadcastInvalidateLayout(pSender, BroadCastOnly);
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
  if (!combined || !mpParent)
    return mVisible;
  if (mVisible)
    return mpParent->IsVisible(true);
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


bool nuiLayoutBase::IsInsideFromRoot(nuiSize X, nuiSize Y, nuiSize GrowOffset)
{
  CheckValid();
  if (!IsVisible(false))
    return false;

  GlobalToLocal(X, Y);
  return IsInsideFromSelf(X,Y, GrowOffset);
}

bool nuiLayoutBase::IsInsideFromParent(nuiSize X, nuiSize Y, nuiSize GrowOffset)
{
  CheckValid();
  if (!IsVisible(false))
    return false;
  return IsInsideFromSelf(X - mRect.Left(), Y - mRect.Top(), GrowOffset);
}

bool nuiLayoutBase::IsInsideFromSelf(nuiSize X, nuiSize Y, nuiSize GrowOffset)
{
  CheckValid();
  if (!IsVisible(false))
    return false;

  if (mInteractiveDecoration)
  {
    nuiRect r = mVisibleRect;
    r.Intersect(mVisibleRect, GetOverDrawRect(true, true));
    r.Grow(GrowOffset, GrowOffset);
    return r.IsInside(X, Y);
  }
  if (mInteractiveOD)
  {
    nuiRect r = mVisibleRect;
    r.Intersect(r, GetOverDrawRect(true, false));
    r.Grow(GrowOffset, GrowOffset);
    return r.IsInside(X, Y);
  }

  nuiRect r(GetRect().Size());
  r.Grow(GrowOffset, GrowOffset);
  return r.IsInside(X,Y);
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
  return mpParent ? mpParent->IsInSetRect() : false;
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
  nuiSize Left = mBorderLeft;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Left = MAX(Left, mpDecoration->GetBorder(nuiLeft, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Left = MAX(Left, mpFocusDecoration->GetBorder(nuiLeft, this));
  return Left;
}

nuiSize nuiLayoutBase::GetActualBorderTop() const
{
  CheckValid();
  nuiSize Top = mBorderTop;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Top = MAX(Top, mpDecoration->GetBorder(nuiTop, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Top = MAX(Top, mpFocusDecoration->GetBorder(nuiTop, this));
  return Top;
}

nuiSize nuiLayoutBase::GetActualBorderRight() const
{
  CheckValid();
  nuiSize Right = mBorderRight;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Right = MAX(Right, mpDecoration->GetBorder(nuiRight, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Right = MAX(Right, mpFocusDecoration->GetBorder(nuiRight, this));
  return Right;
}

nuiSize nuiLayoutBase::GetActualBorderBottom() const
{
  CheckValid();
  nuiSize Bottom = mBorderBottom;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Bottom = MAX(Bottom, mpDecoration->GetBorder(nuiBottom, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Bottom = MAX(Bottom, mpFocusDecoration->GetBorder(nuiBottom, this));
  return Bottom;
}

void nuiLayoutBase::InternalSetLayout(const nuiRect& rect)
{
  CheckValid();
  bool PositionChanged = (rect.Left() != mRect.Left()) || (rect.Top() != mRect.Top());
  bool SizeChanged = !rect.Size().IsEqual(mRect.Size());
  mNeedSelfLayout = mNeedSelfLayout || SizeChanged;

  InternalSetLayout(rect, PositionChanged, SizeChanged);

  if (!mOverrideVisibleRect)
    mVisibleRect = GetOverDrawRect(true, true);

  if (PositionChanged && mpParent)
    mpParent->Invalidate();

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
    bool SizeChanged = !mUserRect.Size().IsEqual(mRect.Size());
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
      mpParent->Invalidate();
      Invalidate();
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
  if (mpParent)
  {
    nuiRect r(rRect);
    LocalToLocal(mpParent, r);
    mpParent->OnChildHotRectChanged(this, r);
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

  mLayoutRect = rRect;

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


/// Matrix Operations:
nuiMatrix nuiLayoutBase::mIdentityMatrix;

void nuiLayoutBase::AddMatrixNode(nuiMatrixNode* pNode)
{
  CheckValid();
  nuiLayoutBase::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  if (!mpMatrixNodes)
    mpMatrixNodes = new std::vector<nuiMatrixNode*>;

  pNode->Acquire();
  mpMatrixNodes->push_back(pNode);
  mGenericWidgetSink.Connect(pNode->Changed, &nuiLayoutBase::AutoInvalidateLayout);

  // Usual clean up needed for the partial redraw to work correctly
  nuiLayoutBase::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
}

void nuiLayoutBase::DelMatrixNode(uint32 index)
{
  if (!mpMatrixNodes)
    return;

  CheckValid();
  nuiLayoutBase::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  mGenericWidgetSink.Disconnect(mpMatrixNodes->at(index)->Changed, &nuiLayoutBase::AutoInvalidateLayout);
  mpMatrixNodes->at(index)->Release();
  mpMatrixNodes->erase(mpMatrixNodes->begin() + index);

  // Usual clean up needed for the partial redraw to work correctly
  nuiLayoutBase::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
}


int32 nuiLayoutBase::GetMatrixNodeCount() const
{
  CheckValid();
  if (!mpMatrixNodes)
    return 0;
  return mpMatrixNodes->size();
}


nuiMatrixNode* nuiLayoutBase::GetMatrixNode(uint32 index) const
{
  CheckValid();
  if (mpMatrixNodes)
    return mpMatrixNodes->at(index);
  return nullptr;
}


void nuiLayoutBase::LoadIdentityMatrix()
{
  CheckValid();
  Invalidate();

  if (mpMatrixNodes)
  {
    for (uint32 i = 0; i < mpMatrixNodes->size(); i++)
      mpMatrixNodes->at(i)->Release();
    delete mpMatrixNodes;
    mpMatrixNodes = NULL;
  }

  Invalidate();
  DebugRefreshInfo();
}

bool nuiLayoutBase::IsMatrixIdentity() const
{
  return !mpMatrixNodes;
}

void nuiLayoutBase::GetMatrix(nuiMatrix& rMatrix) const
{
  CheckValid();
  rMatrix.SetIdentity();
  if (IsMatrixIdentity())
    return;

  for (uint32 i = 0; i < mpMatrixNodes->size(); i++)
    mpMatrixNodes->at(i)->Apply(rMatrix);
}

nuiMatrix nuiLayoutBase::GetMatrix() const
{
  CheckValid();
  nuiMatrix m;
  GetMatrix(m);
  return m;
}

nuiMatrix nuiLayoutBase::_GetMatrix() const
{
  CheckValid();
  return GetMatrix();
}

void nuiLayoutBase::_SetMatrix(nuiMatrix Matrix)
{
  SetMatrix(Matrix);
}

void nuiLayoutBase::SetMatrix(const nuiMatrix& rMatrix)
{
  CheckValid();
  nuiLayoutBase::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  // Special case: we only need one simple static matrix node at max
  LoadIdentityMatrix(); // So we load the identity matrix (i.e. clear any existing node)
  if (!rMatrix.IsIdentity()) // If the user wasn't asking for the identity matrix
  {
    AddMatrixNode(new nuiMatrixNode(rMatrix));
  }

  // Usual clean up needed for the partial redraw to work correctly
  nuiLayoutBase::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
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
  //  nuiRect r(GetRect());
  //  mInSetRect = true;
  //  SetRect(r);
  SetLayout(mLayoutRect);
  //  mInSetRect = false;
  Invalidate();
}

void nuiWidget::ApplyCSSForStateChange(uint32 MatchersTag)
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

void nuiWidget::ResetCSSPass()
{
  CheckValid();
  InternalResetCSSPass();
}

void nuiWidget::IncrementCSSPass()
{
  CheckValid();
  mCSSPasses++;
}

uint32 nuiWidget::GetCSSPass() const
{
  CheckValid();
  return mCSSPasses;
}

void nuiWidget::AutoInvalidateLayout(const nuiEvent& rEvent)
{
  InvalidateLayout();
}

void nuiWidget::SetVisible(bool Visible)
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

void nuiWidget::BroadcastVisible()
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetNeedSelfRedraw())
    {
      pItem->InvalidateRect(pItem->GetRect().Size());
    }
    nuiWidget* pContainer = dynamic_cast<nuiWidget*>(pItem);
    if (pContainer)
    {
      pContainer->BroadcastVisible();
    }
  }
  delete pIt;
}

nuiRect nuiWidget::CalcIdealSize()
{
  CheckValid();
  nuiRect temp;

  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    nuiRect r(pItem->GetIdealRect()); // Dummy call. Only the side effect is important: the object recalculates its layout.
    temp.Union(temp, r.Size());
  }
  delete pIt;

  DebugRefreshInfo();
  return temp.Size();
}

bool nuiWidget::SetSelfRect(const nuiRect& rRect)
{
  CheckValid();
#ifdef _DEBUG_LAYOUT
  if (GetDebug())
    NGL_OUT("nuiWidget::SetRect on '%s' (%f, %f - %f, %f)\n", GetObjectClass().GetChars(), rRect.mLeft, rRect.mTop, rRect.GetWidth(), rRect.GetHeight());
#endif

  bool inval = mNeedInvalidateOnSetRect;
  if (!(mRect == rRect))
    inval = true;

  if (inval)
    Invalidate();
  if (mForceIdealSize)
    mRect.Set(rRect.Left(), rRect.Top(), mIdealRect.GetWidth(), mIdealRect.GetHeight());
  else
    mRect = rRect;

  if (!mOverrideVisibleRect)
    mVisibleRect = GetOverDrawRect(true, true);

  if (inval)
    Invalidate();

  DebugRefreshInfo();
}

bool nuiWidget::SetRect(const nuiRect& rRect)
{
  SetSelfRect(rRect);
  nuiRect rect(rRect.Size());
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    pItem->GetIdealRect();
    pItem->SetLayout(rect);
  }
  delete pIt;

  DebugRefreshInfo();
  return true;
}

void nuiWidget::InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged)
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
    mRect = rect;

    if (mNeedLayout)
    {
      // The children need to be re layed out (at least one of them!).
      nuiWidget::IteratorPtr pIt = GetFirstChild(false);
      do
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
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
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->IsVisible())
    {
      NGL_ASSERT(!pItem->GetNeedLayout());
    }
  }
  delete pIt;
  //#TEST end
#endif
}

void nuiWidget::OnChildHotRectChanged(nuiWidget* pChild, const nuiRect& rChildHotRect)
{
  CheckValid();
  SetHotRect(rChildHotRect);
}

void nuiWidget::InternalResetCSSPass()
{
  CheckValid();
  mCSSPasses = 0;

  IteratorPtr pIt = GetFirstChild(false);
  for (; pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->ResetCSSPass();
  }
  delete pIt;
}

void nuiWidget::SetChildrenLayoutAnimationDuration(float duration)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationDuration(duration);
  }
  delete pIt;
}

void nuiWidget::SetChildrenLayoutAnimationEasing(const nuiEasingMethod& rMethod)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationEasing(rMethod);
  }
  delete pIt;
}

