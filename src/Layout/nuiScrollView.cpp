/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

#define SCROLL_SIZE 12.0f
#define NUI_SMOOTH_SCROLL_RATIO 0.2
#define HIDE_SCROLLBARS_DELAY 0.6
#define INERTIA_SPEED 0.8
#define INITIAL_BRAKES 0.5
#define INERTIA_BRAKES 0.95
#define INERTIA_OFFSET_BRAKES 0.5

#define EXTRA_OUT_SIZE_RATIO 0.5

#ifdef DEBUG
#define _DEBUG_LAYOUT 1
#endif


nuiScrollView::nuiScrollView(bool Horizontal, bool Vertical)
: nuiWidget(),
mSVSink(this),
mVerticalHotRectActive(true),
mHorizontalHotRectActive(true)
{
  Init(NULL, NULL, Horizontal, Vertical);
}

nuiScrollView::nuiScrollView(nuiScrollBar* pHorizontalScrollBar, nuiScrollBar* pVerticalScrollBar )
: nuiWidget(),
mSVSink(this),
mVerticalHotRectActive(true),
mHorizontalHotRectActive(true)
{
  Init(pHorizontalScrollBar, pVerticalScrollBar, false, false);
}



void nuiScrollView::InitAttributes()
{
  AddAttribute(new nuiAttribute<bool>(_T("EnableHorizontalScroll"), nuiUnitYesNo, nuiMakeDelegate(this, &nuiScrollView::GetEnableHorizontalScroll), nuiMakeDelegate(this, &nuiScrollView::SetEnableHorizontalScroll)));
  AddAttribute(new nuiAttribute<bool>(_T("EnableVerticalScroll"), nuiUnitYesNo, nuiMakeDelegate(this, &nuiScrollView::GetEnableHorizontalScroll), nuiMakeDelegate(this, &nuiScrollView::SetEnableHorizontalScroll)));
  AddAttribute(new nuiAttribute<bool>(_T("EnableSmoothScrolling"), nuiUnitYesNo, nuiMakeDelegate(this, &nuiScrollView::IsSmoothScrollingEnabled), nuiMakeDelegate(this, &nuiScrollView::EnableSmoothScrolling)));
  
  AddAttribute(new nuiAttribute<float>(_T("HIncrement"), nuiUnitPixels, nuiMakeDelegate(this, &nuiScrollView::GetHIncrement), nuiMakeDelegate(this, &nuiScrollView::SetHIncrement)));
  AddAttribute(new nuiAttribute<float>(_T("VIncrement"), nuiUnitPixels, nuiMakeDelegate(this, &nuiScrollView::GetVIncrement), nuiMakeDelegate(this, &nuiScrollView::SetVIncrement)));
  
  AddAttribute(new nuiAttribute<float>(_T("HPos"), nuiUnitPixels, nuiMakeDelegate(this, &nuiScrollView::GetXPos), nuiMakeDelegate(this, &nuiScrollView::SetXPos)));
  AddAttribute(new nuiAttribute<float>(_T("VPos"), nuiUnitPixels, nuiMakeDelegate(this, &nuiScrollView::GetYPos), nuiMakeDelegate(this, &nuiScrollView::SetYPos)));
  
  AddAttribute(new nuiAttribute<float>(_T("HOffset"), nuiUnitPixels, nuiMakeDelegate(this, &nuiScrollView::GetXOffset), nuiMakeDelegate(this, &nuiScrollView::SetXOffset)));
  AddAttribute(new nuiAttribute<float>(_T("VOffset"), nuiUnitPixels, nuiMakeDelegate(this, &nuiScrollView::GetYOffset), nuiMakeDelegate(this, &nuiScrollView::SetYOffset)));
}

void nuiScrollView::Init(nuiScrollBar* pHorizontalScrollBar, nuiScrollBar* pVerticalScrollBar, bool Horizontal, bool Vertical)
{
  if (SetObjectClass(_T("nuiScrollView")))
    InitAttributes();
  
  SetAutoClip(true);
  mFillChildren = true;
  mVIncrement = 64.f;
  mHIncrement = 64.f;
  mCanRespectConstraint = true;
  
  mSmoothScrolling = true;
  mXOffset = 0;
  mYOffset = 0;
  mSpeedX = 0;
  mSpeedY = 0;
  mLastX = 0;
  mLastY = 0;
  
  mpSmoothTimer = nuiAnimation::AcquireTimer();
  mSVSink.Connect(mpSmoothTimer->Tick, &nuiScrollView::OnSmoothScrolling);
  mTimerOn = false;
  mBarSize = SCROLL_SIZE;
  
  mMinimalResize = false;
  
  mpHorizontal = NULL;
  mpVertical   = NULL;
  
  mDragEnabled = false;
  mHideScrollBars = false;
  
  mTouched = false;
  mLeftClick = 0;
  //  NGL_OUT("nuiScrollView::Init LeftClick: %d\n", mLeftClick);
  
  
  if (pHorizontalScrollBar)
  {
    mHorizontalIsExternal = true;
    mForceNoHorizontal    = false;
  }
  else
  {
    mHorizontalIsExternal = false;
    mForceNoHorizontal    = !Horizontal;
  }
  
  if (pVerticalScrollBar)
  {
    mVerticalIsExternal = true;
    mForceNoVertical    = false;
  }
  else
  {
    mVerticalIsExternal = false;
    mForceNoVertical    = !Vertical;
  }
  
  if (pHorizontalScrollBar)
  {
    mpHorizontal = pHorizontalScrollBar;
  }
  else
  {
    mpHorizontal = new nuiScrollBar(nuiHorizontal);
    AddChild(mpHorizontal);
  }
  
  if (pVerticalScrollBar)
  {
    mpVertical = pVerticalScrollBar;
  }
  else
  {
    mpVertical = new nuiScrollBar(nuiVertical);
    AddChild(mpVertical);
  }
  
  
  mpHorizontal->SetSerializeMode(eDontSaveNode);
  mpVertical->SetSerializeMode(eDontSaveNode);
  
  mHThumbPressed = false;
  mVThumbPressed = false;
  
  
  mSVSink.Connect(mpHorizontal->GetRange().ValueChanged, &nuiScrollView::Scrolled);
  mSVSink.Connect(mpVertical->GetRange().ValueChanged,   &nuiScrollView::Scrolled);
  
  mSVSink.Connect(mpHorizontal->ThumbPressed, &nuiScrollView::OnHThumbPressed);
  mSVSink.Connect(mpHorizontal->ThumbDepressed, &nuiScrollView::OnHThumbDepressed);
  mSVSink.Connect(mpVertical->ThumbPressed, &nuiScrollView::OnVThumbPressed);
  mSVSink.Connect(mpVertical->ThumbDepressed, &nuiScrollView::OnVThumbDepressed);
  
  mSVSink.Connect(ChildAdded, &nuiScrollView::OnChildAdded);
  mSVSink.Connect(ChildDeleted, &nuiScrollView::OnChildRemoved);
  
  mpHideTimer = new nuiTimer(HIDE_SCROLLBARS_DELAY);
  mSVSink.Connect(mpHideTimer->Tick, &nuiScrollView::OnHideTick);
  
  mpShowAnimV = mpShowAnimH = NULL;
  mpHideAnimV = mpHideAnimH = NULL;
  
  //#ifdef _UIKIT_
  ActivateMobileMode();
  //#endif
}

