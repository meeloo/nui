/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include <iterator>

#define PARTIAL_REDRAW_DEFAULT true

#if 0 //defined(_MULTI_TOUCHES_) && defined(_DEBUG_)
# define NGL_TOUCHES_DEBUG(X) (X)
# define _NGL_DEBUG_TOUCHES_
#else//!_MULTI_TOUCHES_
# define NGL_TOUCHES_DEBUG(X)
#endif//!_MULTI_TOUCHES_

#ifdef _NGL_DEBUG_TOUCHES_
#define PRINT_GRAB_IDS()\
{\
  for (nuiGrabMap::const_iterator it = mpGrab.begin(); it != mpGrab.end(); ++it)\
  {\
    NGL_OUT(_T("[%d] -> "), it->first);\
    if (!(it->second)) {\
      NGL_OUT(_T("NULL\n"));\
    } else {\
      NGL_OUT(_T("%s\n"),\
      it->second->GetObjectClass().GetChars());\
    }\
  }\
}
#else
#define PRINT_GRAB_IDS() {}
#endif

// Simple helper class that can prevents deleting a while subtree while doing event propagation (maintely for grab and ungrab)
class WidgetBranchGuard
{
public:
  WidgetBranchGuard(nuiWidgetPtr pWidget)
  {
    NGL_ASSERT(pWidget);
    nuiTopLevel* pTop = pWidget->GetTopLevel();
    NGL_ASSERT(pTop);
    while (pWidget != pTop)
    {
      pWidget->Acquire();
      mBranch.push_back(pWidget);
      pWidget = pWidget->GetParent();
    }
  }

  ~WidgetBranchGuard()
  {
    for (auto w : mBranch)
    {
      w->Release();
    }
  }

private:
  std::vector<nuiWidgetPtr> mBranch;
};



#ifndef DISABLE_TOOLTIP
class nuiToolTip : public nuiWidget
{
public:
  nuiToolTip();

  virtual bool SetRect(const nuiRect& rRect);
  virtual nuiRect CalcIdealSize();

  virtual bool Draw(nuiDrawContext* pContext);

  void SetText(const nglString& rText);
  nglString GetText() const;

  virtual void BroadcastInvalidateLayout(nuiWidgetPtr pSender, bool BroadCastOnly)
  {
    Invalidate();
    mNeedSelfLayout = true;
    mNeedIdealRect = true;
    mNeedRender = true;
    if (mpBackingLayer)
      GetRenderThread()->InvalidateLayerContents(mpBackingLayer);
    DebugRefreshInfo();
  }

protected:
  virtual ~nuiToolTip();
  nuiLabel* mpLabel;
};

nuiToolTip::nuiToolTip()
 : nuiWidget()
{
  SetObjectClass(_T("nuiToolTip"));
  SetObjectName(_T("ToolTipContainer"));
  mpLabel = new nuiLabel(nglString::Empty);
  mpLabel->SetWrapping(true);
  mpLabel->SetObjectName(_T("ToolTipLabel"));
  AddChild(mpLabel);
}

nuiToolTip::~nuiToolTip()
{
}

#define NUI_TOOLTIP_MARGIN_X 3
#define NUI_TOOLTIP_MARGIN_Y 2

bool nuiToolTip::SetRect(const nuiRect& rRect)
{
  nuiWidget::SetSelfRect(rRect);
  nuiRect r(rRect.Size());
 
  r.Grow(-NUI_TOOLTIP_MARGIN_X, -NUI_TOOLTIP_MARGIN_Y);

  IteratorPtr pIt;
  for (pIt = GetFirstChild(true); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->GetIdealRect();
    pItem->SetLayout(r);
  }
  delete pIt;

  return true;
}

nuiRect nuiToolTip::CalcIdealSize()
{
  nuiRect ideal(nuiWidget::CalcIdealSize());
  ideal.Grow(NUI_TOOLTIP_MARGIN_X, NUI_TOOLTIP_MARGIN_Y);
  return ideal;
}

bool nuiToolTip::Draw(nuiDrawContext* pContext)
{
  nuiRect rect = GetRect().Size();

  pContext->EnableBlending(true);
  pContext->SetBlendFunc(nuiBlendTransp);
  pContext->SetStrokeColor(GetColor(eToolTipBorder));
  pContext->SetFillColor(GetColor(eToolTipBg));
  pContext->DrawRect(rect,eStrokeAndFillShape);
  pContext->EnableBlending(false);

  DrawChildren(pContext);

  return true;
}

void nuiToolTip::SetText(const nglString& rText)
{
  mpLabel->SetText(rText);
  mpLabel->SetTextColor(GetColor(eToolTipFg));
}

