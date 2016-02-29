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
  BindChild("ConnectLabel", mpConnectLabel);
  BindChild("Server", mpServer);
  BindChild("Port", mpPort);

  if (mpConnect)
  {
    mEventSink.Connect(mpConnect->Activated, [=](const nuiEvent& rEvent){
      GetDebugger().Disconnect();
      
      if (GetDebugger().GetState() == nuiDebugger::eDbgState_Connecting)
      {
        return;
      }
      
      nglString server = mpServer->GetText();
      int16 port = mpPort->GetText().GetInt();
      printf("Connect %s:%d!\n", server.GetChars(), port);
      GetDebugger().Connect(server, port);
    });
    
    GetDebugger().StateChanged.Connect([&](nuiDebugger::State state){
      switch (state)
      {
        case nuiDebugger::eDbgState_Connected:
        {
          mpServer->SetEnabled(false);
          mpPort->SetEnabled(false);
          mpConnectLabel->SetText("Disconnect");
        }break;
        case nuiDebugger::eDbgState_Offline:
        {
          mpServer->SetEnabled(true);
          mpPort->SetEnabled(true);
          mpConnectLabel->SetText("Connect");
        }break;
        case nuiDebugger::eDbgState_Connecting:
        {
          mpServer->SetEnabled(false);
          mpPort->SetEnabled(false);
          mpConnectLabel->SetText("Stop");
        }break;
          
      }
    });
  }
}
