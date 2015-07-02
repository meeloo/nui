/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/



#include "nui.h"

using namespace std;

#define NUI_MULTISAMPLES          0
#define NUI_MAINWINDOW_USE_LAYERS (1 && NUI_USE_LAYERS)

nuiContextInfo::nuiContextInfo(Type type)
{
  TargetAPI = (nglTargetAPI)nuiMainWindow::GetRenderer();

  StencilBits = 0;
  DepthBits = 0;
  FrameBitsA = 0;
  AABufferCnt = 0;
  AASampleCnt = 0;

  switch (type)
  {
  case StandardContext2D:
      CopyOnSwap = true;
  case StandardContext3D:
    Offscreen = false;
    RenderToTexture = false;
    AASampleCnt = NUI_MULTISAMPLES;
    if (AASampleCnt)
      AABufferCnt = 1;
    break;
  case OffscreenContext2D:
  case OffscreenContext3D:
    Offscreen = true;
    RenderToTexture = true;
    AASampleCnt = NUI_MULTISAMPLES;
    if (AASampleCnt)
      AABufferCnt = 1;
    CopyOnSwap = true;
    break;
  }

  if (type == StandardContext3D || type == OffscreenContext3D)
    DepthBits = 16;
}

nuiMainWindow::nuiMainWindow(uint Width, uint Height, bool Fullscreen, const nglPath& rResPath)
  : nuiTopLevel(rResPath),
    mMainWinSink(this),
    mQuitOnClose(true),
    mpDragSource(NULL),
    mPaintEnabled(true)

{
  Register();
  mFullFrameRedraw = 2;
  mpNGLWindow = new NGLWindow(this, Width, Height, Fullscreen);

  nuiRect rect(0.0f, 0.0f, (nuiSize)Width, (nuiSize)Height);
  //nuiWidget::SetSelfRect(rect);
  if (SetObjectClass(_T("nuiMainWindow")))
    InitAttributes();
  mMaxFPS = 0.0f;
  mFPSCount = 0;
  mFPS = 0;

  uint w,h;
  mpNGLWindow->GetSize(w,h);

  SetRect(nuiRect(0.0f, 0.0f, (nuiSize)w, (nuiSize)h));
  
  mLastRendering = 0;

  mDisplayMouseOverInfo = false;
  mDisplayMouseOverObject = false;
  mpInfoLabel = NULL;

  mDebugMode = false;
  mDebugSlowRedraw = false;
  mInvalidatePosted = false;
  mpInspectorWindow = NULL;
  
  mpWidgetCanDrop = NULL;
  
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = 0;
  nuiDefaultDecoration::MainWindow(this);
  
//  mMainWinSink.Connect(nuiAnimation::AcquireTimer()->Tick, &nuiMainWindow::InvalidateTimer);

  nuiDrawContext* pContext = GetDrawContext();
  nuiRenderThread* pRenderThread = GetRenderThread();
  
  SetDrawToLayer(false);
  
  if (NUI_MAINWINDOW_USE_LAYERS)
  {
    SetDrawToLayer(true);
  }
  
  if (GetDrawToLayer())
  {
    pRenderThread->SetLayerTree(mpBackingLayer);
    mpBackingLayer->UpdateContents(pRenderThread, pContext);
  }
}

nuiMainWindow::nuiMainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, const nglContext* pShared, const nglPath& rResPath)
  : nuiTopLevel(rResPath),
    mMainWinSink(this),
    mQuitOnClose(true),
    mpDragSource(NULL),
    mPaintEnabled(true)
{
  Register();
  mFullFrameRedraw = 2;
  mpNGLWindow = new NGLWindow(this, rContextInfo, rInfo, pShared);
  nuiRect rect(0.0f, 0.0f, (nuiSize)rInfo.Width, (nuiSize)rInfo.Height);
  //nuiWidget::SetSelfRect(rect);
  if (SetObjectClass(_T("nuiMainWindow")))
    InitAttributes();

  mMaxFPS = 0.0f;
  mFPSCount = 0;
  mFPS = 0;
  mLastPaint = nglTime();
  uint w,h;
  mpNGLWindow->GetSize(w,h);

  SetRect(nuiRect(0.0f, 0.0f, (nuiSize)w, (nuiSize)h));
  
  mLastRendering = 0;

  mDisplayMouseOverInfo = false;
  mDisplayMouseOverObject = false;
  mpInfoLabel = NULL;

  mDebugMode = false;
  mDebugSlowRedraw = false;
  mInvalidatePosted = false;
  mpInspectorWindow = NULL;
  mpWidgetCanDrop = NULL;
  
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = 0;

  nuiDefaultDecoration::MainWindow(this);  

//  mMainWinSink.Connect(nuiAnimation::AcquireTimer()->Tick, &nuiMainWindow::InvalidateTimer);

  nuiDrawContext* pContext = GetDrawContext();
  nuiRenderThread* pRenderThread = GetRenderThread();

  SetDrawToLayer(false);
  
  if (NUI_MAINWINDOW_USE_LAYERS)
  {
    SetDrawToLayer(true);
  }
  
  if (GetDrawToLayer())
  {
    pRenderThread->SetLayerTree(mpBackingLayer);
    mpBackingLayer->UpdateContents(pRenderThread, pContext);
  }
}

nuiMainWindow::~nuiMainWindow()
{
  if (mpRenderThread)
  {
    mpRenderThread->Exit();
    mpRenderThread->Join();
  }
  
  if (mpInspectorWindow)
    mpInspectorWindow->Release();
  nuiTopLevel::Exit();
  
  delete mpNGLWindow;
  mpNGLWindow = NULL;
  //OnDestruction();
  
  mMainWinSink.DisconnectAll();
  nuiAnimation::ReleaseTimer();
  
  Unregister();
}