nglString nuiToolTip::GetText() const
{
  if (!mpLabel)
    return nglString::Empty;
  return mpLabel->GetText();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
nuiTopLevel::nuiTopLevel(const nglPath& rResPath)
  : nuiWidget(),
#ifndef DISABLE_TOOLTIP
    mToolTipTimerOn(0.5f),
    mToolTipTimerOff(5.0f),
#endif
    mpDraggedWidget(NULL),
    mpWatchedWidget(NULL),
    mFillTrash(false),
    mpDrawContext(NULL),
    mpRenderThread(NULL),
    mTopLevelSink(this),
    mIsDrawing(false),
    mIsRendering(false),
    mpCSS(NULL)
{
  mNeedInvalidateOnSetRect = true;
  mClearBackground = true;
  SetObjectClass(_T("nuiTopLevel"));

  mResPath = rResPath;

#ifndef DISABLE_TOOLTIP
  mToolTipDelayOn = .5f;
  mToolTipDelayOff = 5.0f;
  
  mDisplayToolTip = false;
  mpToolTipSource = NULL;
  mpToolTipLabel = new nuiToolTip();
  AddChild(mpToolTipLabel);

  mTopLevelSink.Connect(mToolTipTimerOn.Tick, &nuiTopLevel::ToolTipOn);
  mTopLevelSink.Connect(mToolTipTimerOff.Tick, &nuiTopLevel::ToolTipOff);
#endif
//  mTopLevelSink.Connect(nuiAnimation::AcquireTimer()->Tick, &nuiTopLevel::OnMessageQueueTick);

  mpInfoLabel = NULL;

  mpGrab.clear();
  mpGrabAcquired.clear();
  mMouseInfo.TouchId = -1;
  mpFocus = NULL;
  mpUnderMouse = NULL;

  mLastClickedButton = nglMouseInfo::ButtonNone;

  EnablePartialRedraw(PARTIAL_REDRAW_DEFAULT);

  SetWantKeyboardFocus(true);
  SetFocusVisible(false);
  SetMouseCursor(eCursorArrow);

  mDirtyWidgets.insert(this);
  Acquire();
}

nuiTopLevel::~nuiTopLevel()
{
  if (GetDrawToLayer())
    GetRenderThread()->SetLayerTree(nullptr);
  CheckValid();
  Exit();
}

void nuiTopLevel::Exit()
{
  CheckValid();

//   mTopLevelSink.Disconnect(&nuiMainWindow::ToolTipOn);
//   mTopLevelSink.Disconnect(&nuiMainWindow::ToolTipOff);
  mTopLevelSink.DisconnectAll();
#ifndef DISABLE_TOOLTIP
  mpToolTipLabel = NULL;
  mpToolTipSource = NULL;
  mpToolTipLabel = NULL;
#endif
  mpGrab.clear();
  mpGrabAcquired.clear();
  mpFocus = NULL;
  mpUnderMouse = NULL;

  nuiWidgetList wlist(mpChildren);
  nuiWidgetList::iterator wit = wlist.begin();
  
  while (wlist.end() != wit)
  {
    nuiWidgetPtr pItem = *wit;
    pItem->CallOnTrash();
    nuiWidget* pCtr = pItem->GetParent();
    if (pCtr)
      pCtr->DelChild(pItem);
//    del//ete pItem;
    
    ++wit;
  }

  mpChildren.clear();
  
  if (mpDrawContext)
  {
    delete mpDrawContext;
    mpDrawContext = nullptr;
  }

  std::map<nglString, nuiHotKey*>::iterator it = mHotKeys.begin();
  std::map<nglString, nuiHotKey*>::iterator end = mHotKeys.end();

  while (it != end)
  {
    nuiHotKey* pHotKey = it->second;
    delete pHotKey;
    ++it;
  }

//  nuiAnimation::ReleaseTimer();
}

void nuiTopLevel::DisconnectWidget(nuiWidget* pWidget)
{
  CheckValid();
  AdviseObjectDeath(pWidget);
}

void nuiTopLevel::AdviseObjectDeath(nuiWidgetPtr pWidget)
{
  CheckValid();
  if (mpWatchedWidget == pWidget)
    mpWatchedWidget = NULL;
  
  pWidget->StopAutoDraw();
  mHoveredWidgets.erase(pWidget);

  mDirtyWidgets.erase(pWidget);
  
  nuiGrabMap::iterator it2 = mpGrab.begin();
  while (it2 != mpGrab.end())
  {
    if (pWidget == it2->second)
    {
      it2->second = NULL;
      auto found = mpGrabAcquired.find(it2->first);
      if (found != mpGrabAcquired.end())
      {
        mpGrabAcquired.erase(found);
        NGL_TOUCHES_DEBUG( NGL_OUT("Released acquired grab (in AdviseObjectDeath)\n"));
      }
    }
    ++it2;
  }
  DumpGrabMap(__LINE__);
  
  if (mpFocus == pWidget)
  {
    mpFocus = NULL;
    EndTextInput();
  }
  if (mpUnderMouse == pWidget)
  {
    mpUnderMouse = NULL;
    GlobalHoverChanged();
  }
#ifndef DISABLE_TOOLTIP
  if (mpToolTipSource == pWidget)
    ReleaseToolTip(pWidget);
#endif

  mCSSWidgets.erase(pWidget);
  
  {
    // Update the tab order containers:
    SetTabForward(pWidget, NULL, false);
    SetTabBackward(pWidget, NULL, false);
    
    {
      std::set<nuiWidgetPtr> ForwardSources;
      GetTabForwardSources(pWidget, ForwardSources);
      std::set<nuiWidgetPtr>::iterator it = ForwardSources.begin();
      std::set<nuiWidgetPtr>::iterator end = ForwardSources.end();
      while (it != end)
      {
        nuiWidgetPtr pSource = *it;
        SetTabForward(pSource, NULL, false);
        ++it;
      }
    }

    {
      std::set<nuiWidgetPtr> BackwardSources;
      GetTabBackwardSources(pWidget, BackwardSources);
      std::set<nuiWidgetPtr>::iterator it = BackwardSources.begin();
      std::set<nuiWidgetPtr>::iterator end = BackwardSources.end();
      while (it != end)
      {
        nuiWidgetPtr pSource = *it;
        SetTabBackward(pSource, NULL, false);
        ++it;
      }
    }
    
  }
  
}

void nuiTopLevel::AdviseSubTreeDeath(nuiWidgetPtr pWidget)
{
  CheckValid();
  IteratorPtr pIt;
  nuiWidget* pContainer = dynamic_cast<nuiWidget*> (pWidget);
  if (pContainer)
  {  
    for (pIt = pContainer->GetFirstChild(true); pIt && pIt->IsValid(); pContainer->GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      
      if (mpBackingLayer)
      {
        nuiLayer* pLayer = pItem->GetLayer();
        if (pLayer)
        {
          mpBackingLayer->DelChild(pLayer);
        }
      }
      if (pItem)
        AdviseSubTreeDeath(pItem);
    }
    delete pIt;
  }

  AdviseObjectDeath(pWidget);
}

nuiDrawContext* nuiTopLevel::GetDrawContext()
{
  CheckValid();
  if (mpDrawContext)
    return mpDrawContext;
  
  nuiRect rect = GetRect().Size();
  mpDrawContext = nuiDrawContext::CreateDrawContext(rect, GetNGLContext()->GetPainter());
  nuiRef<nuiMetaPainter> pMeta = nuiNewRef<nuiMetaPainter>();
  mpDrawContext->SetPainter(pMeta);
  return mpDrawContext;
}

nuiRenderThread* nuiTopLevel::GetRenderThread()
{
  CheckValid();
  if (mpRenderThread)
    return mpRenderThread;
  
  nglContext* pContext = GetNGLContext();
  nuiPainter* pPainter = pContext->GetPainter();

  nuiRect rect = GetRect().Size();
  nuiDrawContext* pDrawContext = nuiDrawContext::CreateDrawContext(rect, GetNGLContext()->GetPainter());
//  nuiDrawContext* pDrawContext = GetDrawContext();

  mpRenderThread = new nuiRenderThread(pContext, pDrawContext, pPainter, nuiMakeDelegate(this, &nuiTopLevel::OnRenderingDone));
  return mpRenderThread;
}

void nuiTopLevel::OnRenderingDone(nuiRenderThread* pThread, bool Result)
{
  NGL_ASSERT(mpRenderThread == pThread);
  mIsRendering = false;
}

void nuiTopLevel::SetDrawContext(nuiDrawContext* pDrawContext)
{
  CheckValid();
  if (mpDrawContext)
    delete mpDrawContext;

  mpDrawContext = pDrawContext;
}

nuiTopLevel* nuiTopLevel::GetTopLevel() const
{
  CheckValid();
  void* Self = (void*)this;
  return (nuiTopLevel*)Self;
}

nuiRenderer nuiTopLevel::mRenderer = eOpenGL2;

void nuiTopLevel::SetRenderer(nuiRenderer Renderer)
{
  mRenderer = Renderer;
}

nuiRenderer nuiTopLevel::GetRenderer()
{
  return mRenderer;
}


bool nuiTopLevel::DispatchGrab(nuiWidgetPtr pWidget)
{
  CheckValid();
  return Grab(pWidget);
}

bool nuiTopLevel::DispatchUngrab(nuiWidgetPtr pWidget)
{
  CheckValid();
  return Ungrab(pWidget);
}

bool nuiTopLevel::DispatchHasGrab(nuiWidgetPtr pWidget)
{
  nuiAutoRef;
  CheckValid();
  return HasGrab(pWidget);
}

bool nuiTopLevel::DispatchHasGrab(nuiWidgetPtr pWidget, nglTouchId TouchId)
{
  nuiAutoRef;
  return pWidget == GetGrab(TouchId);
}


bool nuiTopLevel::Grab(nuiWidgetPtr pWidget)
{
  return Grab(pWidget, mMouseInfo);
}

bool nuiTopLevel::Grab(nuiWidgetPtr pWidget, const nglMouseInfo& rInfo)
{
  nuiAutoRef;
  CheckValid();
  NGL_TOUCHES_DEBUG( NGL_OUT(_T("nuiTopLevel::Grab 0x%x\n"), pWidget) );

///< some widgets acquire the grab on creation, which is pretty unpleasant (this hack is quite bad)
  nglTouchId touchId = rInfo.TouchId;
  if (touchId < 0)
  {
    return false;
  }
  
  if (pWidget && !pWidget->AcceptsMultipleGrabs() ///< Checks if the Widget has been grabbed by another touches
      && HasGrab(pWidget))
  {

  NGL_TOUCHES_DEBUG( NGL_OUT(_T("TouchId[%d] "), mMouseInfo.TouchId) );
  NGL_TOUCHES_DEBUG( NGL_OUT(_T("%s of type %s already grabbed from other touch(es)\n"), 
          pWidget->GetObjectName().GetChars(),
          pWidget->GetObjectClass().GetChars()) );

    return false;
  }
  if (pWidget)
  {
    NGL_TOUCHES_DEBUG( NGL_OUT(_T("TouchId[%d] "), mMouseInfo.TouchId) );
    NGL_TOUCHES_DEBUG( NGL_OUT(_T("Grab from %s of type %s\n"),
            pWidget->GetObjectName().GetChars(), pWidget->GetObjectClass().GetChars()) );
  }

  nuiWidgetPtr pGrab = GetGrab(touchId);
  
  if (pGrab)
  {
    pGrab->MouseUngrabbed(touchId);
  }

  pGrab = pWidget;
  mpGrab[touchId] = pWidget;
  DumpGrabMap(__LINE__);
  
  if (pGrab)
    pGrab->MouseGrabbed(touchId);

#ifndef DISABLE_TOOLTIP
  if (pGrab && pGrab->GetProperty("ToolTipOnGrab") == _T("true"))
  {
    mpToolTipSource = pGrab;
    SetToolTipOn(false);
  }
  else
    mDisplayToolTip = false;
#endif

  UpdateWidgetsCSS();
  return true;
}

bool nuiTopLevel::Ungrab(nuiWidgetPtr pWidget)
{
  return Ungrab(pWidget, mMouseInfo);
}

bool nuiTopLevel::Ungrab(nuiWidgetPtr pWidget, const nglMouseInfo& rInfo)
{
  nuiAutoRef;
  CheckValid();
  NGL_TOUCHES_DEBUG( NGL_OUT(_T("nuiTopLevel::Ungrab 0x%x\n"), pWidget) );
  if (pWidget)
  {
  //  NGL_TOUCHES_DEBUG( NGL_OUT(_T("TouchId[%d] "), mMouseInfo.TouchId) );
    NGL_TOUCHES_DEBUG( NGL_OUT(_T("Ungrab from %s of type %s\n"), pWidget->GetObjectName().GetChars(), pWidget->GetObjectClass().GetChars()) );
  }

#ifndef DISABLE_TOOLTIP
  nuiWidgetPtr pGrab = GetGrab(rInfo.TouchId);
  if (pGrab && pGrab->GetProperty("ToolTipOnGrab") == _T("true"))
  {
    mpToolTipSource = NULL;
    mDisplayToolTip = false;
  }
#endif

  Grab(NULL);

  UpdateWidgetsCSS();
  return true;
}

bool nuiTopLevel::CancelGrab()
{
  CheckValid();
  nuiAutoRef;
NGL_TOUCHES_DEBUG( NGL_OUT(_T("CancelGrab()\n")) );
  for (nuiGrabMap::const_iterator it = mpGrab.begin(); it != mpGrab.end(); ++it)
  {
    nglTouchId touchId = it->first;
    nuiWidget* pGrab = it->second;

    while (pGrab)
    {
#ifndef DISABLE_TOOLTIP
      if (pGrab->GetProperty("ToolTipOnGrab") == _T("true"))
      {
        mpToolTipSource = NULL;
        mDisplayToolTip = false;
      }
#endif
      pGrab->MouseUngrabbed(touchId);
      pGrab = NULL;
    }
  }
  mpGrab.clear();
  mpGrabAcquired.clear();
  DumpGrabMap(__LINE__);
  UpdateWidgetsCSS();
  return true;
}

bool nuiTopLevel::StealMouseEvent(nuiWidgetPtr pWidget, const nglMouseInfo& rInfo)
{
  NGL_ASSERT(pWidget);
  
  auto acquired = mpGrabAcquired.find(rInfo.TouchId);
  if (acquired != mpGrabAcquired.end() && acquired->second == true)
  {
    //NGL_OUT("Refused Grab requested by %s %s\n", pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars());
    return false;
  }
  
  nuiWidgetPtr oldgrab = GetGrab(rInfo.TouchId);
  if (oldgrab)
  {
    if (!oldgrab->RequestStolenMouse(rInfo))
    {
      return false;
    }
  }

  DispatchMouseCanceled(pWidget, rInfo);

  pWidget->MouseClicked(rInfo);
  Grab(pWidget, rInfo);
  return true;
}


bool nuiTopLevel::HasGrab(nuiWidgetPtr pWidget)
{
  CheckValid();

  nuiGrabMap::const_iterator end = mpGrab.end();
  for (nuiGrabMap::const_iterator it = mpGrab.begin(); it != end; ++it)
    if (it->second == pWidget)
      return true;
  return false;
}

nuiWidgetPtr nuiTopLevel::GetGrab(nglTouchId touchId) const
{
  CheckValid();
  nuiGrabMap::const_iterator it = mpGrab.find(touchId);
  if (it != mpGrab.end())
    return it->second;
  return NULL;
}

nuiWidgetPtr nuiTopLevel::GetGrab() const
{
  CheckValid();
  return GetGrab(mMouseInfo.TouchId);
}

bool nuiTopLevel::SetFocus(nuiWidgetPtr pWidget)
{
  CheckValid();
  nuiAutoRef;
  if (mpFocus == pWidget)
    return true;

  nuiWidget* pOldFocus = mpFocus;
  
  if (mpFocus)
    mpFocus->DispatchFocus(pWidget);
  if (pWidget)
    pWidget->DispatchFocus(pWidget);

  mpFocus = pWidget;

  if (pOldFocus)
    pOldFocus->Invalidate();
  if (mpFocus)
    mpFocus->Invalidate();
  
  if (!mpFocus)
    EndTextInput();
  
  if (mpFocus)
  {
    nuiRect hotrect;
    mpFocus->GetHotRect(hotrect);
    if (hotrect.GetWidth() == 0 && hotrect.GetHeight() == 0)
    {
      hotrect = mpFocus->GetVisibleRect();
    }
    mpFocus->SetHotRect(hotrect);
  }
  UpdateWidgetsCSS();
  return true;
}

nuiWidgetPtr nuiTopLevel::GetFocus() const
{
  CheckValid();
  return mpFocus;
}

#ifndef DISABLE_TOOLTIP
bool nuiTopLevel::ActivateToolTip(nuiWidgetPtr pWidget, bool Now)
{
  CheckValid();
  if (pWidget == this)
  {
    nglString tt = GetToolTip();
  
    if (tt.IsEmpty())
      return false;
  }

  mpToolTipSource = pWidget;
  mToolTipTimerOn.SetPeriod(mToolTipDelayOn);
  mToolTipTimerOn.Stop();
  mToolTipTimerOn.Start(Now);
  mDisplayToolTip = Now;
  if (Now)
  {
    ToolTipOn(NULL);
  }

  UpdateWidgetsCSS();
  return true;
}

bool nuiTopLevel::ReleaseToolTip(nuiWidgetPtr pWidget)
{
  CheckValid();
  if (mpToolTipSource == pWidget)
  {
    mToolTipTimerOn.Stop();
    mToolTipTimerOff.Stop();
    mDisplayToolTip = false;
    if (mpToolTipLabel)
    {
      mpToolTipLabel->SetVisible(false);
    }
    mpToolTipSource = NULL;
    //Invalidate();
    if (mpToolTipLabel)
      mpToolTipLabel->Invalidate();
  }

  return true;
}

void nuiTopLevel::SetToolTipOn(bool AutoStop)
{
  CheckValid();
  if (mpToolTipSource)
  {
    /*
    NGL_OUT(_T("ToolTipOn class %s / Text: %s\n"),
        mpToolTipSource->GetObjectClass().GetChars(),
        mpToolTipSource->GetToolTip().GetChars()
       );
       */
  }

  mDisplayToolTip = true;
  mToolTipTimerOff.SetPeriod(mToolTipDelayOff);
  mToolTipTimerOn.Stop();
  mToolTipTimerOff.Stop();
  if (AutoStop)
  {
    mToolTipTimerOff.Start(false);
  }
  //Invalidate();
  SetToolTipRect();
}

void nuiTopLevel::ToolTipOn(const nuiEvent& rEvent)
{
  CheckValid();
  if (mpToolTipSource)
  {
    /*
    NGL_OUT(_T("ToolTipOn class %s / Text: %s\n"),
        mpToolTipSource->GetObjectClass().GetChars(),
        mpToolTipSource->GetToolTip().GetChars()
       );
       */
  }

  mDisplayToolTip = true;
  mToolTipTimerOff.SetPeriod(mToolTipDelayOff);
  mToolTipTimerOn.Stop();
  mToolTipTimerOff.Stop();
  mToolTipTimerOff.Start(false);
  
  //Invalidate();
  SetToolTipRect();
}

void nuiTopLevel::ToolTipOff(const nuiEvent& rEvent)
{
  CheckValid();
  //NGL_OUT(_T("ToolTipOff\n"));
  mDisplayToolTip = false;
  mToolTipTimerOn.Stop();
  mToolTipTimerOff.Stop();
  //Invalidate();
  mpToolTipLabel->Invalidate();
}
#endif

bool nuiTopLevel::IsKeyDown (nglKeyCode Key) const
{
  CheckValid();
  return false;
}

void nuiTopLevel::CallTextCompositionStarted()
{
  nuiAutoRef;
  if (mpFocus)
    mpFocus->TextCompositionStarted();
  UpdateWidgetsCSS();
}

void nuiTopLevel::CallTextCompositionConfirmed()
{
  nuiAutoRef;
  if (mpFocus)
    mpFocus->TextCompositionConfirmed();
  UpdateWidgetsCSS();
}

void nuiTopLevel::CallTextCompositionCanceled()
{
  nuiAutoRef;
  if (mpFocus)
  {
    mpFocus->TextCompositionCanceled();
    UpdateWidgetsCSS();
  }
}

void nuiTopLevel::CallTextCompositionUpdated(const nglString& rString, int32 CursorPosition)
{
  nuiAutoRef;
  if (mpFocus)
  {
    mpFocus->TextCompositionUpdated(rString, CursorPosition);
    UpdateWidgetsCSS();
  }
}

nglString nuiTopLevel::CallGetTextComposition() const
{
  nuiAutoRef;
  if (mpFocus)
    return mpFocus->GetTextComposition();
  return nglString::Null;
}

void nuiTopLevel::CallTextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const
{
  nuiAutoRef;
  if (mpFocus)
  {
    mpFocus->TextCompositionIndexToPoint(CursorPosition, x, y);
    mpFocus->LocalToGlobal(x, y);
  }
}

bool nuiTopLevel::CallTextInput (const nglString& rUnicodeText)
{
  nuiAutoRef;
  CheckValid();
  if (mpFocus && mpFocus->IsEnabled())
  {
    if (mpFocus->DispatchTextInput(rUnicodeText))
    {
      UpdateWidgetsCSS();
      return true;
    }
  }
  else
  {
    if (DispatchTextInput(rUnicodeText))
    {
      UpdateWidgetsCSS();
      return true;
    }
  }
  
  UpdateWidgetsCSS();
  return false;
}

void nuiTopLevel::CallTextInputCancelled ()
{
  nuiAutoRef;
  CheckValid();
  if (mpFocus && mpFocus->IsEnabled())
  {
    mpFocus->DispatchTextInputCancelled();
  }
  else
  {
    DispatchTextInputCancelled();
  }
  UpdateWidgetsCSS();
}

////////////////// FOCUS UTILITY FUNCTIONS:
nuiWidgetPtr DeepSearchNextFocussableWidget(nuiWidgetPtr pWidget, bool TryThisNode)
{
  if (TryThisNode && pWidget->GetWantKeyboardFocus())
    return pWidget;
  
  nuiWidget* pContainer = dynamic_cast<nuiWidget*>(pWidget); // Is this a container?
  if (pContainer)
  {
    std::auto_ptr<nuiWidget::Iterator> pIt(pContainer->GetFirstChild(true));
    while (pIt->IsValid())
    {
      nuiWidget* pItem = pIt->GetWidget();
      
      nuiWidgetPtr pChild = DeepSearchNextFocussableWidget(pItem, true);
      if (pChild)
        return pChild;
      
      pContainer->GetNextChild(pIt.get());
    }
  }
  
  return NULL;
}

nuiWidgetPtr GetNextFocussableSibling(nuiWidgetPtr pWidget)
{
  nuiWidget* pParent = pWidget->GetParent();
  if (pParent)
  {
    std::auto_ptr<nuiWidget::Iterator> pIt(pParent->GetChildIterator(pWidget));
    pParent->GetNextChild(pIt.get());
    
    while (pIt->IsValid())
    {
      nuiWidgetPtr pSibling = pIt->GetWidget();
      nuiWidgetPtr pItem = DeepSearchNextFocussableWidget(pSibling, true);
      if (pItem)
        return pItem;
      
      pParent->GetNextChild(pIt.get());
    }
  }
  
  return NULL;
}

nuiWidgetPtr GetNextFocussableWidget(nuiWidgetPtr pWidget)
{
  nuiWidget* pItem = DeepSearchNextFocussableWidget(pWidget, false);
  if (pItem)
    return pItem;
  
  nuiWidgetPtr pNextWidget = pWidget;
  while (pNextWidget)
  {
    pItem = GetNextFocussableSibling(pNextWidget);
    if (pItem)
      return pItem;
    
    pNextWidget = pNextWidget->GetParent();
    if (dynamic_cast<nuiModalContainer*>(pNextWidget))
      return DeepSearchNextFocussableWidget(pNextWidget, true);
  }

  if (pWidget)
  {
    nuiTopLevel* pTop = pWidget->GetTopLevel();
    if (pTop != pWidget)
      return GetNextFocussableWidget(pTop);
  }
  return NULL;
}

nuiWidgetPtr DeepSearchPreviousFocussableWidget(nuiWidgetPtr pWidget, bool TryThisNode)
{
  nuiWidget* pContainer = dynamic_cast<nuiWidget*>(pWidget); // Is this a container?
  if (pContainer)
  {
    std::auto_ptr<nuiWidget::Iterator> pIt(pContainer->GetLastChild());
    while (pIt->IsValid())
    {
      nuiWidget* pItem = pIt->GetWidget();
      
      nuiWidgetPtr pChild = DeepSearchPreviousFocussableWidget(pItem, true);
      if (pChild)
        return pChild;
      
      pContainer->GetPreviousChild(pIt.get());
    }
  }
  
  if (TryThisNode && pWidget->GetWantKeyboardFocus())
    return pWidget;
  
  return NULL;
}

nuiWidgetPtr GetPreviousFocussableWidget(nuiWidgetPtr pWidget)
{
  nuiWidget* pItem = NULL;
  nuiWidget* pParent = pWidget->GetParent();
  if (pParent)
  {
    std::auto_ptr<nuiWidget::Iterator> pIt(pParent->GetChildIterator(pWidget));
    pParent->GetPreviousChild(pIt.get());
    while (pIt->IsValid())
    {
      pItem = pIt->GetWidget();
      nuiWidgetPtr pChild = DeepSearchPreviousFocussableWidget(pItem, true);
      if (pChild)
        return pChild;
      
      pParent->GetPreviousChild(pIt.get());
    }
    
    if (dynamic_cast<nuiModalContainer*> (pParent))
      return DeepSearchPreviousFocussableWidget(pParent, true);

    return GetPreviousFocussableWidget(pParent);
  }
  
  nuiTopLevel* pTop = pWidget->GetTopLevel();
  NGL_ASSERT(pTop);
  return DeepSearchPreviousFocussableWidget(pTop, true);
}
////////////////////////////////// 

bool nuiTopLevel::CallKeyDown (const nglKeyEvent& rEvent)
{
  CheckValid();
  nuiAutoRef;
  if (mpFocus)
  {
    if (mpFocus->IsEnabled())
    {
      if (mpFocus->DispatchKeyDown(rEvent, mHotKeyMask))
      {
        UpdateWidgetsCSS();
        return true;
      }
    }
  }
  else
  {
    if (DispatchKeyDown(rEvent, mHotKeyMask))
    {
      UpdateWidgetsCSS();
      return true;
    }
  }

  if (rEvent.mKey == NK_TAB)
  {
    nuiWidget* pFocus = mpFocus;
    if (!pFocus && GetChildrenCount() != 0)
      pFocus = GetChild(0);

    // The user wants to change the focussed widget
    nuiWidget* pNext = NULL;
    if (IsKeyDown(NK_LSHIFT) || IsKeyDown(NK_RSHIFT))
    {
      // Backward
      pNext = GetTabBackward(pFocus);
      if (!pNext && mpFocus->GetParent())
        pNext = GetPreviousFocussableWidget(pFocus);
    }
    else
    {
      // Forward
      pNext = GetTabForward(pFocus);
      if (!pNext && pFocus && pFocus->GetParent())
        pNext = GetNextFocussableWidget(pFocus);
    }
    
    if (pNext)
      pNext->Focus();
    
    UpdateWidgetsCSS();
    return true;
  }
  
  
  UpdateWidgetsCSS();
  return false;
}

bool nuiTopLevel::CallKeyUp (const nglKeyEvent& rEvent)
{
  CheckValid();
  nuiAutoRef;
  if (mpFocus && mpFocus->IsEnabled())
  {
    if (mpFocus->DispatchKeyUp(rEvent, mHotKeyMask))
    {
      UpdateWidgetsCSS();
      return true;
    }
  }
  else
  {
    if (DispatchKeyUp(rEvent, mHotKeyMask))
    {
      UpdateWidgetsCSS();
      return true;
    }
  }

  UpdateWidgetsCSS();
  return false;
}

const std::map<nglTouchId, nglMouseInfo>& nuiTopLevel::GetMouseStates() const
{
  return mMouseStates;
}

bool nuiTopLevel::CallMouseClick (nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  
  mMouseClickedEvents[rInfo.TouchId] = rInfo;
  mMouseStates[rInfo.TouchId] = rInfo;
  //NGL_OUT("Added %x\n", rInfo.TouchId);

  mMouseInfo.X = rInfo.X;
  mMouseInfo.Y = rInfo.Y;
  mMouseInfo.Buttons |= rInfo.Buttons;

NGL_TOUCHES_DEBUG( NGL_OUT(_T("nuiTopLevel::CallMouseClick X:%d Y:%d\n"), rInfo.X, rInfo.Y) );

  mMouseInfo.TouchId = rInfo.TouchId;

NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseClick [%d] BEGIN\n"), rInfo.TouchId) );

  nuiWidgetPtr pGrab = GetGrab();
  if (pGrab)
  {
    NGL_TOUCHES_DEBUG( NGL_OUT(_T("  destination %p %s %s\n"), pGrab, pGrab->GetObjectClass().GetChars(), pGrab->GetObjectName().GetChars()) );
    WidgetBranchGuard guard(pGrab);
    std::vector<nuiWidgetPtr> containers;
    nuiWidgetPtr pParent = pGrab->GetParent();
    while (pParent)
    {
      containers.push_back(pParent);
      pParent = pParent->GetParent();
    }
    
    bool hook = false;
    for (int32 i = containers.size() - 1; i >= 0 && !hook; i--)
    {
      nglMouseInfo info(rInfo);
      containers[i]->GlobalToLocal(info.X, info.Y);
      hook = containers[i]->CallPreMouseClicked(info);
    }    
		
    bool res = hook;
    if (pGrab->MouseEventsEnabled() && !hook)
    {
      res = pGrab->DispatchMouseClick(rInfo);
    }

PRINT_GRAB_IDS();
NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseClick [%d] END\n"), rInfo.TouchId) );

    UpdateWidgetsCSS();
    return res;
  }

  bool res = DispatchMouseClick(rInfo);

  nuiWidgetList widgets;
  GetChildren(rInfo.X, rInfo.Y, widgets, true);
  UpdateMouseCursor(widgets);
  if (rInfo.Buttons == nglMouseInfo::ButtonLeft || rInfo.Buttons == nglMouseInfo::ButtonRight)
    DispatchKeyboardFocus(widgets);

PRINT_GRAB_IDS();
NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseClick [%d] END2\n"), rInfo.TouchId) );

  UpdateWidgetsCSS();
	return res;
}

void nuiTopLevel::UpdateMouseCursor(const nuiWidgetList& rWidgets)
{
  CheckValid();
  nuiAutoRef;
  nuiWidgetPtr pGrab = GetGrab(mMouseInfo.TouchId);
  if (pGrab)
  {
    nuiMouseCursor cursor = pGrab->GetMouseCursor();
    if (cursor != eCursorDoNotSet)
    {
      SetMouseCursor(cursor);
      return;
    }
  }
  
  int32 i = 0;
  while (i < rWidgets.size())
  {
    nuiWidget* pWidget = rWidgets[i];
    nuiMouseCursor cursor = pWidget->GetMouseCursor();
    if (cursor != eCursorDoNotSet)
    {
      SetMouseCursor(cursor);
      return;
    }
    i++;
  }
  
  SetMouseCursor(GetMouseCursor());
}

void nuiTopLevel::DispatchKeyboardFocus(const nuiWidgetList& rWidgets)
{
  CheckValid();
  nuiAutoRef;
  int32 i = 0;
  
  while (i < rWidgets.size())
  {
    nuiWidget* pWidget = rWidgets.at(i);
    
    if (pWidget->GetWantKeyboardFocus())
    {
      // Focus this widget
      pWidget->Focus();
      UpdateWidgetsCSS();
      return;
    }
    
    if (pWidget->GetMuteKeyboardFocusDispatch())
    {
      // Stop looking for focussed widgets now
      UpdateWidgetsCSS();
      return;
    }
    
    i++;
  }
 
  if (!mpFocus)
    Focus();
  UpdateWidgetsCSS();
}

bool nuiTopLevel::CallMouseUnclick(nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
//  NGL_TOUCHES_DEBUG( NGL_OUT(_T("nuiTopLevel::CallMouseUnclick X:%d Y:%d\n"), rInfo.X, rInfo.Y) );

  // Update counterpart:
  std::map<nglTouchId, nglMouseInfo>::iterator it = mMouseClickedEvents.find(rInfo.TouchId);
  if (it != mMouseClickedEvents.end())
  {
    rInfo.Counterpart = &it->second;
    // Update state:
    auto i = mMouseStates.find(rInfo.TouchId);
    if (i != mMouseStates.end())
      mMouseStates.erase(i);

    //NGL_OUT("Removed %x\n", rInfo.TouchId);
  }

  {
    auto it = mpGrabAcquired.find(rInfo.TouchId);
    if (it != mpGrabAcquired.end())
    {
      mpGrabAcquired.erase(it);
    }
  }

  mMouseInfo.X = rInfo.X;
  mMouseInfo.Y = rInfo.Y;
  mMouseInfo.Buttons &= ~rInfo.Buttons;
  nglMouseInfo::Flags Buttons = rInfo.Buttons | (mLastClickedButton & nglMouseInfo::ButtonDoubleClick);

  mMouseInfo.TouchId = rInfo.TouchId;
NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseUnclick [%d] BEGIN\n"), rInfo.TouchId) );
  nuiWidgetPtr pGrab = GetGrab();
  if (pGrab)
  {
    NGL_TOUCHES_DEBUG( NGL_OUT(_T("  destination %p %s %s\n"), pGrab, pGrab->GetObjectClass().GetChars(), pGrab->GetObjectName().GetChars()) );
    WidgetBranchGuard guard(pGrab);
    NGL_ASSERT(!pGrab->IsTrashed(true));
    pGrab->Acquire();

    std::vector<nuiWidgetPtr> containers;
    nuiWidgetPtr pParent = pGrab->GetParent();
    while (pParent)
    {
      containers.push_back(pParent);
      pParent = pParent->GetParent();
    }
    
    bool hook = false;
    for (int32 i = containers.size() - 1; i >= 0 && !hook; i--)
    {
      nglMouseInfo info(rInfo);
      containers[i]->GlobalToLocal(info.X, info.Y);
      hook = containers[i]->CallPreMouseUnclicked(info);
    }

		bool res = hook;
    if (pGrab->MouseEventsEnabled() && !hook)
    {
      res = pGrab->DispatchMouseUnclick(rInfo);
    }
PRINT_GRAB_IDS();
NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseUnclick [%d] END\n"), rInfo.TouchId) );

    {
      auto it = mpGrab.find(rInfo.TouchId);
      if (it != mpGrab.end())
      {
        if (it->second)
        it->second->DispatchUngrab(it->second);
        mpGrab.erase(it);
      }
    }
    
    if (it != mMouseClickedEvents.end())
      mMouseClickedEvents.erase(it);

    NGL_TOUCHES_DEBUG( NGL_OUT("Released acquired grab1: Grabs %d - %d (from %p)\n", (uint32)mpGrabAcquired.size(), (uint32)mpGrab.size(), pGrab));
#ifdef _NGL_DEBUG_TOUCHES_
    if (pGrab)
    {
      NGL_OUT("  from widget: %s / %s\n", pGrab->GetObjectClass().GetChars(), pGrab->GetObjectName().GetChars());
    }
#endif

    pGrab->Release();

    UpdateWidgetsCSS();
    DumpGrabMap(__LINE__);
    return res ;
  }

  bool res = DispatchMouseUnclick(rInfo);
  
  nuiWidgetList widgets;
  GetChildren(rInfo.X, rInfo.Y, widgets, true);
  UpdateMouseCursor(widgets);
  
PRINT_GRAB_IDS();
NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseUnclick [%d] END\n"), rInfo.TouchId) );

  if (it != mMouseClickedEvents.end())
    mMouseClickedEvents.erase(it);
  NGL_TOUCHES_DEBUG( NGL_OUT("Released acquired grab2: Grabs %d - %d\n", (uint32)mpGrabAcquired.size(), (uint32)mpGrab.size()));
  UpdateWidgetsCSS();
  DumpGrabMap(__LINE__);
	return res;
}

void nuiTopLevel::UpdateHoverList(nglMouseInfo& rInfo)
{
  CheckValid();
  nuiWidgetPtr pGrab = GetGrab();
  if (pGrab)
    return;

  std::list<nuiWidget*> HoverList;
  std::set<nuiWidget*> HoverSet;
  HoverSet.insert(this);
  GetHoverList(rInfo.X, rInfo.Y, HoverSet, HoverList);
  
  // Old Hovered widgets:
  std::set<nuiWidget*> OldHover;
  std::insert_iterator<std::set<nuiWidget*> > oldit(OldHover, OldHover.begin());
  std::set_difference(mHoveredWidgets.begin(), mHoveredWidgets.end(), HoverSet.begin(), HoverSet.end(), oldit);

  {
    std::set<nuiWidget*>::iterator it = OldHover.begin();
    std::set<nuiWidget*>::iterator end = OldHover.end();
    
    while (it != end)
    {
      nuiWidget* pItem = *it;
      pItem->SetHover(false);
      ++it;
    }
  }
  
  // New Hovered widgets:
  std::set<nuiWidget*> NewHover;
  std::insert_iterator<std::set<nuiWidget*> > newit(NewHover, NewHover.begin());
  std::set_difference(HoverSet.begin(), HoverSet.end(), mHoveredWidgets.begin(), mHoveredWidgets.end(), newit);

  {
    std::set<nuiWidget*>::iterator it = NewHover.begin();
    std::set<nuiWidget*>::iterator end = NewHover.end();
    
    while (it != end)
    {
      nuiWidget* pItem = *it;
      pItem->SetHover(true);
      ++it;
    }
  }
  
  mHoveredWidgets = HoverSet;

#ifndef DISABLE_TOOLTIP
  // Update Tooltip:
  if (HoverList.empty())
    return;
  
  HoverList.reverse();
  std::list<nuiWidget*>::iterator tt = HoverList.begin();
  std::list<nuiWidget*>::iterator ttend = HoverList.end();

  //NGL_OUT("\nHover list\n");
  bool res = false;
  while ((tt != ttend) && !res)
  {
    nuiWidget* pWidget = *tt;
    res = pWidget->ActivateToolTip(pWidget);
    //NGL_OUT("%3ls - %s\n", YESNO(res), pWidget->GetObjectClass().GetChars());
    ++tt;
  }
  if (!res && mpToolTipSource)
    ReleaseToolTip(mpToolTipSource);
#endif
}


bool nuiTopLevel::CallMouseMove (nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
NGL_TOUCHES_DEBUG( NGL_OUT(_T("nuiTopLevel::CallMouseMove X:%d Y:%d\n"), rInfo.X, rInfo.Y) );

  // Update counterpart:
  std::map<nglTouchId, nglMouseInfo>::iterator it = mMouseClickedEvents.find(rInfo.TouchId);
  if (it != mMouseClickedEvents.end())
    rInfo.Counterpart = &it->second;
  // Update state:

  if (mMouseStates.find(rInfo.TouchId) != mMouseStates.end())
  {
    NGL_TOUCHES_DEBUG( NGL_OUT("Found %x\n", rInfo.TouchId) );
    // Only update the mouse state if there was an existing touch already registered.
    // On a desktop computer with a mouse, when you move the mouse of the window with no button down there is no existing state to update.
    mMouseStates[rInfo.TouchId] = rInfo;
  }

  mMouseInfo.X = rInfo.X;
  mMouseInfo.Y = rInfo.Y;

  mMouseInfo.TouchId = rInfo.TouchId;
  NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseMove [%d] BEGIN\n"), rInfo.TouchId) );

  nuiWidgetPtr pWidget = NULL;
  nuiWidgetPtr pWidgetUnder = NULL;

#ifndef DISABLE_TOOLTIP
  if (mToolTipTimerOn.IsRunning())
  {
    mToolTipTimerOn.Stop();
    mToolTipTimerOn.Start(false);
  }
#endif	
	nuiWidgetPtr pHandled = NULL;

  // If there is a grab on the mouse serve the grabbing widget and only this one:
  nuiWidgetPtr pGrab = GetGrab();
  if (pGrab)
  {
    NGL_TOUCHES_DEBUG( NGL_OUT(_T("  destination %p %s %s\n"), pGrab, pGrab->GetObjectClass().GetChars(), pGrab->GetObjectName().GetChars()) );
    WidgetBranchGuard guard(pGrab);
    std::vector<nuiWidgetPtr> containers;
    nuiWidgetPtr pParent = pGrab->GetParent();
    while (pParent)
    {
      containers.push_back(pParent);
      pParent = pParent->GetParent();
    }
    
    bool hook = false;
    for (int32 i = containers.size() - 1; i >= 0 && !hook; i--)
    {
      nglMouseInfo info(rInfo);
      containers[i]->GlobalToLocal(info.X, info.Y);
      hook = containers[i]->CallPreMouseMoved(info);
      if (hook)
        pHandled = containers[i];
    }
    
    //NGL_OUT(_T("grabbed mouse move on '%s' / '%s'\n"), mpGrab->GetObjectClass().GetChars(), mpGrab->GetObjectName().GetChars());
    NGL_ASSERT(mpGrab[mMouseInfo.TouchId]);
    if (mpGrab[mMouseInfo.TouchId]->MouseEventsEnabled() && !hook)
    {
      pHandled = pGrab->DispatchMouseMove(rInfo);
    }
    
    nuiWidgetPtr pChild = NULL;
    // There might be no grab object left after the mouse move event!
    if (pGrab) 
      pChild = pGrab;
    else             
      pChild = GetChild((nuiSize)rInfo.X, (nuiSize)rInfo.Y);

    NGL_ASSERT(pChild);

    // Set the mouse cursor to the right object:
    nuiWidgetList widgets;
    GetChildren(rInfo.X, rInfo.Y, widgets, true);
    UpdateMouseCursor(widgets);

#ifndef DISABLE_TOOLTIP
    SetToolTipRect();
#endif

    NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseMove [%d] END1\n"), rInfo.TouchId) );
    UpdateWidgetsCSS();
    return pHandled != NULL;
  }
  else
  {
    return false; ///< easier debugging
    /// this is a mouse over event
    UpdateHoverList(rInfo);
    nuiSize x,y;

    IteratorPtr pIt;
    for (pIt = GetLastChild(false); pIt && pIt->IsValid() && !pHandled; GetPreviousChild(pIt))
    {
      // First find the widget directly under the mouse:
      nuiWidgetPtr pPtr = pIt->GetWidget();

      pWidgetUnder = pPtr->GetChild((nuiSize)rInfo.X, (nuiSize)rInfo.Y);
      pWidget = pWidgetUnder;
      if (pWidget)
      {
        // As long as there is no widget that handles the event, try to find one:
        while (pWidget && !pHandled)
        {
          if (pWidget->MouseEventsEnabled())
          {
            pHandled = pWidget->DispatchMouseMove(rInfo);
          }
          if (!pHandled)
            pWidget = pWidget->GetParent();
          else 
          { 
            pWidgetUnder = pWidget = pHandled;
            break;
          }
        }
      }
    }
    delete pIt;
    
    if (mpUnderMouse && mpUnderMouse != pWidget)
    {
      if (mpUnderMouse->MouseEventsEnabled())
      {
        mpUnderMouse->DispatchMouseMove(rInfo);
      }
    }

    //if (pHandled)
    {
      if (mpUnderMouse != pWidget)
      {
        mpUnderMouse = pWidget;
        GlobalHoverChanged();
      }
    }
  }

  nuiWidgetList widgets;
  GetChildren(rInfo.X, rInfo.Y, widgets, true);
  UpdateMouseCursor(widgets);

#ifndef DISABLE_TOOLTIP
  SetToolTipRect();
#endif
  NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseMove [%d] END2\n"), rInfo.TouchId) );
  UpdateWidgetsCSS();
  return pHandled != NULL;
}

bool nuiTopLevel::CallMouseWheel (nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  //NGL_TOUCHES_DEBUG( NGL_OUT(_T("nuiTopLevel::CallMouseWheel X:%d Y:%d\n"), rInfo.X, rInfo.Y) );

  // Update counterpart:
  std::map<nglTouchId, nglMouseInfo>::iterator it = mMouseClickedEvents.find(rInfo.TouchId);
  if (it != mMouseClickedEvents.end())
    rInfo.Counterpart = &it->second;
  // Update state:

  if (mMouseStates.find(rInfo.TouchId) != mMouseStates.end())
  {
    //NGL_OUT("Found %x\n", rInfo.TouchId);
    // Only update the mouse state if there was an existing touch already registered.
    // On a desktop computer with a mouse, when you move the mouse of the window with no button down there is no existing state to update.
    mMouseStates[rInfo.TouchId] = rInfo;
  }

  mMouseInfo.X = rInfo.X;
  mMouseInfo.Y = rInfo.Y;

  mMouseInfo.TouchId = rInfo.TouchId;
  //NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseWheel [%d] BEGIN\n"), rInfo.TouchId) );

  nuiWidgetPtr pWidget = NULL;
  nuiWidgetPtr pWidgetUnder = NULL;

#ifndef DISABLE_TOOLTIP
  if (mToolTipTimerOn.IsRunning())
  {
    mToolTipTimerOn.Stop();
    mToolTipTimerOn.Start(false);
  }
#endif
	nuiWidgetPtr pHandled = NULL;

  // If there is a grab on the mouse serve the grabbing widget and only this one:
  nuiWidgetPtr pGrab = GetGrab();
  if (pGrab)
  {
    WidgetBranchGuard guard(pGrab);
    std::vector<nuiWidgetPtr> containers;
    nuiWidgetPtr pParent = pGrab->GetParent();
    while (pParent)
    {
      containers.push_back(pParent);
      pParent = pParent->GetParent();
    }

    bool hook = false;
    for (int32 i = containers.size() - 1; i >= 0 && !hook; i--)
    {
      nglMouseInfo info(rInfo);
      containers[i]->GlobalToLocal(info.X, info.Y);
      hook = containers[i]->CallPreMouseWheelMoved(info);
      if (hook)
        pHandled = containers[i];
    }

    //NGL_OUT(_T("grabbed mouse wheel move on '%s' / '%s'\n"), mpGrab->GetObjectClass().GetChars(), mpGrab->GetObjectName().GetChars());
    NGL_ASSERT(mpGrab[mMouseInfo.TouchId]);
    if (mpGrab[mMouseInfo.TouchId]->MouseEventsEnabled() && !hook)
    {
      pHandled = pGrab->DispatchMouseWheelMove(rInfo);
    }

    nuiWidgetPtr pChild = NULL;
    // There might be no grab object left after the mouse wheel move event!
    if (pGrab)
      pChild = pGrab;
    else
      pChild = GetChild((nuiSize)rInfo.X, (nuiSize)rInfo.Y);

    NGL_ASSERT(pChild);

    // Set the mouse cursor to the right object:
    nuiWidgetList widgets;
    GetChildren(rInfo.X, rInfo.Y, widgets, true);
    UpdateMouseCursor(widgets);

#ifndef DISABLE_TOOLTIP
    SetToolTipRect();
#endif

    //NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseWheel [%d] END\n"), rInfo.TouchId) );
    UpdateWidgetsCSS();
    return pHandled != NULL;
  }
  else
  { /// this is a mouse over event
    UpdateHoverList(rInfo);
    nuiSize x,y;

    IteratorPtr pIt;
    for (pIt = GetLastChild(false); pIt && pIt->IsValid() && !pHandled; GetPreviousChild(pIt))
    {
      // First find the widget directly under the mouse:
      nuiWidgetPtr pPtr = pIt->GetWidget();

      pWidgetUnder = pPtr->GetChild((nuiSize)rInfo.X, (nuiSize)rInfo.Y);
      pWidget = pWidgetUnder;
      if (pWidget)
      {
        // As long as there is no widget that handles the event, try to find one:
        while (pWidget && !pHandled)
        {
          if (pWidget->MouseEventsEnabled())
          {
            pHandled = pWidget->DispatchMouseWheelMove(rInfo);
          }
          if (!pHandled)
            pWidget = pWidget->GetParent();
          else
          {
            pWidgetUnder = pWidget = pHandled;
            break;
          }
        }
      }
    }
    delete pIt;

    if (mpUnderMouse && mpUnderMouse != pWidget)
    {
      if (mpUnderMouse->MouseEventsEnabled())
      {
        mpUnderMouse->DispatchMouseWheelMove(rInfo);
      }
    }

    //if (pHandled)
    {
      if (mpUnderMouse != pWidget)
      {
        mpUnderMouse = pWidget;
        GlobalHoverChanged();
      }
    }
  }

  nuiWidgetList widgets;
  GetChildren(rInfo.X, rInfo.Y, widgets, true);
  UpdateMouseCursor(widgets);

#ifndef DISABLE_TOOLTIP
  SetToolTipRect();
#endif
  //NGL_TOUCHES_DEBUG( NGL_OUT(_T("CallMouseWheel [%d] END\n"), rInfo.TouchId) );
  UpdateWidgetsCSS();
  return pHandled != NULL;
}


bool nuiTopLevel::CallMouseCancel(nuiWidgetPtr pThief, nglMouseInfo& rInfo)
{
  DispatchMouseCanceled(pThief, rInfo);

  {
    auto it = mpGrabAcquired.find(rInfo.TouchId);
    if (it != mpGrabAcquired.end())
    {
      mpGrabAcquired.erase(it);
    }
  }

  {
    auto it = mpGrab.find(rInfo.TouchId);
    if (it != mpGrab.end())
    {
      mpGrab.erase(it);
    }
  }

  {
    auto it = mMouseClickedEvents.find(rInfo.TouchId);
    if (it != mMouseClickedEvents.end())
      mMouseClickedEvents.erase(it);
  }

  {
    auto it = mMouseStates.find(rInfo.TouchId);
    if (it != mMouseStates.end())
      mMouseStates.erase(it);
  }

  NGL_TOUCHES_DEBUG( NGL_OUT("CallMouseCancel: Grabs %d - %d / clicks %d / states %d\n", (uint32)mpGrabAcquired.size(), (uint32)mpGrab.size(), (uint32)mMouseClickedEvents.size(), (uint32)mMouseStates.size()));

  UpdateWidgetsCSS();
  return true;
}

void nuiTopLevel::SetDragFeedbackRect(int X, int Y)
{
  if (!mpDraggedWidget)
    return;

  CheckValid();
  
  nuiRect r(mpDraggedWidget->GetIdealRect());
  r.Move(X-r.GetWidth(), Y-r.GetHeight());
  mpDraggedWidget->SetLayout(r);
  InvalidateRect(r);
  UpdateWidgetsCSS();
}


#ifndef DISABLE_TOOLTIP
void nuiTopLevel::SetToolTipRect()
{
  CheckValid();
  if (mDisplayToolTip && mpToolTipSource)
  {
    nglString text(mpToolTipSource->GetToolTip());

    if (text.IsEmpty())
    {
      if (mpToolTipLabel->IsVisible())
      {
        mpToolTipLabel->SetVisible(false);
      }
      return;
    }

    // Place the tool tip:
    if (!mpToolTipLabel->IsVisible())
    {
      mpToolTipLabel->SetVisible(true);
    }


    if (mpToolTipLabel->GetText() != text)
    {
      mpToolTipLabel->SetText(text);
    }

    nuiRect ttrect(mpToolTipLabel->GetIdealRect());

    nglMouseInfo mouse;
    GetMouseInfo(mouse);

    if ((mpToolTipSource->HasProperty("ToolTipFollowMouse") && nuiGetBool(mpToolTipSource->GetProperty("ToolTipFollowMouse"), false))
      || !mpToolTipSource->HasProperty("ToolTipFollowMouse") )
    {
      // Position the tooltip relatively to the mouse
      if (mpToolTipSource->HasProperty("ToolTipPosition"))
      {
        nuiPosition pos = nuiGetPosition(mpToolTipSource->GetProperty("ToolTipPosition"), nuiBottomRight);
        nuiRect tt(mouse.X, mouse.Y, 0, 0);
        tt.Grow(ttrect.GetWidth() + 16, ttrect.GetHeight() + 16);
        ttrect.SetPosition(pos, tt);
      }
      else
      {
        ttrect.Set
          (
          (nuiSize)(mouse.X+16),
          (nuiSize)(mouse.Y+16),
          ttrect.GetWidth(), 
          ttrect.GetHeight()
          );
      }
    }
    else
    {
      // Position the tooltip relatively to the tooltip source widget
      nuiPosition pos = nuiBottom;
      if (mpToolTipSource->HasProperty("ToolTipPosition"))
        pos = nuiGetPosition(mpToolTipSource->GetProperty("ToolTipPosition"), pos);
      nuiRect tt(mpToolTipSource->GetRect().Size());
      mpToolTipSource->LocalToGlobal(tt);
      tt.Grow(ttrect.GetWidth(), ttrect.GetHeight());
      ttrect.SetPosition(pos, tt);
    }
    ttrect.RoundToNearest();
    // do not display tooltip out of the topLevel borders
    ttrect.MoveTo(MIN(mRect.GetWidth()-ttrect.GetWidth(), ttrect.Left()), MIN(mRect.GetHeight()-ttrect.GetHeight(), ttrect.Top()));

    mpToolTipLabel->Invalidate();
    mpToolTipLabel->SetLayout(ttrect);
    InvalidateRect(ttrect);
  }
  else
  {
    if (mpToolTipLabel && mpToolTipLabel->IsVisible())
    {
      mpToolTipLabel->SetVisible(false);
    }
  }
}

void nuiTopLevel::DisplayToolTips(nuiDrawContext* pContext)
{
  CheckValid();
  if (mDisplayToolTip && mpToolTipSource && mpToolTipLabel)
  {
    DrawChild(pContext, mpToolTipLabel);
  }
}
#endif

static const bool DISPLAY_PARTIAL_RECTS = false;

bool nuiTopLevel::Draw(class nuiDrawContext *pContext)
{
  CheckValid();
  
  if (mClearBackground)
  {
    //pContext->Clear();
    nuiColor clearColor("nuiActiveWindowBg");
    pContext->SetFillColor(clearColor);
    pContext->DrawRect(GetRect().Size(), eFillShape);
  }
  
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem
#ifndef DISABLE_TOOLTIP
        && pItem != mpToolTipLabel
#endif
        && pItem != mpDraggedWidget
        )
      DrawChild(pContext, pItem);
  }
  delete pIt;

