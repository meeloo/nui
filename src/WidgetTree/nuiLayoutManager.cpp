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


