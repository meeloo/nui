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
nglCriticalSection nuiRenderThread::ThreadsCS;

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
  Post(nuiMakeTask(this, &nuiRenderThread::_SetWidgetDrawPainter, pWidget, pPainter));
}

void nuiRenderThread::SetWidgetContentsPainter(nuiWidget* pWidget, nuiRef<nuiMetaPainter> pPainter)
{
  Post(nuiMakeTask(this, &nuiRenderThread::_SetWidgetContentsPainter, pWidget, pPainter));
}

void nuiRenderThread::SetLayerDrawPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("SetLayerDrawPainter %p (%p %d)\n", pLayer, pPainter, pPainter->GetRefCount());
//  pPainter->nuiRefCount::SetTrace(true);
  Post(nuiMakeTask(this, &nuiRenderThread::_SetLayerDrawPainter, pLayer, pPainter));
}

void nuiRenderThread::SetLayerContentsPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("SetLayerContentsPainter %p -> %p\n", pLayer, pPainter);
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
    RenderingDone(false);
    return;
  }
  
  nuiRef<nuiMetaPainter> pRootPainter = it->second;
  if (pRootPainter == nullptr)
  {
    RenderingDone(false);
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
  glPushGroupMarkerEXT(0, "Update dirty layers");
  
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
  //    NGL_OUT("DONE - Create surface for current frame (%d)\n", count);
  
//  NGL_OUT("*** Update %d layers\n", mDirtyLayers.size());
  for (auto layer : mDirtyLayers)
  {
    DrawLayerContents(mpDrawContext, layer);
  }
  mDirtyLayers.clear();


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
  
  //NGL_OUT(">>> Draw the widget tree %p\n", this);
  mpDrawContext->ResetState();
  mpDrawContext->ResetClipRect();
  mpDrawContext->Set2DProjectionMatrix(mRect.Size());
  DrawWidget(mpDrawContext, mpRoot);

  mpDrawContext->StopRendering();
  mpPainter->EndSession();
  mpContext->EndSession();
  
  mpContext->GetLock().Unlock();
  
  RenderingDone(true);

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
//  NGL_OUT("_SetWidgetDrawPainter %p %s %s (%p)\n", pWidget, pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars(), pPainter);
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
  //  NGL_OUT("_SetWidgetContentsPainter %p %s %s (%p)\n", pWidget, pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars(), pPainter);
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
//  NGL_OUT("_SetLayerDrawPainter %p (%p)\n", pLayer, pPainter);
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
//  NGL_OUT("_SetLayerContentsPainter %p (%p - %d)\n", pLayer, pPainter, SetLayerContentsPainterCounter);
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
    str.CFormat("Draw widget contents %s %p", pPainter->GetName().GetChars(), pKey);
    nglString indent;
    indent.Fill("  ", mWidgetIndentation);
    str.Prepend(indent);
    glPushGroupMarkerEXT(0, str.GetChars());
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
  auto& stats = mLayerStats[pKey];
  nglTime start;

#if _DEBUG
  static int count = 0;
  nglString str;
  nglString tmp;
  tmp.Fill("  ", count);
//  str.CFormat("%sDraw layer contents %s %s %p", tmp.GetChars(), pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);
  str.CFormat("%sDraw layer contents %p", tmp.GetChars(), pKey);
  glPushGroupMarkerEXT(0, str.GetChars());
//  NGL_OUT("%s\n", str.GetChars());
#endif
  
  nuiRef<nuiMetaPainter> pPainter;
  auto it = mLayerContentsPainters.find(pKey);
  if (it != mLayerContentsPainters.end())
  {
    pPainter = it->second;
//    NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, pPainter, pPainter->GetRefCount());
    NGL_ASSERT(pPainter);

#if _DEBUG
    count++;
#endif
    
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidgetContents), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));

#if _DEBUG
    count--;
#endif
  }
#if _DEBUG
  glPopGroupMarkerEXT();
#endif

  if (pPainter)
  {
    nglTime end;
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
    str.CFormat("Draw widget %s %p", pPainter->GetName().GetChars(), pKey);
    nglString indent;
    indent.Fill("  ", mWidgetIndentation);
    str.Prepend(indent);
    glPushGroupMarkerEXT(0, str.GetChars());
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
  nglString tmp;
  tmp.Fill("  ", count);
  str.CFormat("%sDraw layer %s %s %p", tmp.GetChars(), pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);
  glPushGroupMarkerEXT(0, str.GetChars());
//  NGL_OUT("%s\n", str.GetChars());
#endif
  
  auto it = mLayerDrawPainters.find(pKey);
  if (it != mLayerDrawPainters.end())
  {
    nuiRef<nuiMetaPainter> pPainter = it->second;
    //    NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, pPainter, pPainter->GetRefCount());
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

const std::map<nuiLayer*, nuiRenderingStat>& nuiRenderThread::GetStats() const
{
  return mLayerStats;
}

static bool CompareLayerStats(const std::pair<nuiLayer*, nuiRenderingStat>& first, const std::pair<nuiLayer*, nuiRenderingStat> & second)
{
  return first.second.mTime > second.second.mTime;
}

void nuiRenderThread::DumpStats()
{
  static int i = 0;
  if ((i++ % 100))
    return;

  std::vector<std::pair<nuiLayer*, nuiRenderingStat> > sortedlayers;

  for (const auto& stat : mLayerStats)
  {
    sortedlayers.push_back(stat);
  }

  std::sort(sortedlayers.begin(), sortedlayers.end(), CompareLayerStats);

  for (int i = 0; i < MIN(10, sortedlayers.size()); i++)
  {
    const std::pair<nuiLayer*, nuiRenderingStat> &stat(sortedlayers[i]);
    NGL_OUT("%d: %p %f %d %s\n", i, stat.first, stat.second.mTime, stat.second.mCount, stat.second.mName.GetChars());
  }
}

