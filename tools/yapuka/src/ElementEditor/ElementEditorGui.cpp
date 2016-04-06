/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/
  
#include "ElementEditorGui.h"

#include "nuiVBox.h"
#include "nuiHBox.h"
#include "nuiPane.h"
#include "nuiFileSelector.h"
#include "nuiDialog.h"
#include "nuiLabelRenamer.h"

#include "ElementEditor/ElementEditor.h"
#include "Main/Yapuka.h"


ElementEditorGui::ElementEditorGui()
: nuiWidget(), mEventSink(this)
{
  // Main box [menus + buttons, main area]
  nuiVBox* pVBox = new nuiVBox(2);
  pVBox->SetExpand(nuiExpandShrinkAndGrow);
  AddChild(pVBox);
  
  nuiWidget* pBkg = new nuiWidget();
  pVBox->SetCell(0, pBkg);
  pVBox->SetCellExpand(0, nuiExpandFixed);

  // Buttons box:
  nuiHBox* pButtonBox = new nuiHBox();
  pButtonBox->SetExpand(nuiExpandShrinkAndGrow);
  pBkg->AddChild(pButtonBox);
  
  // Main Buttons
  nuiButton* pLoad = new nuiButton("Load project");
  pButtonBox->AddCell(pLoad,nuiCenter);
  
  nuiButton* pSave = new nuiButton("Save project");
  pButtonBox->AddCell(pSave,nuiCenter);
  
  nuiButton* pFrameEditor = new nuiButton("New Frame");
  pButtonBox->AddCell(pFrameEditor,nuiCenter);
  
  nuiButton* pWidgetEditor = new nuiButton("New Widget");
  pButtonBox->AddCell(pWidgetEditor,nuiCenter);
  
  
  
  // Splitter:
  nuiSplitter* pSplitter = new nuiSplitter(nuiVertical);
  pVBox->SetCell(1, pSplitter);
  pVBox->SetCellExpand(1, nuiExpandShrinkAndGrow);
  
  nuiPane* pLeftPane = new nuiPane(nuiColor(210, 215, 255));
  
  nuiScrollView* pListSV = new nuiScrollView();
  pLeftPane->AddChild(pListSV);
  pSplitter->AddChild(pLeftPane);
  
	
	
  nuiTreeNode* pMainTree = new nuiTreeNode("Elements", true, false);
	mpFrameTree = new nuiTreeNode("Frames", true, false);
	mpWidgetTree = new nuiTreeNode("Widgets", true, false);
	pMainTree->AddChild(mpFrameTree);
	pMainTree->AddChild(mpWidgetTree);
	
	mpElementTree = new nuiTreeView(pMainTree);
  pListSV->AddChild(mpElementTree);
  
  mpEditorContainer = new nuiWidget();
  pSplitter->AddChild(mpEditorContainer);
  
  mEventSink.Connect(pLoad->Activated, &ElementEditorGui::OnLoad);
  mEventSink.Connect(pSave->Activated, &ElementEditorGui::OnSave);
  mEventSink.Connect(pFrameEditor->Activated, &ElementEditorGui::OnNewFrame);
  mEventSink.Connect(pWidgetEditor->Activated, &ElementEditorGui::OnNewWidget);
  
  mEventSink.Connect(mpElementTree->SelectionChanged, &ElementEditorGui::OnElementSelected);
  mEventSink.Connect(mpElementTree->Activated, &ElementEditorGui::OnElementActivated);
  
  UpdateList();	
}




ElementEditorGui::~ElementEditorGui()
{
  
}



void ElementEditorGui::OnSave(const nuiEvent& rEvent)
{
  nglPath path(ePathUserDocuments);
  nuiPane* pEditPane = new nuiPane(nuiColor(255, 255, 255, 192), nuiColor(0, 0, 0, 192));
  nuiEditLine* pEntry = new nuiEditLine();
  pEditPane->AddChild(pEntry);
  nuiPane* pPane = new nuiPane();
  pPane->SetCurve(8);
  nuiFileSelector* pFS = new nuiFileSelector(path, "*", pEntry, nuiFileSelector::eColumn);
  
  nuiVBox* pBox = new nuiVBox();
  pPane->AddChild(pFS);
  pBox->AddCell(pPane);
  pBox->AddCell(pEditPane);
  
  pPane->SetUserSize(GetMainWindow()->GetWidth() * .8, GetMainWindow()->GetHeight() * .8);
  pEditPane->SetUserSize(GetMainWindow()->GetWidth() * .8, 20);
  pEditPane->SetCurve(2);
  pEditPane->SetBorder(5, 5);
  
  //pBox->SetExpand(nuiExpandShrinkAndGrow);
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  pBox->SetCellExpand(1, nuiExpandFixed);
  
  nuiDialog* pDialog = new nuiDialog(this);
  pDialog->InitDialog("Save yapuka project", NULL, nuiDialog::eDialogButtonOk | nuiDialog::eDialogButtonCancel);
  pDialog->SetContents(pBox, nuiCenter);
  pDialog->SetDefaultPos();
  pDialog->SetToken(new nuiToken<nuiFileSelector*>(pFS));
  
  mEventSink.Connect(pDialog->DialogDone, &ElementEditorGui::OnSaved, pDialog);
}