#ifndef DISABLE_TOOLTIP
  DisplayToolTips(pContext);
#endif

  if (mpDraggedWidget)
    DrawChild(pContext, mpDraggedWidget);

  return true;
}

#if NUI_USE_RENDER_THREAD

bool nuiTopLevel::DrawTree(class nuiDrawContext *pContext) ///< Draw caches only
{
  mIsDrawing = true;
  
  CheckValid();
  //nuiStopWatch watch(_T("nuiTopLevel::DrawTree"));
  UpdateWidgetsCSS();
  
//  nuiMetaPainter* pFrame = dynamic_cast<nuiMetaPainter*>(pContext->GetPainter());
//  NGL_ASSERT(pFrame);

  uint32 clipWidth, clipHeight;
  {
    clipWidth=pContext->GetWidth();
    clipHeight=pContext->GetHeight();
  }

//  NGL_OUT("Update cache for %d widgets\n", mDirtyWidgets.size());
  nuiRenderThread* pRenderThread = GetRenderThread();
  for (auto widget : mDirtyWidgets)
  {
//    NGL_OUT("    Update %s / %s \t(%p)\n", widget->GetObjectClass().GetChars(), widget->GetObjectName().GetChars(), widget);
    widget->UpdateCache(pContext, pRenderThread); //  This will update the Meta Painter for each invalidated widget
    widget->Release();
  }
  mDirtyWidgets.clear();
  
  mDirtyRects.clear();
//
//  if (mpWatchedWidget)
//  {
//    nuiRect r(mpWatchedWidget->GetRect());
//    nuiRect r2(mpWatchedWidget->GetBorderedRect().Size());
//    nuiRect r3(mpWatchedWidget->GetOverDrawRect(true, true).Size());
//    if (IsKeyDown(NK_ALT))
//      r = r2;
//    else if (IsKeyDown(NK_META))
//      r = r3;
//    
//    if (mpWatchedWidget->GetParent())
//      mpWatchedWidget->GetParent()->LocalToGlobal(r);
//    
//    pContext->ResetState();
//    pContext->ResetClipRect();
//    pContext->SetStrokeColor(nuiColor(0.0f,0.0f,1.0f,0.5f));
//    pContext->SetFillColor(nuiColor(0.0f,0.0f,1.0f,.25f));
//    pContext->EnableBlending(true);
//    pContext->SetBlendFunc(nuiBlendTransp);
//    pContext->DrawRect(r, eStrokeAndFillShape);
//    pContext->ResetState();
//  }
  
  mIsDrawing = false;
  
  return true;
}

