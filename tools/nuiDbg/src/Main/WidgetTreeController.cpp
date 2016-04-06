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

class LayerTreeNode : public nuiTreeNode
{
public:
  LayerTreeNode(nuiDbgLayer* pLayer)
  : nuiTreeNode(pLayer->GetObjectName(), false, false, true), mpLayer(pLayer)
  {

  }

private:
  nuiDbgLayer* mpLayer = nullptr;
};

class WindowTreeNode : public nuiTreeNode
{
public:
  WindowTreeNode(nuiDbgWindow* pWindow)
  : nuiTreeNode(pWindow->GetObjectName(), false, false, true, true), mpWindow(pWindow)
  {

  }

  void Open(bool opened)
  {
    if (opened)
    {
      Clear();
      auto layers(mpWindow->GetLayers());
      for (auto layer : layers)
      {
        LayerTreeNode* pLayer = new LayerTreeNode(layer);
        AddChild(pLayer);
      }
    }
    else
    {
      Clear();
      nuiTreeNode::Open(opened);
    }
  }

private:
  nuiDbgWindow* mpWindow = nullptr;
};

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
      nuiTreeNode* pNode = new WindowTreeNode(window);
      pRoot->AddChild(pNode);
    }

    mpWindows->SetTree(pRoot);
  });
}

