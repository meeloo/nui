/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "MainWindow.h"
#include "Application.h"
#include "nuiLabel.h"
#include "nuiRadioButtonGroup.h"
#include "nuiHBox.h"
#include "nuiVBox.h"
#include "nuiPane.h"

#include "nuiColorDecoration.h"
#include "nuiGradientDecoration.h"
#include "nuiFrame.h"
#include "nuiCSS.h"

#include "nuiWidgetInspector.h"

#include "nuiDebugServer.h"

////////////////////////////////////////////////////////////////////////////////
#include "nuiTableView.h"


/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
  : nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), mEventSink(this)
{
  SetDebugMode(true);
  //App->GetLog().SetLevel("nuiTexture", NGL_LOG_ALWAYS);
  LoadCSS("rsrc:/css/main.css");
}

MainWindow::~MainWindow()
{
}


void MainWindow::OnClose()
{
  if (GetNGLWindow()->IsInModalState())
    return;

  App->Quit();
}

void MainWindow::OnCreation()
{
  nuiWidget* main = nuiBuilder::Build("main");

  nuiWidget* label1 = main->SearchForChild("label1", true);
  nuiWidget* label2 = main->SearchForChild("label2", true);

  main->EnableAutoLayout(true);
  auto& mainAttribs(main->GetLayoutAttributes());
  auto& label1Attribs(label1->GetLayoutAttributes());
  auto& label2Attribs(label2->GetLayoutAttributes());

  auto& solver(main->GetSolver());

  solver.addConstraint(label1Attribs.CenterX == mainAttribs.CenterX);
  solver.addConstraint(label1Attribs.CenterY == mainAttribs.CenterY);

  solver.addConstraint(label2Attribs.CenterX == label1Attribs.Right);
  solver.addConstraint(label2Attribs.Top >= label1Attribs.Bottom);

  AddChild(main);
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


