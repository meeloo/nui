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
  mpClient->CanRead.Connect([&]{
    mpClient->CanRead.DisconnectFunctions();
    StateChanged(GetState());
    mpMessageClient = new nuiMessageClient(mpClient);
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
