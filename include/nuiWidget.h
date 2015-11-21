/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/




#pragma once

//#include "nui.h"
#include "nuiObject.h"
#include "nuiRect.h"
#include "nuiMouseEvent.h"
#include "nuiWidgetElements.h"
#include "nuiHotKey.h"
#include "nuiDecorationDefines.h"
#include "nuiDefaultDecoration.h"
#include "nuiRenderState.h"
#include "nuiAnimation.h"
#include "nuiMatrixNode.h"
#include "nuiEvent.h"
#include "nuiTreeEvent.h"

class nuiDrawContext;
class nuiWidget;
class nuiMetaPainter;
class nuiDecoration;

class nuiTopLevel;

class nuiAnimation;

class nuiRectAttributeAnimation;

class nuiMatrixNode;
class nuiEventActionHolder;

typedef nuiWidget* nuiWidgetPtr;
typedef std::vector<nuiRef<nuiWidget> > nuiWidgetList;

typedef nuiTreeEventSource<nuiChildAdded, nuiWidget> nuiWidgetAddedEventSource;
typedef nuiTreeEventSource<nuiChildDeleted, nuiWidget> nuiWidgetDeletedEventSource;

class nuiWidget : public nuiObject
{
protected:
  friend class nuiTopLevel;
  class TestWidgetFunctor
  {
  public:
    TestWidgetFunctor() {};
    virtual ~TestWidgetFunctor() {};
    virtual bool operator()(nuiWidgetPtr pWidget)
    {
      return false;
    }
  };


public:

  class Iterator
  {
  public:
    Iterator(nuiWidget* pContainer, bool DoRefCounting = false);
    Iterator(const Iterator& rIterator);
    virtual Iterator& operator = (const Iterator& rIterator);
    virtual ~Iterator();

    bool IsValid() const;
    void SetValid(bool set);
    virtual nuiWidgetPtr GetWidget() const = 0;
    nuiWidgetPtr operator*() const
    {
      return GetWidget();
    }
  protected:
    nuiWidget* mpSource = nullptr;
    bool mValid = false;
    bool mRefCounting = false;
  };

  class ConstIterator
  {
  public:
    ConstIterator(const nuiWidget* pContainer, bool DoRefCounting = false);
    ConstIterator(const ConstIterator& rIterator);
    virtual ConstIterator& operator = (const ConstIterator& rIterator);
    virtual ~ConstIterator();

    bool IsValid() const;
    void SetValid(bool set);
    virtual nuiWidgetPtr GetWidget() const = 0;
    nuiWidgetPtr operator*() const
    {
      return GetWidget();
    }
  protected:
    const nuiWidget* mpSource = nullptr;
    bool mValid = false;
    bool mRefCounting = false;
  };

  typedef Iterator* IteratorPtr;
  typedef ConstIterator* ConstIteratorPtr;
  
  /** @name Widget Container */
  //@{
  virtual bool AddChild(nuiWidgetPtr pChild);
  virtual bool DelChild(nuiWidgetPtr pChild); ///< Remove this child from the object. If Delete is true then the child will be deleted too. Returns true if success.
  virtual int GetChildrenCount() const;
  virtual nuiWidgetPtr GetChild(int index); ///< Returns the child which has the given index (first child = 0). Return NULL in case of faillure.
  virtual nuiWidgetPtr GetChild(nuiSize X, nuiSize Y); ///< Returns the child which is under the pixel (X,Y) in this object or this if there is no such child. X and Y are given in the coordinate system of the parent object.
  virtual nuiWidgetPtr GetChild(const nglString& rName, bool ResolveNameAsPath = true); ///< Find a child by its name property. Try to resolve path names like /window/fixed/toto or ../../tata if deepsearch is true
  virtual bool Clear();///< Clear all children. By default the children are deleted unless Delete == false.

  virtual nuiWidget::Iterator* GetFirstChild(bool DoRefCounting = false);
  virtual nuiWidget::ConstIterator* GetFirstChild(bool DoRefCounting = false) const;
  virtual nuiWidget::Iterator* GetLastChild(bool DoRefCounting = false);
  virtual nuiWidget::ConstIterator* GetLastChild(bool DoRefCounting = false) const;
  virtual bool GetNextChild(nuiWidget::IteratorPtr pIterator);
  virtual bool GetNextChild(nuiWidget::ConstIteratorPtr pIterator) const;
  virtual bool GetPreviousChild(nuiWidget::IteratorPtr pIterator);
  virtual bool GetPreviousChild(nuiWidget::ConstIteratorPtr pIterator) const;

  virtual void RaiseChild(nuiWidgetPtr pChild);
  virtual void LowerChild(nuiWidgetPtr pChild);
  virtual void RaiseChildToFront(nuiWidgetPtr pChild);
  virtual void LowerChildToBack(nuiWidgetPtr pChild);


  /** @name Object relation management */
  //@{
  virtual void GetChildren(nuiSize X, nuiSize Y, nuiWidgetList& rChildren, bool DeepSearch = false); /// Return all the children under the pixel (X, Y) in this container.

  nuiWidgetPtr GetChildIf(nuiSize X, nuiSize Y, TestWidgetFunctor* pFunctor); ///< Returns the child that satisfies the given functor object and that is under the pixel (X,Y) in this object or this if there is no such child. X and Y are given in the coordinate system of the parent object. rFunctor is a std::unary_functor<nuiWidgetPtr, bool> object defined by the user.
  nuiWidgetPtr SearchForChild(const nglString& rName, bool recurse = true);  ///< Find a child by its name property, recurse the search in the subchildren if asked politely.
  nuiWidgetPtr Find (const nglString& rName); ///< Finds a node given its full path relative to the current node. Eg. Find("background/color/red").

