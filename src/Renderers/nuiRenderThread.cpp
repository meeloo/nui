//
//  nuiRenderThread.cpp
//  nui3
//
//  Created by Sebastien Metrot on 14/12/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#include "nui.h"
#include "nuiMetaPainter.h"

#if (defined _UIKIT_) || (defined _COCOA_)
# import <Foundation/NSAutoreleasePool.h>
#endif

#define NUI_ENABLE_THREADED_RENDERING 1

std::set<nuiRenderThread*> nuiRenderThread::mThreads;
nglCriticalSection nuiRenderThread::ThreadsCS(nglString(__FILE__).Add(":").Add(__LINE__).GetChars());

nuiRenderThread::nuiRenderThread(nglContext* pContext, nuiDrawContext* pDrawContext, nuiPainter* pDestinationPainter, const RenderingDoneDelegate& rRenderingDone)
: mpContext(pContext), mpDrawContext(pDrawContext), mpPainter(pDestinationPainter), mRenderingDone(rRenderingDone)
{
  nglCriticalSectionGuard g(ThreadsCS);
  mThreads.insert(this);
}

nuiRenderThread::~nuiRenderThread()
{
  nglCriticalSectionGuard g(ThreadsCS);
  mThreads.erase(this);
}

void nuiRenderThread::Post(nuiTask* pTask)
{
#if NUI_ENABLE_THREADED_RENDERING
  mQueue.Post(pTask);
#else
  pTask->Run();
  pTask->Release();
#endif
}


void nuiRenderThread::OnStart()
{
  nuiTask* pTask = nullptr;

#if (defined _UIKIT_) || (defined _COCOA_)
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
#endif

  while (mContinue && (pTask = mQueue.Get(1000)))
  {
    pTask->Run();
    pTask->Release();
//    NGL_OUT("Unqueued task %p, %d left (%d contents painter requests)\n", pTask, mQueue.GetSize(), SetLayerContentsPainterCounter);
  }

#if (defined _UIKIT_) || (defined _COCOA_)
  [pPool release];
#endif
}

void nuiRenderThread::RunTaskOnRenderThread(nuiTask* pTask, bool OnNextFrame)
{
  if (OnNextFrame)
    mNextFrameQueue.Post(pTask);
  else
    Post(pTask);
}

// Public API:
void nuiRenderThread::StartRendering(uint32 x, uint32 y)
{
  ngl_atomic_inc(mRenderingTicks);
  Post(nuiMakeTask(this, &nuiRenderThread::_StartRendering, x, y));
}

void nuiRenderThread::SetRect(const nuiRect& rRect)
{
  Post(nuiMakeTask(this, &nuiRenderThread::_SetRect, rRect));
}

void nuiRenderThread::Exit()
{
  Post(nuiMakeTask(this, &nuiRenderThread::_Exit));
}

void nuiRenderThread::DestroyWidget(nuiWidget* pWidget)
{
  nglCriticalSectionGuard g(ThreadsCS);

  for (auto it : mThreads)
  {
    it->SetWidgetContentsPainter(pWidget, nullptr);
    it->SetWidgetDrawPainter(pWidget, nullptr);
  }
}

void nuiRenderThread::DestroyLayer(nuiLayer* pLayer)
{
  nglCriticalSectionGuard g(ThreadsCS);

  for (auto it : mThreads)
  {
    it->SetLayerContentsPainter(pLayer, nullptr);
    it->SetLayerDrawPainter(pLayer, nullptr);
  }
}

void nuiRenderThread::SetWidgetDrawPainter(nuiWidget* pWidget, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("SetWidgetDrawPainter %p (%p) [%s]\n", pWidget, (void*)pPainter, pWidget->GetObjectName().GetChars());
  Post(nuiMakeTask(this, &nuiRenderThread::_SetWidgetDrawPainter, pWidget, pPainter));
}

