//
//  ConfigViewController.cpp
//  nuiDbg
//
//  Created by Sebastien Metrot on 28/02/16.
//
//

#include "nui.h"
#include "Main/nuiDbg.h"
#include "Main/ConfigViewController.h"

ConfigViewController::ConfigViewController()
: mEventSink(this)
{
}

ConfigViewController::~ConfigViewController()
{
}

void ConfigViewController::Built()
{
  nuiButton* pConnect = (nuiButton*)SearchForChild("Connect");
  if (pConnect)
  {
    mEventSink.Connect(pConnect->Activated, [=](const nuiEvent& rEvent){
      printf("Connect!\n");
    });
  }
}