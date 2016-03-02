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
  mEventSink.Connect(nuiAnimation::GetTimer()->Tick, [=](const nuiEvent& rEvent)
                     {
                       mSocketPool.DispatchEvents(1);
                     });
}

nuiDebugger::~nuiDebugger()
{
  Disconnect();
}

void nuiDebugger::Connect(const nglString& rAddress, int16 port)
{
  Disconnect();

  mpClient = new nuiTCPClient();
  mpClient->Connected.Connect([&]{
    mpMessageClient = new nuiMessageClient(mpClient);
    StateChanged(GetState());
  });

  mpClient->ReadClosed.Connect([&]{
    mpClient->ReadClosed.DisconnectFunctions();
    delete mpClient;
    mpClient = nullptr;
    StateChanged(GetState());
  });

  mpClient->Connect(rAddress, port, &mSocketPool, nuiSocketPool::eContinuous);
}

void nuiDebugger::Disconnect()
{
  mEventSink.DisconnectAll();
  delete mpMessageClient;
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