void nuiRenderThread::SetWidgetContentsPainter(nuiWidget* pWidget, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("SetWidgetContentsPainter %p (%p) [%s]\n", pWidget, (void*)pPainter, pWidget->GetObjectName().GetChars());
  Post(nuiMakeTask(this, &nuiRenderThread::_SetWidgetContentsPainter, pWidget, pPainter));
}

void nuiRenderThread::SetLayerDrawPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("SetLayerDrawPainter %p (%p) [%s]\n", pLayer, (void*)pPainter, pLayer->GetObjectName().GetChars());
//  pPainter->nuiRefCount::SetTrace(true);
  Post(nuiMakeTask(this, &nuiRenderThread::_SetLayerDrawPainter, pLayer, pPainter));
}

void nuiRenderThread::SetLayerContentsPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("SetLayerContentsPainter %p -> %p [%s]\n", pLayer, (void*)pPainter, pLayer->GetObjectName().GetChars());
  nuiTask* task = nuiMakeTask(this, &nuiRenderThread::_SetLayerContentsPainter, pLayer, pPainter);

  Post(task);
}

void nuiRenderThread::InvalidateLayerContents(nuiLayer* pLayer)
{
//  NGL_OUT("InvalidateLayerContents %p\n", pLayer);
  Post(nuiMakeTask(this, &nuiRenderThread::_InvalidateLayerContents, pLayer));
}


void nuiRenderThread::SetRootWidget(nuiWidget* pRoot)
{
  Post(nuiMakeTask(this, &nuiRenderThread::_SetRootWidget, pRoot));
}

void nuiRenderThread::SetLayerTree(nuiLayer* pLayerRoot)
{
//  NGL_OUT("SetLayerTree %p\n", pLayerRoot);
  Post(nuiMakeTask(this, &nuiRenderThread::_SetLayerTree, pLayerRoot));
}

void nuiRenderThread::InvalidateLayerRect(nuiLayer *pLayer, nuiRect rect)
{
  Post(nuiMakeTask(this, &nuiRenderThread::_InvalidateLayerRect, pLayer, rect));
}

void nuiRenderThread::RenderingDone(bool result)
{
  mRenderingDone(this, result);
}