void ElementEditorGui::OnSaved(const nuiEvent& rEvent)
{
  nuiDialog* pDialog = (nuiDialog*)rEvent.mpUser;
  nuiFileSelector* pFS = ((nuiToken<nuiFileSelector*>*)pDialog->GetToken())->Token;
  
  if (pDialog->GetResult() == nuiDialog::eDialogAccepted)
  {
    nuiXML xml("yapuka");
    
    ElementDescList::iterator it = mElements.begin();
    ElementDescList::iterator end = mElements.end();
    while (it != end)
    {
      ElementDesc* pElement = *it;
      pElement->CommitChanges();
      
      const nuiXMLNode* pXML = pElement->GetXML();
      if (pXML)
      {
        nuiXMLNode* pNode = pXML->Clone(&xml);
        printf("Saving %ls\n", pElement->GetName().GetChars());
        pNode->SetAttribute("Name", pElement->GetName());
      }
      ++it;
    }
    
    nglString Dump(xml.Dump());
    
    NGL_OUT("\n%ls\n", Dump.GetChars());
    
    nglPath path(pFS->GetPath());
    if (path.GetExtension().IsEmpty())
    {
      path = (path.GetPathName() + nglString(".ypk"));
    }
    
    nglFile file(path, eFileWrite);
    
    file.Write(Dump, eUTF8);
  }
}




void ElementEditorGui::OnLoad(const nuiEvent& rEvent)
{
  nglPath path(ePathUserDocuments);
  nuiPane* pEditPane = new nuiPane(nuiColor(255, 255, 255, 192), nuiColor(0, 0, 0, 192));
  nuiEditLine* pEntry = new nuiEditLine();
  pEditPane->AddChild(pEntry);
  nuiPane* pPane = new nuiPane();
  pPane->SetCurve(8);
  nuiFileSelector* pFS = new nuiFileSelector(path, "*", pEntry, nuiFileSelector::eColumn);
  
  nuiVBox* pBox = new nuiVBox();
  pPane->AddChild(pFS);
  pBox->AddCell(pPane);
  pBox->AddCell(pEditPane);
  
  pPane->SetUserSize(GetMainWindow()->GetWidth() * .8, GetMainWindow()->GetHeight() * .8);
  pEditPane->SetUserSize(GetMainWindow()->GetWidth() * .8, 20);
  pEditPane->SetCurve(2);
  pEditPane->SetBorder(5, 5);
  
  //pBox->SetExpand(nuiExpandShrinkAndGrow);
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  pBox->SetCellExpand(1, nuiExpandFixed);
  
  nuiDialog* pDialog = new nuiDialog(this);
  pDialog->InitDialog("Load yapuka project", NULL, nuiDialog::eDialogButtonOk | nuiDialog::eDialogButtonCancel);
  pDialog->SetContents(pBox, nuiCenter);
  pDialog->SetDefaultPos();
  pDialog->SetToken(new nuiToken<nuiFileSelector*>(pFS));
  
  mEventSink.Connect(pDialog->DialogDone, &ElementEditorGui::OnLoaded, pDialog);
}

void ElementEditorGui::OnLoaded(const nuiEvent& rEvent)
{
  nuiDialog* pDialog = (nuiDialog*)rEvent.mpUser;
  nuiFileSelector* pFS = ((nuiToken<nuiFileSelector*>*)pDialog->GetToken())->Token;
  
  if (pDialog->GetResult() == nuiDialog::eDialogAccepted)
  {
    nglPath path(pFS->GetPath());
    nglIFile file(path);
    
    nuiXML XML("yapuka");
    if (!XML.Load(file))
    {
      return;
    }
    
    for (uint32 i = 0; i < XML.GetChildrenCount(); i++)
    {
      nuiXMLNode* pNode = XML.GetChild(i);
      
      ElementDesc* pDesc = new ElementDesc();
      const nglString& rName = pNode->GetName();
      printf("Load Element: %ls\n", rName.GetChars());
      if (rName == "nuiFrame")
      {
        pDesc->SetType(eElementFrame);
      }
      else
      {
        pDesc->SetType(eElementWidget);
      }
      
      pDesc->SetName(pNode->GetAttribute("Name"));
      pDesc->SetXML(pNode->Clone());
      mElements.push_back(pDesc);
    }
    
    UpdateList();
    if (!mElements.empty())
			mpElementTree->Select(mpLastElement, true);
  }
}



