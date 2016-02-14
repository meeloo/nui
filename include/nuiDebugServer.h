//
//  nuiDebugServer.h
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once
#include "nui.h"

enum nuiMessageDataType
{
  nuiMessageDataTypeString = 0,
  nuiMessageDataTypeBuffer,
  nuiMessageDataTypeInt8,
  nuiMessageDataTypeInt16,
  nuiMessageDataTypeInt32,
  nuiMessageDataTypeInt64,
  nuiMessageDataTypeUInt8,
  nuiMessageDataTypeUInt16,
  nuiMessageDataTypeUInt32,
  nuiMessageDataTypeUInt64,
  nuiMessageDataTypeFloat,
  nuiMessageDataTypeDouble,

  nuiMessageDataTypeLast
};


struct nuiMessageData
{
  nuiMessageData(const nglString& rString);
  nuiMessageData(const void* ptr, size_t size);
  nuiMessageData(int8 value);
  nuiMessageData(int16 value);
  nuiMessageData(int32 value);
  nuiMessageData(int64 value);
  nuiMessageData(uint8 value);
  nuiMessageData(uint16 value);
  nuiMessageData(uint32 value);
  nuiMessageData(uint64 value);
  nuiMessageData(float value);
  nuiMessageData(double value);
  nuiMessageData(nuiMessageDataType type, void* data, size_t size);
  nuiMessageData(const nuiMessageData& rData);
  nuiMessageData(nuiMessageData&& rData);
  nuiMessageData& operator= (nuiMessageData&& rData);
  ~nuiMessageData();



  nuiMessageDataType mType;
  union
  {
    int8 _int8;
    int16 _int16;
    int32 _int32;
    int64 _int64;
    uint8 _uint8;
    uint16 _uint16;
    uint32 _uint32;
    uint64 _uint64;
    float _float;
    double _double;

    struct
    {
      uint8* _ptr;
      size_t _size;
    } Pointer;
  } mValue;
};


class nuiMessage
{
public:
  nuiMessage();
  virtual ~nuiMessage();

  void Add(const nglString& rString) { mData.push_back(rString); }
  void Add(const void* ptr, size_t size) { mData.push_back(nuiMessageData(ptr, size)); }
  void Add(int8 value) { mData.push_back(value); }
  void Add(int16 value) { mData.push_back(value); }
  void Add(int32 value) { mData.push_back(value); }
  void Add(int64 value) { mData.push_back(value); }
  void Add(uint8 value) { mData.push_back(value); }
  void Add(uint16 value) { mData.push_back(value); }
  void Add(uint32 value) { mData.push_back(value); }
  void Add(uint64 value) { mData.push_back(value); }
  void Add(float value) { mData.push_back(value); }
  void Add(double value) { mData.push_back(value); }
  void Add(const nuiMessageData& rData) { mData.push_back(rData); }

  size_t GetSize() const { return mData.size(); }

  const nuiMessageData& GetData(size_t index) const { return mData[index]; }

  nglString GetDescription() const;
private:
  std::vector<nuiMessageData> mData;
};

class nuiMessageParser
{
public:
  nuiMessageParser();
  virtual ~nuiMessageParser();
  nuiMessage* Parse(const std::vector<uint8>& rData);
  std::vector<uint8> Build(nuiMessage* pMessage);
  bool Build(const nuiMessageData& rData, std::vector<uint8>& rOut);
private:
  enum State
  {
    Waiting,
    ReadType,
    ReadSize,
    ReadData
  };
  State mState = Waiting;
  uint32 mRemainingDataChunks = 0;
  uint32 mCurrentChunkSize = 0;
  nuiMessageDataType mType;
  std::vector<uint8> mChunck;
  nuiMessage* mpCurrentMessage = nullptr;
};


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