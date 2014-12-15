//
//  nuiRenderThread.h
//  nui3
//
//  Created by Sebastien Metrot on 14/12/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#pragma once


class nuiRenderThread : public nglThread
{
public:
  typedef nuiFastDelegate2<nuiRenderThread*, bool> RenderingDoneDelegate; ///< Beware! This delegate is called on the Render Thread! Not on the main thread!

  nuiRenderThread(nuiDrawContext* pContext, nuiPainter* pDestinationPainter, const RenderingDoneDelegate& RenderingDone);
  virtual ~nuiRenderThread();

  // Public API:
  void StartRendering(uint32 x, uint32 y);
  void Exit();
  void SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void SetRootWidget(nuiWidget* pRoot);




private:
  nuiTaskQueue mQueue;
  nuiWidget* mpRoot = nullptr;
  std::map<nuiWidget*, nuiMetaPainter*> mPainters;
  nuiPainter* mpPainter = nullptr;
  nuiDrawContext* mpContext = nullptr;
  RenderingDoneDelegate mRenderingDone;
  bool mContinue = true;

  void _StartRendering(uint32 x, uint32 y);
  void _Exit();
  void _SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void _SetRootWidget(nuiWidget* pWidget);

  virtual void OnStart();

  void RenderingDone(bool result);
};
