/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiContainer_h__
#define __nuiContainer_h__

#include "nuiWidget.h"
#include "nuiEvent.h"
#include "nuiTreeEvent.h"

typedef nuiTreeEventSource<nuiChildAdded, nuiWidget> nuiWidgetAddedEventSource;
typedef nuiTreeEventSource<nuiChildDeleted, nuiWidget> nuiWidgetDeletedEventSource;

class nuiContainer : public nuiWidget
{
  friend class nuiWidget;
  friend class nuiTopLevel;
public:
  class Iterator
  {
  public:
    Iterator(nuiContainer* pContainer, bool DoRefCounting = false);
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
    nuiContainer* mpSource;
    bool mValid;
    bool mRefCounting;
  };

  class ConstIterator
  {
  public:
    ConstIterator(const nuiContainer* pContainer, bool DoRefCounting = false);
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
    const nuiContainer* mpSource;
    bool mValid;
    bool mRefCounting;
  };

  typedef Iterator* IteratorPtr;
  typedef ConstIterator* ConstIteratorPtr;

  /** @name Life */
  //@{
  nuiContainer(); ///< Create an nuiObject and give him his parent.
  virtual ~nuiContainer(); 
  //@}

  virtual bool AddChild(nuiWidgetPtr pChild);
  virtual bool DelChild(nuiWidgetPtr pChild); ///< Remove this child from the object. If Delete is true then the child will be deleted too. Returns true if success.
  virtual int GetChildrenCount() const;
  virtual nuiWidgetPtr GetChild(int index); ///< Returns the child which has the given index (first child = 0). Return NULL in case of faillure.
  virtual nuiWidgetPtr GetChild(nuiSize X, nuiSize Y); ///< Returns the child which is under the pixel (X,Y) in this object or this if there is no such child. X and Y are given in the coordinate system of the parent object.
  virtual nuiWidgetPtr GetChild(const nglString& rName, bool ResolveNameAsPath = true); ///< Find a child by its name property. Try to resolve path names like /window/fixed/toto or ../../tata if deepsearch is true
  virtual bool Clear();///< Clear all children. By default the children are deleted unless Delete == false.

  virtual nuiContainer::Iterator* GetFirstChild(bool DoRefCounting = false);
  virtual nuiContainer::ConstIterator* GetFirstChild(bool DoRefCounting = false) const;
  virtual nuiContainer::Iterator* GetLastChild(bool DoRefCounting = false);
  virtual nuiContainer::ConstIterator* GetLastChild(bool DoRefCounting = false) const;
  virtual bool GetNextChild(nuiContainer::IteratorPtr pIterator);
  virtual bool GetNextChild(nuiContainer::ConstIteratorPtr pIterator) const;
  virtual bool GetPreviousChild(nuiContainer::IteratorPtr pIterator);
  virtual bool GetPreviousChild(nuiContainer::ConstIteratorPtr pIterator) const;

  virtual void RaiseChild(nuiWidgetPtr pChild);
  virtual void LowerChild(nuiWidgetPtr pChild);
  virtual void RaiseChildToFront(nuiWidgetPtr pChild);
  virtual void LowerChildToBack(nuiWidgetPtr pChild);


  /** @name Object relation management */
  //@{
  virtual void GetChildren(nuiSize X, nuiSize Y, nuiWidgetList& rChildren, bool DeepSearch = false); /// Return all the children under the pixel (X, Y) in this container.
  
  nuiWidgetPtr GetChildIf(nuiSize X, nuiSize Y, TestWidgetFunctor* pFunctor); ///< Returns the child that satisfies the given functor object and that is under the pixel (X,Y) in this object or this if there is no such child. X and Y are given in the coordinate system of the parent object. rFunctor is a std::unary_functor<nuiWidgetPtr, bool> object defined by the user.
  nuiWidgetPtr SearchForChild(const nglString& rName, bool recurse = true);  ///< Find a child by its name property, recurse the search in the subchildren if asked politely. 
  nuiContainerPtr GetRoot() const;
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
  virtual void SilentInvalidateChildren(bool Recurse);
  virtual bool Draw(nuiDrawContext* pContext);
  virtual nuiRect CalcIdealSize();
  virtual bool SetRect(const nuiRect& rRect);
  virtual void SetAlpha(float Alpha);
  virtual void SetEnabled(bool set);
  virtual void SetSelected(bool set);
  virtual bool Trash();

  virtual void OnChildHotRectChanged(nuiWidget* pChild, const nuiRect& rChildHotRect);
  virtual void SetVisible(bool Visible); ///< Show or hide the widget
  //@}

  /** @name Private event management system (do not override unless you know what you're doing!!!) */
  //@{
  virtual bool PreMouseClicked(const nglMouseInfo& rInfo);
  virtual bool PreMouseUnclicked(const nglMouseInfo& rInfo);
  virtual bool PreMouseMoved(const nglMouseInfo& rInfo);
  virtual bool PreMouseWheelMoved(const nglMouseInfo& rInfo);
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

protected:
  
  
  void DrawChild(nuiDrawContext* pContext, nuiWidget* pChild); /// Draw the given widget as a child of this widget.
  bool DrawChildren(nuiDrawContext* pContext); ///< This helper method calls the DrawMethod on all the children of the nuiObject using the DrawChild method. 
  void BroadcastVisible();
  virtual void CallConnectTopLevel(nuiTopLevel* pTopLevel); ///< Connects container to Top Level, then asks children to connect.
  virtual void CallDisconnectTopLevel(nuiTopLevel* pTopLevel); ///< Disconnects container from Top Level, then asks children to disconnect too.
  virtual void CallOnTrash();
  void ChildrenCallOnTrash();
  virtual void InternalResetCSSPass();
  void InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged);

  nuiWidgetList mpChildren;

};

#endif // __nuiContainer_h__
