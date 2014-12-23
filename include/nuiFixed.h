/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiFixed_h__
#define __nuiFixed_h__

//#include "nui.h"
#include "nuiWidget.h"

/// nuiFixed is a widget container that keep all its children at their original position and size.
class nuiFixed : public nuiWidget
{
public:
  nuiFixed();

  virtual nuiRect CalcIdealSize();
  virtual bool SetRect(const nuiRect& rRect);

protected:
  virtual ~nuiFixed();

};

#endif // __nuiFixed_h__

