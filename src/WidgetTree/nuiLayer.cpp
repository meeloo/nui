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
: nuiNode(rName), mClearColor(0, 0, 0, 0)
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
    AddAttribute(new nuiAttribute<nuiColor>
                 ("ClearColor", nuiUnitCustom,
                  nuiMakeDelegate(this, &nuiLayer::GetClearColor),
                  nuiMakeDelegate(this, &nuiLayer::SetClearColor)));
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
}

void nuiLayer::UpdateSizeFromContents()
{
  int w, h;
  if (mpTextureContents)
  {
    SetWidth(mpTextureContents->GetWidth());
    SetHeight(mpTextureContents->GetHeight());
  }
  else if (mpWidgetContents)
  {
    SetWidth(mpWidgetContents->GetRect().GetWidth());
    SetHeight(mpWidgetContents->GetRect().GetHeight());
  }

//  NGL_OUT("Layer %p new size requested: %f x %f\n", this, GetWidth(), GetHeight());
}


void nuiLayer::UpdateContents(nuiDrawContext* pContext, const nuiFastDelegate2<nuiDrawContext*, nuiWidget*>& rDrawWidgetDelegate)
{
//  NGL_OUT("nuiLayer::UpdateContents\n");
  bool recreate = false;
  if (mpSurface == nullptr)
    recreate = true;
  else if (mpSurface->GetWidth() != mWidth || mpSurface->GetHeight() != mHeight)
    recreate = true;

  if (recreate)
  {
    if (mpSurface)
      mpSurface->Release();
    mpSurface = nuiSurface::CreateSurface(GetObjectName(), ToNearest(mWidth), ToNearest(mHeight));
//    NGL_OUT("Recreate Surface for layer %p (size requested: %f x %f)\n", this, GetWidth(), GetHeight());
  }
  
  pContext->SetSurface(mpSurface);

  pContext->ResetState();
  pContext->Set2DProjectionMatrix(nuiRect(mWidth, mHeight));
  pContext->ResetClipRect();

  if (mpWidgetContents)
  {
    mpWidgetContents->GetColor(eActiveWindowBg);
    pContext->Clear();
    rDrawWidgetDelegate(pContext, mpWidgetContents);
  }
  else if (mDrawContentsDelegate)
  {
    pContext->SetClearColor(mClearColor);
    pContext->Clear();
    
    mDrawContentsDelegate(this, pContext);
  }
  // Don't do anything special with Texture contents, it's directly used as a texture in the Draw method

  pContext->SetSurface(nullptr);

}

void nuiLayer::Draw(nuiDrawContext* pContext)
{
//  NGL_OUT("nuiLayer::Draw\n");
  CheckValid();

  nuiTexture* pTex = nullptr;
  if (mpTextureContents)
  {
    pTex = mpTextureContents;
  }
  else
  {
    NGL_ASSERT(mpSurface != nullptr);
    pTex = mpSurface->GetTexture();
  }

  pContext->SetTexture(pTex);
  pContext->SetFillColor(nuiColor(1.f, 1.f, 1.f, 1.f));

  nuiRect src = nuiRect(0, 0, pTex->GetWidth(), pTex->GetHeight());
  nuiRect dst = src;
  dst.Move(-GetPivot()[0], -GetPivot()[1]);
  pContext->DrawImage(dst, src);
}