bool nuiScrollView::GetEnableHorizontalScroll() const
{
  return (mpHorizontal && !mForceNoHorizontal);
}

void nuiScrollView::SetEnableHorizontalScroll(bool set)
{
  mForceNoHorizontal = !set;
  //mpVertical->SetVisible(set);
  InvalidateLayout();
}

bool nuiScrollView::GetEnableVerticalScroll() const
{
  return (mpVertical && !mForceNoVertical);
}

void nuiScrollView::SetEnableVerticalScroll(bool set)
{
  mForceNoVertical = !set;
  //mpVertical->SetVisible(set);
  InvalidateLayout();
}

nuiScrollView::~nuiScrollView()
{
  nuiAnimation::ReleaseTimer();
  
  if (mHorizontalIsExternal && mpHorizontal)
  {
    //    mSVSink.Disconnect(mpHorizontal->GetRange().ValueChanged);
  }
  if (mVerticalIsExternal && mpVertical)
  {
    //    mSVSink.Disconnect(mpVertical->GetRange().ValueChanged);
  }
  
  mpShowAnimH->Release();
  mpShowAnimV->Release();
  mpHideAnimH->Release();
  mpHideAnimV->Release();
  mpShowAnimH = NULL;
  mpShowAnimV = NULL;
  mpHideAnimH = NULL;
  mpHideAnimV = NULL;
  
}



nuiRect nuiScrollView::CalcIdealSize()
{
  nuiRect rect;
  
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem != mpHorizontal && pItem != mpVertical)
    {
      pItem->SetLayoutConstraint(mConstraint);
      rect.Union(rect, pItem->GetIdealRect());
#ifdef _DEBUG_LAYOUT
      if (GetDebug())
      {
        NGL_OUT(_T("\tnuiScrollView::CalcIdealSize: [%s %s] size %s\n"), pItem->GetObjectClass().GetChars(), pItem->GetObjectName().GetChars(), mIdealRect.GetValue().GetChars());
      }
#endif
    }
    else if (pItem)
    {
      pItem->GetIdealRect(); // Dummy get ideal rect for the scroll bars
    }
  }
  delete pIt;
  
  rect.mLeft = 0;
  rect.mTop = 0;
  
  mChildrenUnionRect = rect;
  ///< take scrollbars in IdealRect calculation if necessary
  rect.SetSize(rect.GetWidth() + (mpHorizontal && !mHorizontalIsExternal && !mForceNoHorizontal && !mHideScrollBars ? mBarSize : 0),
               rect.GetHeight() + (mpVertical && !mVerticalIsExternal && !mForceNoVertical && !mHideScrollBars ? mBarSize : 0));
  mIdealRect = rect;
  
#ifdef _DEBUG_LAYOUT
  if (GetDebug())
  {
    NGL_OUT(_T("nuiScrollView::CalcIdealSize: %s\n"), mIdealRect.GetValue().GetChars());
  }
#endif
  
  return mIdealRect;
}

