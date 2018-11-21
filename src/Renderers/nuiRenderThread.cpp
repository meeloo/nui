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
# import <sys/kdebug_signpost.h>
#endif


#define NUI_ENABLE_THREADED_RENDERING 1

#if _DEBUG
#define NUI_LOG_RENDERING 0
#define NUI_LOG_DRAWWIDGET 0
#define NUI_LOG_DRAWLAYER 0
#define NUI_LOG_DRAWWIDGETCONTENTS 0
#define NUI_LOG_DRAWLAYERCONTENTS 0

#define NUI_LOG_INVALLAYERRECT 0
#define NUI_LOG_INVALLAYERCONTENTS 0

#else
#define NUI_LOG_RENDERING 0
#define NUI_LOG_DRAWWIDGET 0
#define NUI_LOG_DRAWLAYER 0
#define NUI_LOG_DRAWWIDGETCONTENTS 0
#define NUI_LOG_DRAWLAYERCONTENTS 0

#define NUI_LOG_INVALLAYERRECT 0
#define NUI_LOG_INVALLAYERCONTENTS 0
#endif

double gTotal = 0;
double gResetState = 0;
double gPartialRedraw = 0;
double gGlobalRedraw = 0;
double gPainterRedraw = 0;

#define kdebug_signpost_start
#define kdebug_signpost_end

std::set<nuiRenderThread*> nuiRenderThread::mThreads;
nglCriticalSection nuiRenderThread::ThreadsCS(nglString(__FILE__).Add(":").Add(__LINE__).GetChars());

nuiRenderThread::nuiRenderThread(nglContext* pContext, nuiDrawContext* pDrawContext, nuiPainter* pDestinationPainter, const RenderingDoneDelegate& rRenderingDone)
: nglThread("nuiRenderThread"), mRenderingTicks(0), mpContext(pContext), mpDrawContext(pDrawContext), mpPainter(pDestinationPainter), mRenderingDone(rRenderingDone), mRenderingLock("nuiRenderThread::RenderingLock"), StatsCS("nuiRenderThread::StatCS")
{
  nglCriticalSectionGuard g(ThreadsCS);
  mThreads.insert(this);
#if (defined _UIKIT_) || (defined _COCOA_)
//  mUseSignPosts = true;
#endif
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

  while (mContinue)
  {
    pTask = mQueue.Get(10);
    if (pTask)
    {
#if (defined _UIKIT_) || (defined _COCOA_)
      @autoreleasepool
#endif
      {
        pTask->Run();
        pTask->Release();
  //    NGL_OUT("Unqueued task %p, %d left (%d contents painter requests)\n", pTask, mQueue.GetSize(), SetLayerContentsPainterCounter);
      }
    }
  }
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
//  NGL_OUT("[nuiRenderThread] POST render %p\n", this);
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
  if (!pPainter)
  {
//    NGL_OUT("SetLayerContentsPainter %p -> %p [%s]\n", pLayer, (void*)pPainter, pLayer->GetObjectName().GetChars());
  }
  nuiTask* task = nuiMakeTask(this, &nuiRenderThread::_SetLayerContentsPainter, pLayer, pPainter);

  Post(task);
}

void nuiRenderThread::InvalidateLayerContents(nuiLayer* pLayer)
{
#if NUI_LOG_INVALLAYERCONTENTS
//  NGL_OUT("InvalidateLayerContents %p [%s]\n", pLayer, pLayer->GetObjectName().GetChars());
#endif
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
#if NUI_LOG_INVALLAYERRECT
  NGL_OUT("InvalidateLayerRect %p %s [%s]\n", pLayer, rect.GetValue().GetChars(), pLayer?pLayer->GetObjectName().GetChars():"ROOT");
#endif
  Post(nuiMakeTask(this, &nuiRenderThread::_InvalidateLayerRect, pLayer, rect));
}

void nuiRenderThread::DumpAllTexturesToFolder(const nglPath& rFolderPath)
{
  Post(nuiMakeTask(this, &nuiRenderThread::_DumpAllTexturesToFolder, rFolderPath));
}

