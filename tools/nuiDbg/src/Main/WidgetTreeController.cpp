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

  mSink.Connect(update->Activated, [=](const nuiEvent& rEvent){
    GetDebugger().UpdateWindowList();
  });
}

