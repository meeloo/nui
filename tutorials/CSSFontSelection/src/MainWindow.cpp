/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "MainWindow.h"
#include "Application.h"
#include "nuiCSS.h"
#include "nuiVBox.h"


/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
  : nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), mEventSink(this)
{
  SetDebugMode(true);
  
  LoadCSS("rsrc:/css/main.css");
}

MainWindow::~MainWindow()
{
}

void MainWindow::OnCreation()
{
  // a vertical box for page layout
  nuiVBox* pLayoutBox = new nuiVBox(0);
  pLayoutBox->SetExpand(nuiExpandShrinkAndGrow);
  AddChild(pLayoutBox);
  
  // image in the first box's cell
  nuiImage* pImg = new nuiImage();
  pImg->SetObjectName("MyImage");
  pImg->SetPosition(nuiCenter);
  pLayoutBox->AddCell(pImg);
  pLayoutBox->SetCellExpand(pLayoutBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  // button in the second cell : we use the default decoration for this button, but you could use the css to assign your own decoration
  nuiButton* pButton = new nuiButton();
  pButton->SetPosition(nuiCenter);
  pLayoutBox->AddCell(pButton);
  pLayoutBox->SetCellExpand(pLayoutBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  // click event on button
  mEventSink.Connect(pButton->Activated, &MainWindow::OnButtonClick);
  
  // label with border in the button (put the label string in the button's constructor if you don't need borders)
  nuiLabel* pButtonLabel = new nuiLabel("click!");
  pButtonLabel->SetPosition(nuiCenter);
  pButtonLabel->SetBorder(8,8);
  pButton->AddChild(pButtonLabel);

  // label with decoration in the third cell
  mMyLabel = new nuiLabel("my label");
  mMyLabel->SetObjectName("MyLabel");
  mMyLabel->SetPosition(nuiCenter);
  pLayoutBox->AddCell(mMyLabel);
  pLayoutBox->SetCellExpand(pLayoutBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
}



void MainWindow::OnButtonClick(const nuiEvent& rEvent)
{
  nglString message;
  double currentTime = nglTime();
  message.Format("click time: %.2f", currentTime);
  mMyLabel->SetText(message);
  
  // the event is caught and not broadcast
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
