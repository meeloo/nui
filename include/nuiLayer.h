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
  
  NUI_GETSETDO(int, Width, Change());
  NUI_GETSETDO(int, Height, Change());
  NUI_GETSETDO(nglImagePixelFormat, PixelFormat, Change());
  NUI_GETSETDO(int, Depth, Change());
  NUI_GETSETDO(int, Stencil, Change());
  
private:
  nglString& mName;
  int mWidth = 0;
  int mHeight = 0;
  nglImagePixelFormat PixelFormat;
  int mDepth = 0;
  int mStencil = 0;
  bool mChanged = true;
};