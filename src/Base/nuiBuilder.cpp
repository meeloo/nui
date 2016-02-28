/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

using namespace std;

//////////////////// Builder class:
nuiBuilder::nuiBuilder()
{
  //Init();
}

nuiBuilder::~nuiBuilder()
{
  nuiWidgetCreatorMap::iterator it = mCreatorMap.begin();
  while (it != mCreatorMap.end())
  {
    delete it->second;
    ++it;
  }

}

void nuiBuilder::Init()
{
  NUI_ADD_WIDGET_CREATOR(nuiWidget);
  NUI_ADD_WIDGET_CREATOR(nuiSplitter);
  NUI_ADD_WIDGET_CREATOR(nuiPopupMenu);
  NUI_ADD_WIDGET_CREATOR(nuiList);
  NUI_ADD_WIDGET_CREATOR(nuiScrollBar);
  NUI_ADD_WIDGET_CREATOR(nuiFixed);
  NUI_ADD_WIDGET_CREATOR(nuiMainWindow);
  NUI_ADD_WIDGET_CREATOR(nuiScrollView);
  NUI_ADD_WIDGET_CREATOR(nuiZoomView);
  NUI_ADD_WIDGET_CREATOR(nuiTabBar);
  NUI_ADD_WIDGET_CREATOR(nuiTreeView);
  NUI_ADD_WIDGET_CREATOR(nuiColumnTreeView);
  NUI_ADD_WIDGET_CREATOR(nuiSlider);
  NUI_ADD_WIDGET_CREATOR(nuiPane);
  NUI_ADD_WIDGET_CREATOR(nuiGrid);
  NUI_ADD_WIDGET_CREATOR(nuiHBox);
  NUI_ADD_WIDGET_CREATOR(nuiVBox);
  NUI_ADD_WIDGET_CREATOR(nuiFolderPane);
  NUI_ADD_WIDGET_CREATOR(nuiLabel);
  NUI_ADD_WIDGET_CREATOR(nuiText);
  NUI_ADD_WIDGET_CREATOR(nuiEditText);
  NUI_ADD_WIDGET_CREATOR(nuiEditLine);
  NUI_ADD_WIDGET_CREATOR(nuiButton);
  NUI_ADD_WIDGET_CREATOR(nuiKnob);
  NUI_ADD_WIDGET_CREATOR(nuiToggleButton);
  NUI_ADD_WIDGET_CREATOR(nuiRadioButton);
  NUI_ADD_WIDGET_CREATOR(nuiImage);
  NUI_ADD_WIDGET_CREATOR(nuiTab);
  NUI_ADD_WIDGET_CREATOR(nuiProgressBar);
  
  //NUI_ADD_WIDGET_CREATOR(nuiMessageBox,"Dialog");
  //NUI_ADD_WIDGET_CREATOR(nuiDialogSelectDirectory,"Dialog");
  //NUI_ADD_WIDGET_CREATOR(nuiDialogSelectFile,"Dialog");
  
  //NUI_ADD_WIDGET_CREATOR(nuiDialog, "Dialog");
  NUI_ADD_WIDGET_CREATOR(nuiFlowView);
  //NUI_ADD_WIDGET_CREATOR(nuiImageSequence);
  NUI_ADD_WIDGET_CREATOR(nuiModalContainer);
  //NUI_ADD_WIDGET_CREATOR(nuiOffscreenView);
  //NUI_ADD_WIDGET_CREATOR(nuiPopupView);
  //NUI_ADD_WIDGET_CREATOR(nuiScrollBox);
  //NUI_ADD_WIDGET_CREATOR(nuiShadeContainer);
  NUI_ADD_WIDGET_CREATOR(nuiTabView);
  NUI_ADD_WIDGET_CREATOR(nuiTitledPane);
  NUI_ADD_WIDGET_CREATOR(nuiColorSelector);
  NUI_ADD_WIDGET_CREATOR(nuiComboBox);
  NUI_ADD_WIDGET_CREATOR(nuiCoverFlow);
  NUI_ADD_WIDGET_CREATOR(nuiFileList);
  //NUI_ADD_WIDGET_CREATOR(nuiFileSelector);
  //NUI_ADD_WIDGET_CREATOR(nuiFileTree);
  NUI_ADD_WIDGET_CREATOR(nuiFontBrowser);
  NUI_ADD_WIDGET_CREATOR(nuiFrameView);
  NUI_ADD_WIDGET_CREATOR(nuiHotKeyEditor);
  //NUI_ADD_WIDGET_CREATOR(nuiImageDropZone);
  //NUI_ADD_WIDGET_CREATOR(nuiShapeView);
  //NUI_ADD_WIDGET_CREATOR(nuiZoomBar);
  NUI_ADD_WIDGET_CREATOR(nuiRSSView);
  //NUI_ADD_WIDGET_CREATOR(nuiHugeImage);
  NUI_ADD_WIDGET_CREATOR(nuiHyperLink);
  //NUI_ADD_WIDGET_CREATOR(nuiLabelAttribute);
  //NUI_ADD_WIDGET_CREATOR(nuiLabelRenamer);
  NUI_ADD_WIDGET_CREATOR(nuiHTMLView);

  NUI_ADD_WIDGET_CREATOR(nuiNavigationBar);
  NUI_ADD_WIDGET_CREATOR(nuiNavigationController);
  NUI_ADD_WIDGET_CREATOR(nuiNavigationButton);
  NUI_ADD_WIDGET_CREATOR(nuiViewController);

  NUI_ADD_WIDGET_CREATOR(nuiLayout);
  NUI_ADD_WIDGET_CREATOR(nuiDrawerView);

#ifndef _OPENGL_ES_
  NUI_ADD_WIDGET_CREATOR(nuiUserArea);
#endif
}

