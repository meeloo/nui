/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nui.h"
#include "nuiWidget.h"
#include "nuiFrame.h"

class nuiFrameView : public nuiWidget
{
public:
  nuiFrameView(nuiFrame* pFrame = NULL);

  virtual nuiRect CalcIdealSize();
  virtual bool SetRect(const nuiRect& rRect);
  virtual bool Draw(nuiDrawContext* pContext);

protected:
  virtual ~nuiFrameView();

private:
    nuiFrame* mpFrame;
};