  virtual IteratorPtr GetChildIterator(nuiWidgetPtr pChild, bool DoRefCounting = false);
  virtual ConstIteratorPtr GetChildIterator(nuiWidgetPtr pChild, bool DoRefCounting = false) const;
  virtual nuiWidgetPtr GetNextFocussableChild(nuiWidgetPtr pChild) const;
  virtual nuiWidgetPtr GetPreviousFocussableChild(nuiWidgetPtr pChild) const;
  virtual nuiWidgetPtr GetNextSibling(nuiWidgetPtr pChild) const;
  virtual nuiWidgetPtr GetPreviousSibling(nuiWidgetPtr pChild) const;
  //@}

  /** @name Outgoing events */
  //@{
  nuiWidgetAddedEventSource ChildAdded;
  nuiWidgetDeletedEventSource ChildDeleted;
  //@}

  /** @name Inherited from nuiWidget: */
  //@{
  virtual void InvalidateChildren(bool Recurse);
  virtual bool Draw(nuiDrawContext* pContext);
  virtual nuiRect CalcIdealSize();
  bool SetSelfRect(const nuiRect& rRect);
  virtual bool SetRect(const nuiRect& rRect);
  virtual void SetAlpha(float Alpha);
  virtual void SetEnabled(bool set);
  virtual void SetSelected(bool set);

  virtual void OnChildHotRectChanged(nuiWidget* pChild, const nuiRect& rChildHotRect);
  virtual void SetVisible(bool Visible); ///< Show or hide the widget
                                         //@}

  /** @name Private event management system (do not override unless you know what you're doing!!!) */
  //@{
  virtual bool CallPreMouseClicked(const nglMouseInfo& rInfo);
  virtual bool CallPreMouseUnclicked(const nglMouseInfo& rInfo);
  virtual bool CallPreMouseMoved(const nglMouseInfo& rInfo);
  virtual bool CallPreMouseWheelMoved(const nglMouseInfo& rInfo);
  //@} 

  /** @name Private event management system (do not override unless you know what you're doing!!!) */
  //@{
  /// Beware: these three methods receive the mouse coordinates in the repair of the root object!
  virtual bool DispatchMouseClick(const nglMouseInfo& rInfo);
  virtual bool DispatchMouseUnclick(const nglMouseInfo& rInfo);
  virtual nuiWidgetPtr DispatchMouseMove(const nglMouseInfo& rInfo);
  virtual nuiWidgetPtr DispatchMouseWheelMove(const nglMouseInfo& rInfo);
  virtual bool DispatchMouseCanceled(nuiWidgetPtr pThief, const nglMouseInfo& rInfo);
  //@}

  /** @name Children Layout animations: */
  //@{
  void SetChildrenLayoutAnimationDuration(float duration);
  void SetChildrenLayoutAnimationEasing(const nuiEasingMethod& rMethod);
  //@}

  virtual void GetHoverList(nuiSize X, nuiSize Y, std::set<nuiWidget*>& rHoverSet, std::list<nuiWidget*>& rHoverList) const;
  //@}


  /** @name Life */
  //@{
  nuiWidget(); ///< Create an nuiObject
  nuiWidget(const nglString& rObjectName); ///< constructor with an objectName by default
	void InitAttributes();
  //@}

  /** @name Serialization */
  //@{
  nglString Dump(); ///< serialize the object and dump the result in a output string
  //@}

  virtual bool SetObjectClass(const nglString& rName);
  virtual void SetObjectName(const nglString& rName);


  /** @name Object relation management */
  //@{
  virtual nuiWidgetPtr GetParent() const; ///< Return the parent of this object
  virtual nuiWidgetPtr GetRoot() const; ///< Return the top parent of this object.
  virtual nuiTopLevel* GetTopLevel() const; ///< Return the top level parent of this object if it exists.
  virtual bool SetParent(nuiWidgetPtr pParent); ///< Set the parent of this object.
  virtual bool IsParentActive() const; ///< Return true if this object main parent is an active window.
  virtual bool Trash(); ///< This method will try to destroy the nuiObject as soon as possible. Returns true if the destruction is ongoing, false if there was a problem. Use this method instead of the delete C++ keyword all the time!!!!
  bool HasInheritedProperty (const nglString& rName); ///< This method returns true if a widget or one of its parents has the given property.
  nglString GetInheritedProperty (const nglString& rName); ///< Returns the value of the given property. This method looks for the property in the object and in its parents recursively.
  virtual void OnTrash(); ///< This method is called whenever a Trash occurred on the widget tree branch
  //@}

  /** @name Coordinates translation */
  //@{
  void LocalToGlobal(int& x, int& y) const; ///< This method recursively calculate the coordinate of the given point if the system of the top parent. (x,y) will be translated from the client coordinates to the main window coordinates.
  void LocalToGlobal(nuiSize& x, nuiSize& y) const; ///< This method recursively calculate the coordinate of the given point if the system of the top parent. (x,y) will be translated from the client coordinates to the main window coordinates.
  void LocalToGlobal(nuiRect& rRect) const;
  void GlobalToLocal(int& x, int& y) const; ///< This method recursively calculate the coordinate of the given point if the system of the top parent. (x,y) will be interpreted as main window coordinate and translated to client coordinates.
  void GlobalToLocal(nuiSize& x, nuiSize& y) const; ///< This method recursively calculate the coordinate of the given point if the system of the top parent. (x,y) will be interpreted as main window coordinate and translated to client coordinates.
  void GlobalToLocal(nuiRect& rRect) const;
  void LocalToLocal(nuiWidgetPtr pWidget, int& x, int& y) const; ///< This method translates (x,y) from the Local coordinate system of this object to the local coordinate system of pWidget. They must be both owned by the same top level object.
  void LocalToLocal(nuiWidgetPtr pWidget, nuiSize& x, nuiSize& y) const; ///< This method translates (x,y) from the Local coordinate system of this object to the local coordinate system of pWidget. They must be both owned by the same top level object.
  void LocalToLocal(nuiWidgetPtr pWidget, nuiRect& rRect) const; ///< This method translates rRect from the Local coordinate system of this object to the local coordinate system of pWidget. They must be both owned by the same top level object.
  //@}

