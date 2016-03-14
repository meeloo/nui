//
//  Debugger.h
//  nuiDbg
//
//  Created by Sebastien Metrot on 28/02/2016.
//
//

#include "nui.h"

#pragma once

typedef uint64 nuiDbgID;

class nuiDbgObject : public nuiObject
{
public:
  nuiDbgObject(nuiDbgID ID, const nglString& rName = nglString::Null)
  : nuiObject(rName), mID(ID)
  {
      AddAttribute(nuiMakeAttribute(this, mID, "ID"));
  }


  NUI_GETSET(nuiDbgID, ID);
private:
  nuiDbgID mID = 0;
};


class nuiDbgLayer : public nuiDbgObject
{
public:
  nuiDbgLayer(nuiDbgID ID, const nglString& rName)
  : nuiDbgObject(ID, rName)
  {

  }
};

class nuiDbgWindow : public nuiDbgObject
{
public:
  nuiDbgWindow(nuiDbgID ID, const nglString& rName)
  : nuiDbgObject(ID, rName)
  {

  }
};


class nuiDebugger : public nuiProtocol
{
public:
  enum State
  {
    eDbgState_Offline,
    eDbgState_Connecting,
    eDbgState_Connected
  };
  

  nuiDebugger();
  virtual ~nuiDebugger();

  void Connect(const nglString& rAddress, int16 port); // "127.0.0.1", 31337
  void Disconnect();

  void UpdateWindowList();
  void UpdateLayerList(uint64 window);

  State GetState() const;

  nuiSignal1<State> StateChanged;
private:
  nuiEventSink<nuiDebugger> mEventSink;
  nuiSocketPool mSocketPool;
  nuiMessageClient *mpMessageClient = nullptr;



};
