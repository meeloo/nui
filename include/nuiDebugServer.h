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
  nuiMessageDataTypeString,
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
  nuiMessageDataTypeDouble
};


struct nuiMessageData
{
  nuiMessageData(const nglString& rString)
  {
    mType = nuiMessageDataTypeString;
    if (rString.IsNull())
    {
      mValue.Pointer._ptr = new uint8[rString.GetLength()];
      mValue.Pointer._size = rString.GetLength();
      memcpy(mValue.Pointer._ptr, rString.GetChars(), mValue.Pointer._size);
    }
    else
    {
      mValue.Pointer._ptr = nullptr;
      mValue.Pointer._size = 0;
    }
  }

  nuiMessageData(const void* ptr, size_t size)
  {
    mType = nuiMessageDataTypeBuffer;
    if (ptr)
    {
      mValue.Pointer._ptr = new uint8[size];
      mValue.Pointer._size = size;
      memcpy(mValue.Pointer._ptr, ptr, size);
    }
    else
    {
      mValue.Pointer._ptr = nullptr;
      mValue.Pointer._size = 0;
    }
  }

  nuiMessageData(int8 value)
  {
    mType = nuiMessageDataTypeInt8;
    mValue._int8 = value;
  }

  nuiMessageData(int16 value)
  {
    mType = nuiMessageDataTypeInt16;
    mValue._int16 = value;
  }

  nuiMessageData(int32 value)
  {
    mType = nuiMessageDataTypeInt32;
    mValue._int32 = value;
  }

  nuiMessageData(int64 value)
  {
    mType = nuiMessageDataTypeInt64;
    mValue._int64 = value;
  }

  nuiMessageData(uint8 value)
  {
    mType = nuiMessageDataTypeUInt8;
    mValue._uint8 = value;
  }

  nuiMessageData(uint16 value)
  {
    mType = nuiMessageDataTypeUInt16;
    mValue._uint16 = value;
  }

  nuiMessageData(uint32 value)
  {
    mType = nuiMessageDataTypeUInt32;
    mValue._uint32 = value;
  }

  nuiMessageData(uint64 value)
  {
    mType = nuiMessageDataTypeUInt64;
    mValue._uint64 = value;
  }

  nuiMessageData(float value)
  {
    mType = nuiMessageDataTypeFloat;
    mValue._float = value;
  }

  nuiMessageData(double value)
  {
    mType = nuiMessageDataTypeDouble;
    mValue._double = value;
  }


  nuiMessageData(const nuiMessageData& rData)
  : mType(rData.mType)
  {
    if (rData.mType == nuiMessageDataTypeBuffer || rData.mType == nuiMessageDataTypeString)
    {
      mValue.Pointer._ptr = new uint8[rData.mValue.Pointer._size];
      mValue.Pointer._size = rData.mValue.Pointer._size;
      memcpy(mValue.Pointer._ptr, rData.mValue.Pointer._ptr, mValue.Pointer._size);
    }
    else
    {
      mValue = rData.mValue;
    }
  }

  nuiMessageData(nuiMessageData&& rData)
  : mType(rData.mType)
  {
    if (rData.mType == nuiMessageDataTypeBuffer || rData.mType == nuiMessageDataTypeString)
    {
      mValue.Pointer._ptr = rData.mValue.Pointer._ptr;
      rData.mValue.Pointer._ptr = nullptr;
      mValue.Pointer._size = rData.mValue.Pointer._size;
    }
    else
    {
      mValue = rData.mValue;
    }
  }

  nuiMessageData& operator= (nuiMessageData&& rData)
  {
    mType = rData.mType;
    if (rData.mType == nuiMessageDataTypeBuffer || rData.mType == nuiMessageDataTypeString)
    {
      mValue.Pointer._ptr = rData.mValue.Pointer._ptr;
      rData.mValue.Pointer._ptr = nullptr;
      mValue.Pointer._size = rData.mValue.Pointer._size;
    }
    else
    {
      mValue = rData.mValue;
    }
  }

  ~nuiMessageData()
  {
    if (mType == nuiMessageDataTypeBuffer || mType == nuiMessageDataTypeString)
    {
      delete[] mValue.Pointer._ptr;
    }
  }



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

  size_t GetSize() const { return mData.size(); }

  const nuiMessageData& GetData(size_t index) const { return mData[index]; }

private:
  std::vector<nuiMessageData> mData;
};

class nuiMessageClient
{
public:
  nuiMessageClient(nuiTCPClient* pTCPClient);

  bool Post(const nuiMessage& rMessage);
  nuiMessage* Read();

private:
  bool Post(const nuiMessageData& rData);
  nuiTCPClient *mpTCPClient;
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