void ElementEditorGui::OnNewFrame(const nuiEvent& rEvent)
{
  ElementDesc* pDesc = new ElementDesc();
  pDesc->SetType(eElementFrame);
  pDesc->SetName("unnamed frame");
  mElements.push_back(pDesc);
  UpdateList();
  if (!mElements.empty())
    mpElementTree->Select(mpLastElement, true);
}

void ElementEditorGui::OnNewWidget(const nuiEvent& rEvent)
{
  ElementDesc* pDesc = new ElementDesc();
  pDesc->SetType(eElementWidget);
  pDesc->SetName("unnamed widget");
  mElements.push_back(pDesc);
  UpdateList();
  if (!mElements.empty())
    mpElementTree->Select(mpLastElement, true);
}

void ElementEditorGui::UpdateList()
{
  mpFrameTree->Clear();
  mpWidgetTree->Clear();
  
  if (mElements.empty())
  {
    nuiTreeNode* pNode = new nuiTreeNode("Empty");
    mpFrameTree->AddChild(pNode);
		pNode = new nuiTreeNode("Empty");
    mpWidgetTree->AddChild(pNode);
  }
  else
  {
    uint nbFrames=0;
    uint nbWidgets=0;
    ElementDescList::iterator it = mElements.begin();
    ElementDescList::iterator end = mElements.end();
    while (it != end)
    {
      ElementDesc* pElement = *it;
      nuiTreeNode* pNode = new nuiTreeNode(pElement->GetName());
			mpLastElement = pNode;
      pNode->SetToken(new nuiToken<ElementDesc*>(pElement));
			switch (pElement->GetType())
			{
        case eElementFrame:
          mpFrameTree->AddChild(pNode);
          nbFrames++;
          break;
        case eElementWidget:
          mpWidgetTree->AddChild(pNode);
          nbWidgets++;
          break;
			}
      
      
      ++it;
    }
    
    if (!nbFrames)
      mpFrameTree->AddChild(new nuiTreeNode("Empty"));
    
    if (!nbWidgets)
      mpWidgetTree->AddChild(new nuiTreeNode("Empty"));
    
  }
  
  if (mElements.empty())
  {
    ClearEditor();
  }
}

void ElementEditorGui::ClearEditor()
{
  nuiLabel* pLabel = new nuiLabel("No editor", nuiFont::GetFont(36));
  pLabel->SetPosition(nuiCenter);
  pLabel->SetTextColor(nuiColor("lightgray"));
  mpEditorContainer->AddChild(pLabel);
}

void ElementEditorGui::OnElementSelected(const nuiEvent& rEvent)
{
  nuiTreeNode* pNode = mpElementTree->GetSelectedNode();
  if (pNode)
  {
    mpEditorContainer->Clear();
    nuiToken<ElementDesc*>* pToken = (nuiToken<ElementDesc*>*) pNode->GetToken();
    
		if (!pToken)
		{
      return;
    }
		
		ElementDesc* pDesc = pToken->Token;
    NGL_ASSERT(pDesc);
    ElementEditor* pEditor = new ElementEditor(pDesc);
    mpEditorContainer->AddChild(pEditor);
  }
  else
  {
    ClearEditor();
  }
}

void ElementEditorGui::OnElementActivated(const nuiEvent& rEvent)
{
  nuiTreeNode* pNode = mpElementTree->GetSelectedNode();
  if (pNode)
	{
    nuiToken<ElementDesc*>* pToken = (nuiToken<ElementDesc*>*) pNode->GetToken();
		
		if (!pToken)
		{
      return;
    }
    
		// take care about that... be sure it's a nuiLabel
		nuiLabel* pLabel = (nuiLabel*)pNode->GetElement();
		
    nuiLabelRenamer* pRenamer = new nuiLabelRenamer(pLabel);
    pRenamer->SetToken(new nuiToken<ElementDesc*>(pToken->Token));
    mEventSink.Connect(pRenamer->Renamed, &ElementEditorGui::OnElementRenamed, pRenamer);
  }
}

void ElementEditorGui::OnElementRenamed(const nuiEvent& rEvent)
{
  nuiLabelRenamer* pItem = (nuiLabelRenamer*)rEvent.mpUser;
  NGL_ASSERT(pItem);
  
  nuiToken<ElementDesc*>* pToken = (nuiToken<ElementDesc*>*)pItem->GetToken();
  
  ElementDesc* pDesc = pToken->Token;
  pDesc->SetName(pItem->GetText());
  
  OnElementSelected(rEvent);
  rEvent.Cancel();
}

