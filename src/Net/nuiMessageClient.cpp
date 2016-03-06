//
//  nuiMessageClient.cpp
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nui.h"

///////////////////////////////// MessageClient
// class nuiMessageClient
nuiMessageClient::nuiMessageClient(nuiTCPClient* pTCPClient)
: mpTCPClient(pTCPClient)
{

}

bool nuiMessageClient::Post(const nuiMessage& rMessage)
{
  mOutData.clear();
  mOutData = mParser.Build(rMessage);
  size_t size = mOutData.size();
  return (size != mpTCPClient->Send(&mOutData[0], size));
}

void nuiMessageClient::Read(std::function<bool(nuiMessage*)> onNewMessage)
{
  mInData.clear();
  mpTCPClient->ReceiveAvailable(mInData);
  mParser.Parse(mInData, onNewMessage);
}

void nuiMessageClient::SetClient(nuiTCPClient* pTCPClient)
{
  if (mpTCPClient == pTCPClient)
    return;
  mpTCPClient = pTCPClient;
}