void nuiRenderThread::_StartRendering(uint32 x, uint32 y)
{
  if (ngl_atomic_dec(mRenderingTicks) > 0)
  {
//    NGL_OUT("[nuiRenderThread] skipping frame\n");
    return;
  }
//  NGL_OUT("[nuiRenderThread] render %p\n", this);

//  NGL_OUT("Widget %d / ContentsLayer %d / DrawLayer %d / DirtyLayers %d (total meta painters %d)\n", mWidgetPainters.size(), mLayerContentsPainters.size(), mLayerDrawPainters.size(), mDirtyLayers.size(), (int32)nuiMetaPainter::GetNbInstances());

//    NGL_ASSERT(0);
  auto it = mWidgetDrawPainters.find(mpRoot);
  if (it == mWidgetDrawPainters.end())
  {
    App->GetMainQueue().Post(nuiMakeTask(this, &nuiRenderThread::RenderingDone, false));
    return;
  }
  
  nuiRef<nuiMetaPainter> pRootPainter = it->second;
  if (pRootPainter == nullptr)
  {
    App->GetMainQueue().Post(nuiMakeTask(this, &nuiRenderThread::RenderingDone, false));
    return;
  }

  // Try locking
  if (!TryLockRendering())
  {
    return;
  }

  mpContext->GetLock().Lock();
  
  mpPainter->ResetStats();
  mpContext->BeginSession();
  mpPainter->BeginSession();
  
  mpDrawContext->SetPainter(mpPainter);
  mpDrawContext->StartRendering();
  
  nuiTask* pTask = nullptr;
  while (mContinue && (pTask = mNextFrameQueue.Get(0)))
  {
    pTask->Run();
    pTask->Release();
  }
  
  mpDrawContext->ResetState();
  mpDrawContext->ResetClipRect();

  // Update Layers:
  glPushGroupMarkerEXT(0, "Create new layers");
  
  // Create needed surfaces once for this frame:
  //    NGL_OUT("Create surface for current frame\n");
  int count = 0;
  for (auto elem : mLayerContentsPainters)
  {
    nuiRef<nuiMetaPainter> pPainter = elem.second;
    auto surfaces(pPainter->GetSurfaces());
    for (auto surface : surfaces)
    {
      mpPainter->CreateSurface(surface);
      count++;
    }
  }

  glPopGroupMarkerEXT();
  //    NGL_OUT("DONE - Create surface for current frame (%d)\n", count);
  
  glPushGroupMarkerEXT(0, "Update dirty layers");
//  NGL_OUT("*** Update %d layers\n", mDirtyLayers.size());
  for (auto layer : mDirtyLayers)
  {
    DrawLayerContents(mpDrawContext, layer);
  }
  mDirtyLayers.clear();
  mPartialRects.clear();
  glPopGroupMarkerEXT();


  mpDrawContext->SetClearColor(nuiColor(255,255,255));
  //  if (mClearBackground)
  {
    mpDrawContext->Clear();
  }
  //  else
  //  {
  //    // Force the initial render state anyway!
  //    mpDrawContext->DrawRect(nuiRect(0,0,0,0), eStrokeShape);
  //  }
  
  mWidgetIndentation = 0;
  
  mpDrawContext->StartRendering();
  glPushGroupMarkerEXT(0, "Reset State");
//  NGL_OUT(">>> Draw the widget tree %p\n", this);
  mpDrawContext->ResetState();
//  NGL_OUT("Reset clip rect 1\n");
  mpDrawContext->ResetClipRect();
  mpDrawContext->EnableClipping(false);
  mpDrawContext->Set2DProjectionMatrix(mRect.Size());
  glPopGroupMarkerEXT();

  glPushGroupMarkerEXT(0, "Draw widget tree");
  DrawWidget(mpDrawContext, mpRoot);
  glPopGroupMarkerEXT();

  glPushGroupMarkerEXT(0, "Finish Rendering");
  mpDrawContext->StopRendering();
  mpPainter->EndSession();
  mpContext->EndSession();
  glPopGroupMarkerEXT();

  mpContext->GetLock().Unlock();

  App->GetMainQueue().Post(nuiMakeTask(this, &nuiRenderThread::RenderingDone, true));

  UnlockRendering();

//  DumpStats();
}

void nuiRenderThread::_SetRect(const nuiRect& rRect)
{
  mRect = rRect;
}

void nuiRenderThread::_Exit()
{
  NGL_OUT("Exit render thread requested\n");
  mContinue = false;
}

void nuiRenderThread::_SetWidgetDrawPainter(nuiWidget* pWidget, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("_SetWidgetDrawPainter %p %s %s (%p)\n", pWidget, pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars(), (void*)pPainter);
  auto it = mWidgetDrawPainters.find(pWidget);
  if (it != mWidgetDrawPainters.end())
  {
    mpContext->GetLock().Lock();

    nuiRef<nuiMetaPainter> pOld = it->second;
    NGL_ASSERT(pOld);

    if (pPainter)
    {
      it->second = pPainter;
    }
    else
    {
      mWidgetDrawPainters.erase(it);
    }

    mpContext->GetLock().Unlock();
    return;
  }
  else if (pPainter)
  {
    mWidgetDrawPainters[pWidget] = pPainter;
  }
}

void nuiRenderThread::_SetWidgetContentsPainter(nuiWidget* pWidget, nuiRef<nuiMetaPainter> pPainter)
{
//    NGL_OUT("_SetWidgetContentsPainter %p %s %s (%p)\n", pWidget, pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars(), (void*)pPainter);
  auto it = mWidgetContentsPainters.find(pWidget);
  if (it != mWidgetContentsPainters.end())
  {
    mpContext->GetLock().Lock();
    nuiRef<nuiMetaPainter> pOld = it->second;
    NGL_ASSERT(pOld);
    

    if (pPainter)
    {
      it->second = pPainter;
    }
    else
    {
      mWidgetContentsPainters.erase(it);
    }

    mpContext->GetLock().Unlock();
    return;
  }
  else if (pPainter)
  {
    mWidgetContentsPainters[pWidget] = pPainter;
  }
}

