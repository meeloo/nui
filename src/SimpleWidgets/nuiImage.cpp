/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

nuiImage::nuiImage (nuiTexture* pTexture, bool AlreadyAcquired)
  : nuiWidget(), mColor(255, 255, 255, 255)
{
  if (SetObjectClass("nuiImage"))
    InitAttributes();

  mpTexture = pTexture;
  if (!AlreadyAcquired && pTexture)
    pTexture->Acquire();

  mUseAlpha = true;
  //SetFixedAspectRatio(true);

  mBlendFunc = nuiBlendTransp;
  ResetTextureRect();
}

nuiImage::nuiImage (nglIStream* pInput, nglImageCodec* pCodec)
  : nuiWidget(), mColor(255, 255, 255, 255)
{
  if (SetObjectClass("nuiImage"))
    InitAttributes();

  mpTexture = nuiTexture::GetTexture(pInput, pCodec);
  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  mBlendFunc = nuiBlendTransp;
  ResetTextureRect();
}

nuiImage::nuiImage (const nglPath& rPath, nglImageCodec* pCodec)
  : nuiWidget(), mColor(255, 255, 255, 255)
{
  if (SetObjectClass("nuiImage"))
    InitAttributes();

  mpTexture = nuiTexture::GetTexture(rPath, pCodec);
  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  mBlendFunc = nuiBlendTransp;
  SetProperty("Source",rPath.GetPathName());
  ResetTextureRect();
}

nuiImage::nuiImage (nglImageInfo& rInfo, bool Clone)
  : nuiWidget(), mColor(255, 255, 255, 255)
{
  if (SetObjectClass("nuiImage"))
    InitAttributes();

  mpTexture = nuiTexture::GetTexture(rInfo, Clone);
  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  mBlendFunc = nuiBlendTransp;
  ResetTextureRect();
}

nuiImage::nuiImage (const nglImage& rImage)
  : nuiWidget(), mColor(255, 255, 255, 255)
{
  if (SetObjectClass("nuiImage"))
    InitAttributes();

  mpTexture = nuiTexture::GetTexture(rImage);
  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  mBlendFunc = nuiBlendTransp;
  ResetTextureRect();
}

nuiImage::nuiImage (nglImage* pImage, bool OwnImage)
  : nuiWidget(), mColor(255, 255, 255, 255)
{
  if (SetObjectClass("nuiImage"))
    InitAttributes();

  mpTexture = nuiTexture::GetTexture(pImage,OwnImage);
  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  mBlendFunc = nuiBlendTransp;
  ResetTextureRect();
}


void nuiImage::InitAttributes()
{
  AddAttribute(new nuiAttribute<const nglPath&>
               (nglString("Texture"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiImage::GetTexturePath), 
                nuiMakeDelegate(this, &nuiImage::SetTexturePath)));

  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("TextureRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiImage::GetTextureRect), 
                nuiMakeDelegate(this, &nuiImage::SetTextureRect)));

  AddAttribute(new nuiAttribute<const nuiColor&>
               (nglString("Color"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiImage::GetColor), 
                nuiMakeDelegate(this, &nuiImage::SetColor)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("UseAlpha"), nuiUnitYesNo,
                nuiMakeDelegate(this, &nuiImage::GetUseAlpha),
                nuiMakeDelegate(this, &nuiImage::SetUseAlpha)));
}


const nglPath& nuiImage::GetTexturePath()
{
  return mTexturePath;
}

void nuiImage::SetTexturePath(const nglPath& rTexturePath)
{
  mTexturePath = rTexturePath;

  nuiTexture* pTexture = nuiTexture::GetTexture(mTexturePath, NULL);
  SetTexture(pTexture);
  pTexture->Release();
//  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  SetProperty("Source", mTexturePath.GetPathName());
}

