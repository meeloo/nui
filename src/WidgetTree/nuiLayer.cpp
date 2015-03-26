//
//  nuiLayer.cpp
//  nui3
//
//  Created by Sébastien Métrot on 07/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#include "nui.h"

std::map<nglString, nuiLayer*> nuiLayer::mLayers;

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
  
  if (!pLayer)
  {
    return new nuiLayer(rName, width, height);
  }
  
  if (pLayer->GetWidth() == width && pLayer->GetHeight() == height)
  {
    return pLayer;
  }

  pLayer->SetWidth(width);
  pLayer->SetHeight(height);
  
  return pLayer;
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

  mWidth = width;
  mHeight = height;
}

nuiLayer::~nuiLayer()
{
  if (mpSurface)
    mpSurface->Release();
}


void nuiLayer::SetContents(nuiWidget* pWidget)
{
  // Layers don't own their widgets, it's just a link and the widget should tell the layer when it's dead
  //  if (pWidget)
  //    pWidget->Acquire();
  //
  //  if (mpWidgetContents)
  //    mpWidgetContents->Release();

  if (mpTextureContents)
    mpTextureContents->Release();
  
  mpWidgetContents = pWidget;
  mpTextureContents = nullptr;
  mDrawContentsDelegate.clear();

  mContentsChanged = true;
}

void nuiLayer::SetContents(nuiTexture* pTexture)
{
  if (pTexture)
    pTexture->Acquire();

  // Layers don't own their widgets, it's just a link and the widget should tell the layer when it's dead
  //  if (mpWidgetContents)
  //    mpWidgetContents->Release();

  if (mpTextureContents)
    mpTextureContents->Release();
  
  mpWidgetContents = nullptr;
  mpTextureContents = pTexture;
  mDrawContentsDelegate.clear();

  mContentsChanged = true;
}

void nuiLayer::SetContents(const DrawContentsDelegate& rDelegate)
{
  // Layers don't own their widgets, it's just a link and the widget should tell the layer when it's dead
  //  if (mpWidgetContents)
  //    mpWidgetContents->Release();

  if (mpTextureContents)
    mpTextureContents->Release();

  mpWidgetContents = nullptr;
  mpTextureContents = nullptr;

  mDrawContentsDelegate = rDelegate;

  mContentsChanged = true;
}

void nuiLayer::UpdateContents(nuiDrawContext* pContext, const nuiFastDelegate2<nuiDrawContext*, nuiWidget*>& rDrawWidgetDelegate)
{
  if (!mContentsChanged || mpTextureContents)
    return;

  pContext->SetSurface(mpSurface);

  pContext->ResetState();
  pContext->Set2DProjectionMatrix(nuiRect(mWidth, mHeight));
  pContext->ResetClipRect();

  if (mpWidgetContents)
  {
    rDrawWidgetDelegate(pContext, mpWidgetContents);
  }
  else if (mDrawContentsDelegate)
  {
    mDrawContentsDelegate(this, pContext);
  }
  // Don't do anything special with Texture contents, it's directly used as a texture in the Draw method

  pContext->SetSurface(nullptr);

  mContentsChanged = false;
}

void nuiLayer::Draw(nuiDrawContext* pContext)
{
  CheckValid();

  nuiTexture* pTex = nullptr;
  if (mpTextureContents)
  {
    pTex = mpTextureContents;
  }
  else
  {
    pTex = mpSurface->GetTexture();
  }

  pContext->SetTexture(pTex);
  pContext->SetFillColor(nuiColor(1.f, 1.f, 1.f, 1.f));

  nuiRect src = nuiRect(0, 0, pTex->GetWidth(), pTex->GetHeight());
  nuiRect dst = src;
  dst.Move(-GetPivot()[0], -GetPivot()[1]);
  pContext->DrawImage(dst, src);
}


