//
//  nuiLayer.h
//  nui3
//
//  Created by Sébastien Métrot on 07/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#pragma once

class nuiLayer : public nuiObject
{
public:
  static nuiLayer* GetLayer(const nglString& rName);
  static nuiLayer* CreateLayer(const nglString& rName, int width, int height, nglImagePixelFormat PixelFormat = eImagePixelRGBA, int depth = 0, int stencil = 0);
  
  NUI_GETSETDO(nuiRect, Frame, Change());
  NUI_GETSETDO(nglImagePixelFormat, PixelFormat, Change());
  NUI_GETSETDO(int, Depth, Change());
  NUI_GETSETDO(int, Stencil, Change());

  void AddChild(nuiLayer* pLayer);
  void DelChild(nuiLayer* pLayer);
  const std::vector<nuiLayer*>& GetChildren() const;

private:
  std::vector<nuiLayer*> mLayers;
  void Change() { mChanged = true; }
  bool mChanged = true;
  nglString& mName;
  nuiRect mFrame;
  nuiVector2 mAnchor;
  nglImagePixelFormat PixelFormat;
  int mDepth = 0;
  int mStencil = 0;
};