void nuiRenderThread::_SetLayerDrawPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("_SetLayerDrawPainter %p (%p) %s\n", pLayer, (void*)pPainter, pPainter?pPainter->GetName().GetChars():"NULL");
  auto it = mLayerDrawPainters.find(pLayer);
  if (it != mLayerDrawPainters.end())
  {
    mpContext->GetLock().Lock();
    nuiRef<nuiMetaPainter> pOld = it->second;
    NGL_ASSERT(pOld);
    
//    NGL_OUT("                   %p replaces %p\n", pPainter, pOld);

    if (pPainter)
    {
      it->second = pPainter;
    }
    else
    {
      mLayerDrawPainters.erase(it);
    }
    mpContext->GetLock().Unlock();
    return;
  }
  else if (pPainter)
  {
    mLayerDrawPainters[pLayer] = pPainter;
  }
}

void nuiRenderThread::_SetLayerContentsPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("_SetLayerContentsPainter %p (%p) %s\n", pLayer, (void*)pPainter, pPainter?pPainter->GetName().GetChars():"NULL");
  auto it = mLayerContentsPainters.find(pLayer);
  if (it != mLayerContentsPainters.end())
  {
    mpContext->GetLock().Lock();

    nuiRef<nuiMetaPainter> pOld = it->second;
    NGL_ASSERT(pOld);
//    NGL_OUT("                   %p replaces %p\n", pPainter, pOld);

    if (pOld->GetRefCount() == 1)
      it->second = nullptr;

    if (pPainter)
    {
      it->second = pPainter;
      mDirtyLayers.insert(pLayer);
    }
    else
    {
      mLayerContentsPainters.erase(it);
      mDirtyLayers.erase(pLayer);
    }
    mpContext->GetLock().Unlock();
    return;
  }
  else if (pPainter)
  {
    mLayerContentsPainters[pLayer] = pPainter;
    mDirtyLayers.insert(pLayer);
  }
  else
  {
    mDirtyLayers.erase(pLayer);
  }


//  NGL_OUT("LayerContentsPainters (%d)\n", mLayerContentsPainters.size());
  for (auto it : mLayerContentsPainters)
  {
//    NGL_OUT("\t%p -> %p [%d]\n", it.first, it.second, it.second->GetRefCount());
  }
}

void nuiRenderThread::_InvalidateLayerContents(nuiLayer* pLayer)
{
//    NGL_OUT("_InvalidateLayerContents %p\n", pLayer);
    mDirtyLayers.insert(pLayer);
}


void nuiRenderThread::_SetRootWidget(nuiWidget* pRoot)
{
  mpRoot = pRoot;
}

void nuiRenderThread::_SetLayerTree(nuiLayer* pRoot)
{
//  NGL_OUT("_SetLayerTree %p\n", pRoot);
  mpLayerTreeRoot = pRoot;
}

void nuiRenderThread::_InvalidateLayerRect(nuiLayer *pLayer, nuiRect rect)
{
//    NGL_OUT("+++ AddInvalidRect in %s %s %s\n", pLayer->GetObjectClass().GetChars(), pLayer->GetObjectName().GetChars(), rect.GetValue().GetChars());

  size_t count = 0;
  auto it = mPartialRects.find(pLayer);
  if (it != mPartialRects.end())
    count = it->second.size();

  nuiRect intersect;
  nuiSize surface = rect.GetSurface();
  if (surface == 0.0f)
    return;

  for (size_t i = 0; i < count; i++)
  {
    nuiRect r(it->second[i]);
    if (intersect.Intersect(rect, r))
    {
      // The rectangles intersect so we create one big out of them
      nuiRect u;
      u.Union(rect, r);
      // Let's remove the coalesced rect from the list
      it->second.erase(it->second.begin() + (long)i);
      // Try to add the new big rect to the list:
      _InvalidateLayerRect(pLayer, u);
      return;
    }
  }

  // Found no rect to blend into, let's create a new one:
  //  NGL_OUT("--- AddInvalidRect OK %s\n", rRect.GetValue().GetChars());
  mPartialRects[pLayer].push_back(rect);
  mDirtyLayers.insert(pLayer);

}

