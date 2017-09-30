//
//  nuiLayer.h
//  nui3
//
//  Created by Sébastien Métrot on 07/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#pragma once

class nuiLayer : public nuiNode
{
public:
  typedef nuiFastDelegate2<const nuiLayer*, nuiDrawContext*> DrawContentsDelegate;

  static nuiLayer* GetLayer(const nglString& rName);
  static nuiLayer* CreateLayer(const nglString& rName, int width, int height);
  
  void SetContents(nuiWidget* pWidget);
  void SetContents(nuiTexture* pTexture);
  void SetContents(const DrawContentsDelegate& rDelegate);

//  void SetClearColor(const nuiColor& rClearColor);
//  nuiColor GetClearColor() const;

  void UpdateDraw(nuiRenderThread* pRenderThread, nuiDrawContext* pContext);
  void UpdateContents(nuiRenderThread* pRenderThread, nuiDrawContext* pContext, bool ShouldSkipRendering);
  void UpdateContentsPainter(nuiDrawContext* pContext, nuiMetaPainter* pContentsPainter);
  nuiMetaPainter* GetDrawPainter() const;

  NUI_GETSETDO(float, Width, Change());
  NUI_GETSETDO(float, Height, Change());
  NUI_GETSETDO(nuiColor, ClearColor, Change());
  NUI_GETSETDO(nuiBlendFunc, BlendFunc, Change());
  NUI_GETSETDO(bool, UseSurface, Change());

  virtual void SetObjectName(const nglString &rName);

  static void GetLayers(std::map<nglString, nuiRef<nuiLayer>>& layers);
  
  static nuiSimpleEventSource<0> LayersChanged;

  nuiTexture* GetTexture() const;
private:
  nuiLayer(const nglString& rName, int width, int height);
  virtual ~nuiLayer();

  static std::map<nglString, nuiLayer*> mLayers;
  static nglCriticalSection mLayersCS;
  static void DumpLayers();

  float mWidth = 0;
  float mHeight = 0;
  float mOffsetX = 0;
  float mOffsetY = 0;
  nuiColor mClearColor;
  nuiBlendFunc mBlendFunc = nuiBlendTransp;

  nuiSurface* mpSurface = nullptr;
  bool UpdateSurface(nuiRenderThread* pRenderThread);
  bool mSurfaceChanged = false;
  
  bool mDraw = true;
  bool mUseSurface = true;

  void Change() { mChanged = true; }
  bool mChanged = true;

  nuiTexture* mpTextureContents = nullptr;
  nuiWidget* mpWidgetContents = nullptr;
  DrawContentsDelegate mDrawContentsDelegate;
  nuiMetaPainter* mpMetaPainterContents;

  nuiRef<nuiMetaPainter> mpDrawPainter; ///< This containts the rendering instructions to display the contents of this layer
  
  friend class nuiWidget;
  std::vector<nuiWidget*> mChildWidgets;
  void AddChildWidget(nuiWidget* pWidget);
  void ResetChildWidgets();
  
  bool mClipContents = true;///////////
};