void nuiBuilder::Uninit()
{
  //mBuilderMap.clear();
  {
    nuiWidgetCreatorMap::iterator it = mCreatorMap.begin();
    nuiWidgetCreatorMap::iterator end = mCreatorMap.end();
    while (it != end)
    {
      nuiWidgetCreator* pCreator = it->second;
      delete pCreator;
      ++it;
    }
    
    mCreatorMap.clear();
  }
}

void nuiBuilder::SetHandler(const nglString& ClassName, nuiCreateWidgetFn pHandler)
{
  //wprintf(_T("Adding Widget handler: %s\n"), ClassName.GetChars());
  NGL_ASSERT(mBuilderMap.find(ClassName) == mBuilderMap.end());
  NGL_ASSERT(mCreatorMap.find(ClassName) == mCreatorMap.end());
  nuiWidgetDesc desc(ClassName, pHandler);
  mBuilderMap[ClassName] = desc;
}

void nuiBuilder::SetHandler(const nglString& rClassName, nuiWidgetCreator* pCreator)
{
  NGL_ASSERT(mBuilderMap.find(rClassName) == mBuilderMap.end());
  NGL_ASSERT(mCreatorMap.find(rClassName) == mCreatorMap.end());
  mCreatorMap[rClassName] = pCreator;
}

nuiCreateWidgetFn nuiBuilder::GetHandler(const nglString& ClassName) const
{
  map<nglString, nuiWidgetDesc, nglString::LessFunctor>::const_iterator it = mBuilderMap.find(ClassName);
  if (it == mBuilderMap.end())
    return NULL;
  
  const nuiWidgetDesc& desc = it->second;
  return desc.GetHandler();
}

bool nuiBuilder::GetClassList(list<nuiWidgetDesc>& rClassNames) const
{
  map<nglString, nuiWidgetDesc, nglString::LessFunctor>::const_iterator it;
  map<nglString, nuiWidgetDesc, nglString::LessFunctor>::const_iterator end = mBuilderMap.end();

  for (it = mBuilderMap.begin(); it != end; ++it)
    rClassNames.push_back((*it).second);

  return true;
}

nuiWidget* nuiBuilder::CreateWidget(const nglString& rClassName) const
{
  std::map<nglString, nglString> ParamDictionary;
  return CreateWidget(rClassName, ParamDictionary);
}

nuiWidget* nuiBuilder::CreateWidget(const nglString& rClassName, const std::map<nglString, nglString>& rParamDictionary) const
{
  nuiCreateWidgetFn pFn = GetHandler(rClassName);
  if (pFn)
    return pFn();
  
  map<nglString, nuiWidgetCreator*, nglString::LessFunctor>::const_iterator it = mCreatorMap.find(rClassName);
  if (it == mCreatorMap.end())
    return NULL;
  
  return (it->second)->Create(rParamDictionary, this);
}

nuiWidget* nuiCreateWidget(const nglString& rClassName)
{
  return nuiBuilder::Get().CreateWidget(rClassName);
}