void nuiRenderThread::DrawWidgetContents(nuiDrawContext* pContext, nuiWidget* pKey)
{
  mWidgetIndentation++;

  nuiRef<nuiMetaPainter> pPainter = nullptr;
  auto it = mWidgetContentsPainters.find(pKey);
  if (it != mWidgetContentsPainters.end())
    pPainter = it->second;

  if (pPainter)
  {
#if _DEBUG
    nglString str;
    str.CFormat("DrawWidgetContents %s %p", pPainter->GetName().GetChars(), pKey);
    glPushGroupMarkerEXT(0, str.GetChars());
    nglString indent;
    indent.Fill("  ", mWidgetIndentation);
    str.Prepend(indent);
//    NGL_OUT("%s\n", str.GetChars());
#endif
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
//    pContext->SetStrokeColor(nuiColor("green"));
//    pContext->DrawLine(0, 0, pKey->GetRect().GetWidth(), pKey->GetRect().GetHeight());
#if _DEBUG
    glPopGroupMarkerEXT();
#endif
  }
  mWidgetIndentation--;
}

void nuiRenderThread::DrawLayerContents(nuiDrawContext* pContext, nuiLayer* pKey)
{
  nglTime start;

#if _DEBUG
  static int count = 0;
  nglString str;
  nglString tmp;
  str.CFormat("DrawLayerContents %p", pKey);
  glPushGroupMarkerEXT(0, str.GetChars());
//  tmp.Fill("  ", count);
//  str.Prepend(tmp);
  //  str.CFormat("%sDraw layer contents %s %s %p", tmp.GetChars(), pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);

//  NGL_OUT("%s\n", str.GetChars());

  count++;
#endif


  nuiRef<nuiMetaPainter> pPainter;
  auto it = mLayerContentsPainters.find(pKey);
  if (it != mLayerContentsPainters.end())
  {
    pPainter = it->second;
//    NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, pPainter, pPainter->GetRefCount());
    NGL_ASSERT(pPainter);

    size_t count = 0;
    auto itt = mPartialRects.find(pKey);
    if (itt != mPartialRects.end())
      count = itt->second.size();

//    uint32 Width = 0, Height = 0;
//    pPainter->GetSize(Width, Height);
//    nuiRect rect((float)Width, (float)Height);
    mpDrawContext->ResetState();
//    mpDrawContext->Set2DProjectionMatrix(rect);

    if (count == 0)
    {
//      NGL_OUT("%sReset clip rect 2\n", tmp.GetChars());
      mpDrawContext->ResetClipRect();
//      NGL_OUT("%sDisable clipping\n", tmp.GetChars());
      mpDrawContext->EnableClipping(false);

      pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidgetContents), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    }
    else
    {

      for (size_t i = 0; i < count; i++)
      {
        mpDrawContext->ResetState();
        nuiRect cliprect(itt->second[i]); // Partial rect...
//        NGL_OUT("%sReset clip rect 3\n", tmp.GetChars());
        mpDrawContext->ResetClipRect();
//        NGL_OUT("%sClip to %s\n", tmp.GetChars(), cliprect.GetValue().GetChars());
        mpDrawContext->Clip(cliprect);
//        NGL_OUT("%sDisable clipping\n", tmp.GetChars());
        mpDrawContext->EnableClipping(false);
        pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidgetContents), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
      }
    }

  }

#if _DEBUG
  count--;
  glPopGroupMarkerEXT();
