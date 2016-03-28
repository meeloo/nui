/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "MainWindow.h"
#include "Application.h"
#include "nuiLabel.h"
#include "nuiComboBox.h"
#include "nuiBackgroundPane.h"
#include "nuiCSS.h"
/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
  : nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), mEventSink(this)
{
#ifdef _DEBUG_
  SetDebugMode(true);
#endif
  
#ifdef _WIN32_
  LoadCSS("rsrc:/css/MXEditor-Win32.css");
#else
  LoadCSS("rsrc:/css/main.css");
#endif  
}

MainWindow::~MainWindow()
{
}

void MainWindow::OnCreation()
{
  // build combo choices tree
  nuiTreeNodePtr pNode = NULL;
  nuiTreeNodePtr pTree = new nuiTreeNode("Root", true);
  for (uint i = 0; i < 10; i++)
  {
    nglString str;
    str.Format("Choice #%d", i);
    pNode = new nuiTreeNode(new nuiLabel(str,"Tutorial::Label"), false);
    pTree->AddChild(pNode);
  }
  
  // combo choices sub-tree
  for (uint i = 0; i < 10; i++)
  {
    nglString str;
    str.Format("Choice %c", (char)(_T('A') + i));
    nuiTreeNode* pNode2 = new nuiTreeNode(new nuiLabel(str,"Tutorial::Label"), false);
    pNode->AddChild(pNode2);
  }
  
  
  // create combo box
  nuiComboBox* pCombo = new nuiComboBox(pTree, true);
  pCombo->SetUserSize(150,25);
  mEventSink.Connect(pCombo->ValueChanged, &MainWindow::OnComboChanged, (void*)pCombo);
  
  pCombo->SetPosition(nuiCenter);
  AddChild(pCombo);
  
  // create label for output display
  nuiBackgroundPane* pPane = new nuiBackgroundPane(eInnerBackground);
  pPane->SetBorder(20,30);
  pPane->SetPosition(nuiTop);
  pPane->SetUserWidth(280);
  pPane->SetUserHeight(40);
  AddChild(pPane);
  mpLabel = new nuiLabel(" ");
  mpLabel->SetObjectName("Tutorial::Label");
  pPane->AddChild(mpLabel);

}

void MainWindow::OnComboChanged(const nuiEvent& rEvent)
{
  nuiComboBox* pCombo = (nuiComboBox*)rEvent.mpUser;
  
  nuiWidget* pWidget = pCombo->GetSelectedWidget();
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  uint32 index = pCombo->GetValue();
  
  nglString msg;
  msg.Format("the user choice is:\n '%ls' (index %d)", pLabel->GetText().GetChars(), index);
  mpLabel->SetText(msg);
  
  rEvent.Cancel();
}


void MainWindow::OnClose()
{
  if (GetNGLWindow()->IsInModalState())
    return;
  
  
  App->Quit();
}


bool MainWindow::LoadCSS(const nglPath& rPath)
{
  nglIStream* pF = rPath.OpenRead();
  if (!pF)
  {
    NGL_OUT("Unable to open CSS source file '%ls'\n", rPath.GetChars());
    return false;
  }
  
  nuiCSS* pCSS = new nuiCSS();
  bool res = pCSS->Load(*pF, rPath);
  delete pF;
  
  if (res)
  {
    nuiMainWindow::SetCSS(pCSS);
    return true;
  }
  
  NGL_OUT("%ls\n", pCSS->GetErrorString().GetChars());
  
  delete pCSS;
  return false;
}