bool nuiScrollView::SetRect(const nuiRect& rRect)
{
#ifdef _DEBUG_LAYOUT
  if (GetDebug())
  {
    NGL_OUT(_T("nuiScrollView::SetRect: %s\n"), rRect.GetValue().GetChars());
  }
#endif
  
  nuiWidget::SetSelfRect(rRect);
  
  if (!mpHorizontal || !mpVertical)
    return false;
  
  nuiSize XOffset = mpHorizontal->GetRange().GetValue();
  nuiSize YOffset = mpVertical->GetRange().GetValue();
  
  if (mSmoothScrolling)
  {
    if (!mHThumbPressed)
      XOffset = mXOffset;
    if (!mVThumbPressed)
      YOffset = mYOffset;
  }
  
  if (0)
  {
    mXOffset = XOffset;
    mYOffset = YOffset;
  }
  
  XOffset = (nuiSize)ToNearest(XOffset);
  YOffset = (nuiSize)ToNearest(YOffset);
  
  //  printf("SetRect YOffset %f\n", YOffset);
  GetIdealRect();
  nuiRect rIdealRect(mChildrenUnionRect); ///< needed because GetIdealRect may return a UserRect
  
  if (mMinimalResize)
  {
    rIdealRect.Right() = MAX(rIdealRect.Right(), mOldIdealRect.Right());
    rIdealRect.Bottom() = MAX(rIdealRect.Bottom(), mOldIdealRect.Bottom());
    mOldIdealRect = rIdealRect;
  }
  
  nuiSize x = rIdealRect.GetWidth();
  nuiSize y = rIdealRect.GetHeight();
  
  nuiRange& hrange = mpHorizontal->GetRange();
  nuiRange& vrange = mpVertical->GetRange();
  
  if (mMinimalResize)
  {
    if( (hrange.GetValue() + hrange.GetPageSize()) < hrange.GetMaximum() )
    {
      x = mChildrenUnionRect.GetWidth();
      mOldIdealRect = mChildrenUnionRect;
    }
    if( (vrange.GetValue() + vrange.GetPageSize()) < vrange.GetMaximum() )
    {
      y = mChildrenUnionRect.GetHeight();
      mOldIdealRect = mChildrenUnionRect;
    }
  }
  
  nuiSize xx = rRect.GetWidth();
  nuiSize yy = rRect.GetHeight();
  
  bool needv = y > yy;
  bool needh = x > xx;
  
  needh = needh && !mForceNoHorizontal;
  needv = needv && !mForceNoVertical;
  
  nuiSize scrollh = (needh && !mHorizontalIsExternal) ? mBarSize:0;
  nuiSize scrollv = (needv && !mVerticalIsExternal)   ? mBarSize:0;
  
  for (int i = 0; i<2; i++)
  {
    if (needv && !mForceNoHorizontal)
    {
      needh = x > (xx - scrollv);
      if (!mVerticalIsExternal)
        scrollv = needv?mBarSize:0;
    }
    
    if (needh && !mForceNoVertical)
    {
      needv = y > (yy - scrollh);
      if (!mHorizontalIsExternal)
        scrollh = needh?mBarSize:0;
    }
  }
  
  if (needh)
  {
    double pagesize = xx - scrollv;
    if (mHideScrollBars)
      pagesize = xx;
    hrange.SetRange(0, x);
    hrange.SetPageSize(MAX(0, pagesize));
    hrange.SetPageIncrement(MAX(0, pagesize));
    hrange.SetIncrement(mHIncrement);
    hrange.EnableEvents(false);
    nuiSize oldv = hrange.GetValue();
    hrange.SetValue(hrange.GetValue());
    if (hrange.GetValue() != oldv)
    {
      mXOffset = hrange.GetValue();
    }
    hrange.EnableEvents(true);
  }
  else
  {
    double pagesize = x;
    hrange.SetRange(0, x);
    hrange.SetPageSize(MAX(0, pagesize));
    hrange.SetPageIncrement(MAX(0, pagesize));
    hrange.SetIncrement(mHIncrement);
    mXOffset = XOffset= 0.f;
    hrange.EnableEvents(false);
    hrange.SetValue(XOffset);
    hrange.EnableEvents(true);
  }
  
  if (needv)
  {
    double pagesize = yy - scrollh;
    if (mHideScrollBars)
      pagesize = yy;
    vrange.SetRange(0, y);
    vrange.SetPageSize(MAX(0, pagesize));
    vrange.SetPageIncrement(MAX(0, pagesize));
    vrange.SetIncrement(mVIncrement);
    vrange.EnableEvents(false);
    nuiSize oldv = vrange.GetValue();
    vrange.SetValue(vrange.GetValue());
    if (vrange.GetValue() != oldv)
    {
      mYOffset = vrange.GetValue();
    }
    vrange.EnableEvents(true);
  }
  else
  {
    double pagesize = y;
    vrange.SetRange(0, y);
    vrange.SetPageSize(MAX(0, pagesize));
    vrange.SetPageIncrement(MAX(0, pagesize));
    vrange.SetIncrement(mVIncrement);
    mYOffset = YOffset = 0.f;
    vrange.EnableEvents(false);
    vrange.SetValue(YOffset);
    vrange.EnableEvents(true);
  }
  OffsetsChanged();
  
  nuiSize scrollbarH = scrollh;
  nuiSize scrollbarV = scrollv;
  if (mHideScrollBars)
  {
    scrollbarH = 0;
    scrollbarV = 0;
  }
  SetChildrenRect(x, y, xx, yy, scrollbarV, scrollbarH);
  
  if (!mHorizontalIsExternal)
  {
    mpHorizontal->SetVisible(needh);
    mpHorizontal->SetLayout(nuiRect(0.0f, yy-scrollh, xx - scrollv, scrollh));
  }
  if (!mVerticalIsExternal)
  {
    mpVertical->SetVisible(needv);
    mpVertical->SetLayout(nuiRect(xx - scrollv, 0.0f, scrollv, yy-scrollh));
  }
  
  return true;
}