void nuiRenderThread::_DumpAllTexturesToFolder(const nglPath FolderPath)
{
  if (!FolderPath.Exists())
  {
    FolderPath.Create(true);
  }

  if (FolderPath.IsLeaf())
  {
    return;
  }

  mpContext->BeginSession();
  mpPainter->BeginSession();

  auto textures = nuiTexture::Enum();
  for (auto it : textures)
  {
    nuiTexture* pTexture = it.second;
    nglImage* pImage = mpPainter->CreateImageFromGPUTexture(pTexture);
    if (pImage)
    {
      nglPath path(FolderPath);
      nglString name;
      name.CFormat("%s.png", pTexture->GetSource().GetChars());
      path += name;

      nglImageCodec* codec = nglImage::CreateCodec("PNG");

      bool result = pImage->Save(path, codec);
      NGL_OUT("[%s] Saved Texture %p to '%s'\n", (result?"OK": "FAILED"), pTexture, path.GetChars());
      delete pImage;
      delete codec;
    }

  }
}

void nuiRenderThread::RenderingDone(bool result)
{
  mRenderingDone(this, result);
}

void nuiRenderThread::_StartRendering(uint32 x, uint32 y)
{
  if (mUseSignPosts)
    kdebug_signpost_start(0, (uintptr_t)this, 0, 0, 0);
  if (ngl_atomic_dec(mRenderingTicks) > 0)
  {
    NGL_OUT("[nuiRenderThread] skipping frame\n");
    if (mUseSignPosts)
      kdebug_signpost_end(0, (uintptr_t)this, 0, 0, 0);
    return;
  }
  
  nglGuard<nglLock> guard(mpContext->GetLock());
  
//  NGL_OUT("[nuiRenderThread] render %p\n", this);
#if NUI_LOG_RENDERING
NGL_OUT("#######################################################################################\n");
  NGL_OUT("Widget Contents %d / ContentsLayer %d / DrawLayer %d / DirtyLayers %d (total meta painters %d)\n", mWidgetContentsPainters.size(), mLayerContentsPainters.size(), mLayerDrawPainters.size(), mDirtyLayers.size(), (int32)nuiMetaPainter::GetNbInstances());
#endif

//    NGL_ASSERT(0);
  auto it = mWidgetDrawPainters.find(mpRoot);
  if (it == mWidgetDrawPainters.end())
  {
    App->GetMainQueue().Post(nuiMakeTask(this, &nuiRenderThread::RenderingDone, false));
    if (mUseSignPosts)
      kdebug_signpost_end(0, (uintptr_t)this, 1, 0, 0);
    return;
  }
  
  nuiRef<nuiMetaPainter> pRootPainter = it->second;
  if (pRootPainter == nullptr)
  {
    App->GetMainQueue().Post(nuiMakeTask(this, &nuiRenderThread::RenderingDone, false));
    if (mUseSignPosts)
      kdebug_signpost_end(0, (uintptr_t)this, 2, 0, 0);
    return;
  }

  // Try locking
  if (!TryLockRendering())
  {
    NGL_OUT("Unable to lock rendering thread???\n");
    if (mUseSignPosts)
      kdebug_signpost_end(0, (uintptr_t)this, 3, 0, 0);
    return;
  }

  mpContext->GetLock().Lock();
  if (mUseSignPosts)
    kdebug_signpost_start(1, (uintptr_t)this, 0, 0, 0);

  mpPainter->ResetStats();
  mpContext->BeginSession();
  mpPainter->BeginSession();

  // Cache the render arrays
  {
    for (auto array : mNewRenderArrays)
    {
      mpPainter->CacheRenderArray(array);
    }
    mNewRenderArrays.clear();
  }
  
  mpDrawContext->SetPainter(mpPainter);
  mpDrawContext->StartRendering();
  
  nuiTask* pTask = nullptr;
  while (mContinue && (pTask = mNextFrameQueue.Get(0)))
  {
    pTask->Run();
    pTask->Release();
  }
  if (mUseSignPosts)
  {
    kdebug_signpost_end(1, (uintptr_t)this, 0, 0, 0);
    kdebug_signpost_start(2, (uintptr_t)this, 0, 0, 0);
  }

  mpDrawContext->ResetState();
  mpDrawContext->ResetClipRect();

  // Update Layers:
  mpContext->StartMarkerGroup("Create new layers");
  
  // Create needed surfaces once for this frame:
//  NGL_OUT("[nuiRenderThread] Create surface for current frame %p\n", this);
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

  mpContext->StopMarkerGroup();
  //    NGL_OUT("DONE - Create surface for current frame (%d)\n", count);
  if (mUseSignPosts)
  {
    kdebug_signpost_end(2, (uintptr_t)this, 0, 0, 0);
    kdebug_signpost_start(3, (uintptr_t)this, 0, 0, 0);
  }

//  NGL_OUT("[nuiRenderThread] List %d dirty layers %p\n", mDirtyLayers.size(), this);
  mpContext->StartMarkerGroup("Update dirty layers");
#if NUI_LOG_RENDERING
  NGL_OUT("*** Update %d layers\n", mDirtyLayers.size());
#endif
  std::vector<std::pair<nuiLayer*, nuiMetaPainter*> > layers;
  layers.reserve(mDirtyLayers.size());
  for (auto layer : mDirtyLayers)
  {
    auto it = mLayerContentsPainters.find(layer);
    if (it != mLayerContentsPainters.end())
    {
      nuiMetaPainter* pPainter = it->second;
      NGL_ASSERT(pPainter);
      layers.push_back(std::make_pair(layer, pPainter));
    }
  }

//  NGL_OUT("[nuiRenderThread] Sort %d dirty layers %p\n", mDirtyLayers.size(), this);
  std::sort(layers.begin(), layers.end(), [](auto& a, auto& b)
            {
              return a.second->GetPriority() < b.second->GetPriority();
            }
            );

//  NGL_OUT("[nuiRenderThread] Update %d dirty layers %p\n", mDirtyLayers.size(), this);
  int i = 0;
  for (auto layer : layers)
  {
    nglString s;
    s.CFormat("layer %d %p %p", i++, layer.first, layer.second);
    mpContext->StartMarkerGroup(s.GetChars());
#if NUI_LOG_RENDERING
    NGL_OUT("Area %d %p %p\n", layer.second->GetPriority(), layer.first, layer.second);
#endif
    DrawLayerContents(mpDrawContext, layer.first);
    mpContext->StopMarkerGroup();
  }
  mpContext->StopMarkerGroup();

  if (mUseSignPosts)
    kdebug_signpost_end(3, (uintptr_t)this, 0, 0, 0);
  float layercount = layers.size();
//  NGL_OUT("#########\nTotal %f\nResetState %f\nPartial %f\nGlobal %f\nPainter %f\n", (float)gTotal, (float)gResetState, (float)gPartialRedraw, (float)gGlobalRedraw, (float)gPainterRedraw);
//  NGL_OUT(">>>>> Averages\nTotal %f\nResetState %f\nPartial %f\nGlobal %f\nPainter %f\n#######\n", (float)gTotal / layercount, (float)gResetState / layercount, (float)gPartialRedraw / layercount, (float)gGlobalRedraw / layercount, (float)gPainterRedraw / layercount);

  gTotal = 0;
  gResetState = 0;
  gPartialRedraw = 0;
  gGlobalRedraw = 0;
  gPainterRedraw = 0;

//  NGL_OUT("[nuiRenderThread] Draw Widget Tree %p\n", this);
  if (mUseSignPosts)
    kdebug_signpost_start(4, (uintptr_t)this, 0, 0, 0);

  //mpDrawContext->SetClearColor(nuiColor(255,255,255));
  //  if (mClearBackground)
  {
  //  mpDrawContext->Clear();
  }
  //  else
  //  {
  //    // Force the initial render state anyway!
  //    mpDrawContext->DrawRect(nuiRect(0,0,0,0), eStrokeShape);
  //  }
  
  mWidgetIndentation = 0;
  
//  mpDrawContext->StartRendering();
  mpContext->StartMarkerGroup("Reset State");
//  NGL_OUT(">>> Draw the widget tree %p\n", this);
  mpDrawContext->ResetState();
//  NGL_OUT("Reset clip rect 1\n");
  mpDrawContext->ResetClipRect();
  mpDrawContext->EnableClipping(false);
  mpDrawContext->Set2DProjectionMatrix(mRect.Size());
  mpContext->StopMarkerGroup();

  if (mpOffscreen && (mpOffscreen->GetWidth() != ToNearest(mRect.GetWidth()) || mpOffscreen->GetHeight() != ToNearest(mRect.GetHeight())))
  {
    mpOffscreen->Release();
    mpOffscreen = nullptr;
  }
  
  if (!mpOffscreen)
  {
    mpOffscreen = nuiSurface::CreateSurface("Offscreen", mRect.GetWidth(), mRect.GetHeight());
  }
  
  mpDrawContext->SetSurface(mpOffscreen);

  {
    size_t count = 0;
    auto itt = mPartialRects.find(nullptr);
    if (itt != mPartialRects.end())
      count = itt->second.size();

    mpDrawContext->ResetState();

    
    if (count == 0)
    {
      mpContext->StartMarkerGroup("Draw widget tree [Full Frame]");
      mpDrawContext->ResetClipRect();
      mpDrawContext->EnableClipping(false);

      DrawWidget(mpDrawContext, mpRoot);
      mpContext->StopMarkerGroup();
    }
    else
    {
      mpContext->StartMarkerGroup("Draw widget tree [Partial Frame]");
      for (size_t i = 0; i < count; i++)
      {
        mpContext->StartMarkerGroup(nglString().CFormat("[Partial Frame #%d]", i).GetChars());
        mpDrawContext->ResetState();
        nuiRect cliprect(itt->second[i]); // Partial rect...
        mpDrawContext->ResetClipRect();
        mpDrawContext->Clip(cliprect);
        mpDrawContext->EnableClipping(true);
        DrawWidget(mpDrawContext, mpRoot);
        mpContext->StopMarkerGroup();
      }
      mpContext->StopMarkerGroup();
    }

    // Draw debug redrawn rects
    if (0)
    {
      mpDrawContext->ResetState();
      mpDrawContext->ResetClipRect();
      mpDrawContext->EnableClipping(false);
      mpDrawContext->SetStrokeColor(nuiColor(255, 0, 0, 128));
      mpDrawContext->SetFillColor(nuiColor(255, 0, 0, 32));
      mpDrawContext->SetBlendFunc(nuiBlendTransp);
      mpDrawContext->EnableBlending(true);
      
      for (size_t i = 0; i < count; i++)
      {
        nuiRect cliprect(itt->second[i]); // Partial rect...
        mpDrawContext->DrawRect(cliprect, eStrokeShape);
      }
    }
  }

  mpContext->StartMarkerGroup("Resolve Frame");
  mpDrawContext->ResetState();
  //  NGL_OUT("Reset clip rect 1\n");
  mpDrawContext->ResetClipRect();
  mpDrawContext->EnableClipping(false);
  mpDrawContext->Set2DProjectionMatrix(mRect.Size());
  mpDrawContext->SetSurface(nullptr);
  mpDrawContext->SetTexture(mpOffscreen->GetTexture());
  mpDrawContext->EnableBlending(false);
  mpDrawContext->SetFillColor(nuiColor(255, 255, 255, 255));
  mpDrawContext->DrawImage(mRect, mRect);
  mpDrawContext->SetTexture(nullptr);
  mpContext->StopMarkerGroup();

  if (mUseSignPosts)
  {
    kdebug_signpost_end(4, (uintptr_t)this, 0, 0, 0);
    kdebug_signpost_start(5, (uintptr_t)this, 0, 0, 0);
  }

  mDirtyLayers.clear();
  mPartialRects.clear();

//  NGL_OUT("[nuiRenderThread] Stop Rendering %p\n", this);
  mpContext->StartMarkerGroup("Finish Rendering");
  mpDrawContext->StopRendering();
  mpPainter->EndSession();
  mpContext->EndSession();
  mpContext->StopMarkerGroup();

  mpContext->GetLock().Unlock();
  UnlockRendering();
  App->GetMainQueue().Post(nuiMakeTask(this, &nuiRenderThread::RenderingDone, true));

//  NGL_OUT("[nuiRenderThread] render DONE %p\n", this);
//  DumpStats();
  if (mUseSignPosts)
  {
    kdebug_signpost_end(5, (uintptr_t)this, 0, 0, 0);
    kdebug_signpost_end(0, (uintptr_t)this, 4, 0, 0);
  }
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

void nuiRenderThread::PreCacheRenderArrays(nuiMetaPainter* pPainter)
{
  if (pPainter)
  {
    auto arrays(pPainter->GetRenderArrays());
    for (auto array : arrays)
    {
      mNewRenderArrays.insert(array);
    }
  }
}

void nuiRenderThread::RemovePreCachedRenderArrays(nuiMetaPainter* pPainter)
{
  if (pPainter)
  {
    auto arrays(pPainter->GetRenderArrays());
    for (auto array : arrays)
    {
      mNewRenderArrays.erase(array);
    }
  }
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

    RemovePreCachedRenderArrays(it->second);

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

  PreCacheRenderArrays(pPainter);
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
    
    RemovePreCachedRenderArrays(it->second);
    
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

  PreCacheRenderArrays(pPainter);
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

    RemovePreCachedRenderArrays(it->second);

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

  PreCacheRenderArrays(pPainter);
}

void nuiRenderThread::_SetLayerContentsPainter(nuiLayer* pLayer, nuiRef<nuiMetaPainter> pPainter)
{
//  NGL_OUT("_SetLayerContentsPainter %p (%p) %s\n", pLayer, (void*)pPainter, pPainter?pPainter->GetName().GetChars():"NULL");
  auto it = mLayerContentsPainters.find(pLayer);
  if (it != mLayerContentsPainters.end())
  {
    mpContext->GetLock().Lock();

    RemovePreCachedRenderArrays(it->second);
    nuiRef<nuiMetaPainter> pOld = it->second;
    NGL_ASSERT(pOld);
//    NGL_OUT("                   %p replaces %p\n", (void*)pPainter, (void*)pOld);

    if (pOld->GetRefCount() == 1)
      it->second = nullptr;

    if (pPainter)
    {
      it->second = pPainter;
//      NGL_OUT("----------------------------- New Dirty Layer [1] %p (%d)\n", pLayer, mDirtyLayers.size());
      mDirtyLayers.insert(pLayer);
    }
    else
    {
      mLayerContentsPainters.erase(it);
      mDirtyLayers.erase(pLayer);
//      NGL_OUT("----------------------------- Remove From Dirty Layer [1] %p (%d)\n", pLayer, mDirtyLayers.size());
//      NGL_OUT("_SetLayerContentsPainter %p (%p) %s\n", pLayer, (void*)pPainter, pPainter?pPainter->GetName().GetChars():"NULL");
    }
    mpContext->GetLock().Unlock();
    return;
  }
  else if (pPainter)
  {
    mLayerContentsPainters[pLayer] = pPainter;
    mDirtyLayers.insert(pLayer);
//    NGL_OUT("----------------------------- New Dirty Layer [2] %p (%d)\n", pLayer, mDirtyLayers.size());
  }
  else
  {
//    NGL_OUT("_SetLayerContentsPainter %p (%p) %s\n", pLayer, (void*)pPainter, pPainter?pPainter->GetName().GetChars():"NULL");
//    NGL_OUT("----------------------------- Remove Dirty Layer [2] %p (%d)\n", pLayer, mDirtyLayers.size());
    mDirtyLayers.erase(pLayer);
  }


//  NGL_OUT("LayerContentsPainters (%d)\n", mLayerContentsPainters.size());
  for (auto it : mLayerContentsPainters)
  {
//    NGL_OUT("\t%p -> %p [%d]\n", it.first, it.second, it.second->GetRefCount());
  }

  PreCacheRenderArrays(pPainter);
}

void nuiRenderThread::_InvalidateLayerContents(nuiLayer* pLayer)
{
//    NGL_OUT("+++ InvalidateLayerContents in %s %s\n", pLayer->GetObjectClass().GetChars(), pLayer->GetObjectName().GetChars());
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
//    NGL_OUT("+++ AddInvalidRect in %p %s\n", pLayer, rect.GetValue().GetChars());

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
//  NGL_OUT("--- AddInvalidRect OK %s\n", rect.GetValue().GetChars());
  mPartialRects[pLayer].push_back(rect);
  if (pLayer)
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
#if NUI_LOG_DRAWWIDGETCONTENTS
    nglString str;
    str.CFormat("DrawWidgetContents %s %p", pPainter->GetName().GetChars(), pKey);
    mpContext->StartMarkerGroup(str.GetChars());
    nglString indent;
    indent.Fill("  ", mWidgetIndentation);
    str.Prepend(indent);
    NGL_OUT("%s\n", str.GetChars());
#endif
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
//    pContext->SetStrokeColor(nuiColor("green"));
//    pContext->DrawLine(0, 0, pKey->GetRect().GetWidth(), pKey->GetRect().GetHeight());
#if NUI_LOG_DRAWWIDGETCONTENTS
    mpContext->StopMarkerGroup();
#endif
  }
  mWidgetIndentation--;
}

