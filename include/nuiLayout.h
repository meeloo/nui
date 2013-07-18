/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2011 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#pragma once

/* Contraint syntax:

 Each line contains: "HorizontalConstraint/VerticalConstraint"
 Any one of them can be omited:
 "/VerticalConstraint"
 "HorizontalConstraint/"
 
 Both vertical and horizontal constraint can be described like this:
 
 1 Set Start And Stop: [StartAnchorName,StopAnchorName]
 2 Set Start: [StartAnchorName}
 3 Set Start and size: [StartAnchorName, size}
 4 Set Stop: {StopAnchorName]
 5 Set Stop and size: {size, StopAnchorName]
 6 Set Middle: {MiddleAnchor}
 7 Set Middle And Size: {MiddleAnchor, size}
 8 Set Size: size
 */

enum nuiLayoutConstraintType
{
  eFreeLayout,
  eSetStartAndStop,

  eSetStart,
  eSetStartAndSize,

  eSetStop,
  eSetStopAndSize,

  eSetMiddle,
  eSetMiddleAndSize,

  eSetSize
};

enum nuiAnchorType
{
  eAnchorAbsolute = 0,
  eAnchorRelative
};


class nuiLayoutConstraint
{
public:
  nuiLayoutConstraint();
  virtual ~nuiLayoutConstraint();

  nuiLayoutConstraintType GetType() const;
  const nglString& GetAnchor(int i) const;
  float GetSize() const;

  void SetStartAndStop(const nglString& rStart, const nglString& rStop);

  void SetStart(const nglString& rStart);
  void SetStartAndSize(const nglString& rStart, float size);

  void SetStop(const nglString& rStop);
  void SetStopAndSize(const nglString& rStop, float size);

  void SetMiddle(const nglString& rMiddle);
  void SetMiddle(const nglString& rStart, const nglString& rStop);
  void SetMiddleAndSize(const nglString& rMiddle, float size);
  void SetMiddleAndSize(const nglString& rStart, const nglString& rStop, float size);

  void SetSize(float size);
  void SetFree();

  bool Set(const nglString& rDescription);
protected:
  nuiLayoutConstraintType mType;
  nglString mAnchor[2];
  float mSize;
};

class nuiLayout : public nuiSimpleContainer
{
public:
  nuiLayout();
  virtual ~nuiLayout();

  void SetVerticalAnchor(const nglString& rName, float position, nuiAnchorType Type);
  void SetHorizontalAnchor(const nglString& rName, float position, nuiAnchorType Type);

  void SetVerticalAnchorPosition(const nglString& rName, float position);
  void SetHorizontalAnchorPosition(const nglString& rName, float position);

  void SetVerticalAnchorMovable(const nglString& rName, bool movable);
  void SetHorizontalAnchorMovable(const nglString& rName, bool movable);
  void SetVerticalAnchorMinimum(const nglString& rName, float position);
  void SetVerticalAnchorMaximum(const nglString& rName, float position);
  void SetHorizontalAnchorMinimum(const nglString& rName, float position);
  void SetHorizontalAnchorMaximum(const nglString& rName, float position);

  bool GetVerticalAnchorMovable(const nglString& rName) const;
  bool GetHorizontalAnchorMovable(const nglString& rName) const;
  float GetVerticalAnchorMinimum(const nglString& rName) const;
  float GetHorizontalAnchorMinimum(const nglString& rName) const;
  float GetVerticalAnchorMaximum(const nglString& rName) const;
  float GetHorizontalAnchorMaximum(const nglString& rName) const;

  void SetVerticalAnchorType(const nglString& rName, nuiAnchorType Type);
  void SetHorizontalAnchorType(const nglString& rName, nuiAnchorType Type);

  float GetVerticalAnchorPosition(const nglString& rName) const;
  float GetHorizontalAnchorPosition(const nglString& rName) const;

  nuiAnchorType GetVerticalAnchorType(const nglString& rName) const;
  nuiAnchorType GetHorizontalAnchorType(const nglString& rName) const;

  void SetConstraint(nuiWidget* pWidget, const nglString& rDescription);
  void SetConstraint(nuiWidget* pWidget, const nuiLayoutConstraint& rHorizontal, const nuiLayoutConstraint& rVertical);
  void GetConstraints(nuiWidget* pWidget, nuiLayoutConstraint& rHorizontal, nuiLayoutConstraint& rVertical) const;
  void GetConstraintString(nuiWidget* pWidget, nglString& rString) const;

  bool SetRect(const nuiRect& rRect);

  virtual void SetProperty(const nglString& rName, const nglString& rValue); ///< Add or change a property of the object.
  virtual bool AddChild(nuiWidgetPtr pChild);

  virtual bool MouseClicked(const nglMouseInfo& rInfo);
  virtual bool MouseUnclicked(const nglMouseInfo& rInfo);
  virtual bool MouseMoved(const nglMouseInfo& rInfo);

private:
  void ComputeConstraint(const nuiLayoutConstraint& rC, float& ActualStart, float& ActualStop, float Start, float Stop, float IdealSize, int32 AnchorIndex);
  float ComputeAnchorPosition(const nglString& rName, int32 AnchorIndex, float Start, float Stop) const;
  std::map<nuiWidget*, std::pair<nuiLayoutConstraint, nuiLayoutConstraint> > mConstraints;
  class Anchor
  {
  public:
    Anchor(float pos = 0, nuiAnchorType type = eAnchorAbsolute, bool movable = false, float min = 0, float max = 0);
    float mPosition;
    nuiAnchorType mType;
    float mMinimum;
    float mMaximum;
    bool mMovable;
  };
  std::map<nglString, Anchor > mAnchors[2];

  nglString mMovingAnchor[2];
  float mMovingAnchorPos[2];
  void DoLayout(const nuiRect& rRect);
  float mClickX, mClickY;
};

