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
: nuiProtocol(nullptr), mEventSink(this)
{
  AddMethod("NewWindow",
            [=](uint64 pointer, nglString name) )
            {
              NGL_OUT("Remote: New Window: 0x%x / %s\n", pointer, name.GetChars());
            });
  

  
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

    mpMessageClient->Post(nuiMessage("UpdateWindowList", 1));
  });

  mpClient->ReadClosed.Connect([&]{
    Disconnect();
  });

  mpClient->ConnectError.Connect([&]{
    Disconnect();
  });
  
  mpClient->Connect(rAddress, port, &mSocketPool, nuiSocketPool::eContinuous);
}

void nuiDebugger::Disconnect()
{
  delete mpMessageClient;
  mpMessageClient = nullptr;
  mpClient = nullptr;
  StateChanged(GetState());
}


nuiDebugger::State nuiDebugger::GetState() const
{
  if (!mpClient)
    return eDbgState_Offline;

  if (mpClient->IsConnected())
    return eDbgState_Connected;

  return eDbgState_Connecting;
}