  /** @name Object size management */
  //@{
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
  
  void SetOverDrawLeft(nuiSize border);
  void SetOverDrawTop(nuiSize border);
  void SetOverDrawRight(nuiSize border);
  void SetOverDrawBottom(nuiSize border);
  nuiSize GetOverDrawLeft() const;
  nuiSize GetOverDrawTop() const;
  nuiSize GetOverDrawRight() const;
  nuiSize GetOverDrawBottom() const;

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

  /** @name Rendering */
  //@{

  virtual void UpdateCache(nuiDrawContext* pContext, nuiRenderThread* pRenderThread);
  virtual void InvalidateRect(const nuiRect& rRect);
  virtual void Invalidate(); ///< Ask for a redraw of the object. Only the nuiMainWindow class should redefine this method.

  virtual void InvalidateLayout(); ///< Tell the system that this widget's geometry should be recalculated, unless it has a user rect.
  virtual void ForcedInvalidateLayout(); ///< Nobody should use this method, please use InvalidateLayout instead. Tell the system that this widget's geometry should be recalculated, no exception!
  virtual void UpdateLayout(); ///< Force the re layout of this widget without telling the parrents that the internal geometry has changed.
  virtual void SilentInvalidateLayout(); ///< Mark this widget as invalid with invalid layout. No event is broadcast. Most of the time you really want to use InvalidateLayout instead of SilentInvalidateLayout().
  float GetMixedAlpha() const; ///< Returns the current alpha transparency value of this object, mixed with the one of its parents if MixWithParent==true.
  float GetAlpha() const; ///< Returns the current alpha transparency value of this object.
  virtual nuiDrawContext* GetDrawContext(); ///< Retrieve the draw context from the root object (the main window).
  virtual nuiRenderThread* GetRenderThread(); ///< Retrieve the render thread from the root object (the main window).

  void SetOverDraw(nuiSize Left, nuiSize Top, nuiSize Right, nuiSize Bottom); ///< Set the amount of over draw for each border of the widget.
  void ResetOverDraw(); ///< Reset the overdraw borders to 0. Equivalent to SetOverDraw(0, 0, 0, 0);
  void GetOverDraw(nuiSize& Left, nuiSize& Top, nuiSize& Right, nuiSize& Bottom, bool IncludeDecorations) const; ///< Fill the arguments with the amount of overdraw for each border of the widget
  nuiRect GetOverDrawRect(bool LocalRect, bool IncludeDecorations) const; ///< Return the overdraw rect (i.e. GetRect() + the overdraw values for each border). If LocalRect is true then the rect is relative to this widget's coordinates system (default). Otherwise, it's relative to this widget's parent coordinate system.
  void EnableInteractiveOverDraw(bool set); ///< If set is true then the widget will receive mouse events on its overdrawn borders.
  bool IsOverDrawInteractive() const; ///< Returns true if the widget is to receive mouse events on its overdrawn borders.
  void EnableInteractiveDecoration(bool set); ///< If set is true then the widget will receive mouse events on its overdrawn decoration borders.
  bool IsDecorationInteractive() const; ///< Returns true if the widget is to receive mouse events on its overdrawn borders.
  bool GetAutoClip() const;
  void SetAutoClip(bool set);
  //@}

  /** @name Incoming keyboard events */
  //@{
  virtual bool IsKeyDown (nglKeyCode Key) const;
  virtual bool DispatchTextInput(const nglString& rUnicodeText);
  virtual void DispatchTextInputCancelled();
  virtual bool DispatchKeyDown(const nglKeyEvent& rEvent, nuiKeyModifier Mask);
  virtual bool DispatchKeyUp(const nglKeyEvent& rEvent, nuiKeyModifier Mask);

  virtual void TextCompositionStarted(); ///< Tells the widget that a complex text input session is starting (mostly used to enter diacritics with dead keys and complex scripts like east asian glyphs)
  virtual void TextCompositionConfirmed(); ///< Tells the widget to confirm the composed text as final and end the composition session start with TextCompositionStart.
  virtual void TextCompositionCanceled(); ///< Tells the widget to cancel the composed text input and end the composition session started with TextCompositionStart.
  virtual void TextCompositionUpdated(const nglString& rString, int32 CursorPosition);
  virtual nglString GetTextComposition() const;
  virtual void TextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const;
  
  virtual bool TextInput(const nglString& rUnicodeText);
  virtual void TextInputCancelled();
  virtual bool KeyDown(const nglKeyEvent& rEvent);
  virtual bool KeyUp(const nglKeyEvent& rEvent);
  //@}

  /** @name Incomming mouse events */
  //@{
  virtual bool AcceptsMultipleGrabs() const { return false; }
  // These three methods receive the mouse coordinates in this object referential
  void EnableMouseEvents(bool enable);
  bool MouseEventsEnabled() const;
//  virtual bool MouseClicked  (nuiSize X, nuiSize Y, nglMouseInfo::Flags Button);
//  virtual bool MouseUnclicked(nuiSize X, nuiSize Y, nglMouseInfo::Flags Button);
//  virtual bool MouseCanceled (nuiSize X, nuiSize Y, nglMouseInfo::Flags Button);
//  virtual bool MouseMoved    (nuiSize X, nuiSize Y);
//  virtual bool MouseGrabbed();
//  virtual bool MouseUngrabbed();

