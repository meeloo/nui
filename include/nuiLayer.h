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

  NUI_GETSETDO(float, Width, Change());
  NUI_GETSETDO(float, Height, Change());

  void UpdateContents(nuiDrawContext* pContext);
  void Draw(nuiDrawContext* pContext);
private:
  nuiLayer(const nglString& rName, int width, int height);
  virtual ~nuiLayer();

  static std::map<nglString, nuiLayer*> mLayers;

  float mWidth = 0;
  float mHeight = 0;

  nuiSurface* mpSurface = nullptr;

  void Change() { mChanged = true; }
  bool mChanged = true;
  bool mContentsChanged = true;

  nuiTexture* mpTextureContents = nullptr;
  nuiWidget* mpWidgetContents = nullptr;
  DrawContentsDelegate mDrawContentsDelegate;
};

