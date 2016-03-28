/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

nuiFlowView::nuiFlowView(float IdealWidth, float HSpace, float VSpace)
{
  if (SetObjectClass("nuiFlowView"))
  {

  }

  mIdealWidth = IdealWidth;
  mCurrentIdealWidth = 0;
  mVSpace = VSpace;
  mHSpace = HSpace;
}

nuiFlowView::~nuiFlowView()
{
  
}

nuiRect nuiFlowView::CalcIdealSize()
{
  if (mCurrentIdealWidth == 0)
  {
    if (GetObjectName() == "Grid")
    {
      NGL_OUT("Flow Layout %p '%s'\n", this, GetObjectName().GetChars());
    }
  }
  if (mIdealWidth > 0)
  {
    mCurrentIdealWidth = mIdealWidth;
  }
  else if (mRect.GetWidth() > 0)
  {
    mCurrentIdealWidth = mRect.GetWidth();
  }
  nuiRect idealsize = Layout(false, mCurrentIdealWidth);

  if (GetDebug())
  {
    NGL_OUT("nuiFlowView::CalcIdealSize[%f]: %s\n", mCurrentIdealWidth, idealsize.GetValue().GetChars());
  }

  return idealsize;
}

void nuiFlowView::LayoutLine(nuiWidgetList& line, float& x, float &y, float& w, float& h, float& HSpace, float &VSpace, bool setLayout)
{
  // Process the line
  if (setLayout)
  {
    int32 size = line.size();
    x = 0;
    for (int32 j = 0; j < size; j++)
    {
      nuiWidgetPtr pWidget = line[j];
      nuiRect r(pWidget->GetIdealRect());
      r.SetHeight(h);
      r.MoveTo(x, y);
      pWidget->SetLayout(r);
      x += r.GetWidth();
      x += HSpace;
    }
  }
  line.clear();
  y += h +VSpace;
  h = 0;
  w = 0;
  
}

nuiRect nuiFlowView::Layout(bool setLayout, float IdealWidth)
{
  float x = 0;
  float y = 0;
  float VSpace = mVSpace;
  float HSpace = mHSpace;
  
  nuiWidgetList line;
  
  int32 count = mpChildren.size();
  float w = 0;
  float h = 0;
  for (int32 i = 0; i < count; i++)
  {
    nuiWidgetPtr pWidget = mpChildren[i];
    
    nuiRect r(pWidget->GetIdealRect());
    if (w + r.GetWidth() > IdealWidth)
    {
      LayoutLine(line, x, y, w, h, HSpace, VSpace, setLayout);
    }
    
    h = MAX(h, r.GetHeight());
    w += r.GetWidth();
    if (!line.empty())
      w += HSpace;
    line.push_back(pWidget);
  }
  if (!line.empty())
    LayoutLine(line, x, y, w, h, HSpace, VSpace, setLayout);
  
  if (y > 0)
    y -= VSpace;

  return nuiRect(IdealWidth, y);
}

bool nuiFlowView::SetRect(const nuiRect& rRect)
{
  nuiWidget::SetSelfRect(rRect);
  Layout(true, mCurrentIdealWidth);
  return true;
}

bool nuiFlowView::Draw(nuiDrawContext* pContext)
{
  nuiWidget::Draw(pContext);
  if (mIdealWidth > 0 && mIdealWidth != mCurrentIdealWidth)
  {
    InvalidateLayout();
  }
  else if (mCurrentIdealWidth != mRect.GetWidth())
  {
    InvalidateLayout();
  }

  return true;
}

void nuiFlowView::SetIdealWidth(float IdealWidth)
{
  mIdealWidth = IdealWidth;
  InvalidateLayout();
}

float nuiFlowView::GetIdealWidth() const
{
  return mIdealWidth;
}

float nuiFlowView::GetVSpace() const
{
  return mVSpace;
}

float nuiFlowView::GetHSpace() const
{
  return mHSpace;
}

void nuiFlowView::SetVSpace(float VSpace)
{
  mVSpace = VSpace;
  InvalidateLayout();
}

void nuiFlowView::SetHSpace(float HSpace)
{
  mHSpace = HSpace;
  InvalidateLayout();
}