  // Multitouch interface:
  virtual bool MouseClicked(const nglMouseInfo& rInfo);
  virtual bool MouseUnclicked(const nglMouseInfo& rInfo);
  virtual bool MouseCanceled(const nglMouseInfo& rInfo);
  virtual bool MouseMoved(const nglMouseInfo& rInfo);
  virtual bool MouseWheelMoved(const nglMouseInfo& rInfo);
  virtual bool MouseGrabbed(nglTouchId Id);
  virtual bool MouseUngrabbed(nglTouchId Id);
  virtual bool RequestStolenMouse(const nglMouseInfo& rInfo); ///< Return false if you want to refuse to the mouse grab be stolen by another widget
  virtual const std::map<nglTouchId, nglMouseInfo>& GetMouseStates() const;
  //@}

  /** @name Other Incomming events */
  //@{
  virtual void OnSetFocus(nuiWidgetPtr pWidget); ///< Advise the objet of a change of focus object. pWidget can be null.
  virtual void Built(); ///< This method is called right after a widget has been created by the CSS engine and all the CSS inits have been applied.
  //@}

  /** @name Misc. & incoming events management */
  //@{
  bool GetWantKeyboardFocus() const;
  void SetWantKeyboardFocus(bool Set); ///< A widget that wants the keyboard focus will receive it automatically whenever the user clicks them (only left and right button) or presses tab to change the focus.
  bool GetMuteKeyboardFocusDispatch() const;
  void SetMuteKeyboardFocusDispatch(bool Set); ///< A widget that mutes the keyboard focus will prevent its parent from gaining the focus.
  bool HasGrab();  ///< Ask if this widget has the mouse grab, or any touch grab
  bool HasGrab(nglTouchId TouchId);  ///< Ask if this widget has this touch grab \p TouchID.
  bool StealMouseEvent(const nglMouseInfo& rInfo); ///< Requests the grab concerning the given mouse info event, cancelling the use of this event in any currently grabbing widget. Returns if the grab was correctly acquired.

  bool Grab();  ///< Request the mouse grab
  bool Ungrab(); ///< Release the mouse grab
  bool HasFocus() const; ///< True if the object has the focus.
  bool Focus(); ///< Ask the system to put the keyboard focus on this widget so that it can receive keyboard events. 
  bool UnFocus(); ///< Ask the system to remove the keyboard focus from this widget so that it can not receive keyboard events anymore. 
  virtual bool SetMouseCursor(nuiMouseCursor Cursor); ///< Set the mouse cursor of the object (when the mouse is hovering on the object). The default cursor is eCursorArrow.
  void SetAttrMouseCursor(nuiMouseCursor Cursor); ///< same, but for the Cursor attributes
  nuiMouseCursor GetMouseCursor() const; ///< Get the mouse cursor of the object (when the mouse is hovering on the object). The default cursor is eCursorArrow.
  void SetClickThru(bool set); ///< If a widget is ClickThru it lets its parent widgets handle the mouse events it doens't want, otherwise it will intercept all mouse events. ClickThru behaviour is not handled is MouseClicked/Unclicked/Moved are redefined in a subclass.
  bool GetClickThru() const; ///< If a widget is ClickThru it lets its parent widgets handle the mouse events it doens't want, otherwise it will intercept all mouse events. ClickThru behaviour is not handled is MouseClicked/Unclicked/Moved are redefined in a subclass.
  //@}

  /** @name State & flags management */
  //@{
  bool GetHover() const; ///< Return true if the widget is in hover state (most of the time it is when the mouse hovers over the widget)
  virtual void SetHover(bool Hover); ///< Set the hover state of the object and its children. Most application don't need to use this method directly. Will generate an invalidation of the rendering.
  bool IsEnabled(bool combined = true) const;
	bool AttrIsEnabled();
  
  bool IsDisabled(bool combined = true) const
  {
    return !IsEnabled(combined);
  }
  bool IsSelected(bool combined = true) const;
  bool AttrIsSelected() const;
  virtual void LockState(); ///< Lock the state of the object. The state can't be changed until it is unlocked, also, the object will not combine its state with its parent's anymore.
  virtual void UnlockState(); ///< Unlock the object's state. See LockState() for for info. 
  void SilentSetVisible(bool Visible); ///< Change the visible flag of the widget but don't invalidate. This is useful if you are changing the visibility of a widget from a parent's SetRect method: you are already aware of the rendering and invalidation change.
  bool IsVisible(bool combined = true) const; ///< Return true if the object is visible. By default the returned state will be the state of the object combined with the states of its hierarchy recursively. If one of its parents is invisible the widget becomes invisible. If combine==false the returned value will be the local visible state of the object.
	bool AttrIsVisible();
  void SetSelectionExclusive(bool Exclusive); ///< When an object is set to exclusive selection it will deselect all its slibblings before selecting itself (for easy radio buttons and the like).
  bool IsSelectionExclusive() const; ///< Return true if the object is part of a selection group.
  void SetRedrawOnHover(bool RedrawOnHover); ///< Should the widget be redrawn on hover
  bool GetRedrawOnHover() const; ///< Return true if the widget is redrawn on hover

  virtual void SetPosition(nuiPosition mode); ///< Set the position of the widget inside its give rect. nuiTiled is not supported at the moment.
  virtual nuiPosition GetPosition() const; ///< Get the position where the widget will be layed out in the rectangle given by its parent container.
  virtual void SetFillRule(nuiPosition mode); ///< If the widget's position if nuiFill, this method sets the behaviour of the object when the drawing region doesn't respects the 1:1 ratio of the widget's ideal rect. 
  virtual nuiPosition GetFillRule() const; ///< If the widget's position if nuiFill, this method gets the behaviour of the object when the drawing region doesn't respects the 1:1 ratio of the widget's ideal rect. 
  //@}


  /** @name Generic tooltips */
  //@{
  bool SetToolTip(const nglString& rToolTip); ///< Change the text of the tooltip.
  nglString GetToolTip() const; ///< Retrieve the text of the tooltip.
  virtual bool ActivateToolTip(nuiWidgetPtr pWidget, bool Now = false); ///< The user doesn't need to use this method directly.
  virtual bool ReleaseToolTip(nuiWidgetPtr pWidget); ///< Tell the system to stop displaying this object's tooltip.
  //@}