void nuiMainWindow::InitAttributes()
{
  AddAttribute(new nuiAttribute<nuiRect>
               (nglString("WindowRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiMainWindow::GetWindowRect),
                nuiMakeDelegate(this, &nuiMainWindow::SetWindowRect)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("DrawToSurface"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiMainWindow::GetDrawToSurface),
                nuiMakeDelegate(this, &nuiMainWindow::SetDrawToSurface)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("DrawDirtyRects"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiMainWindow::GetDrawDirtyRects),
                nuiMakeDelegate(this, &nuiMainWindow::SetDrawDirtyRects)));
}

void nuiMainWindow::OnPaint()
{
  mLastEventTime = nglTime();
  
  LazyPaint();
  
  nglTime now;
  mFPSCount++;
  if (now - mFPSDelay > 1.0)
  {
    double v = (now - mFPSDelay);
    double c = mFPSCount;
    mFPS = c / v;
    NGL_LOG(_T("fps"), NGL_LOG_DEBUG, _T("FPS: %f (%f seconds - %d frames)\n"), mFPS, v, ToNearest(c));
    
    mFPSCount = 0;
    mFPSDelay = now;
  }
  
  mLastPaint = now;
}

void nuiMainWindow::LazyPaint()
{
  if (mInvalidatePosted || GetNeedRender() || GetNeedSelfRedraw() || GetNeedLayout() || GetNeedIdealRect())
  {
    mInvalidatePosted = true;
    Paint();
  }
  else
  {
    nuiRenderThread* pRenderThread = GetRenderThread();
    pRenderThread->StartRendering(0,0);
  }
}

nuiSize nuiMainWindow::GetStatusBarSize() const
{
  return mpNGLWindow->GetStatusBarSize();
}


#ifdef NUI_USE_RENDER_THREAD

void nuiMainWindow::Paint()
{
  if (!App->IsActive()) // Only repaint if the application is active!
    return;
  
  if (!IsPaintEnabled())
    return;


  mLastEventTime = nglTime();
  
//  if (mIsRendering)
//    return;

  mIsRendering = true;

  //nuiStopWatch watch(_T("nuiMainWindow::Paint"));
  //  do
  //  {
  GetIdealRect();
  SetLayout(nuiRect(0, 0, mpNGLWindow->GetWidth(), mpNGLWindow->GetHeight()));
  
  //  } while (IsTrashFull());
  
  if (!(mInvalidatePosted  || GetNeedRender() || GetNeedSelfRedraw() || GetNeedLayout() || GetNeedIdealRect()))
  {
    return;
  }
  
  //watch.AddIntermediate(_T("After FillTrash()"));
  
  nuiDrawContext* pContext = GetDrawContext();
  nuiRenderThread* pRenderThread = GetRenderThread();
  NGL_ASSERT(pRenderThread);
  if (pRenderThread->GetState() != nglThread::Running)
  {
    pRenderThread->Start();
  }

//#ifdef _UIKIT_
//  pContext->GetPainter()->SetAngle(mpNGLWindow->GetRotation());
//#endif
//#ifndef __NUI_NO_SOFTWARE__
//  nuiSoftwarePainter* pCTX = dynamic_cast<nuiSoftwarePainter*>(pContext->GetPainter());
//#endif


  pRenderThread->SetRect(GetRect().Size());

  bool DrawFullFrame = !mInvalidatePosted || (mFullFrameRedraw > 0);
  bool RestorePartial = IsPartialRedrawEnabled();
  mInvalidatePosted = false;
  if (DrawFullFrame && RestorePartial)
    EnablePartialRedraw(false);
  
  std::vector<nuiRect> RedrawList(mDirtyRects);
  
  //  static int counter = 0;
  //NGL_OUT(_T("%d OnPaint %d - %d\n"), counter++, DrawFullFrame, RestorePartial);
  
//  if (!IsMatrixIdentity())
//    pContext->MultMatrix(GetMatrix());
  mLastRendering = nglTime();

  DrawTree(pContext);
  
  if (mDisplayMouseOverObject)
    DBG_DisplayMouseOverObject();
  
  if (mDisplayMouseOverInfo)
    DBG_DisplayMouseOverInfo();
  
  if (DrawFullFrame && RestorePartial)
    EnablePartialRedraw(true);


  pRenderThread->SetRootWidget(this);
  pRenderThread->StartRendering(0,0);
  
  if (mFullFrameRedraw)
    mFullFrameRedraw--;
  
  if (mDebugSlowRedraw)
  {
    nglThread::Sleep(1);
  }
}

#else

static float Gx = 0;

void nuiMainWindow::Paint()
{
  if (!App->IsActive()) // Only repaint if the application is active!
    return;

  if (!IsPaintEnabled())
    return;
  
  mLastEventTime = nglTime();
  //nuiStopWatch watch(_T("nuiMainWindow::Paint"));
//  do 
//  {
    GetIdealRect();
    SetLayout(nuiRect(0, 0, mpNGLWindow->GetWidth(), mpNGLWindow->GetHeight()));
    
//  } while (IsTrashFull());

  if (!(mInvalidatePosted  || GetNeedRender() || GetNeedSelfRedraw() || GetNeedLayout() || GetNeedIdealRect()))
  {
    return;
  }

  //watch.AddIntermediate(_T("After FillTrash()"));
  
  nuiDrawContext* pContext = GetDrawContext();  
  pContext->GetPainter()->ResetStats();

#ifdef _UIKIT_  
  pContext->GetPainter()->SetAngle(mpNGLWindow->GetRotation());
#endif

#ifndef __NUI_NO_SOFTWARE__
  nuiSoftwarePainter* pCTX = dynamic_cast<nuiSoftwarePainter*>(pContext->GetPainter());
#endif

  mpNGLWindow->BeginSession();

  pContext->StartRendering();

  if (mDrawToSurface)
  {
    if (mpSurface)
    {
      if (mpSurface->GetWidth() != GetWidth() || mpSurface->GetHeight() != GetHeight() )
      {
        mpSurface->Release();
        mpSurface = nullptr;
      }
    }

    if (!mpSurface)
    {
      nglString name;
      static int count = 0;
      name.CFormat("nuiMainWindow %p %d", this, count++);
      mpSurface = nuiSurface::CreateSurface(name, GetWidth(), GetHeight());
      name.CFormat("nuiMainWindow_Texture %p %d", this, count++);
      mpSurface->GetTexture()->SetSource(name);
    }

    pContext->SetSurface(mpSurface);
  }

  pContext->Set2DProjectionMatrix(GetRect().Size());
  bool DrawFullFrame = !mInvalidatePosted || (mFullFrameRedraw > 0);
  bool RestorePartial = IsPartialRedrawEnabled();
  mInvalidatePosted = false;
  if (DrawFullFrame && RestorePartial)
    EnablePartialRedraw(false);

  std::vector<nuiRect> RedrawList(mDirtyRects);
    
//  static int counter = 0;
  //NGL_OUT(_T("%d OnPaint %d - %d\n"), counter++, DrawFullFrame, RestorePartial);

  if (!IsMatrixIdentity())
    pContext->MultMatrix(GetMatrix());
  mLastRendering = nglTime();
  DrawTree(pContext);

  if (mDisplayMouseOverObject)
    DBG_DisplayMouseOverObject();

  if (mDisplayMouseOverInfo)
    DBG_DisplayMouseOverInfo();

  if (DrawFullFrame && RestorePartial)
    EnablePartialRedraw(true);

  if (0)
  {
    nuiRect r(32, 32);
    r.MoveTo(Gx, 10.0f);
    Gx += 4;
    
    if (Gx > GetWidth() - r.GetWidth())
      Gx = 0;
    
    pContext->SetStrokeColor(nuiColor(128, 0, 0, 255));
    pContext->SetFillColor(nuiColor(0, 0, 128, 255));
    pContext->EnableBlending(false);
    //pContext->SetBlendFunc(nuiBlendTranspAdd);
    pContext->DrawRect(r, eStrokeAndFillShape);
  }

//  if (mpDragFeedback)
//  {
//    
//  }

  if (mDrawToSurface)
  {
    pContext->SetSurface(nullptr);

  //  pContext->StartRendering();
    pContext->Set2DProjectionMatrix(GetRect().Size());
    nuiTexture* pTex = mpSurface->GetTexture();
    pContext->SetTexture(pTex);
    pContext->EnableTexturing(true);
    pContext->SetFillColor(nuiColor(255, 255, 255));
    pContext->ResetClipRect();
    pContext->EnableClipping(false);
    nuiRect r((int)pTex->GetWidth(), (int)pTex->GetHeight());
    pContext->DrawImage(r,  r);
  }

  pContext->StopRendering();

#ifndef __NUI_NO_SOFTWARE__
  if (pCTX)
  {
    if (DrawFullFrame)
    {
      pCTX->Display(GetNGLWindow(), GetRect());      
    }
    else
    {
      for (uint i = 0; i < RedrawList.size(); i++)
        pCTX->Display(GetNGLWindow(), RedrawList[i]);
    }
  }
#endif//__NUI_NO_SOFTWARE__

  if (mDrawDirtyRects)
  {
    pContext->SetFillColor(nuiColor(255, 255, 255, 100));
    pContext->SetStrokeColor(nuiColor(255, 255, 255, 182));
    pContext->EnableBlending(true);
    for (uint i = 0; i < RedrawList.size(); i++)
      pContext->DrawRect(RedrawList[i], eStrokeAndFillShape);
  }

  //watch.AddIntermediate(_T("Before EndSession()"));
  pContext->EndSession();
  //watch.AddIntermediate(_T("Before End()"));
  mpNGLWindow->EndSession();

  if (mFullFrameRedraw)
    mFullFrameRedraw--;
  
  uint32 rops = pContext->GetPainter()->GetRenderOperations();
  uint32 verts = pContext->GetPainter()->GetVertices();
  uint32 batches = pContext->GetPainter()->GetBatches();
  //printf("Frame stats | RenderOps: %d | Vertices %d | Batches %d\n", rops, verts, batches);
  
  //Invalidate();

  if (mDebugSlowRedraw)
  {
    nglThread::Sleep(1);
  }
}
#endif


void nuiMainWindow::OnResize(uint Width, uint Height)
{
  nuiRect Rect((nuiSize)Width, (nuiSize)Height);
  //SetLayout(Rect);

  //NGL_OUT(_T("(OnResize)nglWindow::Invalidate()\n"));;
  InvalidateLayout();

  mFullFrameRedraw++;
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
}

void nuiMainWindow::OnCreation()
{
}

void nuiMainWindow::OnDestruction()
{
  //NGL_OUT(_T("OnDestruction\n"));
}

void nuiMainWindow::OnActivation()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  //OUT("OnActivation\n");
  CancelGrab();
  mMouseInfo.Buttons = 0;
}