nuiBuilder nuiBuilder::mBuilder;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class nuiWidgetCreatorOperation
{
public:
  enum Type
  {
    eAddChild,
    eSetCell1,
    eSetCell2,
    eSetProperty,
    eSetAttribute,
  };
  
  nuiWidgetCreatorOperation(Type type, nuiWidgetCreator* pCreator, uint32 index1, uint32 index2)
  {
    mType = type;
    mpCreator = pCreator;
    mIndex1 = index1;
    mIndex2 = index2;
  }
  
  nuiWidgetCreatorOperation(Type type, const nglString& rName, const nglString& rValue, uint32 index1 = -1, uint32 index2 = -1)
  {
    mType = type;
    mpCreator = NULL;
    mIndex1 = index1;
    mIndex2 = index2;
    mName = rName;
    mValue = rValue;
  }
  
  Type mType;
  nuiWidgetCreator* mpCreator;
  uint32 mIndex1;
  uint32 mIndex2;
  nglString mName;
  nglString mValue;
};

nuiWidgetCreator::nuiWidgetCreator(const nglString& rClassName, const nglString& rObjectName)
{
  mClassName = rClassName;
  mObjectName = rObjectName;
}

nuiWidgetCreator::~nuiWidgetCreator()
{
  for (uint32 i = 0; i < mOperations.size(); i++)
  {
    delete mOperations[i].mpCreator;
  }
}

const nglString& nuiWidgetCreator::LookUp(const std::map<nglString, nglString>& rParamDictionary, const nglString& rString) const
{
  if (!rString.IsEmpty() && rString[0] == '$')
  {
    const nglString& res = nuiObject::GetGlobalProperty(rString.Extract(1));
    return res;
  }
  const std::map<nglString, nglString>::const_iterator it = rParamDictionary.find(rString);
  if (it != rParamDictionary.end())
    return it->second;

  return rString;
}

void nuiWidgetCreator::SetObjectNameIsClass()
{
  mObjectNameIsClass = true;
}

nuiWidget* nuiWidgetCreator::Create(const std::map<nglString, nglString>& rParamDictionary, const nuiBuilder* pBuilder) const
{
  if (!pBuilder)
    pBuilder = &nuiBuilder::Get();

  // Merge the dictionaries:
  std::map<nglString, nglString> ParamDictionary(mDefaultDictionary);
  ParamDictionary.insert(rParamDictionary.begin(), rParamDictionary.end());
  
//  NGL_LOG(_T("nuiWidgetCreator"), NGL_LOG_DEBUG, _T("\nBuild %s / %s\n"), mClassName.GetChars(), mObjectName.GetChars());
//  std::map<nglString, nglString>::iterator it = ParamDictionary.begin();
//  std::map<nglString, nglString>::iterator end = ParamDictionary.end();
//  while (it != end)
//  {
//    NGL_LOG(_T("nuiWidgetCreator"), NGL_LOG_DEBUG, _T("\t%s - %s\n"), it->first.GetChars(), it->second.GetChars());
//    ++it;
//  }
  
  
  nglString classname(LookUp(ParamDictionary, mClassName));
  nglString objectname(LookUp(ParamDictionary,mObjectName));
  nuiWidget* pWidget = pBuilder->CreateWidget(classname, ParamDictionary);
  if (!pWidget)
  {
    NGL_LOG(_T("nuiWidgetCreator"), NGL_LOG_ERROR, _T("Error while creating a %s named %s (translated to %s - %s"), 
            mClassName.GetChars(), mObjectName.GetChars(),
            classname.GetChars(), objectname.GetChars());
    return NULL;
  }

  if (mObjectNameIsClass)
    pWidget->SetObjectClass(mObjectName);

  if (!objectname.IsEmpty())
    pWidget->SetObjectName(objectname);
  
  nuiTabView* pTabView = dynamic_cast<nuiTabView*> (pWidget);
  nuiWidget* pContainer = dynamic_cast<nuiWidget*> (pWidget);
  nuiBox* pBox = dynamic_cast<nuiBox*> (pWidget);
  nuiGrid* pGrid = dynamic_cast<nuiGrid*> (pWidget);
  
  for (uint32 i = 0; i < mOperations.size(); i++)
  {
    nuiWidget* pChild = NULL;
    if (mOperations[i].mpCreator)
      pChild = mOperations[i].mpCreator->Create(ParamDictionary, pBuilder);
    
    if (pChild)
    {
      switch (mOperations[i].mType)
      {
        case nuiWidgetCreatorOperation::eAddChild:
          if (pBox)
            pBox->AddCell(pChild);
          else if (pTabView)
          {
            if (pChild->HasProperty("TabWidget"))
            {
              pTabView->AddTab(pBuilder->CreateWidget(pChild->GetProperty("TabWidget"), rParamDictionary), pChild);
            }
            else if (pChild->HasProperty("TabName"))
            {
              pTabView->AddTab(pChild->GetProperty("TabName"), pChild);
            }
            else
              pTabView->AddTab(pChild->GetObjectName(), pChild);
          }
          else if (pContainer)
            pContainer->AddChild(pChild);
          break;
          
        case nuiWidgetCreatorOperation::eSetCell1:
          if (pBox)
            pBox->SetCell(mOperations[i].mIndex1, pChild);
          break;
          
        case nuiWidgetCreatorOperation::eSetCell2:
          if (pGrid)
            pGrid->SetCell(mOperations[i].mIndex1, mOperations[i].mIndex2, pChild);
          break;

        default:
          NGL_ASSERT(0);
          break;
      }
    }
    else
    {
      switch (mOperations[i].mType)
      {
        case nuiWidgetCreatorOperation::eSetProperty:
          pWidget->SetProperty(LookUp(ParamDictionary, mOperations[i].mName),
                               LookUp(ParamDictionary, mOperations[i].mValue));
          break;
          
        case nuiWidgetCreatorOperation::eSetAttribute:
          {
            nuiAttribBase attrib(pWidget->GetAttribute(LookUp(ParamDictionary, mOperations[i].mName)));
            if (attrib.IsValid())
            {
              int32 i0 = mOperations[i].mIndex1;
              int32 i1 = mOperations[i].mIndex2;
              
              if (i0 < 0)
                attrib.FromString(LookUp(ParamDictionary, mOperations[i].mValue));
              else if (i1 < 0)
                attrib.FromString(i0, LookUp(ParamDictionary, mOperations[i].mValue));
              else
                attrib.FromString(i0, i1, LookUp(ParamDictionary, mOperations[i].mValue));
              
            }
          }
          break;
          
        default:
          NGL_ASSERT(0);
          break;
      }
      
    }
  }

  for (const auto& eventaction : mEventActions)
  {
    pWidget->AddEventAction(eventaction.first, eventaction.second);
  }

  if (pWidget)
    pWidget->Built();
  return pWidget;
}

