/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

#define DEFAULTFONTSIZE 11.0f


nuiTheme::nuiTheme()
{
  SetObjectClass(_T("nuiTheme"));
  LoadDefaults();
  mpWindowTitleFont = nuiFont::GetFont(11);
}

void nuiTheme::LoadDefaults()
{

  /// 
  /// Button colors:
  ///

  // Clicked & Hovered
  mButtonBorder[1][1].Set(.6f,.6f,.6f);
  mButtonFill[1][1].Set(.5f,.5f,.53f, 0.6f);

  // Clicked & Normal
  mButtonBorder[1][0].Set(.6f,.6f,.6f);
  mButtonFill[1][0].Set(.3f,.3f,.3f, 0.5f);

  // Normal & Hovered
  mButtonBorder[0][1].Set(.6f,.6f,.6f);
  mButtonFill[0][1].Set(.7f,.7f,.7f, 1.f);

  // Normal & Normal
  mButtonBorder[0][0].Set(.6f,.6f,.6f);
  mButtonFill[0][0].Set(0.7f,0.7f,0.7f, 0.9f);

  /// 
  /// tab colors:
  ///

  // Clicked & Hovered
  for (int i = 0; i<3; i++)
  {
    int j,k;
    j = i&1;
    k = i>>1;
    mTabBorder[j][k] = mButtonBorder[j][k];
    mTabFill[j][k] = mButtonFill[j][k];
  }

  Acquire();
}

nuiTheme::~nuiTheme()
{
  mpWindowTitleFont->Release();
  
  if (mpTheme == this)
    mpTheme = NULL;
}


// Window decorations:
nuiDialog*  nuiTheme::CreateDefaultDialog(nuiWidget* pParent)
{
  return new nuiDialog(pParent);
}

nuiLabel *nuiTheme::CreateTreeNodeLabel(const nglString &text)
{
  return new nuiLabel(text);
}

void nuiTheme::DrawScrollBarBackground(nuiDrawContext* pContext, nuiScrollBar* pScroll)
{
  const nuiRect& rRect = pScroll->GetRangeRect();
  
  nuiDecoration* pDeco = NULL;
  if (pScroll->GetOrientation() == nuiVertical)
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationScrollBarVerticalBkg"));
  else
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationScrollBarHorizontalBkg"));
  NGL_ASSERT(pDeco);
  
  nuiRect rectDest(0.0f, 0.0f, rRect.GetWidth(), rRect.GetHeight());
  pDeco->Draw(pContext, pScroll, rectDest);
  pDeco->Release();
}


void nuiTheme::DrawScrollBarForeground(nuiDrawContext* pContext, nuiScrollBar* pScroll)
{
  nuiDecoration* pDeco = NULL;
  if (pScroll->GetOrientation() == nuiVertical)
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationScrollBarVerticalHdl"));
  else
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationScrollBarHorizontalHdl"));
  NGL_ASSERT(pDeco);
  
  nuiRect rRect = pScroll->GetThumbRect();
  rRect.Grow(-1.f, -1.f);
  
  pDeco->Draw(pContext, pScroll, rRect);
  pDeco->Release();
}

void nuiTheme::DrawSliderBackground(nuiDrawContext* pContext, nuiSlider* pSlider)
{
  nuiRect rect = pSlider->GetRect().Size();
  nuiDecoration* pDeco = NULL;
  
  nuiSize min = pSlider->GetHandlePosMin();
  nuiSize max = pSlider->GetHandlePosMax();
  
  if (pSlider->GetOrientation() == nuiVertical)
  {
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationSliderVerticalBkg"));
    if (!pDeco)
      return;
    nuiSize x = (int)((rect.GetWidth() - pDeco->GetIdealClientRect(pSlider).GetWidth()) / 2);
    nuiSize w = pDeco->GetIdealClientRect(pSlider).GetWidth();
    rect.Set(x, min, w, max-min);
  }
  else
  {
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationSliderHorizontalBkg"));
    if (!pDeco)
      return;
    nuiSize y = (int)((rect.GetHeight() - pDeco->GetIdealClientRect(pSlider).GetHeight()) / 2);
    nuiSize h = pDeco->GetIdealClientRect(pSlider).GetHeight();
    rect.Set(min, y, max - min, h);
  }  
  
  if (pDeco)
  {
    pDeco->Draw(pContext, pSlider, rect);
    pDeco->Release();
  }
}

