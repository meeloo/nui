//
//  nuiMessageData.h
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
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