void nuiImage::SetTexture(nuiTexture* pTex)
{
  if (mpTexture == pTex)
    return;

  if (pTex)
    pTex->Acquire();
  if (mpTexture)
    mpTexture->Release();

  mpTexture = pTex;
//  mUseAlpha = true;
  //SetFixedAspectRatio(true);
  mBlendFunc = nuiBlendTransp;
  if (pTex)
  {
    switch (pTex->GetPixelFormat())
    {
      case eImagePixelNone:
      case eImagePixelIndex:
      case eImagePixelRGB:
      case eImagePixelLum:
#if (!defined NUI_IOS) && (!defined _ANDROID_)
      case eImagePixelBGR:
#endif
        SetBlendFunc(nuiBlendSource);
        break;
      case eImagePixelRGBA:
      case eImagePixelAlpha:
      case eImagePixelLumA:
        SetBlendFunc(nuiBlendTransp);
        break;
    }
  }
  SetProperty("Source", "Memory Buffer");
  ResetTextureRect();
  Invalidate();
}

nuiImage::~nuiImage()
{
  if (mpTexture)
    mpTexture->Release();
}

void nuiImage::SetTextureRect(const nuiRect& rRect)
{
  mTextureRect = rRect;
  InvalidateLayout();
}

const nuiRect& nuiImage::GetTextureRect() const
{
  return mTextureRect;
}

void nuiImage::ResetTextureRect()
{
  if (mpTexture)
    SetTextureRect(nuiRect(0, 0, mpTexture->GetWidth(), mpTexture->GetHeight()));
  else
    SetTextureRect(nuiRect());
}


void nuiImage::ForceReload()
{
  mpTexture->ForceReload();
  Invalidate();
}

bool nuiImage::Draw(nuiDrawContext* pContext)
{
//  int x=0,y=0;
  if (GetDebug())
  {
    NGL_OUT("nuiImage::Draw\n");
  }
  if (!mpTexture)
  {
    return false;
  }

  pContext->PushState();
  pContext->ResetState();
  pContext->SetTexture(mpTexture);
//  mpTexture->Apply(pContext);
  pContext->EnableTexturing(true);
  //nglImage* pImage = mpTexture->GetImage();

  float alpha = 1.0f;

  if (mUseAlpha)
  {
    pContext->EnableBlending(true);
    pContext->SetBlendFunc(mBlendFunc);
    alpha = GetMixedAlpha();
  }

  nuiRect rect = mRect.Size();

  nuiColor c(mColor);
  c.Multiply(alpha);
  
  pContext->SetFillColor(c);
  bool debug = pContext->GetDebug();
  pContext->SetDebug(GetDebug());
//  pContext->SetDebug(true);
  pContext->DrawImage(rect, mTextureRect);
  pContext->SetDebug(debug);

  pContext->EnableBlending(false);
  pContext->EnableTexturing(false);

  pContext->PopState();
  
  return true;
}

void nuiImage::SetBlendFunc(nuiBlendFunc Func)
{
  mBlendFunc = Func;
  Invalidate();
}


nuiBlendFunc nuiImage::GetBlendFunc()
{
  return mBlendFunc;
}


nuiRect nuiImage::CalcIdealSize()
{
  mIdealRect = mTextureRect.Size();
  if (mHasUserWidth != mHasUserHeight && GetFixedAspectRatio())
  {
    // Give good ratio to keep things in proportions
    float w = mUserRect.GetWidth();
    float h = mUserRect.GetHeight();
    float tw = mpTexture->GetWidth();
    float th = mpTexture->GetHeight();
    float r = 1.0f;

    if (mHasUserWidth)
    {
      if (w < tw)
      {
        r = w / tw;
        h = th * r;
      }
    }
    else
    {
      if (h < th)
      {
        r = h / th;
        w = tw * r;
      }
    }
    mIdealRect.Set(0.0f, 0.0f, w, h);
  }
    
  return mIdealRect;
}


void nuiImage::SetUseAlpha(bool Alpha)
{
  mUseAlpha = Alpha;
  SetLayerBlendFunc(mUseAlpha ? nuiBlendTransp : nuiBlendSource);
  Invalidate();
}

bool nuiImage::GetUseAlpha()
{
  return mUseAlpha;
}

nglImage* nuiImage::GetImage()
{
  if (!mpTexture)
    return NULL;
  return mpTexture->GetImage();
}

nuiTexture* nuiImage::GetTexture()
{
  return mpTexture;
}

void nuiImage::SetColor(const nuiColor& rColor)
{
  if (mColor == rColor)
    return;
  mColor = rColor;
  Invalidate();
}

const nuiColor& nuiImage::GetColor() const
{
  return mColor;
}

