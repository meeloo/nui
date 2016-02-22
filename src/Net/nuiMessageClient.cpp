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

nuiMessage* nuiMessageClient::Read()
{
  mInData.clear();
  mpTCPClient->ReceiveAvailable(mInData);
  nuiMessage* pMessage = mParser.Parse(mInData);
  return pMessage;
}

void nuiMessageClient::SetClient(nuiTCPClient* pTCPClient)
{
  mpTCPClient = pTCPClient;
}

