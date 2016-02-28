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
: nuiMainWindow(rContextInfo, rInfo, NULL, nglPath((_T("../data")))),
mWinSink(this)
{
    
    //nuiTopLevel::EnablePartialRedraw(false);
    SetDebugMode(true);
    
    InitAttributes();
    
    LoadCSS(_T("rsrc:/css/nuiDbg.css"));
}



void MainWindow::InitAttributes()
{
    mpAttributeMouseCoord = new nuiAttribute<nuiPoint>
    (nglString(_T("Mouse Coordinates")), nuiUnitPixels,
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
    nuiWidget* pMainCont = new nuiWidget();
    pMainCont->SetObjectName(_T("MainContainer"));
    AddChild(pMainCont);
    
    nuiTabView* pTabView = new nuiTabView(nuiTop);
    pMainCont->AddChild(pTabView);
    
    pTabView->SetChangeOnDrag(true);
    
    nuiLabel* pLabel = new nuiLabel(nuiTR("Debugger"));
    pLabel->SetObjectName(_T("TabView::Title"));
//    ProjectGenerator* pGenerator = new ProjectGenerator();
    nuiWidget* pDebugger = new nuiWidget();
    pTabView->AddTab(pLabel, pDebugger);
    
    if (GetApp()->IsFullVersion())
    {
        pLabel = new nuiLabel(nuiTR("Element Editor"));
        pLabel->SetObjectName(_T("TabView::Title"));
//        pTabView->AddTab(pLabel, new ElementEditorGui());
    }
    
    pTabView->SelectTab(0);
    
    // nui version number
    nglString version;
    version.Format(_T("nui %d.%d r%d %ls"), NUI_VERSION_MAJOR, NUI_VERSION_MINOR, NUI_VERSION_RELEASE, NUI_VERSION_BETA.GetChars());
    pLabel = new nuiLabel(version);
    pLabel->SetPosition(nuiTopRight);
    pLabel->SetObjectName(_T("VersionNumber"));
    pMainCont->AddChild(pLabel);
  
  GetDebugger().Connect("127.0.0.1", 31337);
}



bool MainWindow::LoadCSS(const nglPath& rPath)
{
    nglIStream* pF = rPath.OpenRead();
    if (!pF)
    {
        NGL_OUT(_T("Unable to open CSS source file '%ls'\n"), rPath.GetChars());
        return false;
    }
    
    nuiCSS* pCSS = new nuiCSS();
    bool res = pCSS->Load(*pF, rPath);
    
    if (res)
    {
        SetCSS(pCSS);
        return true;
    }
    
    NGL_OUT(_T("%ls\n"), pCSS->GetErrorString().GetChars());
    
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





