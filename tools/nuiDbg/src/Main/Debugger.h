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

class nuiDebugger;

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

private:
  friend nuiDebugger;
};

class nuiDbgWindow : public nuiDbgObject
{
public:
  nuiDbgWindow(nuiDbgID ID, const nglString& rName)
  : nuiDbgObject(ID, rName)
  {

  }

  std::vector<nuiRef<nuiDbgLayer> > GetLayers() const
  {
    std::vector<nuiRef<nuiDbgLayer> > layers;
    layers.reserve(mLayers.size());
    for (auto layer : mLayers)
    {
      layers.push_back(layer.second);
    }

    return layers;
  }

  nuiSignal0<> LayerListChanged;
private:
  friend nuiDebugger;
  std::map<nuiDbgID, nuiRef<nuiDbgLayer> > mLayers;

  void AddLayer(nuiDbgLayer* pLayer)
  {
    mLayers[pLayer->GetID()] = pLayer;
    LayerListChanged();
  }

  void DelLayer(nuiDbgLayer* pLayer)
  {
    auto it = mLayers.find(pLayer->GetID());
    if (it != mLayers.end())
      mLayers.erase(it);
    LayerListChanged();
  }

  void ClearLayers()
  {
    mLayers.clear();
    LayerListChanged();
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

  std::vector<nuiRef<nuiDbgWindow> > GetWindows() const
  {
    std::vector<nuiRef<nuiDbgWindow> > Windows;
    Windows.reserve(mWindows.size());
    for (auto Window : mWindows)
    {
      Windows.push_back(Window.second);
    }

    return Windows;
  }

  nuiSignal1<State> StateChanged;
  nuiSignal0<> WindowListChanged;


private:
  nuiEventSink<nuiDebugger> mEventSink;
  nuiSocketPool mSocketPool;
  nuiMessageClient *mpMessageClient = nullptr;

  std::map<nuiDbgID, nuiRef<nuiDbgWindow> > mWindows;

  void AddWindow(nuiDbgWindow* pWindow)
  {
    mWindows[pWindow->GetID()] = pWindow;
    WindowListChanged();
    UpdateLayerList(pWindow->GetID());
  }

  void DelWindow(nuiDbgWindow* pWindow)
  {
    auto it = mWindows.find(pWindow->GetID());
    if (it != mWindows.end())
    {
      mWindows.erase(it);
      WindowListChanged();
    }
  }

  void ClearWindows()
  {
    mWindows.clear();
    WindowListChanged();
  }

};
