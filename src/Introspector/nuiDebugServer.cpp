//
//  nuiDebugServer.cpp
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nuiDebugServer.h"

class nuiMessage
{
public:
  nuiMessage();
  virtual ~nuiMessage();
  
  void Add(const nglString& rString);
  void Add(int8 value);
  void Add(int16 value);
  void Add(int32 value);
  void Add(int64 value);
  void Add(uint8 value);
  void Add(uint16 value);
  void Add(uint32 value);
  void Add(uint64 value);
  void Add(const nglString& rName, int8 value);
  void Add(const nglString& rName, int16 value);
  void Add(const nglString& rName, int32 value);
  void Add(const nglString& rName, int64 value);
  void Add(const nglString& rName, uint8 value);
  void Add(const nglString& rName, uint16 value);
  void Add(const nglString& rName, uint32 value);
  void Add(const nglString& rName, uint64 value);
  
};

nuiDebugServer::nuiDebugServer()
{
  
}

nuiDebugServer::~nuiDebugServer()
{
  
}

void nuiDebugServer::Start(int port)
{
  
}

void nuiDebugServer::Stop()
{
  
}