void nuiRenderThread::DrawLayerContents(nuiDrawContext* pContext, nuiLayer* pKey)
{
  nglTime start;

#if NUI_LOG_DRAWLAYERCONTENTS
  static int indent_count = 0;
  nglString str;
  nglString tmp;
  str.CFormat("DrawLayerContents %p", pKey);
  mpContext->StartMarkerGroup(str.GetChars());
  tmp.Fill("  ", indent_count);
  str.Prepend(tmp);
  str.CFormat("%sDraw layer contents %p", tmp.GetChars(), pKey);
  NGL_OUT("%s\n", str.GetChars());

  indent_count++;
#endif


  nuiRef<nuiMetaPainter> pPainter;
  auto it = mLayerContentsPainters.find(pKey);
  if (it != mLayerContentsPainters.end())
  {
    pPainter = it->second;
#if NUI_LOG_DRAWLAYERCONTENTS
    NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, (void*)pPainter, pPainter->GetRefCount());
#endif
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

    gResetState += nglTime() - start;
    nglTime start2;

    if (count == 0)
    {
      mpContext->StartMarkerGroup("Full Redraw");
#if NUI_LOG_DRAWLAYERCONTENTS
      NGL_OUT("%sFull redraw\n", tmp.GetChars());
#endif

      mpDrawContext->ResetClipRect();
      mpDrawContext->EnableClipping(true);
      mpDrawContext->PushMatrix();
      mpDrawContext->LoadMatrix(nuiMatrix());

      pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidgetContents), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));

      mpDrawContext->PopMatrix();
      mpContext->StopMarkerGroup();

      gGlobalRedraw += nglTime() - start2;
    }
    else
    {
      mpContext->StartMarkerGroup("Partial redraw");
#if NUI_LOG_DRAWLAYERCONTENTS
      NGL_OUT("%sPartial redraw\n", tmp.GetChars());
#endif

      for (size_t i = 0; i < count; i++)
      {
        nuiRect cliprect(itt->second[i]); // Partial rect...

        nglString s;
        s.CFormat("rect %d %s", i, cliprect.GetValue().GetChars());
        mpContext->StartMarkerGroup(s.GetChars());
        mpDrawContext->ResetState();
        mpDrawContext->PushClipping();
        mpDrawContext->Clip(cliprect);
        mpDrawContext->EnableClipping(true);
        mpDrawContext->PushMatrix();
        mpDrawContext->LoadMatrix(nuiMatrix());
        nglTime start3;
        pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidgetContents), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
        mpDrawContext->PopMatrix();

        gPainterRedraw += nglTime() - start3;

        mpDrawContext->PopClipping();

        mpContext->StopMarkerGroup();
      }

      mpContext->StopMarkerGroup();
      gPartialRedraw += nglTime() - start2;
    }

  }