void nuiMainWindow::OnDesactivation()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  //OUT("OnDesactivation\n");
  CancelGrab();
  mMouseInfo.Buttons = 0;
}

void nuiMainWindow::OnClose()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  //OUT("OnClose\n");

  bool quit = mQuitOnClose;
  Release();
  if (quit)
    App->Quit(0);
}

void nuiMainWindow::OnState (nglWindow::StateInfo State)
{
  //OUT("OnState\n");
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
}


void nuiMainWindow::InvalidateLayout()
{
  //NGL_OUT(_T("nuiMainWindow::InvalidateLayout\n"));
  nuiTopLevel::InvalidateLayout();
  BroadcastInvalidateLayout(this, false);
}

void nuiMainWindow::BroadcastInvalidate(nuiWidgetPtr pSender)
{
  nuiTopLevel::BroadcastInvalidate(pSender);

  //NGL_OUT(_T("(Invalidate)InvalidatePosted(%s)\n"), pSender->GetObjectClass().GetChars());
  mInvalidatePosted = true;
}

void nuiMainWindow::BroadcastInvalidateRect(nuiWidgetPtr pSender, const nuiRect& rRect)
{
  nuiTopLevel::BroadcastInvalidateRect(pSender, rRect);

  //NGL_OUT(_T("(InvalidateRect)InvalidatePosted(%s)\n"), pSender->GetObjectClass().GetChars());
  mInvalidatePosted = true;
}