bool nuiScrollView::SetChildrenRect(nuiSize x, nuiSize y, nuiSize xx, nuiSize yy, nuiSize scrollv, nuiSize scrollh)
{
  //  #ifdef _DEBUG_LAYOUT
  //  if (GetDebug())
  //  {
  //    NGL_OUT(_T("\tnuiScrollView::SetChildrenRect: (%f, %f) (%f, %f) - (%f, %f)\n"), x, y, xx, yy, scrollv, scrollh);
  //  }
  //  #endif
  
  nuiRect VisibleRect = GetRect().Size();
  VisibleRect.Right() -= scrollv;
  VisibleRect.Bottom() -= scrollh;
  
  nuiRange& hrange = mpHorizontal->GetRange();
  nuiRange& vrange = mpVertical->GetRange();
  nuiSize XOffset = hrange.GetValue();
  nuiSize YOffset = vrange.GetValue();
  
  if (mSmoothScrolling)
  {
    //    if (mTimerOn)
    {
      if (!mHThumbPressed)
        XOffset = mXOffset;
      if (!mVThumbPressed)
        YOffset = mYOffset;
    }
  }
  
  XOffset = (nuiSize)ToNearest(XOffset);
  YOffset = (nuiSize)ToNearest(YOffset);
  //  printf("SetChildrenRect YOffset %f\n", YOffset);
  
  if (!mFillChildren)
  {
    IteratorPtr pIt;
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (pItem != mpVertical && pItem != mpHorizontal)
      {
        nuiRect rect = pItem->GetIdealRect();
        rect.Move(-XOffset, -YOffset);
        
        float w = rect.GetWidth();
        float h = rect.GetHeight();
        
        if (mForceNoHorizontal)
          w = xx;
        if (mForceNoVertical)
          h = yy;
        
        rect.SetSize(w, h);
        
#ifdef _DEBUG_LAYOUT
        if (GetDebug())
        {
          NGL_OUT(_T("\tnuiScrollView::SetChildrenRect: SetLayout(%s)\n"), rect.GetValue().GetChars());
        }
#endif
        
        pItem->SetLayout(rect);
        rect = VisibleRect;
        LocalToLocal(pItem, rect);
        pItem->SetVisibleRect(rect);
      }
    }
    delete pIt;
  }
  else
  {
    if (x < xx - scrollv)
      x = xx - scrollv;
    if (y < yy - scrollh)
      y = yy - scrollh;
    
    IteratorPtr pIt;
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (pItem != mpVertical && pItem != mpHorizontal)
      {
        float w = x, h = y;
        
        if (mForceNoHorizontal)
        {
          w = xx - scrollv;
        }
        if (mForceNoVertical)
        {
          h = yy - scrollh;
        }
        
        nuiRect rect(-XOffset, -YOffset, w, h);
        
#ifdef _DEBUG_LAYOUT
        if (GetDebug())
        {
          NGL_OUT(_T("\tnuiScrollView::SetChildrenRect: SetLayout(%s)\n"), rect.GetValue().GetChars());
        }
#endif
        
        pItem->SetLayout(rect);
        
        rect = VisibleRect;
        LocalToLocal(pItem, rect);
        pItem->SetVisibleRect(rect);
      }
    }
    delete pIt;
  }
  
  return true;
}


bool nuiScrollView::Draw(nuiDrawContext* pContext)
{
  nuiSize scrollv = (mpVertical->IsVisible() && !mForceNoVertical && !mVerticalIsExternal)?mBarSize:0;
  nuiSize scrollh = (mpHorizontal->IsVisible() && !mForceNoHorizontal && !mHorizontalIsExternal)?mBarSize:0;
  nuiRect rect(0.0f,0.0f,mRect.GetWidth()-scrollv, mRect.GetHeight()-scrollh);

  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    nuiRect intersect;
    if (intersect.Intersect(rect, pItem->GetOverDrawRect(true, true)))
    {
      DrawChild(pContext, pItem);
    }
  }
  
  delete pIt;
  
  if (GetDebug())
  {
    pContext->SetStrokeColor(nuiColor("red"));
    pContext->DrawRect(GetRect().Size(), eStrokeShape);
    pContext->SetStrokeColor(nuiColor("blue"));
    pContext->DrawRect(GetIdealRect().Size(), eStrokeShape);
    pContext->SetStrokeColor("yellow");
    float h = GetRect().GetHeight() - scrollh - 1;
    pContext->DrawLine(0, h, GetRect().GetWidth(), h);
  }
  return true;
}

void nuiScrollView::Scrolled(const nuiEvent& rEvent)
{
  if (mSmoothScrolling && !mHThumbPressed && !mVThumbPressed)
    mTimerOn = true;
  else
    UpdateLayout();
  Invalidate();
}

bool nuiScrollView::Clear()
{
  int childCount = GetChildrenCount();
  for (childCount--; childCount >= 0; childCount--)
  {
    nuiWidget* pWidget = GetChild(childCount);
    if (pWidget && pWidget != mpHorizontal && pWidget != mpVertical)
    {
      DelChild(pWidget);
    }
  }
  mpChildren.clear();
  if (!mVerticalIsExternal)
    mpChildren.push_back(mpVertical);
  if (!mHorizontalIsExternal)
    mpChildren.push_back(mpHorizontal);
  InvalidateLayout();
  DebugRefreshInfo();
  return true;
}

void nuiScrollView::OnChildAdded(const nuiEvent& rEvent)
{
  const nuiTreeEvent<nuiWidget>* pTreeEvent = dynamic_cast<const nuiTreeEvent<nuiWidget>*>(&rEvent);
  nuiWidgetPtr pNode = dynamic_cast<nuiWidgetPtr>(pTreeEvent->mpChild);
  
  if (pNode)
  {
    mSVSink.Connect(pNode->HotRectChanged, &nuiScrollView::OnHotRectChanged, pNode);
  }
  
  // The scroll bar must be the first to get mouse events, so let's put them on top of the list:
  if (!mHorizontalIsExternal && mpHorizontal)
  {
    RaiseChildToFront(mpHorizontal);
  }
  if (!mVerticalIsExternal && mpVertical)
  {
    RaiseChildToFront(mpVertical);
  }
}

void nuiScrollView::OnChildRemoved(const nuiEvent& rEvent)
{
  const nuiTreeEvent<nuiWidget>* pTreeEvent = dynamic_cast<const nuiTreeEvent<nuiWidget>*>(&rEvent);
  nuiWidgetPtr pNode = dynamic_cast<nuiWidgetPtr>(pTreeEvent->mpChild);
  
  if (pNode)
  {
    mSVSink.DisconnectSource(pNode->HotRectChanged);
  }
}

