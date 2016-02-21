//
//  nuiDebugServer.h
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once
#include "nui.h"


class nuiDebugServer : nuiTCPServer
{
public:
  nuiDebugServer();
  virtual ~nuiDebugServer();
  
  void Start(int port);
  void Stop();
  
private:

  nuiTCPClient* OnCreateClient(nuiSocket::SocketType sock);
};