void nuiMainWindow::BroadcastInvalidateLayout(nuiWidgetPtr pSender, bool BroadCastOnly)
{
//  nglString senderclass = pSender->GetObjectClass();
//  if (senderclass == "nuiMainWindow"
//      || senderclass == "nuiNavigationController"
//      || senderclass == "MainViewInstance"
//      )
//  {
//    printf("");
//  }
//  NGL_OUT(_T("(Invalidate)BroadcastInvalidateLayout(%s)\n"), pSender->GetObjectClass().GetChars());

  nuiTopLevel::BroadcastInvalidateLayout(pSender, BroadCastOnly);

  mInvalidatePosted = true;
}

void nuiMainWindow::DBG_DisplayMouseOverObject()
{
  nglMouseInfo mouse;
  mpNGLWindow->GetMouse(mouse);

  nuiWidgetPtr pWidget = GetChild((nuiSize)mouse.X,(nuiSize)mouse.Y);
  if (pWidget)
  {
    if (!nuiGetBool(pWidget->GetInheritedProperty(_T("DebugObject")),true))
      return;

    nuiRect rect = pWidget->GetRect().Size();
    pWidget->LocalToGlobal(rect);
    nuiDrawContext* pContext = GetDrawContext();
    pContext->SetFillColor(nuiColor(.5f, .5f, 1.0f, 0.1f));
    pContext->SetStrokeColor(nuiColor(0.f, 0.f, 0.f, 0.2f));
    pContext->EnableBlending(true);
    pContext->SetBlendFunc(nuiBlendTransp);
    pContext->DrawRect(rect,eStrokeAndFillShape);
    pContext->EnableBlending(false);
  }
}

void nuiMainWindow::DBG_SetMouseOverObject(bool set)
{
  if (mDisplayMouseOverObject == set)
    return;
  mDisplayMouseOverObject = set;
  nuiTopLevel::Invalidate();
}

bool nuiMainWindow::DBG_GetMouseOverObject()
{
  return mDisplayMouseOverObject;
}


void nuiMainWindow::DBG_DisplayMouseOverInfo()
{
  if (!mpInfoLabel)
    return;
  nglMouseInfo mouse;
  mpNGLWindow->GetMouse(mouse);
  nuiWidgetPtr pWidget = GetChild((nuiSize)mouse.X,(nuiSize)mouse.Y);
  if (pWidget)
  {
    if (!nuiGetBool(pWidget->GetInheritedProperty(_T("DebugInfo")),true))
      return;

    nglString text;
    text.CFormat
      (
        _T("Class: '%s'\nName: '%s'\n"),
        pWidget->GetObjectClass().GetChars(),
        pWidget->GetObjectName().GetChars()
      );
    
#if 0
    //nuiXMLNode* pNode = pWidget->Serialize(NULL,false);
    if (!pNode) // We have no information
      return;

    nuiXMLNode* pChild;
    while ((pChild = pNode->GetChild(0)))
      pNode->DelChild(pChild);
    nglString xmltext = pNode->Dump(0);
    xmltext.Replace(' ','\n');

    mpInfoLabel->SetText(text+xmltext);
#endif
  }

  nuiRect rect;
  rect = mpInfoLabel->GetIdealRect();
  // Keep at reasonable distance from mouse pointer
  rect.MoveTo((nuiSize)mouse.X+16, (nuiSize)mouse.Y+16);
  // Try to keep the tooltip inside the nuiMainWindow
  if (rect.mRight > GetRect().mRight)
    rect.Move(GetRect().mRight - rect.mRight, 0);
  if (rect.mBottom > GetRect().mBottom)
    rect.Move(0,GetRect().mBottom - rect.mBottom);
  mpInfoLabel->SetLayout(rect);

  nuiDrawContext* pContext = GetDrawContext();
  pContext->PushMatrix();
  float x = (float)mpInfoLabel->GetRect().mLeft;
  float y = (float)mpInfoLabel->GetRect().mTop;
  pContext->Translate( x, y );
  rect = mpInfoLabel->GetRect().Size();
  rect.mLeft -= 3;
  rect.mRight += 3;

  pContext->SetFillColor(nuiColor(1.f,1.f,1.f,.8f));
  pContext->SetStrokeColor(nuiColor(0.f,0.f,0.f,.3f));
  pContext->EnableBlending(true);
  pContext->SetBlendFunc(nuiBlendTransp);
  pContext->DrawRect(rect, eStrokeAndFillShape);
  DrawChild(pContext, mpInfoLabel);
  pContext->PopMatrix();
  pContext->EnableBlending(false);
}

