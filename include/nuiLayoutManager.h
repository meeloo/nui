//
//  nuiLayoutManager.h
//  nui3
//
//  Created by Sébastien Métrot on 18/10/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#pragma once

#include "nui.h"

class nuiRectAttributeAnimation;

class nuiMatrixNode;
class nuiEventActionHolder;


class nuiLayoutBase : public nuiObject
{
public:
  nuiLayoutBase();
  void InitAttributes();
  
  bool SetSelfRect(const nuiRect& rRect);
  virtual bool SetRect(const nuiRect& rRect);
  
  virtual void OnChildHotRectChanged(nuiWidget* pChild, const nuiRect& rChildHotRect);

  /** @name Children Layout animations: */
  //@{
  void SetChildrenLayoutAnimationDuration(float duration);
  void SetChildrenLayoutAnimationEasing(const nuiEasingMethod& rMethod);
  //@}
  
  virtual void SetLayout(const nuiRect& rRect); ///< This method asks the object to recalculate its layout with the given nuiRect. It will NOT force a SetRect. SetRect will be called if the widget asked for a re-layout (InvalidateLayout()) or if the given rectangle is different than the current rectangle of the widget. Returns the value returned by the SetRect method or false.
  nuiRect GetLayoutForRect(const nuiRect& rRect); ///< This method returns the actual rectangle that this widget would occupy in its parent if SetLayout was called with the given rRect argument.
  void SetBorders(nuiSize AllBorders); ///< Sets the empty space around the widget itself
  void SetBorder(nuiSize X, nuiSize Y); ///< Sets the empty space around the widget itself
  void GetBorder(nuiSize& rXLeft, nuiSize& rXRight, nuiSize& rYTop, nuiSize& rYBottom);
  void SetBorder(nuiSize XLeft, nuiSize XRight, nuiSize YTop, nuiSize YBottom); ///< Sets the empty space around the widget itself
  void SetBorderLeft(nuiSize border);
  void SetBorderTop(nuiSize border);
  void SetBorderRight(nuiSize border);
  void SetBorderBottom(nuiSize border);
  nuiSize GetBorderLeft() const;
  nuiSize GetBorderTop() const;
  nuiSize GetBorderRight() const;
  nuiSize GetBorderBottom() const;
  nuiSize GetActualBorderLeft() const;
  nuiSize GetActualBorderTop() const;
  nuiSize GetActualBorderRight() const;
  nuiSize GetActualBorderBottom() const;
  
  virtual void SetVisibleRect(const nuiRect& rRect); ///< This sets the rectangle that will actually be displayed in the parent widget (for example in case this widget is inside a nuiScrollView, only a part of it may be visible at once). The rectangle is local to the widget rect.
  void SilentSetVisibleRect(const nuiRect& rRect); ///< This method change the visible rect of the widget without invalidating it. It is useful if you need to change the visible rect from a parent's SetRect method: you allready know that you will need to redraw it. See SetVisibleRect for more information.
  const nuiRect& GetVisibleRect() const; ///< This sets the rectangle that will actually be displayed in the parent widget (for example in case this widget is inside a nuiScrollView, only a part of it may be visible at once). The rectangle is local to the widget rect.
  void ResetVisibleRect();
  
  void StartTransition(); ///< Signals to this widget that its state is being transitionned and that it should start ignoring layout changes from its parent.
  void StopTransition(); ///<  Signals to this widget that its state transition is done and that must obei to the layout changes from its parent again.
  bool IsInTransition() const;
  bool IsInSetRect() const; ///< Returns true if this widget or one of its parents' SetRect method is currently being called.
  
  virtual const nuiRect& GetIdealRect(); ///< Return the ideal area used by this Object. If the layout of this object has changed CalIdealRect will be called and mIdealRect will contain the ideal rectangle. If the user specified a user size then mIdealRect will be overwritten with mUserRect.
  virtual const nuiRect& GetRect() const; ///< Return the current area used by this Object.
  virtual nuiRect GetBorderedRect() const; ///< Return the current area used by this Object including its border
  nuiRect GetBorderedRect(const nuiRect& rRect) const;
  nuiRect GetBorderLessRect(const nuiRect& rRect) const;
  
  void SetUserWidth(nuiSize s);
  nuiSize GetUserWidth();
  void SetUserHeight(nuiSize s);
  nuiSize GetUserHeight();
  
