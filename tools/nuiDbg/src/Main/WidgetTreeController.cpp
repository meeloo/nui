//
//  WidgetTreeController.cpp
//  nuiDbg
//
//  Created by Sebastien Metrot on 06/03/2016.
//
//

#include "nui.h"
#include "Main/WidgetTreeController.h"
#include "Main/nuiDbg.h"

WidgetTreeController::WidgetTreeController()
: mSink(this)
{

}

WidgetTreeController::~WidgetTreeController()
{
    
}

void WidgetTreeController::Built()
{
  BindChild("Windows", mpWindows);
  nuiButton* update = BindChild<nuiButton>("UpdateWindows");
  mpWindows = BindChild<nuiTreeView>("Windows");

  mSink.Connect(update->Activated, [=](const nuiEvent& rEvent){
    GetDebugger().UpdateWindowList();
  });

  GetDebugger().WindowListChanged.Connect([&](){
    mpWindows->Clear();
    auto windows(GetDebugger().GetWindows());
    nuiTreeNode* pRoot = new nuiTreeNode("windows", true);
    for (auto window : windows)
    {
      nuiTreeNode* pNode = new nuiTreeNode(window->GetObjectName(), true, false);
      pRoot->AddChild(pNode);
    }

    mpWindows->SetTree(pRoot);
  });
}