void nuiMainWindow::DBG_SetMouseOverInfo(bool set)
{
  if (mDisplayMouseOverInfo == set)
    return;
  mDisplayMouseOverInfo = set;
  if (set)
  {
    mpInfoLabel = new nuiLabel(_T("No info"));
    mpInfoLabel->SetVisible(false);
    mpChildren.insert(mpChildren.begin(), mpInfoLabel);
  }
  else
  {
    DelChild(mpInfoLabel);
    mpInfoLabel = NULL;
  }

  nuiTopLevel::Invalidate();
}

bool nuiMainWindow::DBG_GetMouseOverInfo()
{
  return mDisplayMouseOverInfo;
}

void nuiMainWindow::InvalidateTimer(const nuiEvent& rEvent)
{
  if (!App->IsActive()) // Only repaint if the application is active!
    return;
  
  LazyPaint();

  nglTime now;
  mFPSCount++;
  if (now - mFPSDelay > 1.0)
  {
    double v = (now - mFPSDelay);
    double c = mFPSCount;
    mFPS = c / v;
    NGL_LOG(_T("fps"), NGL_LOG_DEBUG, _T("FPS: %f (%f seconds - %d frames)\n"), mFPS, v, ToNearest(c));
    
    mFPSCount = 0;
    mFPSDelay = now;
  }
  mLastPaint = now;
}


bool nuiMainWindow::IsKeyDown (nglKeyCode Key) const
{
  return mpNGLWindow->IsKeyDown(Key);
}

bool nuiMainWindow::SetMouseCursor(nuiMouseCursor Cursor)
{
  return mpNGLWindow->SetCursor(Cursor);
}

bool nuiMainWindow::OnMouseMove(nglMouseInfo& rInfo)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallMouseMove(rInfo);
}

bool nuiMainWindow::OnMouseCanceled (nglMouseInfo& rInfo)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallMouseCancel(nullptr, rInfo);
}

bool nuiMainWindow::OnMouseClick(nglMouseInfo& rInfo)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallMouseClick(rInfo);
}

bool nuiMainWindow::OnMouseWheel(nglMouseInfo& rInfo)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallMouseWheel(rInfo);
}

bool nuiMainWindow::OnMouseUnclick(nglMouseInfo& rInfo)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallMouseUnclick(rInfo);
}

void nuiMainWindow::OnTextCompositionStarted()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  CallTextCompositionStarted();
}

void nuiMainWindow::OnTextCompositionConfirmed()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  CallTextCompositionConfirmed();
}

void nuiMainWindow::OnTextCompositionCanceled()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  CallTextCompositionCanceled();
}

void nuiMainWindow::OnTextCompositionUpdated(const nglString& rString, int32 CursorPosition)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  CallTextCompositionUpdated(rString, CursorPosition);
}

nglString nuiMainWindow::OnGetTextComposition() const
{
  return CallGetTextComposition();
}

void nuiMainWindow::OnTextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const
{
  return CallTextCompositionIndexToPoint(CursorPosition, x, y);
}

bool nuiMainWindow::OnTextInput(const nglString& rUnicodeText)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallTextInput(rUnicodeText);
}

void nuiMainWindow::OnTextInputCancelled()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  CallTextInputCancelled();
}

bool nuiMainWindow::OnKeyUp(const nglKeyEvent& rEvent)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  return CallKeyUp(rEvent);
}

bool nuiMainWindow::OnKeyDown(const nglKeyEvent& rEvent)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  if (mDebugMode)
  {
    if ((IsKeyDown(NK_LCTRL) || IsKeyDown(NK_RCTRL)) &&
        (IsKeyDown(NK_LSHIFT) || IsKeyDown(NK_RSHIFT))
        )
    {
      if (rEvent.mKey == NK_D)
      {
        ShowWidgetInspector();
      }
      else if (rEvent.mKey == NK_K)
      {
        ShowWidgetInspector();
      }
      else if (rEvent.mKey == NK_T)
      {
        nuiDrawContext* pCtx = GetDrawContext();
        nuiPainter* pPainter = pCtx->GetPainter();
        pPainter->DEBUG_EnableDrawArray(!pPainter->DEBUG_GetEnableDrawArray());
        
        InvalidateLayout();
      }
      else if (rEvent.mKey == NK_W)
      {
        mDebugSlowRedraw = !mDebugSlowRedraw;
        InvalidateLayout();
      }
#if 0
#ifndef _UIKIT_
      else if (rEvent.mKey == NK_S)
      {
        nuiXMLNode* pNode = Serialize(NULL, true);
        nglString dump(pNode->Dump(0));
        nglMimeTextSource mimesrc(dump);
        App->GetClipBoard().SetData(mimesrc);
        
        //NGL_OUT(_T("Dumping this widget tree XML description:\n%s\n"), dump.GetChars());
      }
#endif
#endif
    }
  }
  return CallKeyDown(rEvent);
}

bool nuiMainWindow::OnRotation(uint Angle)
{
  mLastEventTime = nglTime();
  return true;
}

