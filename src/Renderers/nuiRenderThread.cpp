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

void nuiRenderThread::RunTaskOnRenderThread(nuiTask* pTask)
{
  mQueue.Post(pTask);
}

// Public API:
void nuiRenderThread::StartRendering(uint32 x, uint32 y)
{
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

void nuiRenderThread::SetRootWidget(nuiWidget* pRoot)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_SetRootWidget, pRoot));
}

void nuiRenderThread::RenderingDone(bool result)
{
  mRenderingDone(this, result);
}

void nuiRenderThread::_StartRendering(uint32 x, uint32 y)
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

  pRootPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawChild));

  mpDrawContext->StopRendering();
  mpPainter->EndSession();
  mpContext->EndSession();

  mpContext->GetLock().Unlock();

  RenderingDone(true);
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

void nuiRenderThread::_SetRootWidget(nuiWidget* pRoot)
{
  mpRoot = pRoot;
}

void nuiRenderThread::DrawChild(nuiDrawContext* pContext, nuiWidget* pKey)
{
  auto it = mPainters.find(pKey);
  if (it != mPainters.end())
  {
    nuiMetaPainter* pPainter = it->second;
    NGL_ASSERT(pPainter);
    pPainter->ReDraw(mpDrawContext, nuiMakeDelegate(this, &nuiRenderThread::DrawChild));
  }
}


nuiPainter* nuiRenderThread::GetPainter() const
{
  return mpPainter;
}

nuiDrawContext* nuiRenderThread::GetDrawContext() const
{
  return mpDrawContext;
}

