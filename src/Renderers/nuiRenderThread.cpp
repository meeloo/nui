//
//  nuiRenderThread.cpp
//  nui3
//
//  Created by Sebastien Metrot on 14/12/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#include "nui.h"
#include "nuiMetaPainter.h"

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
  while (mContinue && (pTask = mQueue.Get(60000)))
  {
    pTask->Run();
  }
}

// Public API:
void nuiRenderThread::StartRendering(uint32 x, uint32 y)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_StartRendering, x, y));
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

void nuiRenderThread::RenderFrame(nuiMetaPainter* pFrame)
{
  mQueue.Post(nuiMakeTask(this, &nuiRenderThread::_RenderFrame, pFrame));
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

  pRootPainter->ReDraw(mpDrawContext);

  RenderingDone(true);
}

void nuiRenderThread::_Exit()
{
  mContinue = false;
}

void nuiRenderThread::_SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter)
{
  auto it = mPainters.find(pWidget);
  if (pPainter)
    pPainter->Acquire();

  if (it != mPainters.end())
  {
    nuiMetaPainter* pOld = it->second;
    pOld->Release();
    if (pPainter)
      it->second = pPainter;
    else
      mPainters.erase(it);
    return;
  }

  if (pPainter)
    mPainters[pWidget] = pPainter;
}

void nuiRenderThread::_SetRootWidget(nuiWidget* pRoot)
{
  mpRoot = pRoot;
}

void nuiRenderThread::_RenderFrame(nuiMetaPainter* pFrame)
{
  mpPainter->ResetStats();
  mpContext->BeginSession();
  mpPainter->BeginSession();
  
  pFrame->ReDraw(mpDrawContext);
  pFrame->Release();
  
  mpPainter->EndSession();
  mpContext->EndSession();
}


