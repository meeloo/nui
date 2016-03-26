/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot

 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "nuiCSS.h"
#include "nuiTabView.h"

#include "Main/MainWindow.h"
#include "Main/nuiDbg.h"


/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo )
: nuiMainWindow(rContextInfo, rInfo, NULL, nglPath(("../data"))),
mWinSink(this)
{

  //nuiTopLevel::EnablePartialRedraw(false);
  SetDebugMode(true);

  InitAttributes();

  LoadCSS("rsrc:/css/nuiDbg.css");
}



void MainWindow::InitAttributes()
{
  mpAttributeMouseCoord = new nuiAttribute<nuiPoint>
  ("Mouse Coordinates", nuiUnitPixels,
   nuiAttribute<nuiPoint>::GetterDelegate(this, &MainWindow::GetMouseCoord),
   nuiAttribute<nuiPoint>::SetterDelegate(this, &MainWindow::SetMouseCoord));
  mAttributeMouseCoord = nuiAttribBase(this, mpAttributeMouseCoord);
}



MainWindow::~MainWindow()
{
  delete mpAttributeMouseCoord;
}

void MainWindow::OnClose()
{
  App->Quit();
}


void MainWindow::OnCreation()
{
  printf("size of variant: %ld\n", sizeof(nuiVariant));

  nuiWidget* pMainCont = new nuiWidget();
  pMainCont->SetObjectName("MainContainer");
  AddChild(pMainCont);

  nuiTabView* pTabView = new nuiTabView(nuiTop);
  pMainCont->AddChild(pTabView);

  pTabView->SetChangeOnDrag(true);

  {
    nuiLabel* pLabel = new nuiLabel(nuiTR("Debugger"));
    pLabel->SetObjectName("TabView::Title");
    //    ProjectGenerator* pGenerator = new ProjectGenerator();
    nuiWidget* pDebugger = nuiBuilder::Build("ConfigView");

    pTabView->AddTab(pLabel, pDebugger);
  }

  {
    nuiLabel* pLabel = new nuiLabel(nuiTR("Widgets tree"));
    pLabel->SetObjectName("TabView::Title");
    nuiWidget* pWidgetTree = nuiBuilder::Build("WidgetTree");

    pTabView->AddTab(pLabel, pWidgetTree);
  }

  if (GetApp()->IsFullVersion())
  {
    nuiLabel* pLabel = new nuiLabel(nuiTR("Element Editor"));
    pLabel->SetObjectName("TabView::Title");
    //        pTabView->AddTab(pLabel, new ElementEditorGui());
  }

  pTabView->SelectTab(0);

  // nui version number
  nglString version;
  version.Format("nui %d.%d r%d %ls", NUI_VERSION_MAJOR, NUI_VERSION_MINOR, NUI_VERSION_RELEASE, NUI_VERSION_BETA.GetChars());
  {
    nuiLabel* pLabel = new nuiLabel(version);
    pLabel->SetPosition(nuiTopRight);
    pLabel->SetObjectName("VersionNumber");
    pMainCont->AddChild(pLabel);
  }
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

  if (res)
  {
    SetCSS(pCSS);
    return true;
  }

  NGL_OUT("%ls\n", pCSS->GetErrorString().GetChars());

  delete pCSS;
  return false;
}

nuiPoint MainWindow::GetMouseCoord()
{
  return mMouseCoord;
}

void MainWindow::SetMouseCoord(nuiPoint point)
{
  mMouseCoord = point;
}