#if NUI_LOG_DRAWLAYERCONTENTS
  indent_count--;
  mpContext->StopMarkerGroup();
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

  gTotal += nglTime() - start;
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
#if NUI_LOG_DRAWWIDGET
    nglString str;
    str.CFormat("DrawWidget %s %p", pPainter->GetName().GetChars(), pKey);
    mpContext->StartMarkerGroup(str.GetChars());
    nglString indent;
    indent.Fill("  ", mWidgetIndentation);
    str.Prepend(indent);
    NGL_OUT("%s\n", str.GetChars());
#endif
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    //    pContext->SetStrokeColor(nuiColor("green"));
    //    pContext->DrawLine(0, 0, pKey->GetRect().GetWidth(), pKey->GetRect().GetHeight());
#if NUI_LOG_DRAWWIDGET
    mpContext->StopMarkerGroup();
#endif
  }
  mWidgetIndentation--;
}

void nuiRenderThread::DrawLayer(nuiDrawContext* pContext, nuiLayer* pKey)
{
#if NUI_LOG_DRAWLAYER
  static int count = 0;
  nglString str;
  str.CFormat("DrawLayer %s %s %p", pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);
  mpContext->StartMarkerGroup(str.GetChars());
  nglString tmp;
  tmp.Fill("  ", count);
  tmp.Add(str);
  NGL_OUT("%s\n", str.GetChars());
#endif
  
  auto it = mLayerDrawPainters.find(pKey);
  if (it != mLayerDrawPainters.end())
  {
    nuiRef<nuiMetaPainter> pPainter = it->second;
//        NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, pPainter, pPainter->GetRefCount());
    NGL_ASSERT(pPainter);
    
#if NUI_LOG_DRAWLAYER
    count++;
#endif
    
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    
#if NUI_LOG_DRAWLAYER
    count--;
#endif
  }
#if NUI_LOG_DRAWLAYER
  mpContext->StopMarkerGroup();
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


