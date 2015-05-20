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

nuiRenderThread::nuiRenderThread(nglContext* pContext, nuiDrawContext* pDrawContext, nuiPainter* pDestinationPainter, const RenderingDoneDelegate& rRenderingDone)
: mpContext(pContext), mpDrawContext(pDrawContext), mpPainter(pDestinationPainter), mRenderingDone(rRenderingDone)
{
}

nuiRenderThread::~nuiRenderThread()
{

}

void nuiRenderThread::OnStart()
{
  nuiTask* pTask = nullptr;

#if (defined _UIKIT_) || (defined _COCOA_)
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
#endif

  while (mContinue && (pTask = mQueue.Get(60000)))
  {
    pTask->Run();
    pTask->Release();
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
    mQueue.Post(pTask);
}

// Public API:
void nuiRenderThread::StartRendering(uint32 x, uint32 y)
{
  ngl_atomic_inc(mRenderingTicks);
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_StartRendering, x, y));
}

void nuiRenderThread::SetRect(const nuiRect& rRect)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetRect, rRect));
}

void nuiRenderThread::Exit()
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_Exit));
}

void nuiRenderThread::SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetWidgetPainter, pWidget, pPainter));
}

void nuiRenderThread::SetLayerDrawPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetLayerDrawPainter, pLayer, pPainter));
}

void nuiRenderThread::SetLayerContentsPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter)
{
//  NGL_OUT("SetLayerContentsPainter %p\n", pLayer);
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetLayerContentsPainter, pLayer, pPainter));
}

void nuiRenderThread::InvalidateLayerContents(nuiLayer* pLayer)
{
//  NGL_OUT("InvalidateLayerContents %p\n", pLayer);
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_InvalidateLayerContents, pLayer));
}


void nuiRenderThread::SetRootWidget(nuiWidget* pRoot)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetRootWidget, pRoot));
}

void nuiRenderThread::SetLayerTree(nuiLayer* pLayerRoot)
{
//  NGL_OUT("SetLayerTree %p\n", pLayerRoot);
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetLayerTree, pLayerRoot));
}



void nuiRenderThread::RenderingDone(bool result)
{
  mRenderingDone(this, result);
}

void nuiRenderThread::_StartRendering(uint32 x, uint32 y)
{
  if (ngl_atomic_dec(mRenderingTicks) > 0)
  {
    NGL_OUT("[nuiRenderThread] skipping frame\n");
    return;
  }
//  NGL_OUT("[nuiRenderThread] render\n");

  if (mpLayerTreeRoot)
  {
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
    
    mpDrawContext->Set2DProjectionMatrix(mRect.Size());
    
    mpDrawContext->ResetState();
    mpDrawContext->ResetClipRect();
    
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
    
//    mpLayerTreeRoot->UpdateContents(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget));
    for (auto layer : mDirtyLayers)
    {
      auto it = mPainters.find(layer);
      if (it != mPainters.end())
      {
        NGL_OUT("Update dirty layer %p\n", layer);
        nuiMetaPainter* pPainter = it->second;
        mpDrawContext->ResetState();
        mpDrawContext->ResetClipRect();
        pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
        //      layer->UpdateContents(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget));
      }
    }
    mDirtyLayers.clear();

//    auto i = mLayerPainters.find(mpLayerTreeRoot);
//    if (i != mLayerPainters.end())
//    {
//      nuiMetaPainter* pPainter = i->second;
//      pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
//
//    }

    if (mpLayerTreeRoot)
    {
      auto it = mLayerPainters.find(mpLayerTreeRoot);
      if (it != mLayerPainters.end())
      {
        nuiMetaPainter* pPainter = it->second;
        pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
      }
    }

    mpDrawContext->StopRendering();
    mpPainter->EndSession();
    mpContext->EndSession();
    
    mpContext->GetLock().Unlock();
    
    RenderingDone(true);
  }
  else
  {
    auto it = mPainters.find(mpRoot);
    if (it == mPainters.end())
    {
      RenderingDone(false);
      return;
    }
    
    nuiMetaPainter* pRootPainter = it->second;
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
    
    
    mpDrawContext->Set2DProjectionMatrix(mRect.Size());
    
    mpDrawContext->ResetState();
    mpDrawContext->ResetClipRect();
    
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
    
    pRootPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    
    mpDrawContext->StopRendering();
    mpPainter->EndSession();
    mpContext->EndSession();
    
    mpContext->GetLock().Unlock();
    
    RenderingDone(true);
  }
}