void nuiScrollView::OnHotRectChanged(const nuiEvent& rEvent)
{
  if (mHorizontalHotRectActive || mVerticalHotRectActive)
  {
    nuiWidgetPtr pChild = (nuiWidgetPtr)rEvent.mpUser;
    
    if (!pChild)
      return;
    
    nuiRect rect;
    pChild->GetHotRect(rect);
    
    if (!mForceNoVertical && mVerticalHotRectActive)
    {
      bool moved = mpVertical->GetRange().MakeInRangeVisual(rect.Top(), rect.GetHeight());
      if (moved)
      {
        //        printf("moved vertical\n");
      }
    }
    
    if (!mForceNoHorizontal && mHorizontalHotRectActive)
    {
      bool moved = mpHorizontal->GetRange().MakeInRangeVisual(rect.Left(), rect.GetWidth());
      if (moved)
      {
        //        printf("moved horizontal\n");
      }
    }
  }
}

void nuiScrollView::SetFillChildren(bool Set) ///< If filling is enabled the scroll view will try to move its child in order to honor the available space
{
  mFillChildren = Set;
  Invalidate();
}

bool nuiScrollView::GetFillChildren()
{
  return mFillChildren;
}

bool nuiScrollView::MouseWheelMoved(const nglMouseInfo& rInfo)
{
  if (mpHorizontal && !mForceNoHorizontal)
  {
    mpHorizontal->GetRange().SetValue(mpHorizontal->GetRange().GetValue() + rInfo.DeltaX * 5);
  }
  
  if (mpVertical && !mForceNoVertical)
  {
    mpVertical->GetRange().SetValue(mpVertical->GetRange().GetValue() + rInfo.DeltaY * 5);
  }
  
  if (mHideScrollBars)
  {
    ShowScrollBars(true);
  }
  
  
  return true;
}

bool nuiScrollView::MouseClicked(const nglMouseInfo& rInfo)
{
  bool res = false;
  bool autoHideScrollbars = true;
  bool v = IsKeyDown(NK_LSHIFT) || IsKeyDown(NK_RSHIFT);
  if (rInfo.Buttons & nglMouseInfo::ButtonWheelUp)
  {
    if (v)
    {
      if (mpHorizontal && !mForceNoHorizontal)
      {
        auto value = mpHorizontal->GetRange().GetValue();
        mpHorizontal->GetRange().Decrement();
        res = value != mpHorizontal->GetRange().GetValue();
      }
      else if (mpVertical && !mForceNoVertical)
      {
        auto value = mpVertical->GetRange().GetValue();
        mpVertical->GetRange().Decrement();
        res = value != mpVertical->GetRange().GetValue();
      }
    }
    else
    {
      if (mpVertical && !mForceNoVertical)
      {
        auto value = mpVertical->GetRange().GetValue();
        mpVertical->GetRange().Decrement();
        res = value != mpVertical->GetRange().GetValue();
      }
      else if (mpHorizontal && !mForceNoHorizontal && !mForceNoSmartScroll)
      {
        auto value = mpHorizontal->GetRange().GetValue();
        mpHorizontal->GetRange().Decrement();
        res = value != mpHorizontal->GetRange().GetValue();
      }
    }
    
  }
  else if (rInfo.Buttons & nglMouseInfo::ButtonWheelDown)
  {
    if (v)
    {
      if (mpHorizontal && !mForceNoHorizontal)
      {
        auto value = mpHorizontal->GetRange().GetValue();
        mpHorizontal->GetRange().Increment();
        res = value != mpHorizontal->GetRange().GetValue();
      }
      else if (mpVertical && !mForceNoVertical)
      {
        auto value = mpVertical->GetRange().GetValue();
        mpVertical->GetRange().Increment();
        res = value != mpVertical->GetRange().GetValue();
      }
      
    }
    else
    {
      if (mpVertical && !mForceNoVertical)
      {
        auto value = mpVertical->GetRange().GetValue();
        mpVertical->GetRange().Increment();
        res = value != mpVertical->GetRange().GetValue();
      }
      else if (mpHorizontal && !mForceNoHorizontal && !mForceNoSmartScroll)
      {
        auto value = mpHorizontal->GetRange().GetValue();
        mpHorizontal->GetRange().Increment();
        res = value != mpHorizontal->GetRange().GetValue();
      }
    }
  }
  else if (rInfo.Buttons & nglMouseInfo::ButtonWheelLeft)
  {
    if (mpHorizontal && !mForceNoHorizontal)
    {
      auto value = mpHorizontal->GetRange().GetValue();
      mpHorizontal->GetRange().Decrement();
      res = value != mpHorizontal->GetRange().GetValue();
    }
  }
  else if (rInfo.Buttons & nglMouseInfo::ButtonWheelRight)
  {
    if (mpHorizontal && !mForceNoHorizontal)
    {
      auto value = mpHorizontal->GetRange().GetValue();
      mpHorizontal->GetRange().Increment();
      res = value != mpHorizontal->GetRange().GetValue();
    }
  }
  else if (rInfo.Buttons & nglMouseInfo::ButtonLeft && mDragEnabled && !mLeftClick)
  {
    //    mLeftClick++;
    //    NGL_OUT("nuiScrollView::MouseClicked LeftClick: %d\n", mLeftClick);
    mTimerOn = false;
    mSpeedX = 0;
    mSpeedY = 0;
    mLastX = rInfo.X;
    mLastY = rInfo.Y;
    mClickValueH = GetRange(nuiHorizontal)->GetValue();
    mClickValueV = GetRange(nuiVertical)->GetValue();
    
    if (mHideScrollBars)
    {
      ShowScrollBars();
    }
    
    res = true;
    autoHideScrollbars = false;
    mLastTime = nglTime();
  }
  
  if (res && mHideScrollBars)
  {
    ShowScrollBars(autoHideScrollbars);
  }
  
  return res;
}