void nuiMainWindow::EnableAutoRotation(bool set)
{
  mpNGLWindow->EnableAutoRotation(set);
}

bool nuiMainWindow::GetAutoRotation() const
{
  return mpNGLWindow->GetAutoRotation();
}

bool nuiMainWindow::ShowWidgetInspector()
{
#ifndef NUI_IOS
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();

  if (mpInspectorWindow)
  {
    mpInspectorWindow->Release();
    mpInspectorWindow = NULL;
  }
  else
  {
    nglWindowInfo Info;

    nglPath ResPath = nglPath(ePathCurrent);

    Info.Pos = nglWindowInfo::ePosAuto;

    Info.Width  = 800;
    Info.Height = 600;

    mpInspectorWindow = new nuiMainWindow(nuiContextInfo(nuiContextInfo::StandardContext2D), Info, GetNGLContext(), ResPath);
    mpInspectorWindow->Acquire();
    mpInspectorWindow->SetQuitOnClose(false);
    mpInspectorWindow->AddChild(new nuiIntrospector(this));
    mpInspectorWindow->SetState(nglWindow::eShow);
    mMainWinSink.Connect(mpInspectorWindow->Destroyed, &nuiMainWindow::OnInspectorDeath);
  }
#endif
  return false;
}

void nuiMainWindow::OnInspectorDeath(const nuiEvent& rEvent)
{
  mpInspectorWindow = NULL;
  SetWatchedWidget(NULL);
}

bool nuiMainWindow::Trash()
{
  delete this;
  return true;
}



void nuiMainWindow::SetDebugMode(bool Set)
{
  mDebugMode = Set;
}

void nuiMainWindow::BeginSession()
{
  mpNGLWindow->BeginSession();
}

nglContext* nuiMainWindow::GetNGLContext() const
{
  return mpNGLWindow;
}

void nuiMainWindow::SetState(nglWindow::StateChange State)
{
  mpNGLWindow->SetState(State);
}


uint32 nuiMainWindow::GetWidth() const
{
  return mpNGLWindow->GetWidth();
}

uint32 nuiMainWindow::GetHeight() const
{
  return mpNGLWindow->GetHeight();
}

int32 nuiMainWindow::GetPosX() const
{
  int x, y;
  mpNGLWindow->GetPosition(x, y);
  return x;
}

int32 nuiMainWindow::GetPosY() const
{
  int x, y;
  mpNGLWindow->GetPosition(x, y);
  return y;
}



bool nuiMainWindow::SetSize (uint Width, uint Height)
{
  return mpNGLWindow->SetSize(Width, Height);
}

bool nuiMainWindow::SetPos(int x, int y)
{
  return mpNGLWindow->SetPosition(x, y);
}


void nuiMainWindow::SetRotation(uint Angle)
{
  mpNGLWindow->SetRotation(Angle);
}

uint nuiMainWindow::GetRotation() const
{
  return mpNGLWindow->GetRotation();
}

uint nuiMainWindow::GetError() const
{
  return mpNGLWindow->GetError();
}

const nglChar* nuiMainWindow::GetErrorStr() const
{
  return mpNGLWindow->GetErrorStr();
}

const nglChar* nuiMainWindow::GetErrorStr(uint Error) const
{
  return mpNGLWindow->GetErrorStr(Error);
}

nglWindow* nuiMainWindow::GetNGLWindow() const
{
  return mpNGLWindow;
}

void nuiMainWindow::OnDragEnter()
{
  //NGL_OUT(_T("nuiMainWindow::OnDragEnter\n"));
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
}

void nuiMainWindow::OnDragLeave()
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  //NGL_OUT(_T("nuiMainWindow::OnDragLeave\n"));
  if (mpWidgetCanDrop)
  {
    mpWidgetCanDrop->OnDropLeave();
    mpWidgetCanDrop = NULL;
  }
  if (mpDraggedWidget) {
    DelChild(mpDraggedWidget);
    mpDraggedWidget = NULL;
  }
}


nglDropEffect nuiMainWindow::OnCanDrop (nglDragAndDrop* pDragObject, int X, int Y, nglMouseInfo::Flags Button)
{
  if (mpDraggedWidget)
    SetDragFeedbackRect(X,Y);

  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  
  nuiSize x = (nuiSize)X;
  nuiSize y = (nuiSize)Y;

  WidgetCanDrop Functor(pDragObject, x, y);
  nuiWidget* pWidget = GetChildIf(x,y, &Functor);
  if (!pWidget)
  {
    if (mpWidgetCanDrop)
    {
      mpWidgetCanDrop->OnDropLeave();
      mpWidgetCanDrop = NULL;
    }
    return eDropEffectNone;
  }

  pWidget->GlobalToLocal(x, y);
  
  nglDropEffect effect = pWidget->OnCanDrop(pDragObject, x, y);
  NGL_ASSERT(effect != eDropEffectNone);
  
  if (mpWidgetCanDrop && pWidget != mpWidgetCanDrop)
    mpWidgetCanDrop->OnDropLeave();
  
  mpWidgetCanDrop = pWidget;
  return effect;
}

