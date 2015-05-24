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
//  NGL_OUT("SetLayerDrawPainter %p (%p %d)\n", pLayer, pPainter, pPainter->GetRefCount());
  pPainter->nuiRefCount::SetTrace(true);
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
//  if (ngl_atomic_dec(mRenderingTicks) > 0)
//  {
//    NGL_OUT("[nuiRenderThread] skipping frame\n");
//    return;
//  }
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

    if (!mDirtyLayers.empty())
    {
      NGL_OUT("Update %d dirty layers!\n", mDirtyLayers.size());
    }
//    mpLayerTreeRoot->UpdateContents(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget));
    glPushGroupMarkerEXT(0, "Update dirty layers");

    for (auto layer : mDirtyLayers)
    {
      nglString str;
      str.CFormat("Draw Layer Contents %s %s %p", layer->GetObjectClass().GetChars(), layer->GetObjectName().GetChars(), layer);
      glPushGroupMarkerEXT(0, str.GetChars());

      auto it = mLayerContentsPainters.find(layer);
      NGL_ASSERT(it != mLayerContentsPainters.end());
      if (it != mLayerContentsPainters.end())
      {
        nuiMetaPainter* pPainter = it->second;
        mpDrawContext->ResetState();
        mpDrawContext->ResetClipRect();
//        NGL_OUT("Update Dirty Layer %p (%p - %d)\n", layer, pPainter, pPainter->GetRefCount());
        pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
        //      layer->UpdateContents(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget));
      }

      glPopGroupMarkerEXT();
    }
    mDirtyLayers.clear();

    glPopGroupMarkerEXT();

//    auto i = mLayerDrawPainters.find(mpLayerTreeRoot);
//    if (i != mLayerDrawPainters.end())
//    {
//      nuiMetaPainter* pPainter = i->second;
//      pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
//
//    }

    if (mpLayerTreeRoot)
    {
      glPushGroupMarkerEXT(0, "Draw Layer tree");
      mpDrawContext->ResetState();
      mpDrawContext->ResetClipRect();
      mpDrawContext->EnableBlending(true);
      mpDrawContext->EnableTexturing(true);
      mpDrawContext->LoadIdentity();

      auto it = mLayerDrawPainters.find(mpLayerTreeRoot);
      if (it != mLayerDrawPainters.end())
      {
        nuiMetaPainter* pPainter = it->second;
        NGL_OUT("Draw Layer Tree %p (%p - %d)\n", mpLayerTreeRoot, pPainter, pPainter->GetRefCount());
        pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
      }
    }
    glPopGroupMarkerEXT();

    mpDrawContext->StopRendering();
    mpPainter->EndSession();
    mpContext->EndSession();
    
    mpContext->GetLock().Unlock();
    
    RenderingDone(true);
  }
  else
  {
    NGL_ASSERT(0);
    auto it = mWidgetPainters.find(mpRoot);
    if (it == mWidgetPainters.end())
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
  NGL_OUT("_SetWidgetPainter %p %s %s (%p)\n", pWidget, pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars(), pPainter);
  auto it = mWidgetPainters.find(pWidget);
  if (it != mWidgetPainters.end())
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
      mWidgetPainters.erase(it);
    }
    return;
  }
  else if (pPainter)
  {
    mWidgetPainters[pWidget] = pPainter;
  }
}

void nuiRenderThread::_SetLayerDrawPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter)
{
//  NGL_OUT("_SetLayerDrawPainter %p (%p)\n", pLayer, pPainter);
  auto it = mLayerDrawPainters.find(pLayer);
  if (it != mLayerDrawPainters.end())
  {
    nuiMetaPainter* pOld = it->second;
    NGL_ASSERT(pOld);
    
    mpContext->GetLock().Lock();
    NGL_OUT("                   %p replaces %p\n", pPainter, pOld);
    pOld->Release();
    mpContext->GetLock().Unlock();
    
    if (pPainter)
    {
      it->second = pPainter;
    }
    else
    {
      mLayerDrawPainters.erase(it);
    }
    return;
  }
  else if (pPainter)
  {
    mLayerDrawPainters[pLayer] = pPainter;
  }
}

void nuiRenderThread::_SetLayerContentsPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter)
{
  NGL_OUT("_SetLayerContentsPainter %p (%p)\n", pLayer, pPainter);
  auto it = mLayerContentsPainters.find(pLayer);
  if (it != mLayerContentsPainters.end())
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
      mLayerContentsPainters.erase(it);
      mDirtyLayers.erase(pLayer);
    }
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
//  NGL_OUT("_SetLayerTree %p\n", pRoot);
  mpLayerTreeRoot = pRoot;
}

void nuiRenderThread::DrawWidget(nuiDrawContext* pContext, nuiWidget* pKey)
{
  nglString str;
  str.CFormat("Draw sub widget %s %s %p", pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);
  glPushGroupMarkerEXT(0, str.GetChars());
  NGL_OUT("%s\n", str.GetChars());
  auto it = mWidgetPainters.find(pKey);
  NGL_ASSERT(it != mWidgetPainters.end());
  if (it != mWidgetPainters.end())
  {
    nuiMetaPainter* pPainter = it->second;
    NGL_ASSERT(pPainter);
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
    pContext->SetStrokeColor(nuiColor("green"));
    pContext->DrawLine(0, 0, pKey->GetRect().GetWidth(), pKey->GetRect().GetHeight());
  }
  glPopGroupMarkerEXT();
}

void nuiRenderThread::DrawLayer(nuiDrawContext* pContext, nuiLayer* pKey)
{
  nglString str;
  str.CFormat("Draw sub layer %s %s %p", pKey->GetObjectClass().GetChars(), pKey->GetObjectName().GetChars(), pKey);
  glPushGroupMarkerEXT(0, str.GetChars());
  NGL_OUT("%s\n", str.GetChars());
  auto it = mLayerDrawPainters.find(pKey);
  if (it != mLayerDrawPainters.end())
  {
    nuiMetaPainter* pPainter = it->second;
//    NGL_OUT("DrawLayer %p (%p - %d)\n", pKey, pPainter, pPainter->GetRefCount());
    NGL_ASSERT(pPainter);
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawWidget), nuiMakeDelegate(this, &nuiRenderThread::DrawLayer));
  }
  glPopGroupMarkerEXT();
}


nuiMetaPainter* nuiRenderThread::GetRootPainter() const
{
  auto it = mWidgetPainters.find(mpRoot);
  if (it == mWidgetPainters.end())
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

