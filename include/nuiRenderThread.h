//
//  nuiRenderThread.h
//  nui3
//
//  Created by Sebastien Metrot on 14/12/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#pragma once

class nuiMetaPainter;
class nuiLayer;

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
  void SetWidgetContentsPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void SetWidgetDrawPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void SetLayerDrawPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter);
  void SetLayerContentsPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter);
  void InvalidateLayerContents(nuiLayer* pLayer);
  void SetRootWidget(nuiWidget* pRoot);
  void RunTaskOnRenderThread(nuiTask* rTask, bool OnNextFrame);
  void SetLayerTree(nuiLayer* pLayerRoot);

  nuiPainter* GetPainter() const;
  nuiDrawContext* GetDrawContext() const;
  nglContext* GetContext() const;

private:
  void Post(nuiTask* pTask);
    
  nuiTaskQueue mQueue;
  nuiTaskQueue mNextFrameQueue;

  nuiRect mRect;
  std::vector<nuiRect> mPartialRects;
  nuiWidget* mpRoot = nullptr;
  std::map<nuiWidget*, nuiMetaPainter*> mWidgetContentsPainters;
  std::map<nuiWidget*, nuiMetaPainter*> mWidgetDrawPainters;
  std::map<nuiLayer*, nuiMetaPainter*> mLayerContentsPainters;
  std::map<nuiLayer*, nuiMetaPainter*> mLayerDrawPainters;
  std::set<nuiLayer*> mDirtyLayers;
  nglContext* mpContext = nullptr;
  nuiDrawContext* mpDrawContext = nullptr;
  nuiPainter* mpPainter = nullptr;
  RenderingDoneDelegate mRenderingDone;
  bool mContinue = true;
  nuiLayer* mpLayerTreeRoot = nullptr;

  void _StartRendering(uint32 x, uint32 y);
  void _SetRect(const nuiRect& rRect);
  void _Exit();
  void _SetWidgetContentsPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void _SetWidgetDrawPainter(nuiWidget* pWidget, nuiMetaPainter* pPainter);
  void _SetLayerDrawPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter);
  void _SetLayerContentsPainter(nuiLayer* pLayer, nuiMetaPainter* pPainter);
  void _InvalidateLayerContents(nuiLayer* pLayer);
  void _SetRootWidget(nuiWidget* pWidget);
  void _SetLayerTree(nuiLayer* pRoot);
  void _RenderFrame(nuiMetaPainter* pFrame);


  virtual void OnStart();

  void RenderingDone(bool result);

public:
  void DrawWidget(nuiDrawContext* pContext, nuiWidget* pKey);
  void DrawLayer(nuiDrawContext* pContext, nuiLayer* pKey);


  nglAtomic mRenderingTicks=0;
  int mWidgetIndentation = 0;
};