  const nuiRect& GetUserRect() const;
  nuiPoint GetUserPos() const;
  void SetUserRect(const nuiRect& rRect); ///< Ask the widget to use the specified rect as its ideal size and position. The position and size may not be granted by the widgets parent. This method is a combination of SetUserPos and SetUserSize.
  void SetUserPos    (nuiSize X, nuiSize Y); ///< Ask the widget to use the specified position as its ideal position. The position and size may not be granted by the widgets parent.
  void SetUserPos    (nuiPoint Pos); ///< Ask the widget to use the specified position as its ideal position. The position and size may not be granted by the widgets parent.
  void SetUserSize   (nuiSize Width, nuiSize Height); ///< Ask the widget to use the specified size as its ideal size. The position and size may not be granted by the widgets parent.
  void UnsetUserRect (); ///< Ask the widget to stop using the user provided rectangle and restart calculating its size on its own.
  void UnsetUserPos  (); ///< Ask the widget to stop using the user provided position and restart calculating its position on its own.
  void UnsetUserSize (); ///< Ask the widget to stop using the user provided size and restart calculating its size on its own.
  bool HasUserRect   () const; ///< Returns true if the widget is using a user provided rectangle to calculate its size and position.
  bool HasUserPos    () const; ///< Returns true if the widget is using a user provided point to calculate its position.
  bool HasUserSize   () const; ///< Returns true if the widget is using a user provided size to calculate its size.
  bool HasUserWidth  () const;
  bool HasUserHeight () const;
  void ForceIdealRect(bool Force); ///< Always. Misuse of this flag can lead to very strange visual bugs so beware!
  void SetFixedAspectRatio(bool set); ///< Change the aspect ratio layout policy. If the aspect ratio is fixed then the layout engine tries to keep the aspect ratio implied by the ideal rect requested by the widget. It is false by default for most widgets.
  bool GetFixedAspectRatio() const; ///< Returns the aspect ratio layout policy. If the aspect ratio is fixed then the layout engine tries to keep the aspect ratio implied by the ideal rect requested by the widget. It is false by default for most widgets.
  
  
  
  /** @name Maximum and minimum IDEAL width and height. Setting these values will only change the IDEAL rectangle and it will NOT force the actual layout rectangle. Use the SetMax[Min]Width[Height] methods to actually force the layout. */
  //@{
  void SetMaxIdealWidth(float MaxWidth); ///< Set to a negative value to disable
  void SetMaxIdealHeight(float MaxHeight); ///< Set to a negative value to disable
  void SetMinIdealWidth(float MinWidth); ///< Set to a negative value to disable
  void SetMinIdealHeight(float MinHeight); ///< Set to a negative value to disable
  float GetMaxIdealWidth() const;
  float GetMaxIdealHeight() const;
  float GetMinIdealWidth() const;
  float GetMinIdealHeight() const;
  //@}
  
  /** @name Maximum and minimum width and height. Setting these values will change the ACTUAL rectangle. To change the min/max IDEAL rectangle use SetMax[Min]IdealWidth[Height] */
  //@{
  void SetMaxWidth(float MaxWidth); ///< Set to a negative value to disable
  void SetMaxHeight(float MaxHeight); ///< Set to a negative value to disable
  void SetMinWidth(float MinWidth); ///< Set to a negative value to disable
  void SetMinHeight(float MinHeight); ///< Set to a negative value to disable
  float GetMaxWidth() const;
  float GetMaxHeight() const;
  float GetMinWidth() const;
  float GetMinHeight() const;
  
  void GetHotRect(nuiRect& rRect) const; ///< Get the user focused rectangle in the widget. This rectangle is an indication only. The parent of this widget can choose to display it or to ignore the recommendation.
  const nuiRect& GetHotRect() const; ///< Get the user focused rectangle in the widget. This rectangle is an indication only. The parent of this widget can choose to display it or to ignore the recommendation.
  void SetHotRect(const nuiRect& rRect); ///< Get the user focused rectangle in the widget. This rectangle is an indication only. The parent of this widget can choose to display it or to ignore the recommendation.
  bool HasLayoutChanged() const; ///< This method returns true if the layout of the object has changed since the last layout/redraw loop.
  
  bool IsInsideFromRoot(nuiSize X, nuiSize Y, nuiSize GrowOffset = 0); ///< Return true if the point (X,Y) (in the coordinates of the root object) is inside the object. This method call IsInsideLocal internally so you may not need to redefine it.
  bool IsInsideFromParent(nuiSize X, nuiSize Y, nuiSize GrowOffset = 0); ///< Return true if the point (X,Y) (in the coordinates of the parent) is inside the object.
  virtual bool IsInsideFromSelf(nuiSize X, nuiSize Y, nuiSize GrowOffset = 0); ///< Return true if the point (X,Y) (in the coordinates of the parent) is inside the object.
  //@}
  
