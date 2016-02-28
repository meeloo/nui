//
//  Debugger.h
//  nuiDbg
//
//  Created by Sebastien Metrot on 28/02/2016.
//
//

#include "nui.h"

#pragma once

class nuiDebugger
{
public:
    nuiDebugger();
    virtual ~nuiDebugger();
    
    void Connect(const nglString& rAddress, int16 port); // "127.0.0.1", 31337
    void Disconnect();
    
private:
    nuiTCPClient* mpClient = nullptr;
    nuiEventSink<nuiDebugger> mEventSink;
    nuiSocketPool mSocketPool;
    nuiMessageClient *mpMessageClient = nullptr;
};
