/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nui.h"

#define NUI_OBJECT_MATCHER_PARENT 0
#define NUI_OBJECT_MATCHER_PARENTCONDITION 0
#define NUI_OBJECT_MATCHER_JOKER 0
#define NUI_OBJECT_MATCHER_STATIC 0
#define NUI_OBJECT_MATCHER_CLASS 2
#define NUI_OBJECT_MATCHER_NAME 3
#define NUI_OBJECT_MATCHER_STATE 4
#define NUI_OBJECT_MATCHER_ATTRIBUTE 5

#define NUI_WIDGET_MATCHTAG_NONE 0
#define NUI_WIDGET_MATCHTAG_BLOCK 0
#define NUI_WIDGET_MATCHTAG_ALL ((uint32)-1)
#define NUI_WIDGET_MATCHTAG_PARENT              (1 < NUI_OBJECT_MATCHER_PARENT)
#define NUI_WIDGET_MATCHTAG_PARENTCONDITION     (1 < NUI_OBJECT_MATCHER_PARENTCONDITION)
#define NUI_WIDGET_MATCHTAG_JOKER               (1 < NUI_OBJECT_MATCHER_JOKER)
#define NUI_WIDGET_MATCHTAG_STATE               (1 < NUI_OBJECT_MATCHER_STATE)
#define NUI_WIDGET_MATCHTAG_CLASS               (1 < NUI_OBJECT_MATCHER_CLASS)
#define NUI_WIDGET_MATCHTAG_NAME                (1 < NUI_OBJECT_MATCHER_NAME)
#define NUI_WIDGET_MATCHTAG_ATTRIBUTE           (1 < NUI_OBJECT_MATCHER_ATTRIBUTE)

/**
 This is a very simple parent class for all widget matchers: there is only one virtual method (Match) that returns true if
 the argument matches the given widget
 **/
class nuiObjectMatcher
{
public:
  nuiObjectMatcher(bool StateMatcher)
  : mStateMatcher(StateMatcher)
  {
    mPriority = 0; // By default we don't permit to skip and optimize any matcher, just to be safe
  }
  
  virtual ~nuiObjectMatcher()
  {
  }
  
  virtual bool Match(nuiObject*& pWidget) = 0;
  
  bool IsStateMatcher() const
  {
    return mStateMatcher;
  }
  
  int32 GetPriority() const
  {
    return mPriority;
  }

  uint32 GetTag() const
  {
    return 1 << mPriority;
  }
protected:
  bool mStateMatcher;
  int32 mPriority;
};

class nuiObjectJokerMatcher : public nuiObjectMatcher // Match all the widgets
{
public:
  nuiObjectJokerMatcher()
  : nuiObjectMatcher(false)
  {
    mPriority = NUI_OBJECT_MATCHER_JOKER;
  }
  
  bool Match(nuiObject*& pWidget)
  {
    return true;
  }
};

class nuiObjectParentMatcher : public nuiObjectMatcher // Change the reference widget to its parent
{
public:
  nuiObjectParentMatcher()
  : nuiObjectMatcher(false)
  {
    mPriority = NUI_OBJECT_MATCHER_PARENT;
  }
  
  bool Match(nuiObject*& pObject)
  {
    nuiLayoutBase* pWidget = dynamic_cast<nuiLayoutBase*>(pObject);
    if (pWidget)
    {
      pWidget = pWidget->GetParentLayout();
    }
    return pWidget != NULL;
  }
};

class nuiObjectParentConditionMatcher : public nuiObjectMatcher // Change the reference widget to its parent
{
public:
  nuiObjectParentConditionMatcher(nuiObjectMatcher* pCondition)
  : nuiObjectMatcher(false)
  {
    mPriority = NUI_OBJECT_MATCHER_PARENTCONDITION;
    mpCondition.push_back(pCondition);
  }
  
  nuiObjectParentConditionMatcher(const std::vector<nuiObjectMatcher*> rMatchers)
  : nuiObjectMatcher(false),
    mpCondition(rMatchers)
  {
    mPriority = NUI_OBJECT_MATCHER_PARENTCONDITION;
  }