void nuiTheme::DrawSliderForeground(nuiDrawContext* pContext, nuiSlider* pSlider)
{
  nuiRect rect = pSlider->GetRect().Size();
  float start;
  const nuiRange& Range = pSlider->GetRange();
  
  start  = Range.ConvertToUnit(Range.GetValue());
  nuiDecoration* pDeco = NULL;
  
  if (pSlider->GetOrientation() == nuiVertical)
  {
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationSliderVerticalHdl"));

    if (pDeco)
    {
      const nuiRect& srcRect = pDeco->GetIdealClientRect(pSlider);
      
      nuiSize h = rect.GetHeight() - srcRect.GetHeight();
      rect.mTop = h - (start * h);
      rect.mBottom = rect.mTop + srcRect.GetHeight();
      
      rect.mLeft = (int)(rect.GetWidth() - srcRect.GetWidth())/2;
      rect.mRight = rect.mLeft + srcRect.GetWidth();
    }
  }
  else
  {
    pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationSliderHorizontalHdl"));
    if (pDeco)
    {
      const nuiRect& srcRect = pDeco->GetIdealClientRect(pSlider);
      
      rect.mLeft = (start * (rect.GetWidth() - srcRect.GetWidth()));
      rect.mRight = rect.mLeft + srcRect.GetWidth();
      
      rect.mTop = (int)(rect.GetHeight() - srcRect.GetHeight())/2;
      rect.mBottom = rect.mTop + srcRect.GetHeight();
    }
  }
  
  if (pDeco)
  {
    pDeco->Draw(pContext, pSlider, rect);
    pDeco->Release();
  }
  else
  {
    if (pSlider->GetOrientation() == nuiVertical)
    {
      nuiRect srcRect = pSlider->GetRect().Size();
      srcRect.SetHeight(srcRect.GetWidth() / 2);
      
      nuiSize h = rect.GetHeight() - srcRect.GetHeight();
      rect.mTop = h - (start * h);
      rect.mBottom = rect.mTop + srcRect.GetHeight();
      
      rect.mLeft = (int)(rect.GetWidth() - srcRect.GetWidth())/2;
      rect.mRight = rect.mLeft + srcRect.GetWidth();
    }
    else
    {
      nuiRect srcRect = pSlider->GetRect().Size();
      srcRect.SetWidth(srcRect.GetHeight() / 2);
      
      rect.mLeft = (start * (rect.GetWidth() - srcRect.GetWidth()));
      rect.mRight = rect.mLeft + srcRect.GetWidth();
      
      rect.mTop = (int)(rect.GetHeight() - srcRect.GetHeight())/2;
      rect.mBottom = rect.mTop + srcRect.GetHeight();
    }
    
    pContext->DrawRect(rect, eFillShape);
  }
}

const nuiColor& nuiTheme::GetElementColor(nuiWidgetElement Element) const
{
  return mElementColors[Element];
}

void nuiTheme::SetElementColor(nuiWidgetElement Element,const nuiColor& rColor)
{
  mElementColors[Element] = rColor;
}

void nuiTheme::DrawTreeHandle(nuiDrawContext* pContext, const nuiRect& rRect, bool IsOpened, nuiSize TREE_HANDLE_SIZE, const nuiColor& rColor)
{
  nuiFrame* pDeco = NULL;
  if (IsOpened)
    pDeco = (nuiFrame*)nuiDecoration::Get(_T("nuiDefaultDecorationArrowOpen"));
  else
    pDeco = (nuiFrame*)nuiDecoration::Get(_T("nuiDefaultDecorationArrowClose"));
  if (pDeco)
  {
    const nuiRect& rectSrc = pDeco->GetIdealClientRect(NULL);
    
    nuiSize x,y;
    x = rRect.Left() + (int)((rRect.GetWidth() - rectSrc.GetWidth()) / 2.f);
    y = rRect.Top() + (int)((rRect.GetHeight() - rectSrc.GetHeight()) / 2.f);
    nuiRect rectDest(x, y, rectSrc.GetWidth() , rectSrc.GetHeight());
    
    pDeco->SetColor(rColor);
    pDeco->Draw(pContext, NULL, rectDest);
    
    pDeco->Release();
  }
  else
  {
    pContext->SetFillColor(rColor);
    nuiRenderArray* array = new nuiRenderArray(GL_TRIANGLES);
    float l = MIN(rRect.GetWidth(), rRect.GetHeight());
    nuiRect r(l, l);
    r.SetPosition(nuiCenter, rRect);
    const float x = r.Left();
    const float y = r.Top();
    const float w = r.GetWidth();
    const float h = r.GetHeight();
    
    if (IsOpened)
    {
      array->SetVertex(x,       y);     array->PushVertex();
      array->SetVertex(x + w,   y);     array->PushVertex();
      array->SetVertex(x + w/2, y + h); array->PushVertex();
    }
    else
    {
      array->SetVertex(x,     y);       array->PushVertex();
      array->SetVertex(x + w, y + h/2); array->PushVertex();
      array->SetVertex(x,     y + h);   array->PushVertex();
    }
    
    pContext->DrawArray(array);
  }
     
}


void nuiTheme::DrawSelectionBackground(nuiDrawContext* pContext, const nuiRect& rRect, nuiWidget* pWidget)
{
  nuiDecoration* pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationSelectionBackground"));
  if (!pDeco)
    return;
  
  pDeco->Draw(pContext, pWidget, rRect);
  pDeco->Release();
}


void nuiTheme::DrawSelectionForeground(nuiDrawContext* pContext, const nuiRect& rRect, nuiWidget* pWidget)
{
  nuiDecoration* pDeco = nuiDecoration::Get(_T("nuiDefaultDecorationSelectionForeground"));
  if (!pDeco)
    return;
  
  pDeco->Draw(pContext, pWidget, rRect);  
  pDeco->Release();
}


void nuiTheme::DrawMarkee(nuiDrawContext* pContext, const nuiRect& rRect, nuiWidgetElement Color)
{
  pContext->EnableAntialiasing(false);
  pContext->EnableBlending(true);
  pContext->EnableBlending(true);
  pContext->SetBlendFunc(nuiBlendTransp);
  pContext->EnableTexturing(false);
  nuiColor col = GetElementColor(Color);
  pContext->SetFillColor(col);
  col.Multiply(.3f);
  pContext->SetStrokeColor(col);
  pContext->DrawRect(rRect, eStrokeAndFillShape);
}


/****************************************************************************
 *
 * Generic Theme Support
 *
 ****************************************************************************/
nuiTheme* nuiTheme::GetTheme()
{
  if (!mpTheme)
  {
    mpTheme = new nuiTheme();
  }
  else
  {
    mpTheme->Acquire();
  }
  return mpTheme;
}

void nuiTheme::SetTheme(nuiTheme* pTheme)
{
  if ( mpTheme )
    mpTheme->Release();
  mpTheme = pTheme;
  if (mpTheme)
    mpTheme->Acquire();
}


nuiTheme* nuiTheme::mpTheme = NULL;
