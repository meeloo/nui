//
//  nuiLayer.cpp
//  nui3
//
//  Created by Sébastien Métrot on 07/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#include "nui.h"

std::map<nglString, nuiLayer*> nuiLayer::mLayers;
nglCriticalSection nuiLayer::mLayersCS;
nuiSimpleEventSource<0> nuiLayer::LayersChanged;


nuiLayer* nuiLayer::GetLayer(const nglString& rName)
{
  nglCriticalSectionGuard gcs(mLayersCS);
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

void nuiLayer::DumpLayers()
{
  nglCriticalSectionGuard gcs(mLayersCS);
  int i = 0;
  for (auto it : mLayers)
  {
    nuiLayer* l = it.second;
    NGL_OUT("Layer %d: %s %p %fx%f\n", i, l->GetObjectName().GetChars(), l, l->GetWidth(), l->GetHeight());
    i++;
  }
}

nuiLayer::nuiLayer(const nglString& rName, int width, int height)
: nuiNode(rName), mClearColor(1.0f, 1.0f, 1.0f, 0.0f)
{
  #if _DEBUG
  {
    nglCriticalSectionGuard gcs(mLayersCS);
    NGL_ASSERT(mLayers.find(rName) == mLayers.end());
  }
  #endif
  
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

  {
    nglCriticalSectionGuard gcs(mLayersCS);
    mLayers[rName] = this;
  }
  mWidth = width;
  mHeight = height;
  App->GetMainQueue().Post(nuiMakeTask(&LayersChanged, &nuiEventSource::SendEvent));
}

nuiLayer::~nuiLayer()
{
  nuiRenderThread::DestroyLayer(this);

  if (mpSurface)
    mpSurface->Release();
  mpSurface = nullptr;
  {
    nglCriticalSectionGuard gcs(mLayersCS);
    nglString name = GetObjectName();
    auto it = mLayers.find(name);
    NGL_ASSERT(it != mLayers.end());
    mLayers.erase(it);
    App->GetMainQueue().Post(nuiMakeTask(&LayersChanged, &nuiEventSource::SendEvent));
  }
}

void nuiLayer::SetObjectName(const nglString &rName)
{
  nglCriticalSectionGuard gcs(mLayersCS);
  nglString name = GetObjectName();
  auto it = mLayers.find(name);
  NGL_ASSERT(it != mLayers.end());
  mLayers[rName] = this;
  mLayers.erase(it);
  App->GetMainQueue().Post(nuiMakeTask(&LayersChanged, &nuiEventSource::SendEvent));

  nuiNode::SetObjectName(rName);
}

void nuiLayer::SetContents(nuiWidget* pWidget)
{
  // Layers don't own their widgets, it's just a link and the widget should tell the layer when it's dead
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

bool nuiLayer::UpdateSurface(nuiRenderThread* pRenderThread)
{
  if (!mDraw)
  {
    pRenderThread->SetLayerContentsPainter(this, nullptr);
  }
  
  bool needRedraw = false;
  int w = ToAbove(mWidth), h = ToAbove(mHeight);
  if (mpTextureContents)
  {
    w = mpTextureContents->GetWidth();
    h = mpTextureContents->GetHeight();
  }
  else if (mpWidgetContents)
  {
//    nuiRect overdraw = mpWidgetContents->GetOverDrawRect(true, true);
    nuiRect overdraw = mpWidgetContents->GetVisibleRect();
    w = ToAbove(overdraw.GetWidth());
    h = ToAbove(overdraw.GetHeight());
    if (mOffsetX != overdraw.Left() || mOffsetY != overdraw.Top())
    {
        needRedraw = true;
    }
    mOffsetX = overdraw.Left();
    mOffsetY = overdraw.Top();
  }

  if (w < 1)
    w = 1;
  if (h < 1)
    h = 1;
 
  SetWidth(w);
  SetHeight(h);
  
  if (mDraw)
  {
    bool recreate = false;
    if (mpSurface == nullptr)
      recreate = true;
    else if (mpSurface->GetWidth() != mWidth || mpSurface->GetHeight() != mHeight)
      recreate = true;

    if (recreate)
    {
      if (mpSurface)
      {
        mpSurface->Release();
        mpSurface = nullptr;
      }

      nglString name;
      if (mpWidgetContents)
        name.CFormat("Wid %s %s %p", mpWidgetContents->GetObjectClass().GetChars(), mpWidgetContents->GetObjectName().GetChars(), mpWidgetContents);
      else
        name = GetObjectName();

      NGL_ASSERT(mpSurface == nullptr);
      mpSurface = nuiSurface::CreateSurface(name, ToNearest(mWidth), ToNearest(mHeight));
  //    mpSurface->SetTrace(true);
//      NGL_OUT("Recreate Surface for layer %p (size requested: %f x %f) (src = %s)\n", this, GetWidth(), GetHeight(), mpWidgetContents->GetObjectClass().GetChars());
      mSurfaceChanged = true;
      return true;
    }
  }
    
  mSurfaceChanged = needRedraw;
  return mSurfaceChanged;
}

void nuiLayer::UpdateContents(nuiRenderThread* pRenderThread, nuiDrawContext* pContext, bool ShouldSkipRendering)
{
//  NGL_OUT("nuiLayer::UpdateContents %p\n", this);
  mDraw = !ShouldSkipRendering;
//  mDraw = true;
  
  UpdateSurface(pRenderThread);
  if (mSurfaceChanged)
  { // Surface has changed
    mSurfaceChanged = false;

    nuiRef<nuiMetaPainter> pContentsPainter = nuiNewRef<nuiMetaPainter>();
    nuiPainter* oldpainter = pContext->GetPainter();
    
    pContext->SetPainter(pContentsPainter);
    
    nglString name;
    name.CFormat("layer contents %f x %f %s", GetWidth(), GetHeight(), GetObjectName().GetChars());
    pContentsPainter->SetName(name);


    pContext->ResetState();
    pContext->Set2DProjectionMatrix(nuiRect(mWidth, mHeight));
    pContext->ResetClipRect();

    if (mpSurface)
      pContentsPainter->SetSurface(mpSurface);

    float offsetX = mOffsetX;
    float offsetY = mOffsetY;
//    float offsetX = mOffsetX + (mpWidgetContents ? -mpWidgetContents->GetRect().Left() : 0);
//    float offsetY = mOffsetY + (mpWidgetContents ? -mpWidgetContents->GetRect().Top() : 0);

    if (offsetX != 0 || offsetY != 0)
    {
      pContext->Translate(-offsetX, -offsetY);
    }
    
    if (mpWidgetContents)
    {
      pContext->SetClearColor(mClearColor);
      pContext->Clear();
      pContentsPainter->DrawWidget(pContext, mpWidgetContents);
//      pContext->SetStrokeColor(nuiColor(0, 0, 255, 128));
//      pContext->EnableTexturing(false);
//      pContext->DrawRect(nuiRect(GetWidth(), GetHeight()), eStrokeShape);
//      pContext->EnableTexturing(true);
    }
    else if (mDrawContentsDelegate)
    {
      pContext->SetClearColor(mClearColor);
      pContext->Clear();
      
      mDrawContentsDelegate(this, pContext);
    }
    // Don't do anything special with Texture contents, it's directly used as a texture in the Draw method

    if (mpSurface)
      pContentsPainter->SetSurface(nullptr);

    if (offsetX != 0 || offsetY != 0)
    {
      pContext->Translate(offsetX, offsetY);
    }
    pContext->SetPainter(oldpainter);

    pRenderThread->SetLayerContentsPainter(this, pContentsPainter);
  }
  else
  {
    // Only the contents has changed
    pRenderThread->InvalidateLayerContents(this);
  }
}

void nuiLayer::UpdateDraw(nuiRenderThread* pRenderThread, nuiDrawContext* pContext)
{
//  NGL_OUT("nuiLayer::UpdateDraw %p\n", this);
  CheckValid();

  UpdateSurface(pRenderThread);

  mpDrawPainter = nuiNewRef<nuiMetaPainter>();
  nglString name;
  name.CFormat("layer draw %f x %f %s", GetWidth(), GetHeight(), GetObjectName().GetChars());
  mpDrawPainter->SetName(name);

//  NGL_OUT("nuiLayer::UpdateDraw %s %s\n", mpWidgetContents?mpWidgetContents->GetObjectClass().GetChars():"???", name.GetChars());

  nuiPainter* pPainter = pContext->GetPainter();

  pContext->SetPainter(mpDrawPainter);

  nuiMatrix Matrix;
  GetMatrix(Matrix);
  pContext->PushMatrix();
  pContext->MultMatrix(Matrix);

  nuiRect dst;
  
  if (mDraw)
  {
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
    
    nuiRect src = nuiRect(0, 0, pTex->GetWidth(), pTex->GetHeight());
    dst = src;
    dst.Move(mOffsetX, mOffsetY);

    pContext->SetTexture(pTex);
    pContext->SetFillColor(nuiColor(1.0f, 1.0f, 1.0f, 1.0f));
    pContext->SetBlendFunc(nuiBlendTransp);
    pContext->EnableBlending(true);
    
    pContext->DrawImage(dst, src);

//    pContext->SetBlendFunc(nuiBlendTransp);
//  pContext->SetStrokeColor(nuiColor(255, 0, 0, 128));
//  pContext->EnableTexturing(false);
//  pContext->DrawRect(dst, eStrokeShape);
//  pContext->EnableTexturing(true);
  }
  else
  {
//    NGL_OUT("Draw is false for %p %s %s\n", mpWidgetContents, mpWidgetContents->GetObjectName().GetChars(), mpWidgetContents->GetObjectClass().GetChars());
  }

  if (mpWidgetContents)
  {
//    NGL_OUT("Update layer contents with widget [%p %s] ( (%f, %f) / (%f x %f))\n",
//            mpWidgetContents, mpWidgetContents->GetObjectClass().GetChars(),
//            mpWidgetContents->GetRect().Left(), mpWidgetContents->GetRect().Top(),
//            mpWidgetContents->GetRect().GetWidth(), mpWidgetContents->GetRect().GetHeight());
    
    if (mpWidgetContents->GetLayerPolicy() == nuiDrawPolicyDrawSelf)
    {
      if (!mChildWidgets.empty())
      {
        mpDrawPainter->PushClipping();
        nuiRect clp(mpWidgetContents->GetRect().Size());
        mpDrawPainter->EnableClipping(mClipContents);
//        clp.Move(mOffsetX, mOffsetY);
        mpDrawPainter->Clip(clp);
        
        for (auto widget : mChildWidgets)
        {
          mpDrawPainter->PushMatrix();
          nuiMatrix m;
          nuiRect r = widget->GetRect();
          m.SetTranslation(r.Left(), r.Top(), 0);
          mpDrawPainter->MultMatrix(m);
          mpDrawPainter->DrawWidget(pContext, widget);
          mpDrawPainter->PopMatrix();
//          NGL_OUT("draw subwidget of [%p %s] -> [%p %s] ( (%f, %f) / (%f x %f))\n",
//                  mpWidgetContents, mpWidgetContents->GetObjectClass().GetChars(),
//                  widget, widget->GetObjectClass().GetChars(),
//                  widget->GetRect().Left(), widget->GetRect().Top(),
//                  widget->GetRect().GetWidth(), widget->GetRect().GetHeight());
        }
        mpDrawPainter->PopClipping();
      }
    }
  }
  
  static int level = 0;
  level++;
  for (auto child : mpChildren)
  {
    nuiLayer* pLayer = (nuiLayer*)child;
//    nglString l;
//    l.Fill("  ", level);
//    NGL_OUT("%sSubLayer %p %s %s\n", l.GetChars(), pLayer, pLayer->GetObjectClass().GetChars(), pLayer->GetObjectName().GetChars());
    mpDrawPainter->DrawLayer(pContext, pLayer);
  }
  level--;

  pContext->PopMatrix();
  pContext->SetPainter(pPainter);

  pRenderThread->SetLayerDrawPainter(this, mpDrawPainter);
  if (mpWidgetContents)
    pRenderThread->SetWidgetDrawPainter(mpWidgetContents, mpDrawPainter);
}

nuiMetaPainter* nuiLayer::GetDrawPainter() const
{
  return mpDrawPainter;
}

void nuiLayer::AddChildWidget(nuiWidget* pWidget)
{
  mChildWidgets.push_back(pWidget);
}

void nuiLayer::ResetChildWidgets()
{
  mChildWidgets.clear();
}

void nuiLayer::GetLayers(std::map<nglString, nuiRef<nuiLayer>>& layers)
{
  nglCriticalSectionGuard gcs(mLayersCS);
  for (const auto& it : mLayers)
  {
    layers[it.first] = it.second;
  }
  
  return layers;
}

nuiTexture* nuiLayer::GetTexture() const
{
   if (!mpSurface)
     return nullptr;
  return mpSurface->GetTexture();
}