#else

bool nuiTopLevel::DrawTree(class nuiDrawContext *pContext)
{
  mIsDrawing = true;

  CheckValid();
  //nuiStopWatch watch(_T("nuiTopLevel::DrawTree"));
  UpdateWidgetsCSS();

  uint32 clipWidth, clipHeight;
  {
    clipWidth=pContext->GetWidth();
    clipHeight=pContext->GetHeight();
  }

//  if (0)
  if (mPartialRedraw && !DISPLAY_PARTIAL_RECTS)
  {
    //nuiStopWatch watch(_T("nuiTopLevel::DrawTree / PartialReDraw"));
    // Prepare the layout changes:
    pContext->ResetState();

    int count = mDirtyRects.size();

//    NGL_OUT("drawing %d partial rects\n", count);

    for (int i = 0; i < count; i++)
    {
//      NGL_OUT("\tDirty Rect: %d: %s\n", i, mDirtyRects[i].GetValue().GetChars());
      pContext->ResetState();
      pContext->ResetClipRect();
      pContext->Clip(mDirtyRects[i]);
      pContext->EnableClipping(true);

      nuiColor clearColor("nuiActiveWindowBg");
      //pContext->SetClearColor(clearColor);
      if (mClearBackground)
      {
        //pContext->Clear();
        pContext->SetFillColor(clearColor);
        pContext->DrawRect(mDirtyRects[i], eFillShape);
      }
      else
      {
        // Force the initial render state anyway!
        pContext->DrawRect(nuiRect(0,0,0,0), eStrokeShape);
      }

      pContext->SetStrokeColor(nuiColor(1.0f, 0.0f, 0.0f, 0.0f));
      pContext->SetFillColor(nuiColor(1.0f, 0.0f, 0.0f, 0.5f));

      DrawWidget(pContext);

    }

  }
  else
  {
    //nuiStopWatch watch(_T("nuiTopLevel::DrawTree / FullDraw"));
    pContext->ResetState();
    pContext->ResetClipRect();

    pContext->SetClearColor("nuiActiveWindowBg");
    if (mClearBackground)
    {
      pContext->Clear();
    }
    else
    {
      // Force the initial render state anyway!
      pContext->DrawRect(nuiRect(0,0,0,0), eStrokeShape);
    }

    DrawWidget(pContext);

    if (DISPLAY_PARTIAL_RECTS)
    {
      int count = mDirtyRects.size();
      pContext->ResetState();
      pContext->ResetClipRect();
      pContext->SetStrokeColor(nuiColor(1.0f,0.0f,0.0f,0.0f));
      pContext->SetFillColor(nuiColor(1.0f,0.0f,0.0f,.5f));
      pContext->EnableBlending(true);
      pContext->SetBlendFunc(nuiBlendTransp);
      for (int i=0; i < count; i++)
      {
        pContext->DrawRect(mDirtyRects[i], eStrokeAndFillShape);
      }
      pContext->ResetState();    
    }
    
  }

  mDirtyRects.clear();

  if (mpWatchedWidget)
  {
    nuiRect r(mpWatchedWidget->GetRect());
    nuiRect r2(mpWatchedWidget->GetBorderedRect().Size());
    nuiRect r3(mpWatchedWidget->GetOverDrawRect(true, true).Size());
    if (IsKeyDown(NK_ALT))
      r = r2;
    else if (IsKeyDown(NK_META))
      r = r3;
    
    if (mpWatchedWidget->GetParent())
      mpWatchedWidget->GetParent()->LocalToGlobal(r);
    
    pContext->ResetState();
    pContext->ResetClipRect();
    pContext->SetStrokeColor(nuiColor(0.0f,0.0f,1.0f,0.5f));
    pContext->SetFillColor(nuiColor(0.0f,0.0f,1.0f,.25f));
    pContext->EnableBlending(true);
    pContext->SetBlendFunc(nuiBlendTransp);
    pContext->DrawRect(r, eStrokeAndFillShape);
    pContext->ResetState();
  }
  
  mIsDrawing = false;

  return true;
}

