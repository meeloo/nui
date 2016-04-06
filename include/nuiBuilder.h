/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

//#include "nui.h"
#include "nglString.h"
#include "nuiWidget.h"

class nuiBuilder;
class nuiEventActionHolder;

typedef nuiWidgetPtr (*nuiCreateWidgetFn)();

class nuiWidgetDesc
{
public:
  nuiWidgetDesc(){};
  nuiWidgetDesc(const nglString& rClassName, nuiCreateWidgetFn pHandler)
  {
    mClassName = rClassName;
    mpHandler = pHandler;
  };
  
  ~nuiWidgetDesc(){};

  const nglString& GetClassName() const
  {
    return mClassName;
  };
  
  nuiCreateWidgetFn GetHandler() const
  {
    return mpHandler;
  };
  
  
  
private : 

  nglString mClassName;
  nuiCreateWidgetFn mpHandler;
};


typedef std::map<nglString, nuiWidgetDesc, nglString::LessFunctor> nuiBuilderMap;


class nuiWidgetCreator;
typedef std::map<nglString, nuiWidgetCreator*, nglString::LessFunctor> nuiWidgetCreatorMap;

///////////////////////////////////////////////////////////////////

class nuiBuilder
{
public:
  void SetHandler(const nglString& ClassName, nuiCreateWidgetFn pHandler); ///< This method permits to add or override a widget creation function.
  void SetHandler(const nglString& ClassName, nuiWidgetCreator* pCreator); ///< This method permits to add or override a widget creation function.
  nuiCreateWidgetFn GetHandler(const nglString& ClassName) const; ///< This method retrieves thewidget creation function associated with a class name.
  bool GetClassList(std::list<nuiWidgetDesc>& rClassNames) const; ///< This method fills the given nuiWidgetDesc list with the description (name) of the classes that this map can handle. 

  nuiWidget* CreateWidget(const nglString& rClassName) const;
  nuiWidget* CreateWidget(const nglString& rClassName, const std::map<nglString, nglString>& rParamDictionary) const;

  static nuiWidget* Build(const nglString& rClassName)
  {
    return nuiBuilder::Get().CreateWidget(rClassName);
  }

  static nuiWidget* Build(const nglString& rClassName, const std::map<nglString, nglString>& rParamDictionary)
  {
    return nuiBuilder::Get().CreateWidget(rClassName, rParamDictionary);
  }

  static nuiBuilder& Get()
  {
    return mBuilder;
  }

  static void Init();
  void Uninit();
protected:
  nuiBuilderMap mBuilderMap;
  nuiWidgetCreatorMap mCreatorMap;
  static nuiBuilder mBuilder;

private:
  nuiBuilder();
  ~nuiBuilder();
};

template <class T>
nuiWidgetPtr nuiWidgetCreateFunction()
{
  return new T();
}

#define NUI_ADD_WIDGET_CREATOR(X) { nuiBuilder::Get().SetHandler(#X,&nuiWidgetCreateFunction<X>); }

nuiWidget* nuiCreateWidget(const nglString& rClassName);

///////////////////////////////////////////////////////////////////
class nuiWidgetCreatorOperation;

class nuiWidgetCreator
{
public:
  nuiWidgetCreator(const nglString& rClassName, const nglString& rObjectName);
  virtual ~nuiWidgetCreator();
  
  nuiWidget* Create(const std::map<nglString, nglString>& rParamDictionary, const nuiBuilder* pBuilder = NULL) const;
  nuiWidget* Create(const nuiBuilder* pBuilder = NULL) const;
  
  // For any simple container widget:
  void AddChild(nuiWidgetCreator* pCreator);
  
  // For box containers (nuiBox: nuiHBox, nuiVBox):
  void SetCell(uint32 cell, nuiWidgetCreator* pCreator);

  // For nuiGrid based containers
  void SetCell(uint32 column, uint32 row, nuiWidgetCreator* pCreator);
  
  void SetProperty(const nglString& rName, const nglString& rValue);
  void SetAttribute(const nglString& rName, const nglString& rValue, int32 index0 = -1, int32 index1 = -1);
  void SetActions(const std::vector<std::pair<nglString, nuiEventActionHolder*> >& rEventActions);

  const nglString& GetObjectClass() const;
  const nglString& GetObjectName() const;
  
  void SetDefaultDictionary(const std::map<nglString, nglString>& rParamDictionary);
  const std::map<nglString, nglString>& GetDefaultDictionary() const;
  std::map<nglString, nglString>& GetDefaultDictionary();
  void SetObjectNameIsClass();
protected:
  std::vector<nuiWidgetCreatorOperation> mOperations;
  nglString mClassName;
  nglString mObjectName;
  bool mObjectNameIsClass = false;
  std::map<nglString, nglString> mDefaultDictionary;
  const nglString& LookUp(const std::map<nglString, nglString>& rParamDictionary, const nglString& rString) const;
  std::vector<std::pair<nglString, nuiEventActionHolder*> > mEventActions;
};





