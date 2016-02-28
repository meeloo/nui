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
  BindChild("Connect", mpConnect);
  BindChild("Server", mpServer);
  BindChild("Port", mpPort);
  BindChild("Port", mpTruc);
  BindChild("Prort", mpBidule);

  if (mpConnect)
  {
    mEventSink.Connect(mpConnect->Activated, [=](const nuiEvent& rEvent){
      printf("Connect!\n");
    });
  }
}