  /** @name Drag and drop */
  //@{
  //  void CancelDrag(nuiDragObject* pDragObject); ///< A widget calls this method to stop an ongoing drag and drop operation with the designated object.
  virtual nglDropEffect OnCanDrop(nglDragAndDrop* pDragObject, nuiSize X, nuiSize Y); ///< This method is called by the window manager whenever the mouse is dragging an object over this widget. This method should test the object for supported drag and drop types and return true if one of them is supported or false otherwise.
  virtual void OnDropLeave(); ///< this method is called when a drop operation leave the last widget that was responding true to OnCanDrop 
  virtual void OnDropped(nglDragAndDrop* pDragObject, nuiSize X, nuiSize Y, nglMouseInfo::Flags Button); ///< This method is called by the window manager whenever the mouse is dropping an object on this widget. This method should retreive the dropped data from the object for supported drag and drop types. The mouse flag argument contains the mouse event that initiated the drop operation. X & Y are the coordinates of the mouse inside the destination widget.

  bool Drag(nglDragAndDrop* pDragObj); ///< This method is used to initiate a Drag operation from the widget, it has to allocate pDragObj with types the source widget can provide
  virtual void OnDragRequestData(nglDragAndDrop* pDragObject, const nglString& rMimeType); ///< This method is called on the drag and drop source widget by the window manager whenever the drag & drop operation was accepted by the user (by releasing the mouse button on a widget that support the dragged object type). This is the last time the source widget is allowed to place data in the drag and dropped object. 
  virtual void OnDragStop(bool canceled); ///< Called when a drag operation, initiated by the widget, is canceled or finished
  //@}

  /** @name Basic outgoing events */
  //@{
  nuiSimpleEventSource <nuiTrashed> Trashed; ///< Send an event when the object is trashed (if there is a TRASH animation this event is sent once the anim is done)
  nuiSimpleEventSource <nuiTrashed> TrashRequested; ///< Send an event when the object is trashed (if there is a TRASH animation this event is sent before it is started)
  nuiSimpleEventSource <nuiDestroyed> Destroyed; ///< Send an event when the object is destroyed
  nuiSimpleEventSource <nuiWidgetParentChanged> ParentChanged; ///< Send an event when the object is reparented

  nuiSimpleEventSource <nuiWidgetSelected    > Selected; ///< Send an event when the object is Selected
  nuiSimpleEventSource <nuiWidgetDeselected  > Deselected; ///< Send an event when the object is Deselected
  nuiSimpleEventSource <nuiWidgetDisabled    > Disabled; ///< Send an event when the object is disabled
  nuiSimpleEventSource <nuiWidgetEnabled     > Enabled; ///< Send an event when the object is disabled
  nuiSimpleEventSource <nuiWidgetStateChanged> StateChanged; ///< Send an event when the object change its state (Normal, Selected or Disabled)

  nuiSimpleEventSource <nuiWidgetShown> Shown; ///< Send an event when the object is shown
  nuiSimpleEventSource <nuiWidgetHiden> Hiden; ///< Send an event when the object is hidden
  nuiSimpleEventSource <nuiWidgetVisibilityChanged> VisibilityChanged; ///< Send an event when the object is either hiden or shown.

  nuiSimpleEventSource <nuiWidgetHoverOn     > HoverOn; ///< Send an event when the mouse is hovering on the object.
  nuiSimpleEventSource <nuiWidgetHoverOff    > HoverOff; ///< Send an event when the mouse stops hovering on the object.
  nuiSimpleEventSource <nuiWidgetHoverChanged> HoverChanged; ///< Send an event when the hovering state changes.

  nuiSimpleEventSource <nuiMoved> UserRectChanged; ///< Send an event when the user preferred rectangle changes.
  nuiSimpleEventSource <nuiMoved> HotRectChanged; ///< Send an event when the hot spot of the widget is moved.

  nuiSimpleEventSource <nuiFocus> FocusChanged; ///< Send an event when the focus has changed (gain or lost).

  nuiMouseClicked PreClicked; ///< Send an event when the widget is clicked. This event is fired before the MouseClicked method is called on the widget.
  nuiMouseUnclicked PreUnclicked; ///< Send an event when the widget is clicked. This event is fired before the MouseUnclicked method is called on the widget.
  nuiMouseUnclicked PreClickCanceled; ///< Send an event when the widget mouse interaction is canceled. This event is fired before the MouseCanceled method is called on the widget.
  nuiMouseMoved PreMouseMoved; ///< Send an event when the mouse moves over the widget. This event is fired before the MouseMoved method is called on the widget.
  nuiMouseMoved PreMouseWheelMoved; ///< Send an event when the mouse moves over the widget. This event is fired before the MouseMoved method is called on the widget.

  nuiMouseClicked Clicked; ///< Send an event when the widget is clicked. This event is fired after the MouseClicked method is called on the widget.
  nuiMouseUnclicked Unclicked; ///< Send an event when the widget is clicked. This event is fired after the MouseUnclicked method is called on the widget.
  nuiMouseUnclicked ClickCanceled; ///< Send an event when the widget is clicked. This event is fired after the MouseUnclicked method is called on the widget.
  nuiMouseMoved MovedMouse; ///< Send an event when the mouse moves over the widget. This event is fired after the MouseMoved method is called on the widget.
  nuiMouseMoved WheelMovedMouse; ///< Send an event when the mouse moves over the widget. This event is fired after the MouseMoved method is called on the widget.
  //@}

  /** @name Private event management system (do not override unless you know what you're doing!!!) */
  //@{
  virtual bool DispatchGrab(nuiWidgetPtr pWidget);
  virtual bool DispatchUngrab(nuiWidgetPtr pWidget);
  virtual bool DispatchHasGrab(nuiWidgetPtr pWidget);
  virtual bool DispatchHasGrab(nuiWidgetPtr pWidget, nglTouchId TouchId);