  virtual void InvalidateLayout(); ///< Tell the system that this widget's geometry should be recalculated, unless it has a user rect.
  virtual void ForcedInvalidateLayout(); ///< Nobody should use this method, please use InvalidateLayout instead. Tell the system that this widget's geometry should be recalculated, no exception!
  virtual void UpdateLayout(); ///< Force the re layout of this widget without telling the parrents that the internal geometry has changed.
  virtual void SilentInvalidateLayout(); ///< Mark this widget as invalid with invalid layout. No event is broadcast. Most of the time you really want to use InvalidateLayout instead of SilentInvalidateLayout().

  virtual void Built(); ///< This method is called right after a widget has been created by the CSS engine and all the CSS inits have been applied.

  virtual void SetVisible(bool Visible); ///< Show or hide the widget
  
  void SilentSetVisible(bool Visible); ///< Change the visible flag of the widget but don't invalidate. This is useful if you are changing the visibility of a widget from a parent's SetRect method: you are already aware of the rendering and invalidation change.
  bool IsVisible(bool combined = true) const; ///< Return true if the object is visible. By default the returned state will be the state of the object combined with the states of its hierarchy recursively. If one of its parents is invisible the widget becomes invisible. If combine==false the returned value will be the local visible state of the object.
  bool AttrIsVisible();
  
  virtual void SetPosition(nuiPosition mode); ///< Set the position of the widget inside its give rect. nuiTiled is not supported at the moment.
  virtual nuiPosition GetPosition() const; ///< Get the position where the widget will be layed out in the rectangle given by its parent container.
  virtual void SetFillRule(nuiPosition mode); ///< If the widget's position if nuiFill, this method sets the behaviour of the object when the drawing region doesn't respects the 1:1 ratio of the widget's ideal rect.
  virtual nuiPosition GetFillRule() const; ///< If the widget's position if nuiFill, this method gets the behaviour of the object when the drawing region doesn't respects the 1:1 ratio of the widget's ideal rect.
  
  nuiSimpleEventSource <nuiWidgetShown> Shown; ///< Send an event when the object is shown
  nuiSimpleEventSource <nuiWidgetHiden> Hiden; ///< Send an event when the object is hidden
  nuiSimpleEventSource <nuiWidgetVisibilityChanged> VisibilityChanged; ///< Send an event when the object is either hiden or shown.
  
  nuiRectAttributeAnimation* GetLayoutAnimation(bool CreateIfNotAvailable);
  void SetLayoutAnimationDuration(float duration);
  float GetLayoutAnimationDuration();
  void SetLayoutAnimationEasing(const nuiEasingMethod& rMethod);

  /** @name Matrix Transformation Support */
  //@{
  void AddMatrixNode(nuiMatrixNode* pNode);
  void DelMatrixNode(uint32 index);
  int32 GetMatrixNodeCount() const;
  nuiMatrixNode* GetMatrixNode(uint32 index) const;
  //@}
  
  /** @name Old (deprecated) Matrix Transformation Support */
  //@{
  void LoadIdentityMatrix();
  bool IsMatrixIdentity() const;
  void GetMatrix(nuiMatrix& rMatrix) const;
  nuiMatrix GetMatrix() const;
  void SetMatrix(const nuiMatrix& rMatrix);
  //@}
  
  bool GetNeedLayout() const { return mNeedSelfLayout; }
  bool GetNeedIdealRect() const { return mNeedIdealRect; }

  NUI_GETSET(bool, AutoUpdateLayout);
  
  /** @name Layout Constraints */
  //@{
  class LayoutConstraint
  {
  public:
    LayoutConstraint();
    LayoutConstraint(const LayoutConstraint& rSource);
    LayoutConstraint& operator=(const LayoutConstraint& rSource);
    bool operator==(const LayoutConstraint& rSource) const;
    
    nuiSize mMaxWidth;
    nuiSize mMaxHeight;
    nuiSize mMinWidth;
    nuiSize mMinHeight;
  };
  
  virtual bool SetLayoutConstraint(const LayoutConstraint& rConstraint); ///< Change the layout constraint imposed by this widget's parent. Return false if the constraint haven't changed, true otherwise.
  const LayoutConstraint& GetLayoutConstraint() const; ///< Retrieve the layout constraint imposed by this widget's parent.
  //@}