bool nuiScrollView::MouseUnclicked(const nglMouseInfo& rInfo)
{
  if (!mLeftClick)
    return false;
  
  Dragged(rInfo);
  //  mLeftClick--;
  //  NGL_OUT("nuiScrollView::MouseUnclicked LeftClick: %d\n", mLeftClick);
  
  if (mLeftClick == 1) ///< will be ungrabbed
  {
    nglTime now;
    double elapsed = now.GetValue() - mLastTime.GetValue();
    if (elapsed > 0.1)
    {
      mSpeedX = 0;
      mSpeedY = 0;
    }
    else
    {
      if (mForceNoHorizontal)
        mSpeedX = 0;
      if (mForceNoVertical)
        mSpeedY = 0;
      if (mSpeedX != 0 || mSpeedY != 0)
      {
        mTimerOn = true;
      }
    }
    
    if (mHideScrollBars)
    {
      HideScrollBars();
    }
  }
  return true;
}

bool nuiScrollView::MouseMoved(const nglMouseInfo& rInfo)
{
  if (!mLeftClick)
    return false;

  mMoved = true;

  nglTime now;
  double elapsed = now.GetValue() - mLastTime.GetValue();
  elapsed = MAX(0.01, elapsed);
  
  double vectX = mLastX - rInfo.X;
  double vectY = mLastY - rInfo.Y;
  
  double oldSpeedX = mSpeedX;
  double oldSpeedY = mSpeedY;
  
  mSpeedX = vectX / elapsed;
  mSpeedY = vectY / elapsed;
  
  double accelX = mSpeedX / oldSpeedX;
  double accelY = mSpeedY / oldSpeedY;
  
  //  NGL_OUT("Elapsed %f VectY %f SpeedY %f AccelY %f\n", elapsed, vectY, mSpeedY, accelY);
  
  mLastX = rInfo.X;
  mLastY = rInfo.Y;
  mLastTime = now;
  
  Dragged(rInfo);
  return true;
}

void nuiScrollView::Dragged(const nglMouseInfo& rInfo)
{
  if (!rInfo.Counterpart)
    return;
  
  float OldX = rInfo.Counterpart->X;
  float OldY = rInfo.Counterpart->Y;
  GlobalToLocal(OldX, OldY);
  nuiSize diffX = OldX - rInfo.X;
  nuiSize diffY = OldY - rInfo.Y;
  
  if (mpHorizontal && !mForceNoHorizontal)
  {
    GetRange(nuiHorizontal)->SetValue(mClickValueH + diffX);
    if (mSmoothScrolling)
    {
      nuiSize maxExtraSize = GetRange(nuiHorizontal)->GetPageSize() * EXTRA_OUT_SIZE_RATIO;
      nuiSize offset = mClickValueH + diffX;
      
      if (offset < 0)
      {
        nuiSize extra = MIN(-offset, 2 * maxExtraSize);
        extra *= 0.5;
        
        mXOffset = -extra;
      }
      else if (offset > GetRange(nuiHorizontal)->GetMaximum() - GetRange(nuiHorizontal)->GetPageSize())
      {
        nuiSize extra = MIN(offset - (GetRange(nuiHorizontal)->GetMaximum() - GetRange(nuiHorizontal)->GetPageSize()), 2 * maxExtraSize);
        extra *= 0.5;
        
        mXOffset = GetRange(nuiHorizontal)->GetMaximum() - GetRange(nuiHorizontal)->GetPageSize() + extra;
      }
      else
      {
        mXOffset = offset;
      }
      mTimerOn = true;
    }
  }
  
  if (mpVertical && !mForceNoVertical)
  {
    GetRange(nuiVertical)->SetValue(mClickValueV + diffY);
    if (mSmoothScrolling)
    {
      nuiSize maxExtraSize = GetRange(nuiVertical)->GetPageSize() * EXTRA_OUT_SIZE_RATIO;
      
      nuiSize offset = mClickValueV + diffY;
      
      if (offset < 0)
      {
        nuiSize extra = MIN(-offset, 2 * maxExtraSize);
        extra *= 0.5;
        
        mYOffset = -extra;
      }
      else if (offset > GetRange(nuiVertical)->GetMaximum() - GetRange(nuiVertical)->GetPageSize())
      {
        nuiSize extra = MIN(offset - (GetRange(nuiVertical)->GetMaximum() - GetRange(nuiVertical)->GetPageSize()), 2 * maxExtraSize);
        extra *= 0.5;
        
        mYOffset = GetRange(nuiVertical)->GetMaximum() - GetRange(nuiVertical)->GetPageSize() + extra;
      }
      else
      {
        mYOffset = offset;
      }
      mTimerOn = true;
    }
  }
}

bool nuiScrollView::MouseCanceled(const nglMouseInfo& rInfo)
{
  mLeftClick = 0;
  //  mLeftClick--;
  //  NGL_OUT("nuiScrollView::MouseCanceled LeftClick: %d\n", mLeftClick);
}

bool nuiScrollView::CallPreMouseClicked(const nglMouseInfo& rInfo)
{
  if (!mDragEnabled)
    return false;
  
  if (mLeftClick)
    return false;
  
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mTouch = rInfo;
    mTouched = true;
    mMoved = false;
    if (mSpeedX != 0 || mSpeedY != 0)
    {
      if (StealMouseEvent(rInfo))
      {
        mTimerOn = false;
        mSpeedX = 0;
        mSpeedY = 0;
        
        mTouched = false;
        SetXPos(mXOffset);
        SetYPos(mYOffset);
        mClickValueH = mXOffset;
        mClickValueV = mYOffset;
        
        mLastTime = nglTime();
        return true;
      }
    }
  }
  return false;
}