  //@}

  /** @name Simple Animation Support */
  //@{
  void StartAutoDraw(); ///< This method acquires the nuiAnimation timer that will ask that this widget be redrawn every animation tick by calling Invalidate() on it.
  void SetAutoDrawAnimateLayout(bool RecalcLayout = false); ///< If RecalcLayout is true the layout of the widget will be recalculated on every animation tick.
  bool GetAutoDrawAnimateLayout() const; ///< If is true the layout of the widget will be recalculated on every animation tick.
  void StopAutoDraw(); ///< This method stops animating the widget. 
  //@}


  /** @name Behavioral Animation Support */
  //@{
  nuiAnimation* GetAnimation(const nglString& rName); ///< Return a pointer to the animation with the given name
  void GetAnimations(std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>& rAnims); ///< Fill \param rAnims with the list of name/anim pairs of this widget.
  uint32 GetAnimCount(); ///< Return the number of animations supported by this widget.
  void AddAnimation(const nglString& rName, nuiAnimation* pAnim, bool TransitionAnimation = false); ///< Add an animation to this widget and associate the given name to it. Any existing animation associated with that name in this widget will be destroyed.
  void DelAnimation(const nglString& rName); ///< Delete the animation associated with the given name from this widget.
  void ClearAnimations(); ///< Remove all animations from this widget and delete them. 
  void AnimateAll(bool Run = true, bool Reset = true); ///< Run/Stop all animations in parallel. If \param Reset is true then every animation will be reset to the first frame.
  void ResetAnimation(const nglString& rName); ///< Set the given animation to it's first frame.
  void StartAnimation(const nglString& rName, double Time = 0.0); ///< Start playing the given animation from the given time.
  void StartAnimation(const nglString& rName, int32 count, nuiAnimLoop loopmode = eAnimLoopForward , double Time = 0.0); ///< Start playing the given animation from the given time.
  void StopAnimation(const nglString& rName); ///< Stop playing the given animation if it exists.
  nuiRectAttributeAnimation* GetLayoutAnimation(bool CreateIfNotAvailable);
  void SetLayoutAnimationDuration(float duration);
  float GetLayoutAnimationDuration();
  void SetLayoutAnimationEasing(const nuiEasingMethod& rMethod);
  //@}

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

  /** @name Render Cache */
  //@{
  nuiMetaPainter* GetRenderCache() const;
  //@}

  virtual void ConnectTopLevel(); ///< This method is called when the widget is connected to the Top Level. Overload it to perform specific actions in a widget.
  virtual void DisconnectTopLevel(); ///< This method is called when the widget is disconnected from the Top Level. Overload it to perform specific actions in a widget.

  
  /** @name Drawing */
  //@{
//  void SetFillColor(nuiDrawContext* pContext, nuiWidgetElement Element);
//  void SetStrokeColor(nuiDrawContext* pContext, nuiWidgetElement Element);
//
//  void SetColor(nuiWidgetElement element, const nuiColor& rColor);
//  void DelColor(nuiWidgetElement element);
//  nuiColor GetColor(nuiWidgetElement element);
//  void SetTheme(nuiTheme* pTheme);
//  nuiTheme* GetTheme();

  void AutoDestroy(const nuiEvent& rEvent); ///< This method will destroy the widget whenever it is called.
  bool IsTrashed(bool combined = true) const;

  void AutoStartTransition(const nuiEvent& rEvent); ///< This method will increment the transition state of this widget.
  void AutoStopTransition(const nuiEvent& rEvent);  ///< This method will decrement the transition state of this widget.

  bool GetNeedLayout() const { return mNeedSelfLayout; }
  bool GetNeedSelfRedraw() const { return mNeedSelfRedraw; }
  bool GetNeedIdealRect() const { return mNeedIdealRect; }

  virtual float GetScale() const; ///< Return current HDPI scaling
  virtual float GetScaleInv() const; ///< Return 1.0/GetScale()
  //@}
  
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

