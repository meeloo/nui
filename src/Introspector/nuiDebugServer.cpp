//
//  nuiDebugServer.cpp
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nuiDebugServer.h"
#include "nuiTCPClient.h"

class nuiDebugProtocol : public nuiProtocol
{
public:
  nuiDebugProtocol(nuiTCPClient* pClient)
  : nuiProtocol(pClient)
  {
    AddMethod("HelloWorld",
    [=]()
    {
      printf("Hello world!\n");
    });
  }
  
};

class nuiDebugClient : public nuiTCPClient
{
public:
  nuiDebugClient(nuiDebugServer *pServer, nuiSocket::SocketType sock)
  : nuiTCPClient(sock), mpServer(pServer)
  {
    mpProtocol = new nuiDebugProtocol(this);
  }
  
  void OnCanRead()
  {
    mpProtocol->HandleMessagesAsync();
  }
  
protected:
  nuiDebugServer* mpServer = nullptr;
  nuiProtocol *mpProtocol = nullptr;
};

///////////////////////////////// Debug Server
nuiDebugServer::nuiDebugServer()
{
  
}

nuiDebugServer::~nuiDebugServer()
{
  
}

void nuiDebugServer::Start(int16 port)
{
  if (Bind("127.0.0.1", port))
  {
    bool res = Listen();
    if (res)
    {
      mSocketPool.Add(this, nuiSocketPool::eStateChange);
      mRunning = true;
      
      mpThread = new nglThreadFunction([=](){
        while (mRunning)
        {
          mSocketPool.DispatchEvents(100);
        }
        
        mSocketPool.Del(this);
        Close();
        delete mpThread;
        mpThread = nullptr;
      });
      
      mpThread->Start();
    }
  }
}

void nuiDebugServer::Stop()
{
  mRunning = false;
}

nuiTCPClient* nuiDebugServer::OnCreateClient(nuiSocket::SocketType sock)
{
  return new nuiDebugClient(this, sock);
}