#endif
void nuiTopLevel::GetMouseInfo(nglMouseInfo& rMouseInfo) const
{
  CheckValid();
  rMouseInfo = mMouseInfo;
}

nglPath nuiTopLevel::GetResourcePath() const
{
  CheckValid();
  return mResPath;
}

nuiSize nuiTopLevel::GetStatusBarSize() const
{
  return 0;
}


void nuiTopLevel::BroadcastInvalidateRect(nuiWidgetPtr pSender, const nuiRect& rRect)
{
//  nglString senderclass = pSender->GetObjectClass();
//  if (senderclass == "nuiMainWindow"
//      || senderclass == "nuiNavigationController"
//      || senderclass == "MainViewInstance"
//      )
//  {
//    printf("");
//  }
  //NGL_ASSERT(!mIsDrawing);
  CheckValid();
  nuiRect r = rRect;
  nuiRect rect = GetRect();

  r.Move(rect.Left(), rect.Top());
  r.Intersect(r, rect);

  nuiVector vec1((float)r.Left(),(float)r.Top(),0);
  nuiVector vec2((float)r.Right(),(float)r.Bottom(),0);
  if (!IsMatrixIdentity())
  {
    nuiMatrix m;
    GetMatrix(m);
    vec1 = m * vec1;
    vec2 = m * vec2;
  }

  r.Set(vec1[0], vec1[1], vec2[0], vec2[1], false);

//  NGL_OUT("nuiTopLevel::BroadcastInvalidateRect %s / %s / 0x%x RECT:%s\n", pSender->GetObjectClass().GetChars(), pSender->GetObjectName().GetChars(), pSender, rRect.GetValue().GetChars());
  AddInvalidRect(r);
  if (mpBackingLayer)
    GetRenderThread()->InvalidateLayerContents(mpBackingLayer);

  mDirtyWidgets.insert(pSender);
  pSender->Acquire();
  DebugRefreshInfo();
}