bool nuiScrollView::CallPreMouseUnclicked(const nglMouseInfo& rInfo)
{
  if (!mDragEnabled)
    return false;
  
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mTouched = false;
  }
  return false;
}

bool nuiScrollView::CallPreMouseMoved(const nglMouseInfo& rInfo)
{
  if (!mDragEnabled)
    return false;
  
  if (mTouched && !mLeftClick)
  {
    float x = 0;
    if (GetEnableHorizontalScroll())
      x = mTouch.X - rInfo.X;
    float y = 0;
    if (GetEnableVerticalScroll())
      y = mTouch.Y - rInfo.Y;
    
    float dist = sqrt(x * x + y * y);

    if (dist > 10 && StealMouseEvent(rInfo) && (rInfo.Counterpart != nullptr))
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
      return true;
    }
  }
  return false;
}

bool nuiScrollView::MouseGrabbed(nglTouchId Id)
{
  mLeftClick++;
  return true;
}

bool nuiScrollView::MouseUngrabbed(nglTouchId Id)
{
  if (!HasGrab(Id))
    return false;
//  NGL_ASSERT(mLeftClick > 0);

  if (mLeftClick > 0)
  {
    mLeftClick--;
  }
  
  return true;
}

nuiScrollBar* nuiScrollView::GetScrollBar(nuiOrientation Orientation)
{
  return Orientation == nuiHorizontal ? mpHorizontal : mpVertical;
}

nuiRange* nuiScrollView::GetRange(nuiOrientation Orientation)
{
  if (Orientation == nuiHorizontal)
  {
    if (mpHorizontal)
      return &mpHorizontal->GetRange();
  }
  else
  {
    if (mpVertical)
      return &mpVertical->GetRange();
  }
  return NULL;
}

void nuiScrollView::SetHIncrement(nuiSize hIncrement)
{
  mHIncrement = hIncrement;
  mpHorizontal->GetRange().SetIncrement(mHIncrement);
}
nuiSize nuiScrollView::GetHIncrement() const
{
  return mHIncrement;
}


void nuiScrollView::SetVIncrement(nuiSize vIncrement)
{
  mVIncrement = vIncrement;
  mpVertical->GetRange().SetIncrement(mVIncrement);
}
nuiSize nuiScrollView::GetVIncrement() const
{
  return mVIncrement;
}

void nuiScrollView::OnSmoothScrolling(const nuiEvent& rEvent)
{
  mSpeedX *= INERTIA_BRAKES;
  mSpeedY *= INERTIA_BRAKES;
  
  const float MINSPEED = 0.1f;
  if (fabs(mSpeedX) < MINSPEED)
    mSpeedX = 0;
  
  if (fabs(mSpeedY) < MINSPEED)
    mSpeedY = 0;
  
  if (!mTimerOn)
    return;
  
  const float XOffset = (float)mpHorizontal->GetRange().GetValue();
  const float YOffset = (float)mpVertical->GetRange().GetValue();
  
  if (!mLeftClick)
  {
    mTimerOn = false;
    nglTime now;
    double elapsed = now.GetValue() - mLastTime.GetValue();
    mLastTime = now;
    
    bool isXOffsetting = (mXOffset < 0 || mXOffset > (GetIdealRect().GetWidth()-GetRect().GetWidth()));
    if (isXOffsetting)
      mSpeedX *= INERTIA_OFFSET_BRAKES;
    
    double speedX = (mSpeedX * elapsed);
    float xdiff = XOffset - mXOffset;
    
    if (std::fabs(speedX) > MINSPEED) ///< inertia
    {
      mXOffset += speedX;
      SetXPos(mXOffset);
      mTimerOn = true;
    }
    else if (std::fabs(xdiff) > 1) ///< smooth
    {
      xdiff *= NUI_SMOOTH_SCROLL_RATIO;
      mXOffset += xdiff;
      mSpeedX = 0;
      mTimerOn = true;
    }
    else
    {
      xdiff = 0;
      mXOffset = XOffset;
      mSpeedX = 0;
    }
    
    
    ////////////////////
    
    bool isYOffsetting = (mYOffset < 0 || mYOffset > (GetIdealRect().GetHeight()-GetRect().GetHeight()));
    if (isYOffsetting)
      mSpeedY *= INERTIA_OFFSET_BRAKES;
    
    double speedY = (mSpeedY * elapsed);
    float ydiff = YOffset - mYOffset;
    
    if (std::fabs(speedY) > MINSPEED) ///< inertia
    {
      mYOffset += speedY;
      SetYPos(mYOffset);
      mTimerOn = true;
    }
    else if (std::fabs(ydiff) > 1) ///< smooth
    {
      ydiff *= NUI_SMOOTH_SCROLL_RATIO;
      mYOffset += ydiff;
      mSpeedY = 0;
      mTimerOn = true;
    }
    else
    {
      ydiff = 0;
      mYOffset = YOffset;
      mSpeedY = 0;
    }
    
    if (mXOffset == XOffset && mYOffset == YOffset && mSpeedX == 0 && mSpeedY == 0)
    {
      mTimerOn = false;
    }
  }
  
  if (!mTimerOn)
  {
    mSpeedX = 0;
    mSpeedY = 0;
    mXOffset = XOffset;
    mYOffset = YOffset;
  }
  
//  UpdateLayout();
  SetRect(mRect);
  
  OffsetsChanged();
}

void nuiScrollView::EnableSmoothScrolling(bool set)
{
  mSmoothScrolling = set;
  mXOffset = mpHorizontal->GetRange().GetValue();
  mYOffset = mpVertical->GetRange().GetValue();
  OffsetsChanged();
}

bool nuiScrollView::IsSmoothScrollingEnabled() const
{
  return mSmoothScrolling;
}