  bool Match(nuiObject*& pWidget)
  {
    int32 i = (int32)mpCondition.size() - 1;
    while (i >= 0)
    {
      if (!MatchOne(mpCondition[i], pWidget))
        return false;
      i--;
    }
    
    return true;
  }
  
protected:
  bool MatchOne(nuiObjectMatcher* pCondition, nuiObject*& pObject)
  {    
    while (pObject)
    {
      if (pCondition->Match(pObject))
        return true;

      nuiLayoutBase* pWidget = dynamic_cast<nuiWidget*>(pObject);
      if (pWidget)
      {
        pObject = pWidget->GetParentLayout();
      }
      else
      {
        pObject = nullptr;
      }
    }
    
    pObject = nullptr;
    return false;
  }
  
  std::vector<nuiObjectMatcher*> mpCondition;
};

class nuiObjectClassMatcher : public nuiObjectMatcher
{
public:
  nuiObjectClassMatcher(const nglString& rClass)
  : nuiObjectMatcher(false), mClass(rClass)
  {
    mPriority = NUI_OBJECT_MATCHER_CLASS;
    mClassIndex = nuiObject::GetClassNameIndex(rClass);
  }
  
  virtual bool Match(nuiObject*& pWidget)
  {
    return pWidget->IsOfClass(mClassIndex);
  }
  
protected:
  nglString mClass;
  uint32 mClassIndex;
};

class nuiObjectNameMatcher : public nuiObjectMatcher
{
public:
  nuiObjectNameMatcher(const nglString& rName)
  : nuiObjectMatcher(false), mName(rName)
  {
    mPriority = NUI_OBJECT_MATCHER_NAME;
  }
  
  virtual bool Match(nuiObject*& pWidget)
  {
    return pWidget->GetObjectName() == mName;
  }
  
protected:
  nglString mName;
};

class nuiObjectAttributeMatcher : public nuiObjectMatcher
{
public:
  nuiObjectAttributeMatcher(const nglString& rAttribute, const nglString& rValue, bool CaseSensitive, bool PartialMatch)
  : nuiObjectMatcher(true), mAttribute(rAttribute), mValue(rValue), mCaseSensitive(CaseSensitive), mPartialMatch(PartialMatch)
  {
    mPriority = NUI_OBJECT_MATCHER_ATTRIBUTE;
  }
  
  virtual bool Match(nuiObject*& pWidget)
  {
    const nuiAttribBase Attribute = pWidget->GetAttribute(mAttribute);
    if (!Attribute.IsValid())
      return false;
    
    nglString value;
    Attribute.ToString(value);
    if (value.IsNull())
      return false;
    
    if (mPartialMatch)
    {
      return value.Contains(mValue, mCaseSensitive) != 0;
    }
    
    return value.Compare(mValue, mCaseSensitive) == 0;
  }
  
protected:
  nglString mAttribute;
  nglString mValue;
  bool mCaseSensitive;
  bool mPartialMatch;
};

class nuiObjectStaticMatcher : public nuiObjectMatcher // Match all the Objects that have never been thru the CSS engine
{
public:
  nuiObjectStaticMatcher()
  : nuiObjectMatcher(false)
  {
    mPriority = NUI_OBJECT_MATCHER_STATIC;
  }
  
  bool Match(nuiObject*& pObject)
  {
    nuiLayoutBase* pWidget = dynamic_cast<nuiLayoutBase*>(pObject);
    if (pWidget)
      return pWidget->GetCSSPass() == 0;
    return true;
  }
};

class nuiGlobalVariableMatcher : public nuiObjectMatcher // Match if the global variable has the given value
{
public:
  nuiGlobalVariableMatcher(const nglString& rVariable, const nglString& rValue)
  : nuiObjectMatcher(false),
    mVariable(rVariable),
    mValue(rValue)
  {
    mPriority = NUI_OBJECT_MATCHER_STATE;
  }
  
  bool Match(nuiObject*& pWidget)
  {
    return nuiObject::GetGlobalProperty(mVariable) == mValue;
  }
  
private:
  nglString mVariable;
  nglString mValue;
};