#endif

  if (pPainter)
  {
    nglTime end;
    
    nglCriticalSectionGuard g(StatsCS);
    auto& stats = mLayerStats[pKey];
    double diff = end - start;
    stats.mCount++;
    stats.mTime += diff;
    stats.mName = pPainter->GetName();
  }
}

void nuiRenderThread::DrawWidget(nuiDrawContext* pContext, nuiWidget* pKey)
{
  mWidgetIndentation++;
  
  nuiRef<nuiMetaPainter> pPainter = nullptr;
  auto it = mWidgetDrawPainters.find(pKey);
  if (it != mWidgetDrawPainters.end())
    pPainter = it->second;
  
  if (pPainter)
  {
#if _DEBUG
    nglString str;
    str.CFormat("DrawWidget %s %p", pPainter->GetName().GetChars(), pKey);
    glPushGroupMarkerEXT(0, str.GetChars());
//    nglString indent;
//    indent.Fill("  ", mWidgetIndentation);
//    str.Prepend(indent);
//    NGL_OUT("%s\n", str.GetChars());
#endif
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    //    pContext->SetStrokeColor(nuiColor("green"));
    //    pContext->DrawLine(0, 0, pKey->GetRect().GetWidth(), pKey->GetRect().GetHeight());
#if _DEBUG
    glPopGroupMarkerEXT();
#endif
  }
  mWidgetIndentation--;
}

void nuiRenderThread::DrawLayer(nuiDrawContext* pContext, nuiLayer* pKey)
{
#if _DEBUG
  static int count = 0;
  nglString str;
  str.CFormat("DrawLayer %s %s %p", pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);
  glPushGroupMarkerEXT(0, str.GetChars());
//  nglString tmp;
//  tmp.Fill("  ", count);
//  tmp.Add(str);
//  NGL_OUT("%s\n", str.GetChars());
#endif
  
  auto it = mLayerDrawPainters.find(pKey);
  if (it != mLayerDrawPainters.end())
  {
    nuiRef<nuiMetaPainter> pPainter = it->second;
//        NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, pPainter, pPainter->GetRefCount());
    NGL_ASSERT(pPainter);
    
#if _DEBUG
    count++;
#endif
    
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    
#if _DEBUG
    count--;
#endif
  }
#if _DEBUG
  glPopGroupMarkerEXT();
#endif
}


nuiPainter* nuiRenderThread::GetPainter() const
{
  return mpPainter;
}

nuiDrawContext* nuiRenderThread::GetDrawContext() const
{
  return mpDrawContext;
}

nglContext* nuiRenderThread::GetContext() const
{
  return mpContext;
}

std::map<nuiLayer*, nuiRenderingStat> nuiRenderThread::GetStats() const
{
  nglCriticalSectionGuard g(StatsCS);
  return mLayerStats;
}

static bool CompareLayerTimes(const std::pair<nuiLayer*, nuiRenderingStat>& first, const std::pair<nuiLayer*, nuiRenderingStat> & second)
{
  return first.second.mTime > second.second.mTime;
}

static bool CompareLayerCounts(const std::pair<nuiLayer*, nuiRenderingStat>& first, const std::pair<nuiLayer*, nuiRenderingStat> & second)
{
  return first.second.mCount > second.second.mCount;
}

void nuiRenderThread::DumpStats()
{
  static int i = 0;
  if ((i++ % 60))
    return;

  std::vector<std::pair<nuiLayer*, nuiRenderingStat> > sortedlayers;

  {
    nglCriticalSectionGuard g(StatsCS);
    for (const auto& stat : mLayerStats)
    {
      sortedlayers.push_back(stat);
    }
  }
  
  std::sort(sortedlayers.begin(), sortedlayers.end(), CompareLayerCounts);

  for (size_t i = 0; i < MIN(20, sortedlayers.size()); i++)
  {
    const std::pair<nuiLayer*, nuiRenderingStat> &stat(sortedlayers[i]);
//    NGL_OUT("%d: %p %f %d %s\n", i, stat.first, stat.second.mTime, stat.second.mCount, stat.second.mName.GetChars());
  }
}