    nuiSize mMaxWidth = std::numeric_limits<float>::max();
    nuiSize mMaxHeight = std::numeric_limits<float>::max();
    nuiSize mMinWidth = 0;
    nuiSize mMinHeight = 0;
  };

  virtual bool SetLayoutConstraint(const LayoutConstraint& rConstraint); ///< Change the layout constraint imposed by this widget's parent. Return false if the constraint haven't changed, true otherwise.
  const LayoutConstraint& GetLayoutConstraint() const; ///< Retrieve the layout constraint imposed by this widget's parent.
  //@}

  /** @name Debug Tags */
  //@{
  void SetDebug(int32 DebugLevel);
  int32 GetDebug(bool recursive = false) const;
  //@}
  
  /** @name Hot Keys */
  //@{
  nuiHotKey* RegisterHotKeyKey(const nglString& rName, nglKeyCode Trigger, nuiKeyModifier Modifiers, bool Priority = false, bool FireOnKeyUp = false, const nglString& rDescription = nglString::Empty); ///< Register a new HotKey for this widget returns a new HotKey if rName is free in Top Level, an existing hotKey otherwise
  nuiHotKey* RegisterHotKeyChar(const nglString& rName, nglChar Trigger, nuiKeyModifier Modifiers, bool Priority = false, bool FireOnKeyUp = false, const nglString& rDescription = nglString::Empty); ///< Register a new HotKey for this widget returns a new HotKey if rName is free in Top Level, an existing hotKey otherwise
  nuiHotKey* GetHotKey(const nglString& rName); ///< Returns the hotkey associated to this name first locally then at top level, NULL if none exists
  nuiSimpleEventSource<nuiWidgetActivated>& GetHotKeyEvent(const nglString& rName); ///< Returns the Event associated to this name (local to widget). NB: No safeguards, only an assertion. Use GetHotKey to check if hotkey is registered locally
  void DelHotKey(const nglString& rName); ///< Removes the event in local map, asks for deletion at top level
  void SetHotKeyMask(nuiKeyModifier Mask);
  nuiKeyModifier GetHotKeyMask() const;
  //@}

  /** @name Generic Public Events */
  //@{
  nuiEventSource* GetEvent(const nglString& rName);
  void GetEvents(std::vector<nglString>& rNames);
  bool AddEventAction(const nglString& rEventName, nuiEventActionHolder* pActions);
  //@}

  /** @name Decorations */
  //@{
  static void ClearDefaultDecorations();
  static void SetDefaultDecoration(int32 objectClassIndex, nuiDecorationDelegate dlg);
  virtual void SetDecoration(const nglString& rName);
  virtual void SetDecoration(nuiDecoration* pDecoration, nuiDecorationMode Mode = eDecorationOverdraw, bool AlreadyAcquired=false);
  void SetDecorationMode(nuiDecorationMode Mode = eDecorationOverdraw);
  nuiDecoration* GetDecoration() const;
  const nglString& GetDecorationName() const;
  nuiDecorationMode GetDecorationMode() const;
  void EnableDecoration(bool set);
  bool IsDecorationEnabled() const;
  //@}
  
  /** @name Keyboard Focus Decorations */
  //@{
  virtual void SetFocusDecoration(const nglString& rName);
  void SetFocusDecoration(nuiDecoration* pDecoration, nuiDecorationMode Mode = eDecorationOverdraw);
  void SetFocusDecorationMode(nuiDecorationMode Mode = eDecorationOverdraw);
  nuiDecoration* GetFocusDecoration() const;
  const nglString& GetFocusDecorationName() const;
  nuiDecorationMode GetFocusDecorationMode() const;
  void SetFocusVisible(bool set);
  static void SetFocusVisibleDefault(bool set);
  bool IsFocusVisible() const;
  //@}
  
  /** @name CSS (Cascading style sheets) */
  //@{
  void IncrementCSSPass();
  void ResetCSSPass();
  uint32 GetCSSPass() const;
  //@}

  /** @name Layers */
  //@{
  void SetLayerPolicy(nuiDrawPolicy policy);
  nuiDrawPolicy GetLayerPolicy() const;
  bool GetDrawToLayer() const;
  nuiLayer* GetLayer() const;
  nuiWidget* GetParentLayerWidget() const;
  nuiLayer* GetParentLayer() const;
  //@}


  NUI_GETSETDO(bool, ReverseRender, Invalidate());
  NUI_GETSET(bool, AutoAcceptMouseCancel);
  NUI_GETSET(bool, AutoAcceptMouseSteal);
  NUI_GETSETDO(bool, ForceNoDrawToLayer, BroadcastForceNoDrawToLayer());