void nuiTopLevel::BroadcastInvalidate(nuiWidgetPtr pSender)
{
  mDirtyWidgets.insert(pSender);
  pSender->Acquire();
//  NGL_OUT("SetDirty %s %s %p\n",
//          pSender->GetObjectClass().GetChars(),
//          pSender->GetObjectName().GetChars(),
//          pSender);
}


//#define _DEBUG_LAYOUT
bool nuiTopLevel::SetRect(const nuiRect& rRect)
{
  CheckValid();
  #ifdef _DEBUG_LAYOUT
  if (GetDebug())
    NGL_OUT("toplevel set rect %f %f %f %f\n", rRect.Left(), rRect.Top(), rRect.GetWidth(), rRect.GetHeight());
  #endif
  
  nuiWidget::SetSelfRect(rRect);

  nuiRect rectfull(mRect.Size());
  nuiRect rect(rectfull);
  float barsize = GetStatusBarSize();
  rect.SetHeight(rect.GetHeight() -  barsize);
  rect.Move(0, barsize);

  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem )
#ifndef DISABLE_TOOLTIP
    if (pItem != mpToolTipLabel)
#endif
    if (pItem != mpDraggedWidget)
    {
      pItem->GetIdealRect();
      if (pItem->GetProperty("Layout").ToLower() == "fullscreen")
        pItem->SetLayout(rectfull);
      else
        pItem->SetLayout(rect);
    }
  }
  delete pIt;

