//
//  nuiRenderThread.h
//  nui3
//
//  Created by Sebastien Metrot on 14/12/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#pragma once

class nuiMetaPainter;

class nuiRenderThread : public nglThread
{
public:
  typedef nuiFastDelegate2<nuiRenderThread*, bool> RenderingDoneDelegate; ///< Beware! This delegate is called on the Render Thread! Not on the main thread!

  nuiRenderThread(nglContext* pContext, nuiDrawContext* pDrawContext, nuiPainter* pDestinationPainter, const RenderingDoneDelegate& RenderingDone);
  virtual ~nuiRenderThread();

  // Public API:
  void StartRendering(uint32 x, uint32 y);
  void SetRect(const nuiRect& rRect);
//  void AddClipRect(const nuiRect& rRect);
  void Exit();
  void SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void SetRootWidget(nuiWidget* pRoot);
  void RunTaskOnRenderThread(nuiTask* rTask, bool OnNextFrame);

  nuiPainter* GetPainter() const;
  nuiDrawContext* GetDrawContext() const;
private:
  nuiTaskQueue mQueue;
  nuiTaskQueue mNextFrameQueue;

  nuiRect mRect;
  std::vector<nuiRect> mPartialRects;
  nuiWidget* mpRoot = nullptr;
  std::map<nuiWidget*, nuiMetaPainter*> mPainters;
  nglContext* mpContext = nullptr;
  nuiDrawContext* mpDrawContext = nullptr;
  nuiPainter* mpPainter = nullptr;
  RenderingDoneDelegate mRenderingDone;
  bool mContinue = true;

  void _StartRendering(uint32 x, uint32 y);
  void _SetRect(const nuiRect& rRect);
  void _Exit();
  void _SetWidgetPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void _SetRootWidget(nuiWidget* pWidget);
  void _RenderFrame(nuiMetaPainter* pFrame);

  void DrawChild(nuiDrawContext* pContext, nuiWidget* pKey);

  virtual void OnStart();

  void RenderingDone(bool result);

  nglAtomic mRenderingTicks=0;

};
