//
//  nuiDebugServer.cpp
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nuiDebugServer.h"
#include "nuiTCPClient.h"

class nuiDebugClient : public nuiTCPClient
{
public:
  nuiDebugClient(nuiDebugServer *pServer, nuiSocket::SocketType sock)
  : nuiTCPClient(sock), mpServer(pServer)
  {
  }
  
protected:
  nuiDebugServer* mpServer;
};

///////////////////////////////// Debug Server
nuiDebugServer::nuiDebugServer()
{
  
}

nuiDebugServer::~nuiDebugServer()
{
  
}

void nuiDebugServer::Start(int port)
{
  if (Bind("127.0.0.1", 1337))
  {
    bool res = Listen();
  }
}

void nuiDebugServer::Stop()
{
  
}

nuiTCPClient* nuiDebugServer::OnCreateClient(nuiSocket::SocketType sock)
{
  return new nuiDebugClient(this, sock);
}