#ifndef DISABLE_TOOLTIP
  SetToolTipRect();
#endif

  return true;
}

bool nuiTopLevel::InitHotKeys(nglIStream* pHotKeys)
{
  CheckValid();
  nuiXML* pXML = new nuiXML(nglString::Null);
  if (pXML->Load(*pHotKeys))
  {
    NGL_OUT(pXML->Dump());
    bool success = InitHotKeys(pXML->GetChild(NUIHOTKEYS_XML_NODEID));
    delete pXML;
    
    return success;
  }
  else
  {
    NGL_OUT(_T("Couldn't load hotkeys from file\n"));
    delete pXML;
    
    return false;
  }
}

bool nuiTopLevel::InitHotKeys(nuiXMLNode* pHotKeys)
{
  CheckValid();
  nglString nodeName = pHotKeys->GetName();
  if (nodeName.Compare(NUIHOTKEYS_XML_NODEID))
  {
    NGL_OUT(_T("Not a hotkeys node\n : %s"), nodeName.GetChars());
    return false;
  }
  
  // Parse the list of hotkeys to import
  const nuiXMLNodeList& pHotKeysList = pHotKeys->GetChildren();
  nuiXMLNodeList::const_iterator pHotKeysIt;
  
  for (pHotKeysIt = pHotKeysList.begin(); pHotKeysIt != pHotKeysList.end(); ++pHotKeysIt)
  {    
    nuiXMLNode* pNode = *pHotKeysIt;
    nglString name = pNode->GetAttribute(_T("Name"));
    nglString description = pNode->GetAttribute(_T("Description"));
    nuiKeyModifier modifiers = pNode->GetAttribute(_T("Modifiers")).GetUInt();
    bool hasPriority = pNode->GetAttribute(_T("Priority")).Compare(_T("true")) ? false : true;
    bool isOnKeyUp = pNode->GetAttribute(_T("OnKeyUp")).Compare(_T("true")) ? false : true;

    bool isHotKeyKey = pNode->GetAttribute(_T("IsHotKeyKey")).Compare(_T("true")) ? false : true;
    
    nuiHotKey* pNewHotKey;
    
    if (isHotKeyKey)
    {
      nglKeyCode trigger = pNode->GetAttribute(_T("Trigger_KeyCode")).GetUInt();
      pNewHotKey = new nuiHotKeyKey(trigger, modifiers, hasPriority, isOnKeyUp);
    }
    else
    {
      nglString trig = pNode->GetAttribute(_T("Trigger_Char"));
      nglChar trigger = (nglChar)trig.GetCInt();
      pNewHotKey = new nuiHotKeyChar(trigger, modifiers, hasPriority, isOnKeyUp);
    }
    
    pNewHotKey->SetName(name);
    pNewHotKey->SetDescription(description);
    
    SetHotKey(name, pNewHotKey);
  }
  
  return true;
}

nuiHotKey* nuiTopLevel::RegisterHotKeyKey(const nglString& rName, nglKeyCode Trigger, nuiKeyModifier Modifiers, bool Priority /*= false*/, bool FireOnKeyUp /*= false*/, const nglString& rDescription /*= nglString::Empty*/)
{
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it = mHotKeys.find(rName);
  if (it == mHotKeys.end())
  {
    nuiHotKeyKey* pHotKeyKey = new nuiHotKeyKey(Trigger, Modifiers, Priority, FireOnKeyUp);
    mHotKeys[rName] = pHotKeyKey;
    pHotKeyKey->SetName(rName);
    if (rDescription.IsEmpty())
    {
      pHotKeyKey->SetDescription(rName);
    }
    else
    {
      pHotKeyKey->SetDescription(rDescription);
    }

    pHotKeyKey->Register();

    std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
    if (it == mHotKeyEvents.end())
      mHotKeyEvents[rName] = new nuiSimpleEventSource<nuiWidgetActivated>();

    return pHotKeyKey;
  }
  else
  {
    nuiHotKey* pHotKey = it->second;
    pHotKey->Register();

    std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
    if (it == mHotKeyEvents.end())
      mHotKeyEvents[rName] = new nuiSimpleEventSource<nuiWidgetActivated>();

    return pHotKey;
  }
}

nuiHotKey* nuiTopLevel::RegisterHotKeyChar(const nglString& rName, nglChar Trigger, nuiKeyModifier Modifiers, bool Priority /*= false*/, bool FireOnKeyUp /*= false*/, const nglString& rDescription /*= nglString::Empty*/)
{
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it = mHotKeys.find(rName);
  if (it == mHotKeys.end())
  {
    nuiHotKeyChar* pHotKeyChar = new nuiHotKeyChar(Trigger, Modifiers, Priority, FireOnKeyUp);
    mHotKeys[rName] = pHotKeyChar;
    pHotKeyChar->SetName(rName);
    if (rDescription.IsEmpty())
    {
      pHotKeyChar->SetDescription(rName);
    }
    else
    {
      pHotKeyChar->SetDescription(rDescription);
    }
    pHotKeyChar->Register();

    std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
    if (it == mHotKeyEvents.end())
      mHotKeyEvents[rName] = new nuiSimpleEventSource<nuiWidgetActivated>();
    
    return pHotKeyChar;
  }
  else
  {
    nuiHotKey* pHotKey = it->second;
    pHotKey->Register();

    std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
    if (it == mHotKeyEvents.end())
      mHotKeyEvents[rName] = new nuiSimpleEventSource<nuiWidgetActivated>();
    
    return pHotKey;
  }
}

void nuiTopLevel::SetHotKey(const nglString& rName, nuiHotKey* pHotKey)
{  
  CheckValid();
  // search toplevel for a registered hotkey for rName
  nuiHotKey* pRegisteredHotKey = GetHotKey(rName);
  
  // if there is one, get its refCount, update the new hotkey with it, then remove the registered hotkey
  if (pRegisteredHotKey)
  {    
    //retrieve the refcount from the registered hotkey
    int refCount = pRegisteredHotKey->GetReferenceCount();
    pHotKey->SetReferenceCount(refCount);
    
    // check wether the hotkey was previously enabled
    bool enabled = pRegisteredHotKey->IsEnabled();
    pHotKey->SetEnabled(enabled);
    
    // remove the old hotkey
    delete pRegisteredHotKey;
  }
  
  // store the new hotkey
  mHotKeys[rName] = pHotKey;
}


nuiHotKey* nuiTopLevel::GetHotKey(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it = mHotKeys.find(rName);
  if (it != mHotKeys.end())
  {
    return it->second;
  }
  return NULL;
}

const std::map<nglString, nuiHotKey*>& nuiTopLevel::GetHotKeys() const
{
  CheckValid();
  return mHotKeys;
}