  /** @name CSS (Cascading style sheets) */
  //@{
  void IncrementCSSPass();
  void ResetCSSPass();
  uint32 GetCSSPass() const;
  //@}
  
  /** @name Generic Public Events */
  //@{
  nuiEventSource* GetEvent(const nglString& rName);
  void GetEvents(std::vector<nglString>& rNames);
  bool AddEventAction(const nglString& rEventName, nuiEventActionHolder* pActions);
  //@}
  
  
  bool SetObjectClass(const nglString& rName);
  void SetObjectName(const nglString& rName);
  
  
protected:
  nuiRect mRect; ///< The bounding box of the nuiWidget (in coordinates of its parent).
  nuiRect mLayoutRect; ///< The rect given by the parent (may be different than mRect)
  nuiRect mVisibleRect; ///< The active bounding box of the nuiObject (in local coordinates).
  nuiRect mIdealRect; ///< The ideal bounding box of the nuiObject (in coordinates of its parent) position should be at the origin.
  nuiRect mUserRect; ///< The bounding box of the nuiObject if set by the user (in coordinates of its parent).
  nuiRect mHotRect; ///< The currently important interactive part of the widget. Containers try to keep this rect in view when possible. For exemple set it as the cursor rectangle in a text edit widget. Is you text edit is contained in a scroll view, the scroll view will try to follow the cursor.
  LayoutConstraint mConstraint;
  
  
  static nuiMatrix mIdentityMatrix;
  std::vector<nuiMatrixNode*>* mpMatrixNodes;
  nuiMatrix _GetMatrix() const;
  void _SetMatrix(nuiMatrix Matrix);
  
  std::map<nglString, nuiEventSource*, nglString::LessFunctor> mEventMap;
  std::vector<nuiEventActionHolder*> mEventActions;
  
  void AddEvent(const nglString& rName, nuiEventSource& rEvent);
  
  virtual void BroadcastInvalidateLayout(nuiWidgetPtr pSender, bool BroadCastOnly);
  void ApplyCSSForStateChange(uint32 MatchersTag); ///< This method will match this widget's state with the CSS and apply the changes needed to display it correctly
  
  
  bool mAnimateLayout : 1;
  bool mVisible : 1;
  bool mHasUserPos: 1;
  bool mHasUserSize: 1;
  bool mHasUserWidth: 1;
  bool mHasUserHeight: 1;
  bool mForceIdealSize: 1;
  bool mNeedSelfLayout: 1; ///< Layout of this widget has been invalidated
  bool mNeedLayout: 1; ///< Layout has been invalidated in this branch of the widget tree. This doesn't mean that this particularobject needs to recalc its layout, it may come from one of its children.
  bool mNeedIdealRect: 1;
  bool mCanRespectConstraint: 1;
  bool mInSetRect: 1;
  bool mFixedAspectRatio: 1;
  bool mOverrideVisibleRect : 1;
  bool mAutoUpdateLayout : 1;

  float mMaxIdealWidth;
  float mMaxIdealHeight;
  float mMinIdealWidth;
  float mMinIdealHeight;
  
  float mMaxWidth;
  float mMaxHeight;
  float mMinWidth;
  float mMinHeight;
  
  void AutoInvalidateLayout(const nuiEvent& rEvent); ///< This methods calls InvalidateLayout()
  
  void InternalSetLayout(const nuiRect& rect);
  virtual void InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged);
  
  nuiPosition mPosition;
  nuiPosition mFillRule;
  nuiRectAttributeAnimation* mpLayoutAnimation;
  
  nuiSize mBorderLeft, mBorderRight; // empty space left left and right the widget itself
  nuiSize mBorderTop, mBorderBottom; // empty space left below and above the widget itself
  
  uint32 mCSSPasses;
  virtual void InternalResetCSSPass();
};

#if 0
class nuiLayoutManager : public nuiObject
{
public:
  nuiLayoutManager();
  virtual ~nuiLayoutManager();
  
  void AddChild(nuiLayoutManager* pManager);
  void AddChild(nuiWidget* pWidget);
  void DelChild(nuiLayoutManager* pManager);
  void DelChild(nuiWidget* pWidget);
  
  
protected:
  std::vector<nuiLayoutManager*> mpLayouts;
  std::vector<nuiLayoutManager*> mpWidgets;
};
#endif

#define NUI_ADD_EVENT(NAME) { AddEvent(_T(#NAME), NAME); }

