//
//  nuiDebugServer.h
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once
#include "nui.h"


class nuiProtocolClient : public nuiMessageClient
{
public:
  nuiProtocolClient(nuiTCPClient* pTCPClient);

  void AddMethod(const nglString& rMethodName);
  void AddMethod(const nglString& rMethodName, nuiMessageDataType type1);
  void AddMethod(const nglString& rMethodName, nuiMessageDataType type1, nuiMessageDataType type2);
  void AddMethod(const nglString& rMethodName, nuiMessageDataType type1, nuiMessageDataType type2, nuiMessageDataType type3);
  void AddMethod(const nglString& rMethodName, nuiMessageDataType type1, nuiMessageDataType type2, nuiMessageDataType type3, nuiMessageDataType type4);
  void AddMethod(const nglString& rMethodName, nuiMessageDataType type1, nuiMessageDataType type2, nuiMessageDataType type3, nuiMessageDataType type4, nuiMessageDataType type5);
  void AddMethod(const nglString& rMethodName, nuiMessageDataType type1, nuiMessageDataType type2, nuiMessageDataType type3, nuiMessageDataType type4, nuiMessageDataType type5, nuiMessageDataType type6);

  
};

class nuiDebugServer : nuiTCPServer
{
public:
  nuiDebugServer();
  virtual ~nuiDebugServer();
  
  void Start(int port);
  void Stop();
  
private:

};