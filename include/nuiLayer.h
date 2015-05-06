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

  void UpdateSizeFromContents();

  void UpdateContents(nuiDrawContext* pContext, const nuiFastDelegate2<nuiDrawContext*, nuiWidget*>& rDrawWidgetDelegate);
  void Draw(nuiDrawContext* pContext);

  NUI_GETSETDO(float, Width, Change());
  NUI_GETSETDO(float, Height, Change());


private:
  nuiLayer(const nglString& rName, int width, int height);
  virtual ~nuiLayer();

  static std::map<nglString, nuiLayer*> mLayers;

  float mWidth = 0;
  float mHeight = 0;

  nuiSurface* mpSurface = nullptr;

  void Change() { mChanged = true; }
  bool mChanged = true;

  nuiTexture* mpTextureContents = nullptr;
  nuiWidget* mpWidgetContents = nullptr;
  DrawContentsDelegate mDrawContentsDelegate;


  nuiMetaPainter* mpContentsPainter = nullptr; ///< This containts the rendering instructions to update the contents of this layer
  nuiMetaPainter* mpDrawPainter = nullptr; ///< This containts the rendering instructions to display the contents of this layer
};

