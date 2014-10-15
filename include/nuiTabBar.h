/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiTabBar_h__
#define __nuiTabBar_h__

//#include "nui.h"
#include "nglString.h"
#include "nuiWidget.h"
#include "nuiButton.h"

class nuiTab : public nuiButton
{
public:
  nuiTab(); 
  nuiTab(const nglString & text);

  virtual ~nuiTab();

  void SetIdentifier(int i) { mIdentifier = i; }
  int GetIdentifier() const { return mIdentifier; }

  // Rendering:
  virtual bool Draw(nuiDrawContext* pContext);

protected:
  int mIdentifier;
};


class nuiTabsSelectedEvent : public nuiEvent
{
public:
  nuiTabsSelectedEvent(const std::list<nuiTab *> &selectedTabs)
    : nuiEvent(nuiClicked), mSelectedTabs(selectedTabs)
  {
  }
  std::list<nuiTab*> mSelectedTabs;
};

class nuiTabsSelected : public nuiEventSource
{
public:
  nuiTabsSelected()
  {
  }

  virtual bool operator() (const std::list<nuiTab *> &selectedTabs)
  {
    return SendEvent(nuiTabsSelectedEvent(selectedTabs));
  }
};

class nuiTabBar : public nuiWidget
{
public:
  nuiTabBar(); ///< Create a tab bar
  virtual ~nuiTabBar();

  virtual int AddTab(nuiTab * pNewTab);
  virtual int InsertTab(nuiTab * pNewTab, int index = -1);
  virtual void RemoveTab (nuiTab * pTab);

  const std::list<nuiTab*> & GetCurrentTabs() const { return mSelectedTabs; }

  nuiTab *GetTabAt(int index) const;
  int Count() const;

  void SelectTab(int id);

  virtual bool Draw(nuiDrawContext* pContext);
  virtual nuiRect CalcIdealSize();
  bool SetRect(const nuiRect& rRect);

  // Public events:
  nuiTabsSelected TabsSelected;

protected:
  virtual void OnTabSelected(const nuiEvent& rEvent);

protected:
  std::vector<nuiTab*> mTabs;
  std::list<nuiTab*> mSelectedTabs;
  nuiEventSink<nuiTabBar> mTabsSink;

};

#endif // __nuiTabBar_h__
