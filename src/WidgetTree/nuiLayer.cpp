//
//  nuiLayer.cpp
//  nui3
//
//  Created by Sébastien Métrot on 07/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#include "nui.h"

nuiLayer* nuiLayer::GetLayer(const nglString& rName)
{
  auto it = mLayers.find(rName);
  if (it != mLayers.end())
  {
    nuiLayer* pLayer = it->second;
    pLayer->Acquire();
    return pLayer;
  }

  return nullptr;
}

nuiLayer* nuiLayer::CreateLayer(const nglString& rName, int width, int height)
{
  nuiLayer* pLayer = GetLayer(rName);
  if (pLayer)
  {
    if (pLayer->GetWidth() == width && pLayer->GetHeight() == height)
    {
      return pLayer;
    }

    pLayer->SetWidth(width);
    pLayer->SetHeight(height);
  }
}


nuiLayer::nuiLayer(const nglString& rName, int width, int height)
: nuiNode(rName)
{
  NGL_ASSERT(mLayers.find(rName) == mLayers.end());

  if (SetObjectClass("nuiLayer"))
  {
    AddAttribute(new nuiAttribute<float>
                 ("Width", nuiUnitCustom,
                  nuiMakeDelegate(this, &nuiLayer::GetWidth),
                  nuiMakeDelegate(this, &nuiLayer::SetWidth)));
    AddAttribute(new nuiAttribute<float>
                 ("Height", nuiUnitCustom,
                  nuiMakeDelegate(this, &nuiLayer::GetHeight),
                  nuiMakeDelegate(this, &nuiLayer::SetHeight)));
  }

  mLayers[rName] = this;

  mContents.Texture = nullptr;
  mWidth = width;
  mHeight = height;
}