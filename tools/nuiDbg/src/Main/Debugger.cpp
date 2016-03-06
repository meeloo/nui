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
  if (GetState() != eDbgState_Offline)
    Disconnect();

  nuiTCPClient* pClient = new nuiTCPClient();
  SetClient(pClient);
  pClient->Connected.Connect([=]{
    mpMessageClient = new nuiMessageClient(pClient);
    StateChanged(GetState());

    pClient->CanRead.Connect([&]{
      HandleMessages([&](nuiMessage&, nuiProtocolFunctionBase*){
        NGL_OUT("Error\n");
        return false;
      });
    });

    pClient->ReadClosed.Connect([&]{
      Disconnect();
    });
    
    pClient->ConnectError.Connect([&]{
      Disconnect();
    });
    
    mpMessageClient->Post(nuiMessage("UpdateWindowList"));
  });

  pClient->Connect(rAddress, port, &mSocketPool, nuiSocketPool::eContinuous);
}

void nuiDebugger::Disconnect()
{
  delete mpMessageClient;
  mpMessageClient = nullptr;
  SetClient(nullptr);
  StateChanged(GetState());
}


nuiDebugger::State nuiDebugger::GetState() const
{
  if (!mpTCPClient)
    return eDbgState_Offline;

  if (mpTCPClient->IsConnected())
    return eDbgState_Connected;

  return eDbgState_Connecting;
}
