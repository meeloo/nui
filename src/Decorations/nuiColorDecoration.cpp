/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

nuiColorDecoration::nuiColorDecoration(const nglString& rName)
: nuiDecoration(rName)
{
  if (SetObjectClass("nuiColorDecoration"))
    InitAttributes();

  mStrokeSize = 0;
  mFillColor = nuiColor(0,0,0);
  mStrokeColor = nuiColor(0,0,0);
  mShapeMode = eFillShape;
  mBlendFunc = nuiBlendTransp;
  mClientRect = nuiRect(0,0,0,0);
}


nuiColorDecoration::nuiColorDecoration(const nglString& rName, const nuiRect& rClientRect, const nuiColor& rFillColor, float strokeSize, const nuiColor& rStrokeColor, nuiShapeMode ShapeMode, nuiBlendFunc BlendFunc)
: nuiDecoration(rName)
{
  if (SetObjectClass("nuiColorDecoration"))
    InitAttributes();

  mStrokeSize = strokeSize;
  mFillColor = rFillColor;
  mStrokeColor = rStrokeColor;
  mShapeMode = ShapeMode;
  mBlendFunc = BlendFunc;
  mClientRect = rClientRect;
}


nuiColorDecoration::nuiColorDecoration(const nglString& rName, const nuiColor& rFillColor, float strokeSize, const nuiColor& rStrokeColor, nuiShapeMode ShapeMode, nuiBlendFunc BlendFunc, const nuiRect& rClientRect)
: nuiDecoration(rName)
{
  if (SetObjectClass("nuiColorDecoration"))
    InitAttributes();

  mStrokeSize = strokeSize;
  mFillColor = rFillColor;
  mStrokeColor = rStrokeColor;
  mShapeMode = ShapeMode;
  mBlendFunc = BlendFunc;
  mClientRect = rClientRect;
}



nuiColorDecoration::~nuiColorDecoration()
{
}


void nuiColorDecoration::InitAttributes()
{
  AddAttribute(new nuiAttribute<const nuiRect&>
   (nglString("ClientRect"), nuiUnitNone,
    nuiAttribute<const nuiRect&>::GetterDelegate(this, &nuiColorDecoration::GetSourceClientRect),
    nuiAttribute<const nuiRect&>::SetterDelegate(this, &nuiColorDecoration::SetSourceClientRect)));

  AddAttribute(new nuiAttribute<float>
  (nglString("StrokeSize"), nuiUnitPixels,
   nuiMakeDelegate(this, &nuiColorDecoration::GetStrokeSize),
   nuiMakeDelegate(this, &nuiColorDecoration::SetStrokeSize)));
  
  AddAttribute(new nuiAttribute<const nuiColor&>
  (nglString("FillColor"), nuiUnitColor,
   nuiMakeDelegate(this, &nuiColorDecoration::GetFillColor), 
   nuiMakeDelegate(this, &nuiColorDecoration::SetFillColor)));

  AddAttribute(new nuiAttribute<const nuiColor&>
  (nglString("StrokeColor"), nuiUnitColor,
   nuiMakeDelegate(this, &nuiColorDecoration::GetStrokeColor), 
   nuiMakeDelegate(this, &nuiColorDecoration::SetStrokeColor)));

  AddAttribute(new nuiAttribute<nuiShapeMode>
  (nglString("ShapeMode"), nuiUnitNone,
   nuiMakeDelegate(this, &nuiColorDecoration::GetShapeMode), 
   nuiMakeDelegate(this, &nuiColorDecoration::SetShapeMode)));

  
}


// virtual
void nuiColorDecoration::Draw(nuiDrawContext* pContext, nuiWidget* pWidget, const nuiRect& rDestRect)
{
  pContext->PushState();
  pContext->ResetState();
  pContext->EnableAntialiasing(false);
  pContext->EnableBlending(true);
  pContext->SetBlendFunc(mBlendFunc);
  pContext->EnableTexturing(false);
  
  nuiColor fillColor = mFillColor;
  nuiColor strokeColor = mStrokeColor;
  if (pWidget && mUseWidgetAlpha)
  {
    fillColor.Multiply(pWidget->GetMixedAlpha());
    strokeColor.Multiply(pWidget->GetMixedAlpha());
  }
  pContext->SetFillColor(fillColor);
  pContext->SetStrokeColor(strokeColor);
  pContext->SetLineWidth(mStrokeSize);

  pContext->DrawRect(rDestRect, mShapeMode);

  pContext->PopState();
}



nuiRect nuiColorDecoration::GetIdealClientRect(const nuiWidget* pWidget) const
{
  return mClientRect.Size();
}



nuiSize nuiColorDecoration::GetBorder(nuiPosition position, const nuiWidget* pWidget) const
{
  if (!mBorderEnabled)
    return 0;
 
  switch (position)
  {
  case nuiLeft:
    return mClientRect.Left();
    break;
  case nuiRight:
    return mClientRect.GetWidth();
    break;
  case nuiTop:
    return mClientRect.Top();
    break;
  case nuiBottom:
    return mClientRect.GetHeight();
    break;
  case nuiFillHorizontal:
    return mClientRect.Right();
    break;
  case nuiFillVertical:
    return mClientRect.Bottom();
    break;
  case nuiNoPosition: break;
  case nuiTopLeft: break;
  case nuiTopRight: break;
  case nuiBottomLeft: break;
  case nuiBottomRight: break;
  case nuiCenter: break;
  case nuiTile: break;
  case nuiFill: break;
  case nuiFillLeft: break;
  case nuiFillRight: break;
  case nuiFillTop: break;
  case nuiFillBottom: break;
  }
  //we should'nt arrive here
  return NULL;
}

void nuiColorDecoration::GetBorders(const nuiWidget* pWidget, float& rLeft, float& rRight, float& rTop, float& rBottom, float& rHorizontal, float& rVertical) const
{
  if (!mBorderEnabled)
  {
    rLeft = rRight = rTop = rBottom = rHorizontal = rVertical = 0;
    return;
  }
  
  rLeft = mClientRect.Left();
  rRight = mClientRect.GetWidth();
  rTop = mClientRect.Top();
  rBottom = mClientRect.Bottom();
  rHorizontal = rLeft + rRight;
  rVertical = rTop + rBottom;
}



//*************************************************************************
//
// attributes setters/getters
//


const nuiRect& nuiColorDecoration::GetSourceClientRect() const
{
  return mClientRect;
}

void nuiColorDecoration::SetSourceClientRect(const nuiRect& rRect)
{
  mClientRect = rRect;
  Changed();
}

const nuiColor& nuiColorDecoration::GetFillColor() const
{
  return mFillColor;
}

void nuiColorDecoration::SetFillColor(const nuiColor& color)
{
  mFillColor = color;
  Changed();
}


const nuiColor& nuiColorDecoration::GetStrokeColor() const
{
  return mStrokeColor;
}


void nuiColorDecoration::SetStrokeColor(const nuiColor& color)
{
  mStrokeColor = color;
  Changed();
}


void nuiColorDecoration::SetStrokeSize(float size)
{
  mStrokeSize = size;
  Changed();
}


float nuiColorDecoration::GetStrokeSize() const
{
  return mStrokeSize;
}


void nuiColorDecoration::SetShapeMode(nuiShapeMode mode)
{
  mShapeMode = mode;
  Changed();
}


nuiShapeMode nuiColorDecoration::GetShapeMode() const
{
  return mShapeMode;
}

