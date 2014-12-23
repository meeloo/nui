/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nui.h"
#include "nuiWidget.h"

class nuiPopupView : public nuiWidget
{
public:
  nuiPopupView(nuiWidget* pWidget, nuiSize X, nuiSize Y);

  void SetPos(nuiSize X, nuiSize Y);
  
  bool MouseClicked(const nglMouseInfo& rInfo);
  bool MouseUnclicked(const nglMouseInfo& rInfo);

  bool KeyUp(const nglKeyEvent& rEvent);
  bool KeyDown(const nglKeyEvent& rEvent);
  
  bool SetRect(const nuiRect& rRect);
  
  bool Draw(nuiDrawContext* pContext);
  
  void SetPopupDecoration(const nglString& rDecorationName);
  void SetPopupDecoration(nuiDecoration* pDecoration);
  nuiDecoration* GetPopupDecoration();
  
  void SetVisibleRect(const nuiRect& rRect);

  void ConnectTopLevel();
protected:
  virtual ~nuiPopupView();

  //#TODO Add some way to give focus back to the previous focussed widget
  nuiSize mX, mY;
  nuiRect mChildrenRect;
  nuiDecoration* mpPopupDecoration;
};