void nuiMainWindow::OnDropped (nglDragAndDrop* pDragObject, int X,int Y, nglMouseInfo::Flags Button)
{
  mLastEventTime = nglTime();
  mLastInteractiveEventTime = nglTime();
  nuiSize x = (nuiSize)X;
  nuiSize y = (nuiSize)Y;

  WidgetCanDrop Functor(pDragObject, x, y);
  nuiWidget* pWidget = GetChildIf(x,y, &Functor);
  if (!pWidget)
    return;
  NGL_ASSERT(pWidget);

  NGL_ASSERT(mpWidgetCanDrop == pWidget);

  pWidget->GlobalToLocal(x,y);
  pWidget->OnDropped(pDragObject, x, y, Button);
  mpWidgetCanDrop = NULL;

  if (mpDraggedWidget)
  {
    DelChild(mpDraggedWidget);
    mpDraggedWidget = NULL;
  }
}

bool nuiMainWindow::Drag(nuiWidget* pDragSource, nglDragAndDrop* pDragObject) 
{ 
  nuiTopLevel::Ungrab(nuiTopLevel::GetGrab());
  mpDragSource = pDragSource;
  mpDraggedWidget = CreateDraggedWidget(pDragSource, pDragObject);
  if (mpDraggedWidget)
    AddChild(mpDraggedWidget);
  return mpNGLWindow->Drag(pDragObject); 
}


void nuiMainWindow::OnDragRequestData(nglDragAndDrop* pDragObject, const nglString& rMimeType)
{
  mLastEventTime = nglTime();
  //NGL_OUT(_T("nuiMainWindow::OnDragRequestData\n"));
  NGL_ASSERT(mpDragSource);
  if (mpDragSource != this)
    mpDragSource->OnDragRequestData(pDragObject, rMimeType); ///< fill data for supported types just before drop occurs
}

void nuiMainWindow::OnDragStop(bool canceled)
{
  mLastEventTime = nglTime();
  if (mpDragSource && mpDragSource != this)
  {
    mpDragSource->OnDragStop(canceled); ///< advise drag source
  }

  if (mpDraggedWidget)
  {
    DelChild(mpDraggedWidget);
    mpDraggedWidget = NULL;
  }
}

///////////////////////////
nuiMainWindow::NGLWindow::NGLWindow(nuiMainWindow* pMainWindow, uint Width, uint Height, bool FullScreen)
: nglWindow(Width, Height, FullScreen),
  mpMainWindow(pMainWindow)
{
}

nuiMainWindow::NGLWindow::NGLWindow(nuiMainWindow* pMainWindow, const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, const nglContext* pShared)
: nglWindow(rContextInfo, rInfo, pShared),
  mpMainWindow(pMainWindow)
{
}

nuiMainWindow::NGLWindow::~NGLWindow()
{
}

void nuiMainWindow::NGLWindow::OnPaint()
{
  mpMainWindow->OnPaint();
}

void nuiMainWindow::NGLWindow::OnResize(uint Width, uint Height)
{
  mpMainWindow->OnResize(Width, Height);
}

void nuiMainWindow::NGLWindow::OnCreation()
{
  mpMainWindow->OnCreation();
}

void nuiMainWindow::NGLWindow::OnDestruction() 
{ 
  if (mpMainWindow) 
    mpMainWindow->OnDestruction();
}

void nuiMainWindow::NGLWindow::OnActivation()
{
  mpMainWindow->SetPaintEnabled(true);
  mpMainWindow->OnActivation();
}

void nuiMainWindow::NGLWindow::OnDesactivation()
{
  mpMainWindow->OnDesactivation();
  mpMainWindow->SetPaintEnabled(false);
}

void nuiMainWindow::NGLWindow::OnClose()
{
  mpMainWindow->OnClose();
}

void nuiMainWindow::NGLWindow::OnState(StateInfo State)
{
  mpMainWindow->OnState(State);
}

bool nuiMainWindow::NGLWindow::OnTextInput(const nglString& rUnicodeText)
{
  return mpMainWindow->OnTextInput(rUnicodeText);
}

void nuiMainWindow::NGLWindow::OnTextInputCancelled()
{
  mpMainWindow->OnTextInputCancelled();
}

bool nuiMainWindow::NGLWindow::OnKeyDown(const nglKeyEvent& rEvent)
{
  return mpMainWindow->OnKeyDown(rEvent);
}

bool nuiMainWindow::NGLWindow::OnKeyUp(const nglKeyEvent& rEvent)
{
  return mpMainWindow->OnKeyUp(rEvent);
}

bool nuiMainWindow::NGLWindow::OnMouseClick(nglMouseInfo& rInfo)
{
  return mpMainWindow->OnMouseClick(rInfo);
}

bool nuiMainWindow::NGLWindow::OnMouseUnclick(nglMouseInfo& rInfo)
{
  return mpMainWindow->OnMouseUnclick(rInfo);
}

bool nuiMainWindow::NGLWindow::OnMouseMove(nglMouseInfo& rInfo)
{
  return mpMainWindow->OnMouseMove(rInfo);
}

bool nuiMainWindow::NGLWindow::OnMouseWheel(nglMouseInfo& rInfo)
{
  return mpMainWindow->OnMouseWheel(rInfo);
}

bool nuiMainWindow::NGLWindow::OnMouseCanceled (nglMouseInfo& rInfo)
{
  return mpMainWindow->OnMouseCanceled(rInfo);
}

bool nuiMainWindow::NGLWindow::OnRotation(uint Angle)
{
  return mpMainWindow->OnRotation(Angle);
}

void nuiMainWindow::NGLWindow::OnTextCompositionStarted()
{
  mpMainWindow->OnTextCompositionStarted();
}

void nuiMainWindow::NGLWindow::OnTextCompositionConfirmed()
{
  mpMainWindow->OnTextCompositionConfirmed();
}

void nuiMainWindow::NGLWindow::OnTextCompositionCanceled()
{
  mpMainWindow->OnTextCompositionCanceled();

}