const nglString& nuiTopLevel::FindHotKeyKey(nglKeyCode Trigger, nuiKeyModifier Modifiers)
{
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it;
  std::map<nglString, nuiHotKey*>::const_iterator end = mHotKeys.end();
  
  for (it = mHotKeys.begin(); it != end; it++)
  {
    nuiHotKeyKey* pKey = (nuiHotKeyKey*)it->second;
    
    if ((pKey->GetTrigger() == Trigger) && (pKey->GetModifiers() == Modifiers))
    {
      return it->first;
    }
  }
  return nglString::Empty;
}

const nglString& nuiTopLevel::FindHotKeyChar(nglChar Trigger, nuiKeyModifier Modifiers)
{
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it;
  std::map<nglString, nuiHotKey*>::const_iterator end = mHotKeys.end();
  
  for (it = mHotKeys.begin(); it != end; it++)
  {
    nuiHotKeyChar* pKey = (nuiHotKeyChar*)it->second;
    
    if ((pKey->GetTrigger() == Trigger) && (pKey->GetModifiers() == Modifiers))
    {
      return it->first;
    }
  }
  return nglString::Empty;
}

void nuiTopLevel::DelHotKey( const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it = mHotKeys.find(rName);
  if (it != mHotKeys.end())
  {
    nuiHotKey* pHotKey = it->second;
    if (pHotKey->Release())
    {
      //mHotKeys.erase(rName);
      //delete pHotKey;
    }
  }
}

void nuiTopLevel::PrintHotKeyMap(const nglString& rText)
{  
  CheckValid();
  std::map<nglString, nuiHotKey*>::const_iterator it = mHotKeys.begin();
  NGL_OUT(_T("\nShortcuts Map (%s):\n"), rText.GetStdString().c_str());
  while (it != mHotKeys.end())
  {
    nglString name = it->first;
    nuiHotKey* pHotKey = it->second;
    NGL_OUT(_T("%s -> %s\n"), name.GetStdString().c_str(), pHotKey->ShortcutToString().GetStdString().c_str());
    //NGL_OUT(_T("%s -> %s\n"), it->first.GetChars(), it->second->ShortcutToString().GetChars());
    ++it;
  }
  NGL_OUT(_T("\n"));
}


void nuiTopLevel::SetWatchedWidget(nuiWidget* pWatchedWidget)
{
  CheckValid();
  mpWatchedWidget = pWatchedWidget;
  Invalidate();
}

//// CSS Stuff:
void nuiTopLevel::PrepareWidgetCSS(nuiWidget* pWidget, bool Recursive, uint32 MatchersTag)
{
  CheckValid();
  std::map<nuiWidgetPtr, uint32>::iterator it = mCSSWidgets.find(pWidget);
  if (it != mCSSWidgets.end())
    mCSSWidgets[pWidget] |= MatchersTag;
  else
    mCSSWidgets[pWidget] = MatchersTag;

  if (!Recursive)
    return;
  
  nuiWidget* pContainer = dynamic_cast<nuiWidget*>(pWidget);
  if (pContainer)
  {
    IteratorPtr pIt;
    for (pIt = pContainer->GetFirstChild(false); pIt && pIt->IsValid(); pContainer->GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      PrepareWidgetCSS(pItem, Recursive, MatchersTag);
    }
    delete pIt;
  }
}

void nuiTopLevel::ApplyWidgetCSS(nuiWidget* pWidget, bool Recursive, uint32 MatchersTag)
{
  CheckValid();
  if (!mpCSS)
    return;
  
  mpCSS->ApplyRules(pWidget, MatchersTag);

  if (Recursive)
  {
    nuiWidget* pContainer = dynamic_cast<nuiWidget*>(pWidget);
    if (pContainer)
    {
      IteratorPtr pIt;
      for (pIt = pContainer->GetFirstChild(false); pIt && pIt->IsValid(); pContainer->GetNextChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        ApplyWidgetCSS(pItem, Recursive, MatchersTag);
      }
      delete pIt;
    }
  }
}

void nuiTopLevel::UpdateWidgetsCSS()
{
  CheckValid();
  std::map<nuiWidgetPtr, uint32>::iterator it = mCSSWidgets.begin();
  std::map<nuiWidgetPtr, uint32>::iterator end = mCSSWidgets.end();
  
  while (it != end)
  {
    nuiWidget* pWidget = it->first;
    uint32 MatchersTag = it->second;
    
    ApplyWidgetCSS(pWidget, false, MatchersTag);
    
    ++it;
  }
  
  mCSSWidgets.clear();
}

void nuiTopLevel::SetCSS(nuiCSS* pCSS)
{
  CheckValid();
  if (mpCSS != pCSS)
    delete mpCSS;
  mpCSS = pCSS;
  
  if (mpCSS)
  {
    ResetCSSPass();
    ApplyWidgetCSS(this, true, NUI_WIDGET_MATCHTAG_ALL);
  }
}

nuiCSS* nuiTopLevel::GetCSS() const
{
  CheckValid();
  return mpCSS;
}

bool nuiTopLevel::LoadCSS(const nglPath& rPath)
{
  nglIStream* pF = rPath.OpenRead();
  if (!pF)
  {
    NGL_OUT(_T("Unable to open CSS source file '%s'\n"), rPath.GetChars());
    return false;
  }
  
  nuiCSS* pCSS = new nuiCSS();
  bool res = pCSS->Load(*pF, rPath);
  delete pF;
  
  if (res)
  {
    SetCSS(pCSS);
    return true;
  }
  
  NGL_LOG("nuiTopLevel", NGL_LOG_ERROR, "%%s\n", pCSS->GetErrorString().GetChars());
  
  delete pCSS;
  return false;
}


void nuiTopLevel::EnterModalState()
{
  CheckValid();
}

void nuiTopLevel::ExitModalState()
{
  CheckValid();
}

void nuiTopLevel::SetTabForward(nuiWidget* pSource, nuiWidget* pDestination, bool AutoReverse)
{
  CheckValid();
  // First check f there is already an entry in the table for this source
  std::map<nuiWidgetPtr, nuiWidgetPtr>::iterator it = mTabForward.find(pSource);
  
  // if this is the case then remove the back pointer
  if (it != mTabForward.end())
  {
    if (!pDestination)
    {
      mTabForward.erase(it);
      return;
    }
    
    if (it->second != pDestination)
      mTabForwardRev[pDestination].erase(pSource);
    else
      return; // Nothing to do
  }

  if (!pDestination)
    return;
  
  // Add the relation to the table
  mTabForward[pSource] = pDestination;

  // Add the relation to the reverse table (for quick removal when the pointed widget will die)
  mTabForwardRev[pDestination].insert(pSource);
  
  // If needed, added the bacward relation too (so that Tab and then shift-Tab goes back to the original source)
  if (AutoReverse)
  {
    SetTabBackward(pDestination, pSource, false);
  }
  
}

void nuiTopLevel::SetTabBackward(nuiWidget* pSource, nuiWidget* pDestination, bool AutoReverse)
{
  CheckValid();
  // First check f there is already an entry in the table for this source
  std::map<nuiWidgetPtr, nuiWidgetPtr>::iterator it = mTabBackward.find(pSource);
  
  // if this is the case then remove the back pointer
  if (it != mTabBackward.end())
  {
    if (!pDestination)
    {
      mTabBackward.erase(it);
      return;
    }
    
    if (it->second != pDestination)
      mTabBackwardRev[pDestination].erase(pSource);
    else
      return; // Nothing to do
  }

  if (!pDestination)
    return;
  
  // Add the relation to the table
  mTabBackward[pSource] = pDestination;
  
  // Add the relation to the reverse table (for quick removal when the pointed widget will die)
  mTabBackwardRev[pDestination].insert(pSource);
  
  // If needed, added the backward relation too (so that Tab and then shift-Tab goes back to the original source)
  if (AutoReverse)
  {
    SetTabForward(pDestination, pSource, false);
  }
  
}

nuiWidget* nuiTopLevel::GetTabForward(nuiWidget* pSource) const
{
  CheckValid();
  std::map<nuiWidgetPtr, nuiWidgetPtr>::const_iterator it = mTabForward.find(pSource);
  if (it != mTabForward.end())
    return it->second;
  return NULL;
}

nuiWidget* nuiTopLevel::GetTabBackward(nuiWidget* pSource) const
{
  CheckValid();
  std::map<nuiWidgetPtr, nuiWidgetPtr>::const_iterator it = mTabBackward.find(pSource);
  if (it != mTabBackward.end())
    return it->second;
  return NULL;
}

void nuiTopLevel::GetTabForwardSources(nuiWidget* pDestination, std::set<nuiWidgetPtr>& rSources) const
{
  CheckValid();
  std::map<nuiWidgetPtr, std::set<nuiWidgetPtr> >::const_iterator it = mTabForwardRev.find(pDestination);
  if (it != mTabForwardRev.end())
  {
    rSources = it->second;
    return;
  }
  rSources.clear();
}

void nuiTopLevel::GetTabBackwardSources(nuiWidget* pDestination, std::set<nuiWidgetPtr>& rSources) const
{
  CheckValid();
  std::map<nuiWidgetPtr, std::set<nuiWidgetPtr> >::const_iterator it = mTabBackwardRev.find(pDestination);
  if (it != mTabBackwardRev.end())
  {
    rSources = it->second;
    return;
  }
  rSources.clear();
}

void nuiTopLevel::StartTextInput(int32 X, int32 Y, int32 W, int32 H)
{
  CheckValid();
  
}

void nuiTopLevel::EndTextInput()
{
  CheckValid();
  
}

bool nuiTopLevel::IsEnteringText() const
{
  CheckValid();
  return false;
}

void nuiTopLevel::PostNotification(nuiNotification* pNotification)
{
  App->PostNotification(pNotification);
}

void nuiTopLevel::BroadcastNotification(const nuiNotification& rNotification)
{
  App->BroadcastNotification(rNotification);
}

void nuiTopLevel::RegisterObserver(const nglString& rNotificationName, nuiNotificationObserver* pObserver)
{
  App->RegisterObserver(rNotificationName, pObserver);
}

void nuiTopLevel::UnregisterObserver(nuiNotificationObserver* pObserver, const nglString& rNotificationName)
{
  App->UnregisterObserver(pObserver, rNotificationName);
}


//typedef std::map<nglTouchId, nuiWidgetPtr> nuiGrabMap;

void nuiTopLevel::DumpGrabMap(int line) const
{
#if 0
#ifdef DEBUG
  NGL_OUT("Grab Map (line %d)\n", line);
  for (const auto& item : mpGrab)
  {
    if (item.second != nullptr)
    {
      NGL_OUT("Grab %d -> %s / %s (%p)\n", item.first, item.second->GetObjectClass().GetChars(), item.second->GetObjectName().GetChars(), item.second);
    }
    else
    {
      NGL_OUT("Grab %d -> NONE\n", item.first);
      
    }
  }
#endif
#endif
}

float nuiTopLevel::GetScale() const
{
  return nuiGetScaleFactor();
}

float nuiTopLevel::GetScaleInv() const
{
  return nuiGetInvScaleFactor();
}

