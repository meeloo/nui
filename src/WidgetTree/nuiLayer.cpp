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
  mpSurface = nullptr;
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

bool nuiLayer::UpdateSurface()
{
  int w, h;
  if (mpTextureContents)
  {
    w = mpTextureContents->GetWidth();
    h = mpTextureContents->GetHeight();
  }
  else if (mpWidgetContents)
  {
    w = mpWidgetContents->GetRect().GetWidth();
    h = mpWidgetContents->GetRect().GetHeight();
  }

  if (w < 1)
    w = 1;
  if (h < 1)
    h = 1;
 
  SetWidth(w);
  SetHeight(h);
  
  bool recreate = false;
  if (mpSurface == nullptr)
    recreate = true;
  else if (mpSurface->GetWidth() != mWidth || mpSurface->GetHeight() != mHeight)
    recreate = true;

  if (recreate)
  {
    if (mpSurface)
      mpSurface->Release();

    nglString name;
    if (mpWidgetContents)
      name.CFormat("Wid %s %s %p", mpWidgetContents->GetObjectClass().GetChars(), mpWidgetContents->GetObjectName().GetChars(), mpWidgetContents);
    else
      name = GetObjectName();

    mpSurface = nuiSurface::CreateSurface(name, ToNearest(mWidth), ToNearest(mHeight));
//    NGL_OUT("Recreate Surface for layer %p (size requested: %f x %f)\n", this, GetWidth(), GetHeight());
  }

  return recreate;
}

void nuiLayer::UpdateContents(nuiRenderThread* pRenderThread, nuiDrawContext* pContext)
{
//  NGL_OUT("nuiLayer::UpdateContents %p\n", this);
  if (UpdateSurface())
  { // Surface has changed

    if (mpContentsPainter)
    {
      mpContentsPainter->Release();
      mpContentsPainter = nullptr;
    }
    mpContentsPainter = new nuiMetaPainter();


    NGL_ASSERT(mpSurface);
    mpContentsPainter->SetSurface(mpSurface);

    pContext->ResetState();
    pContext->Set2DProjectionMatrix(nuiRect(mWidth, mHeight));
    pContext->ResetClipRect();

    if (mpWidgetContents)
    {
  //    mpWidgetContents->GetColor(eActiveWindowBg);
      pContext->SetClearColor(nuiColor(0, 0, 0, 0));
      pContext->Clear();
      mpContentsPainter->DrawWidget(pContext, mpWidgetContents);
    }
    else if (mDrawContentsDelegate)
    {
      pContext->SetClearColor(mClearColor);
      pContext->Clear();
      
      mDrawContentsDelegate(this, pContext);
    }
    // Don't do anything special with Texture contents, it's directly used as a texture in the Draw method

    mpContentsPainter->SetSurface(nullptr);

    pRenderThread->SetLayerContentsPainter(this, mpContentsPainter);
  }
  else
  {
    // Only the contents has changed
    pRenderThread->InvalidateLayerContents(this);
  }
}

void nuiLayer::UpdateDraw(nuiRenderThread* pRenderThread, nuiDrawContext* pContext)
{
//  NGL_OUT("nuiLayer::Draw %p\n", this);
  CheckValid();

  UpdateSurface();
  
  if (mpDrawPainter)
  {
    mpDrawPainter->Release();
    mpDrawPainter = nullptr;
  }
  mpDrawPainter = new nuiMetaPainter();


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

  NGL_ASSERT(pTex);
  NGL_ASSERT(pTex->GetWidth() > 0);
  NGL_ASSERT(pTex->GetHeight() > 0);
//  pTex = nuiTexture::GetTexture("ButtonUp");
  
  nuiPainter* pPainter = pContext->GetPainter();

  pContext->SetPainter(mpDrawPainter);
//  pContext->ResetState();
//  pContext->ResetClipRect();

  pContext->SetTexture(pTex);
  pContext->SetFillColor(nuiColor(0.5f, 0.5f, 1.0f, 1.0f));

  nuiRect src = nuiRect(0, 0, pTex->GetWidth(), pTex->GetHeight());
  nuiRect dst = src;
//  dst.Move(-GetPivot()[0], -GetPivot()[1]);
  nuiMatrix Matrix;
  GetMatrix(Matrix);
  pContext->MultMatrix(Matrix);

  pContext->DrawImage(dst, src);

  pContext->SetStrokeColor("red");
  pContext->EnableTexturing(false);
  pContext->DrawRect(dst, eStrokeShape);
  pContext->EnableTexturing(true);

  for (auto child : mpChildren)
  {
    nuiLayer* pLayer = (nuiLayer*)child;
    mpDrawPainter->DrawLayer(pContext, pLayer);
  }

  pContext->SetPainter(pPainter);

  pRenderThread->SetLayerDrawPainter(this, mpDrawPainter);
}