nuiWidget* nuiWidgetCreator::Create(const nuiBuilder* pBuilder) const
{
  std::map<nglString, nglString> ParamDictionary;
  return Create(ParamDictionary, pBuilder);
}

// For any simple container widget:
void nuiWidgetCreator::AddChild(nuiWidgetCreator* pCreator)
{
  mOperations.push_back(nuiWidgetCreatorOperation(nuiWidgetCreatorOperation::eAddChild, pCreator, 0, 0));
}

// For box containers (nuiBox: nuiHBox, nuiVBox):
void nuiWidgetCreator::SetCell(uint32 cell, nuiWidgetCreator* pCreator)
{
  mOperations.push_back(nuiWidgetCreatorOperation(nuiWidgetCreatorOperation::eSetCell2, pCreator, cell, 0));
}

// For nuiGrid based containers
void nuiWidgetCreator::SetCell(uint32 column, uint32 row, nuiWidgetCreator* pCreator)
{
  mOperations.push_back(nuiWidgetCreatorOperation(nuiWidgetCreatorOperation::eSetCell2, pCreator, column, row));
}

void nuiWidgetCreator::SetProperty(const nglString& rName, const nglString& rValue)
{
  mOperations.push_back(nuiWidgetCreatorOperation(nuiWidgetCreatorOperation::eSetProperty, rName, rValue));
}

void nuiWidgetCreator::SetAttribute(const nglString& rName, const nglString& rValue, int32 index0, int32 index1)
{
  mOperations.push_back(nuiWidgetCreatorOperation(nuiWidgetCreatorOperation::eSetAttribute, rName, rValue, index0, index1));
}

const nglString& nuiWidgetCreator::GetObjectClass() const
{
  return mClassName;
}

const nglString& nuiWidgetCreator::GetObjectName() const
{
  return mObjectName;
}

void nuiWidgetCreator::SetDefaultDictionary(const std::map<nglString, nglString>& rParamDictionary)
{
  mDefaultDictionary = rParamDictionary;
}

const std::map<nglString, nglString>& nuiWidgetCreator::GetDefaultDictionary() const
{
  return mDefaultDictionary;
}

std::map<nglString, nglString>& nuiWidgetCreator::GetDefaultDictionary()
{
  return mDefaultDictionary;
}

void nuiWidgetCreator::SetActions(const std::vector<std::pair<nglString, nuiEventActionHolder*> >& rEventActions)
{
  mEventActions = rEventActions;
}