void nuiRenderThread::_SetRect(const nuiRect& rRect)
{
  mRect = rRect;
}

void nuiRenderThread::_Exit()
{
  mContinue = false;
}

void nuiRenderThread::_SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter)
{
  auto it = mPainters.find(pWidget);
  if (it != mPainters.end())
  {
    nuiMetaPainter* pOld = it->second;
    NGL_ASSERT(pOld);

    mpContext->GetLock().Lock();
    pOld->Release();
    mpContext->GetLock().Unlock();

    if (pPainter)
    {
      it->second = pPainter;
    }
    else
    {
      mPainters.erase(it);
    }
    return;
  }
  else if (pPainter)
  {
    mPainters[pWidget] = pPainter;
  }
}

void nuiRenderThread::_SetLayerDrawPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter)
{
  NGL_OUT("_SetLayerDrawPainter %p (%p)\n", pLayer, pPainter);
  auto it = mLayerPainters.find(pLayer);
  if (it != mLayerPainters.end())
  {
    nuiMetaPainter* pOld = it->second;
    NGL_ASSERT(pOld);
    
    mpContext->GetLock().Lock();
    pOld->Release();
    mpContext->GetLock().Unlock();
    
    if (pPainter)
    {
      it->second = pPainter;
    }
    else
    {
      mLayerPainters.erase(it);
    }
    return;
  }
  else if (pPainter)
  {
    mLayerPainters[pLayer] = pPainter;
  }
}

void nuiRenderThread::_SetLayerContentsPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter)
{
  NGL_OUT("_SetLayerContentsPainter %p\n", pLayer);
  auto it = mPainters.find(pLayer);
  if (it != mPainters.end())
  {
    nuiMetaPainter* pOld = it->second;
    NGL_ASSERT(pOld);

    mpContext->GetLock().Lock();
    if (!pOld->Release())
      it->second = nullptr;

    mpContext->GetLock().Unlock();

    if (pPainter)
    {
      it->second = pPainter;
      mDirtyLayers.insert(pLayer);
    }
    else
    {
      mPainters.erase(it);
      mDirtyLayers.erase(pLayer);
    }
    return;
  }
  else if (pPainter)
  {
    mPainters[pLayer] = pPainter;
    mDirtyLayers.insert(pLayer);
  }
  else
  {
    mDirtyLayers.erase(pLayer);
  }
}

void nuiRenderThread::_InvalidateLayerContents(nuiLayer* pLayer)
{
    NGL_OUT("_InvalidateLayerContents %p\n", pLayer);
    mDirtyLayers.insert(pLayer);
}


void nuiRenderThread::_SetRootWidget(nuiWidget* pRoot)
{
  mpRoot = pRoot;
}

void nuiRenderThread::_SetLayerTree(nuiLayer* pRoot)
{
  NGL_OUT("_SetLayerTree %p\n", pRoot);
  mpLayerTreeRoot = pRoot;
}

void nuiRenderThread::DrawWidget(nuiDrawContext* pContext, nuiWidget* pKey)
{
  auto it = mPainters.find(pKey);
  if (it != mPainters.end())
  {
    nuiMetaPainter* pPainter = it->second;
    NGL_ASSERT(pPainter);
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
  }
}

void nuiRenderThread::DrawLayer(nuiDrawContext* pContext, nuiLayer* pKey)
{
  auto it = mLayerPainters.find(pKey);
  if (it != mLayerPainters.end())
  {
    nuiMetaPainter* pPainter = it->second;
    NGL_ASSERT(pPainter);
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
  }
}


nuiMetaPainter* nuiRenderThread::GetRootPainter() const
{
  auto it = mPainters.find(mpRoot);
  if (it == mPainters.end())
  {
    return nullptr;
  }
  return it->second;
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