protected:
  virtual ~nuiWidget();

  void BroadcastForceNoDrawToLayer();
    
  std::map<nglString, nuiEventSource*, nglString::LessFunctor> mEventMap;
  std::vector<nuiEventActionHolder*> mEventActions;

  void AddEvent(const nglString& rName, nuiEventSource& rEvent);
  virtual void BroadcastInvalidate(nuiWidgetPtr pSender);
  virtual void BroadcastInvalidateRect(nuiWidgetPtr pSender, const nuiRect& rRect);
  virtual void BroadcastInvalidateLayout(nuiWidgetPtr pSender, bool BroadCastOnly);
  virtual void CallConnectTopLevel(nuiTopLevel* pTopLevel); ///< This method is called when the widget is connected to the Top Level. Overload it to perform specific actions in a widget.
  virtual void CallDisconnectTopLevel(nuiTopLevel* pTopLevel); ///< This method is called when the widget is disconnected from the Top Level. Overload it to perform specific actions in a widget.
  bool InternalDrawWidget(nuiDrawContext* pContext, const nuiRect& _self, const nuiRect& _self_and_decorations, bool ApplyMatrix);
  void AddInvalidRect(const nuiRect& rRect);

  virtual void CallOnTrash(); ///< This method is called whenever a Trash occurred on the widget tree branch. It performs some clean up and then calls nuiWidget::OnTrash to enable the user to handle the trash.

  nuiWidgetPtr mpParent = nullptr;
  nuiTopLevel* mpTopLevel = nullptr;

  float mAlpha = 1.0f; ///< Indicates the transparency level of the object. Optional.
  nuiRect mRect; ///< The bounding box of the nuiWidget (in coordinates of its parent).
  nuiRect mLayoutRect; ///< The rect given by the parent (may be different than mRect)
  nuiRect mVisibleRect; ///< The active bounding box of the nuiObject (in local coordinates).
  nuiRect mIdealRect; ///< The ideal bounding box of the nuiObject (in coordinates of its parent) position should be at the origin.
  nuiRect mUserRect; ///< The bounding box of the nuiObject if set by the user (in coordinates of its parent).
  nuiRect mHotRect; ///< The currently important interactive part of the widget. Containers try to keep this rect in view when possible. For exemple set it as the cursor rectangle in a text edit widget. Is you text edit is contained in a scroll view, the scroll view will try to follow the cursor.

  nuiSize mBorderLeft = 0, mBorderRight = 0; // empty space left left and right the widget itself
  nuiSize mBorderTop = 0, mBorderBottom = 0; // empty space left below and above the widget itself

  nuiMouseCursor mObjCursor = eCursorArrow;

  nuiEventSink<nuiWidget> mGenericWidgetSink;

  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor> mAnimations;

  static nuiMatrix mIdentityMatrix;
  std::vector<nuiMatrixNode*>* mpMatrixNodes;
  nuiMatrix _GetMatrix() const;
  void _SetMatrix(nuiMatrix Matrix);

  
  void Animate(const nuiEvent& rEvent);
  
  bool TriggerHotKeys(const nglKeyEvent& rEvent, bool KeyDown,  bool Priority, nuiKeyModifier Mask); ///< Helper function. Triggers appropriate hotkeys
  nuiKeyModifier mHotKeyMask = 0;

  std::map<nuiWidgetElement, nuiColor> mWidgetElementColors;

  LayoutConstraint mConstraint;

  void ApplyCSSForStateChange(uint32 MatchersTag); ///< This method will match this widget's state with the CSS and apply the changes needed to display it correctly

  virtual void DrawFocus(nuiDrawContext* pContext, bool FrontOrBack); ///< Draw a decoration to show that the widget has the keyboard focus. The focus is drawn on top of the regular decoration if it exists.
  void DispatchFocus(nuiWidgetPtr pWidget); ///< Advise the objet of a change of focus object. pWidget can be null.

  static bool mShowFocusDefault;


  bool mAnimateLayout : 1;
  bool mRedrawOnHover : 1;
  bool mMixAlpha : 1;
  bool mInheritAlpha : 1;
  bool mVisible : 1;
  bool mDrawingInCache : 1;
  bool mNeedInvalidateOnSetRect : 1;
  bool mInteractiveOD : 1;
  bool mInteractiveDecoration : 1;
  bool mShowFocus : 1;
  bool mHasFocus: 1; ///< True if the widget has the focus.
  bool mHasUserPos: 1;
  bool mHasUserSize: 1;
  bool mHasUserWidth: 1;
  bool mHasUserHeight: 1;
  bool mForceIdealSize: 1;
  bool mSelectionExclusive: 1;
  bool mSelected: 1;
  bool mEnabled: 1;
  bool mStateLocked: 1;
  bool mHover: 1;
  
  bool mDecorationEnabled: 1;
  bool mNeedSelfLayout: 1; ///< Layout of this widget has been invalidated
  bool mNeedLayout: 1; ///< Layout has been invalidated in this branch of the widget tree. This doesn't mean that this particularobject needs to recalc its layout, it may come from one of its children.
  bool mNeedIdealRect: 1;
  bool mNeedSelfRedraw: 1;
  
  bool mMouseEventEnabled: 1;
  bool mWantKeyboardFocus: 1;
  bool mMuteKeyboardFocusDispatch: 1;
  bool mTrashed: 1;
  bool mDoneTrashed: 1;
  bool mCanRespectConstraint: 1;
  bool mInSetRect: 1;
  bool mAutoClip: 1;
  bool mAutoDraw: 1;
  bool mFixedAspectRatio: 1;
  bool mReverseRender: 1;
  bool mOverrideVisibleRect : 1;
  bool mAutoUpdateLayout : 1;
  bool mAutoAcceptMouseCancel : 1;
  bool mAutoAcceptMouseSteal : 1;
  bool mForceNoDrawToLayer : 1;
  bool mCurrentlyOnScreen : 1;


  bool mClickThru: 1;

  int32 mInTransition = 0;
  
  void InitDefaultValues();
  void Init(); ///< Initialise the basic parameters of the class.

  nuiRef<nuiMetaPainter> mpRenderCache;
  bool mRenderCacheIsEmpty = true;

  uint32 mDebugLevel = 0;
  
  float mMaxIdealWidth = std::numeric_limits<float>::max();
  float mMaxIdealHeight = std::numeric_limits<float>::max();
  float mMinIdealWidth = 0;
  float mMinIdealHeight = 0;

  float mMaxWidth = std::numeric_limits<float>::max();
  float mMaxHeight = std::numeric_limits<float>::max();
  float mMinWidth = 0;
  float mMinHeight = 0;

  friend class nuiTopLevel;
  friend class nuiWidgetInfo;
	
	std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*> mHotKeyEvents;

  nuiSize mODLeft = 0;
  nuiSize mODRight = 0;
  nuiSize mODTop = 0;
  nuiSize mODBottom = 0;
  
  // Normal decoration:
  nuiDecoration* mpDecoration = nullptr;
  nuiDecorationMode mDecorationMode = eDecorationOverdraw;

  // Focus decoration:
  nuiDecoration* mpFocusDecoration = nullptr;
  nuiDecorationMode mFocusDecorationMode = eDecorationOverdraw;
  
  static std::vector<nuiDecorationDelegate> mDefaultDecorations;



  void AutoHide(const nuiEvent& rEvent); ///< This methods calls SetVisible(false) right after the HIDE animation stopped
  void AutoInvalidate(const nuiEvent& rEvent); ///< This methods calls Invalidate()
  void AutoInvalidateLayout(const nuiEvent& rEvent); ///< This methods calls InvalidateLayout()
  
  void InternalSetLayout(const nuiRect& rect);
  virtual void InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged);
  
  nuiPosition mPosition = nuiFill;
  nuiPosition mFillRule = nuiFill;
  nuiRectAttributeAnimation* mpLayoutAnimation = nullptr;
  
  uint32 mCSSPasses = 0;
  virtual void InternalResetCSSPass();
  
  std::vector<nuiRect> mDirtyRects;
  
  int32 _GetDebug() const;
  void OnFinalize();

  void DrawChild(nuiDrawContext* pContext, nuiWidget* pChild); /// Draw the given widget as a child of this widget.
  bool DrawChildren(nuiDrawContext* pContext); ///< This helper method calls the DrawMethod on all the children of the nuiObject using the DrawChild method.
  void BroadcastVisible();

  void ChildrenCallOnTrash();

  nuiWidgetList mpChildren;
  nuiLayer* mpBackingLayer = nullptr;
  nuiDrawPolicy mLayerPolicy = nuiDrawPolicyDrawNone;

  void UpdateTopLevel();  
  void InternalSetLayerPolicy(nuiDrawPolicy policy);


};

#define NUI_ADD_EVENT(NAME) { AddEvent(_T(#NAME), NAME); }