void nuiScrollView::EnableMinimalResize(bool set)
{
  mMinimalResize = set;
}

bool nuiScrollView::IsMinimalResizeEnabled() const
{
  return mMinimalResize;
}

void nuiScrollView::OnHThumbPressed(const nuiEvent& rEvent)
{
  mHThumbPressed = true;
}

void nuiScrollView::OnHThumbDepressed(const nuiEvent& rEvent)
{
  mHThumbPressed = false;
}

void nuiScrollView::OnVThumbPressed(const nuiEvent& rEvent)
{
  mVThumbPressed = true;
}

void nuiScrollView::OnVThumbDepressed(const nuiEvent& rEvent)
{
  mVThumbPressed = false;
}

nuiSize nuiScrollView::GetXOffset() const
{
  return mXOffset;
}

nuiSize nuiScrollView::GetYOffset() const
{
  return mYOffset;
}

void nuiScrollView::SetXOffset(nuiSize offset)
{
  if (mXOffset == offset)
    return;
  mXOffset = offset;
  UpdateLayout();
}

void nuiScrollView::SetYOffset(nuiSize offset)
{
  if (mYOffset == offset)
    return;
  mYOffset = offset;
  UpdateLayout();
}

nuiSize nuiScrollView::GetXPos() const
{
  return mpHorizontal->GetRange().GetValue();
}

nuiSize nuiScrollView::GetYPos() const
{
  return mpVertical->GetRange().GetValue();
}

void nuiScrollView::SetXPos(nuiSize pos)
{
  mpHorizontal->GetRange().SetValue(pos);
}

void nuiScrollView::SetYPos(nuiSize pos)
{
  mpVertical->GetRange().SetValue(pos);
}


void nuiScrollView::SetForceNoSmartScroll(bool set)
{
  mForceNoSmartScroll = set;
}
bool nuiScrollView::GetForceNoSmartScroll(bool set) const
{
  return mForceNoSmartScroll;
}

void nuiScrollView::ShowScrollBars(bool autoHide)
{
  mpHideAnimH->Stop();
  mpHideAnimV->Stop();
  
  if (!mpShowAnimV->IsPlaying() && mpVertical->GetAlpha() != 1)
  {
    mpShowAnimV->SetTime(0);
    mpShowAnimV->Play();
  }
  
  if (!mpShowAnimH->IsPlaying() && mpHorizontal->GetAlpha() != 1)
  {
    mpShowAnimH->SetTime(0);
    mpShowAnimH->Play();
  }
  
  if (autoHide)
  {
    mpHideTimer->Stop();
    mpHideTimer->Start(false/*immediate*/, true/*reset*/);
  }
}

void nuiScrollView::HideScrollBars()
{
  if (mpShowAnimH)
  {
    mpShowAnimH->Stop();
    mpShowAnimV->Stop();
    
    mpHideAnimV->Stop();
    mpHideAnimV->SetTime(0);
    mpHideAnimV->Play();
    
    mpHideAnimH->Stop();
    mpHideAnimH->SetTime(0);
    mpHideAnimH->Play();
  }
}

void nuiScrollView::SetHideScrollBars(bool hide)
{
  mHideScrollBars = hide;
  
  if (mHideScrollBars)
  {
    float inDuration = 0.4;
    float outDuration = 0.7;
    mpShowAnimH = new nuiFadeInWidgetAnim(mpHorizontal, inDuration, true);
    mpShowAnimV = new nuiFadeInWidgetAnim(mpVertical, inDuration, true);
    mpHideAnimH = new nuiFadeOutWidgetAnim(mpHorizontal, outDuration, true);
    mpHideAnimV = new nuiFadeOutWidgetAnim(mpVertical, outDuration, true);
    mpHorizontal->SetAlpha(mLeftClick ? 1 : 0);
    mpVertical->SetAlpha(mLeftClick ? 1 : 0);
  }
  else if (!mpShowAnimH)
  {
    mpShowAnimH->Release();
    mpShowAnimV->Release();
    mpHideAnimH->Release();
    mpHideAnimV->Release();
    mpShowAnimH = NULL;
    mpShowAnimV = NULL;
    mpHideAnimH = NULL;
    mpHideAnimV = NULL;
    mpHorizontal->SetAlpha(1);
    mpVertical->SetAlpha(1);
  }
  
}

bool nuiScrollView::GetHideScrollBars()
{
  return mHideScrollBars;
}

void nuiScrollView::OnHideTick(const nuiEvent& rEvent)
{
  mpHideTimer->Stop();
  HideScrollBars();
}


void nuiScrollView::EnableDrag(bool enable)
{
  mDragEnabled = enable;
}

bool nuiScrollView::IsDragEnabled()
{
  return mDragEnabled;
}


void nuiScrollView::ActivateMobileMode()
{
  EnableDrag(true);
  SetHideScrollBars(true);
  
  mpHorizontal->SetBackgroundDeco(NULL);
  mpVertical->SetBackgroundDeco(NULL);
  
  mpHorizontal->SetForegroundDecoName(_T("nuiDefaultDecorationMobileScrollbarHandle"));
  mpVertical->SetForegroundDecoName(_T("nuiDefaultDecorationMobileScrollbarHandle"));
  SetBarSize(6);
}

void nuiScrollView::ActivateHotRect(bool hset, bool vset)
{
  mHorizontalHotRectActive = hset;
  mVerticalHotRectActive = vset;
}

void nuiScrollView::ActivateHotRect(bool set)
{
  ActivateHotRect(set, set);
}

bool nuiScrollView::IsHorizontalHotRectActive() const
{
  return mHorizontalHotRectActive;
}

bool nuiScrollView::IsVerticalHotRectActive() const
{
  return mVerticalHotRectActive;
}


