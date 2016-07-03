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

  nuiWidget* label = main->SearchForChild("label", true);

  main->EnableAutoLayout(true);
  auto& mainAttribs(main->GetLayoutAttributes());
  auto& labelAttribs(label->GetLayoutAttributes());

  auto& solver(main->GetSolver());

  solver.addConstraint(labelAttribs.CenterX == mainAttribs.CenterX);
  solver.addConstraint(labelAttribs.CenterY == mainAttribs.CenterY);
  solver.addConstraint(labelAttribs.Width == labelAttribs.ContentsWidth);
  solver.addConstraint(labelAttribs.Height == labelAttribs.ContentsHeight);

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


