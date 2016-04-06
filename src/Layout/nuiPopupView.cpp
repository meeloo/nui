/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

nuiPopupView::nuiPopupView(nuiWidget* pWidget, nuiSize X, nuiSize Y)
{
  SetObjectClass("nuiPopupView");

  mpPopupDecoration = NULL;
  
  mX = X;
  mY = Y;
  pWidget->LocalToGlobal(mX, mY);

  nuiTopLevel* pTop = NULL;
  if (pWidget)
    pTop = pWidget->GetTopLevel();
  if (pTop)
    pTop->AddChild(this);

  SetWantKeyboardFocus(true);
  
  //GetTopLevel()->CancelGrab();
}

nuiPopupView::~nuiPopupView()
{
  
}

bool nuiPopupView::SetRect(const nuiRect& rRect)
{
  nuiWidget::SetSelfRect(rRect);
  
  nuiRect Rect(GetIdealRect());
  nuiRect ParentRect(mpParent->GetRect().Size());
  
  nuiSize x, y;
  x = mX;
  y = mY;
  
  if (x + Rect.GetWidth() > ParentRect.GetWidth())
    x = ParentRect.GetWidth() - Rect.GetWidth();
  
  if (y + Rect.GetHeight() > ParentRect.GetHeight())
    y = ParentRect.GetHeight() - Rect.GetHeight();
  
  mChildrenRect = Rect;
  mChildrenRect.MoveTo(x, y);
  
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayout(mChildrenRect);
  }
  delete pIt;
  
  return true;
}


nuiDecoration* nuiPopupView::GetPopupDecoration()
{
  return mpPopupDecoration;
}

void nuiPopupView::SetPopupDecoration(const nglString& rDecorationName)
{
  nuiDecoration* pDeco = nuiDecoration::Get(rDecorationName);
  SetPopupDecoration(pDeco);
}

void nuiPopupView::SetPopupDecoration(nuiDecoration* pDeco)
{
  mpPopupDecoration = pDeco;
}

bool nuiPopupView::Draw(nuiDrawContext* pContext)
{
  if (!mpPopupDecoration) 
  {
    pContext->DrawWindowShade(mChildrenRect, nuiColor(1.0f, 1.0f, 1.0f, GetMixedAlpha()), true);

    pContext->SetFillColor("nuiActiveWindowBg");
    pContext->SetStrokeColor("nuiActiveWindowFg");
    
    pContext->DrawRect(mChildrenRect, eStrokeAndFillShape);
  }
  else
  {
     mpPopupDecoration->Draw(pContext, this, mChildrenRect);
  }
  
  
  
  return DrawChildren(pContext);
}

bool nuiPopupView::MouseClicked(const nglMouseInfo& rInfo)
{
  if (!mChildrenRect.IsInside(rInfo.X, rInfo.Y) && !IsTrashed())
  {
    Trash();
  }
  return true;
}

bool nuiPopupView::MouseUnclicked(const nglMouseInfo& rInfo)
{
  if (!mChildrenRect.IsInside(rInfo.X, rInfo.Y) && !IsTrashed())
  {
    Trash();
  }
  return true;
}

bool nuiPopupView::KeyUp(const nglKeyEvent& rEvent)
{
  if (!IsTrashed())
  {
    Trash();
  }
  return true;
}

bool nuiPopupView::KeyDown(const nglKeyEvent& rEvent)
{
  if (!IsTrashed())
  {
    Trash();
  }
  return true;
}

void nuiPopupView::SetPos(nuiSize X, nuiSize Y)
{
  mX = X;
  mY = Y;
  UpdateLayout();
}

void nuiPopupView::ConnectTopLevel()
{
  Grab();
  Focus();
  nuiWidget::ConnectTopLevel();
}

void nuiPopupView::SetVisibleRect(const nuiRect& rRect)
{
  nuiRect r;
  r.Intersect(rRect, mChildrenRect);
  nuiWidget::SetVisibleRect(r);
}

