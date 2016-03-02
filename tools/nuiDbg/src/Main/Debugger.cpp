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
  AddMethod("NewWindow", nui_make_function(
            [=](uint64 pointer, nglString name)
            {
              NGL_OUT("Remote: New Window: 0x%x / %s\n", pointer, name.GetChars());
            }));
  
  AddMethod("StartWindowList", nui_make_function(
            [=](int32 count)
  {
    NGL_OUT("Remote: Start window list (%d windows)\n", count);
  }));

  
  AddMethod("WindowListDone", nui_make_function(
            [=]()
  {
    NGL_OUT("Remote: Window list done\n");
  }));
  
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

    mpMessageClient->Post(nuiMessage("UpdateWindowList"));
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
