//
//  nuiMessageClient.hpp
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once
#include "nui.h"

class nuiMessageClient
{
public:
  nuiMessageClient(nuiTCPClient* pTCPClient);
  virtual ~nuiMessageClient() { delete mpTCPClient; }

  bool Post(const nuiMessage& rMessage);
  nuiMessage* Read();

private:
  bool Post(const nuiMessageData& rData);
  nuiTCPClient *mpTCPClient;
};

