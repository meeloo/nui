/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiShapeView_h__
#define __nuiShapeView_h__

#include "nuiWidget.h"
#include "nuiShape.h"

// nuishapeView.h

class nuiShape;

class nuiShapeView : public nuiWidget
{
public:
  nuiShapeView(nuiShape* pShape);

  nuiRect CalcIdealSize();
  bool Draw(nuiDrawContext* pContext);

  void SetShapeMode(nuiShapeMode ShapeMode);
  nuiShapeMode GetShapeMode() const;
  void EnableAntialiasing(bool Set);
  bool ISAntialiasingEnabled() const;
  void SetLineWidth(nuiSize Width);
  nuiSize GetLineWidth() const;

  NUI_GETSETDO(nuiColor, StrokeColor, Invalidate());
  NUI_GETSETDO(nuiColor, FillColor, Invalidate());
protected:
  virtual ~nuiShapeView();

  nuiShape* mpShape;
  nuiShapeMode mShapeMode;
  bool mAntialiasing;
  nuiSize mLineWidth;
  nuiColor mStrokeColor;
  nuiColor mFillColor;
};

#endif // __nuiShapeView_h__