void nuiMainWindow::NGLWindow::OnTextCompositionUpdated(const nglString& rString, int32 CursorPosition)
{
  mpMainWindow->OnTextCompositionUpdated(rString, CursorPosition);
}

nglString nuiMainWindow::NGLWindow::OnGetTextComposition() const
{
  return mpMainWindow->OnGetTextComposition();
}

void nuiMainWindow::NGLWindow::OnTextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const
{
  mpMainWindow->OnTextCompositionIndexToPoint(CursorPosition, x, y);
}

// Dnd receive
void nuiMainWindow::NGLWindow::OnDragEnter()
{
  nglWindow::OnDragEnter(); 
  mpMainWindow->OnDragEnter();
}

void nuiMainWindow::NGLWindow::OnDragLeave()
{
  nglWindow::OnDragLeave();
  mpMainWindow->OnDragLeave();
}

nglDropEffect nuiMainWindow::NGLWindow::OnCanDrop(nglDragAndDrop* pDragObject, int X, int Y, nglMouseInfo::Flags Button)
{
//  printf("NGLWindow:: OnCanDrop X:%d Y%d\n", X, Y);
  return mpMainWindow->OnCanDrop(pDragObject, X, Y, Button);
}

void nuiMainWindow::NGLWindow::OnDropped(nglDragAndDrop* pDragObject, int X,int Y, nglMouseInfo::Flags Button)
{
  mpMainWindow->OnDropped(pDragObject, X, Y, Button);
  
}

// Dnd send

bool nuiMainWindow::NGLWindow::Drag(nglDragAndDrop* pDragObject)
{
  return nglWindow::Drag(pDragObject);
}


void nuiMainWindow::NGLWindow::OnDragRequestData(nglDragAndDrop* pDragObject, const nglString& rMimeType)
{
  nglWindow::OnDragRequestData(pDragObject, rMimeType);
  mpMainWindow->OnDragRequestData(pDragObject, rMimeType);
}

void nuiMainWindow::NGLWindow::OnDragStop(bool canceled)
{
  mpMainWindow->OnDragStop(canceled);
  nglWindow::OnDragStop(canceled);
}

void nuiMainWindow::SetQuitOnClose(bool Set)
{
  mQuitOnClose = Set;
}

bool nuiMainWindow::GetQuitOnClose() const
{
  return mQuitOnClose;
}

void nuiMainWindow::EnterModalState()
{
  CancelGrab();
  mMouseInfo.Buttons = 0;
  
  mpNGLWindow->EnterModalState();
  CancelGrab();
  mMouseInfo.Buttons = 0;
}

void nuiMainWindow::ExitModalState()
{
  mpNGLWindow->ExitModalState();
}

void nuiMainWindow::ForceRepaint()
{
  mpNGLWindow->ForceRepaint();
}

void nuiMainWindow::SetMainMenu(nuiMainMenu* pMainMenu)
{
  mpNGLWindow->SetMainMenu(pMainMenu);
}

nuiMainMenu* nuiMainWindow::GetMainMenu()
{
  return mpNGLWindow->GetMainMenu();
}


nuiRect nuiMainWindow::GetWindowRect() const
{
  return nuiRect((int32)GetPosX(), (int32)GetPosY(), (int32)GetWidth(), (int32)GetHeight());
}

void nuiMainWindow::SetWindowRect(nuiRect rect)
{
  SetSize(rect.GetWidth(), rect.GetHeight());
  SetPos(rect.Left(), rect.Top());
}


void nuiMainWindow::StartTextInput(int32 X, int32 Y, int32 W, int32 H)
{
  mpNGLWindow->StartTextInput(X, Y, W, H);
}

void nuiMainWindow::EndTextInput()
{
  mpNGLWindow->EndTextInput();
}

bool nuiMainWindow::IsEnteringText() const
{
  return mpNGLWindow->IsEnteringText();
}

double nuiMainWindow::GetLastEventTime() const
{
  return mLastEventTime;
}

double nuiMainWindow::GetLastInteractiveEventTime() const
{
  return mLastInteractiveEventTime;
}

void nuiMainWindow::SetPaintEnabled(bool set)
{
  mPaintEnabled = set;
  if (set)
    Invalidate();
}

bool nuiMainWindow::IsPaintEnabled() const
{
  return mPaintEnabled;
}

std::vector<nuiMainWindow*> nuiMainWindow::mpWindows;

void nuiMainWindow::Register()
{
  mpWindows.push_back(this);
}

void nuiMainWindow::Unregister()
{
  for (uint32 i = 0; i < mpWindows.size(); i++)
  {
    if (mpWindows[i] == this)
    {
      std::vector<nuiMainWindow*>::iterator it = mpWindows.begin() + i;
      mpWindows.erase(it);
      return;
    }
  }
  
  // We should always be able to unregister a window!
  NGL_ASSERT(0);
}


void nuiMainWindow::DestroyAllWindows()
{
  int32 count = 0;
  while (mpWindows.size() > 0)
  {
    nuiMainWindow* pWin = mpWindows.front();
    NGL_LOG("window", NGL_LOG_ALWAYS, "Destroying window #%d '%s'\n", count, pWin->GetNGLWindow()->GetTitle().GetChars());
    pWin->Release();
    count++;
  }
  NGL_ASSERT(mpWindows.empty());
}

float nuiMainWindow::GetScale() const
{
  return mpNGLWindow->GetScale();
}

float nuiMainWindow::GetScaleInv() const
{
  return mpNGLWindow->GetScaleInv();
}


