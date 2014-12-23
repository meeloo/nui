/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiTabView_h__
#define __nuiTabView_h__

#include "nuiWidget.h"

class nuiGradient;
class nuiFont;
class nuiLabel;

class TabEvent : public nuiEvent
{
public:
  TabEvent(int32 index);
  int32 mTabIndex;
};



class nuiTabView : public nuiWidget
{
private:
  class Tab;
public:
  nuiTabView(nuiPosition tabPosition = nuiTop, bool decoratedBackground = true);

  virtual nuiRect CalcIdealSize();
  virtual bool SetRect(const nuiRect& rRect);
  virtual bool Draw(nuiDrawContext* pContext);
  
  virtual bool MouseClicked   (const nglMouseInfo& rInfo);
  virtual bool MouseUnclicked (const nglMouseInfo& rInfo);
    
  void OnIconClicked(const nuiEvent& rEvent);
  void OnTabEnterDrag(const nuiEvent& rEvent);

  virtual int32 AddTab(const nglString& rTitle, nuiWidget* pContents);
  virtual int32 AddTab(nuiWidget* pTitle, nuiWidget* pContents);
  virtual void InsertTab(nuiWidget* pTitle, nuiWidget* pContents, int32 position);
  virtual void RemoveTab(nuiWidget* pTab);
  virtual void RemoveTab(int32 tab_index);
   
  int32 GetTabCount();

  nuiPosition GetTabPosition();
  
  nuiOrientation GetOrientation();
  void SetTabPosition(nuiPosition tabPosition);

  void SelectTab(int32 index);
  void SelectTabByContents(nuiWidget* pContents);
  void SelectTabByHeader(nuiWidget* pHeader);
  int32 GetTabIndexByContents(nuiWidget* pContents) const;
  int32 GetTabIndexByHeader(nuiWidget* pHeader) const;
  int32 GetSelectedTab() const { return mCurrentTabIndex; }
  void EnableChildrenRectUnion(bool set);
  bool GetChildrenRectUnion() const;

  void SetFoldable(bool Foldable);
  bool GetFoldable() const;

  void SetFolded(bool set, bool Animate = true);
  bool GetFolded() const;
  
  void SetChangeOnDrag(bool change);
  bool GetChangeOnDrag() const;

  nuiEventSink<nuiTabView> mTabViewEvents;
  nuiSimpleEventSource<0> TabSelect;
protected:
  virtual ~nuiTabView();

  int32 mCurrentTabIndex;
  nuiPosition mTabPosition;

  std::vector<nuiWidget*> mTabs;
  std::vector<Tab*> mIcons;
  nuiRect mIdealIconsRect;
  nuiRect mIdealTabsRect;

  bool mChangeOnDrag;
  bool mChildrenRectUnion;
  bool mFoldable;
  bool mFolded;
  bool mSliding;
  bool mDecoratedBackground;
  float mFoldRatio;
  
private:
  class Tab : public nuiWidget
  {
  public:
    Tab(nuiWidget* pWidget);
    virtual nglDropEffect OnCanDrop(nglDragAndDrop* pDragObject, nuiSize X, nuiSize Y);
    nuiSimpleEventSource<0> EnterDrag;

  protected:
    ~Tab() {}
  };
};

#endif//__nuiTabView_h__

