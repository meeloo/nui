/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nui.h"
#include "nuiDecoration.h"



class nuiColorDecoration : public nuiDecoration
{
public:

  nuiColorDecoration(const nglString& rName);
  nuiColorDecoration(const nglString& rName, const nuiRect& rClientRect, const nuiColor& rFillColor, float strokeSize=0, const nuiColor& rStrokeColor = nuiColor(0,0,0,0), nuiShapeMode ShapeMode = eFillShape, nuiBlendFunc BlendFunc = nuiBlendTransp);
  nuiColorDecoration(const nglString& rName, const nuiColor& rFillColor, float strokeSize, const nuiColor& rStrokeColor, nuiShapeMode ShapeMode = eStrokeAndFillShape, nuiBlendFunc BlendFunc = nuiBlendTransp, const nuiRect& rClientRect=nuiRect(0,0,0,0));
  virtual ~nuiColorDecoration();
  
  virtual nuiSize GetBorder(nuiPosition position, const nuiWidget* pWidget) const;
  virtual void GetBorders(const nuiWidget* pWidget, float& rLeft, float& rRight, float& rTop, float& rBottom, float& rHorizontal, float& rVertical) const;
  virtual nuiRect GetIdealClientRect(const nuiWidget* pWidget) const;

  void SetSourceClientRect(const nuiRect& rRect);
  const nuiRect& GetSourceClientRect() const;


  const nuiColor& GetFillColor() const;
	void SetFillColor(const nuiColor& color);
  const nuiColor& GetStrokeColor() const;
	void SetStrokeColor(const nuiColor& color);
  void SetStrokeSize(float size);
  float GetStrokeSize() const;
  void SetShapeMode(nuiShapeMode mode);
  nuiShapeMode GetShapeMode() const;

  	
private:

  virtual void Draw(nuiDrawContext* pContext, nuiWidget* pWidget, const nuiRect& rRect);


  void InitAttributes();

  float mStrokeSize;
  nuiColor mFillColor, mStrokeColor;
  nuiShapeMode mShapeMode;
  nuiRect mClientRect;

};

