//
//  Debugger.cpp
//  nuiDbg
//
//  Created by Sebastien Metrot on 28/02/2016.
//
//


#pragma once

#include "nui.h"
#include "Main/Debugger.h"

nuiDebugger::nuiDebugger()
: mEventSink(this)
{
}

nuiDebugger::~nuiDebugger()
{
  Disconnect();
}

void nuiDebugger::Connect(const nglString& rAddress, int16 port)
{
  Disconnect();
  
  mpClient = new nuiTCPClient();
  if (mpClient->Connect(rAddress, port))
  {
    StateChanged(GetState());
    mSocketPool.Add(mpClient, nuiSocketPool::eContinuous);
    mpMessageClient = new nuiMessageClient(mpClient);
    mEventSink.Connect(nuiAnimation::GetTimer()->Tick, [=](const nuiEvent& rEvent)
                       {
                         mSocketPool.DispatchEvents(1);
                       });
  }
  else
  {
    delete mpClient;
    mpClient = nullptr;
    StateChanged(GetState());
  }
}

void nuiDebugger::Disconnect()
{
  mEventSink.DisconnectAll();
  delete mpMessageClient;
  delete mpClient;
  mpMessageClient = nullptr;
  mpClient = nullptr;
}


nuiDebugger::State nuiDebugger::GetState() const
{
  if (!mpClient)
    return eDbgState_Offline;

  if (mpClient->IsConnected())
    return eDbgState_Connected;

  return eDbgState_Connecting;
}
