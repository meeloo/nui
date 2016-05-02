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
  // Windows:
  AddMethod("NewWindow", nui_make_function(
            [=](uint64 pointer, nglString name)
            {
              NGL_OUT("Remote: New Window: 0x%x / %s\n", pointer, name.GetChars());
              AddWindow(new nuiDbgWindow(pointer, name));
              UpdateLayerList(pointer);
            }));
  
  AddMethod("StartWindowList", nui_make_function(
            [=](int32 count)
  {
    NGL_OUT("Remote: Start window list (%d windows)\n", count);
    ClearWindows();
  }));

  AddMethod("WindowListDone", nui_make_function(
            [=]()
  {
    NGL_OUT("Remote: Window list done\n");
  }));



  // Layers:
  AddMethod("NewLayer", nui_make_function(
                                           [=](uint64 window, uint64 pointer, nglString name, double time, int64 count)
                                           {
                                             NGL_OUT("Remote: New Layer: 0x%x / %s (%f / %d) (window = %p)\n", pointer, name.GetChars(), time, count, window);
                                             auto windowit = mWindows.find(window);
                                             if (windowit != mWindows.end())
                                             {
                                               nuiDbgWindow* pWindow = windowit->second;
                                               pWindow->AddLayer(new nuiDbgLayer(pointer, name));
                                             }
                                           }));

  AddMethod("StartLayerList", nui_make_function(
                                                 [=](uint64 window, int32 count)
                                                 {
                                                   NGL_OUT("Remote: Start layer list (%d layers) for window: %p\n", count, window);
                                                   auto windowit = mWindows.find(window);
                                                   if (windowit != mWindows.end())
                                                   {
                                                     nuiDbgWindow* pWindow = windowit->second;
                                                     pWindow->ClearLayers();
                                                   }
                                                 }));
  
  
  AddMethod("WindowLayerDone", nui_make_function(
                                                [=](uint64 window)
                                                {
                                                  NGL_OUT("Remote: Layer list done (window = %p)\n", window);
                                                }));
  


  //////////////////////////////////////////////////////////////////////
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
      HandleMessages([&](nuiMessage& message, nuiProtocolFunctionBase* pFunction){
        NGL_OUT("Debugger Error. '%s' and '%s' not compatibles\n", message.GetDescription().GetChars(), pFunction->GetDescription().GetChars());
        return false;
      });
    });

    pClient->ReadClosed.Connect([&]{
      Disconnect();
    });
    
    pClient->ConnectError.Connect([&]{
      Disconnect();
    });
    
    UpdateWindowList();
  });

  pClient->Connect(rAddress, port, &mSocketPool, nuiSocketPool::eStateChange);
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

void nuiDebugger::UpdateWindowList()
{
  mpMessageClient->Post(nuiMessage("UpdateWindowList"));
}

void nuiDebugger::UpdateLayerList(uint64 window)
{
  mpMessageClient->Post(nuiMessage("UpdateLayerList", window));
}


