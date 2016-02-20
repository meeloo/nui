//
//  nuiMessageData.cpp
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nui.h"

///////////////////////////////// MessageData
uint8 nui_internal_datasizes[] = {
  sizeof(uint32), //  nuiMessageDataTypeString,
  sizeof(uint32), //  nuiMessageDataTypeBuffer,
  sizeof(int8),   //  nuiMessageDataTypeInt8,
  sizeof(int16),  //  nuiMessageDataTypeInt16,
  sizeof(int32),  //  nuiMessageDataTypeInt32,
  sizeof(int64),  //  nuiMessageDataTypeInt64,
  sizeof(uint8),  //  nuiMessageDataTypeUInt8,
  sizeof(uint16), //  nuiMessageDataTypeUInt16,
  sizeof(uint32), //  nuiMessageDataTypeUInt32,
  sizeof(uint64), //  nuiMessageDataTypeUInt64,
  sizeof(float),  //  nuiMessageDataTypeFloat,
  sizeof(double), //  nuiMessageDataTypeDouble
};

nuiMessageData::nuiMessageData(const nglString& rString)
{
  mType = nuiMessageDataTypeString;
  if (!rString.IsNull())
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

nuiMessageData::nuiMessageData(const void* ptr, size_t size)
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

nuiMessageData::nuiMessageData(int8 value)
{
  mType = nuiMessageDataTypeInt8;
  mValue._int8 = value;
}

nuiMessageData::nuiMessageData(int16 value)
{
  mType = nuiMessageDataTypeInt16;
  mValue._int16 = value;
}

nuiMessageData::nuiMessageData(int32 value)
{
  mType = nuiMessageDataTypeInt32;
  mValue._int32 = value;
}

nuiMessageData::nuiMessageData(int64 value)
{
  mType = nuiMessageDataTypeInt64;
  mValue._int64 = value;
}

nuiMessageData::nuiMessageData(uint8 value)
{
  mType = nuiMessageDataTypeUInt8;
  mValue._uint8 = value;
}

nuiMessageData::nuiMessageData(uint16 value)
{
  mType = nuiMessageDataTypeUInt16;
  mValue._uint16 = value;
}

nuiMessageData::nuiMessageData(uint32 value)
{
  mType = nuiMessageDataTypeUInt32;
  mValue._uint32 = value;
}

nuiMessageData::nuiMessageData(uint64 value)
{
  mType = nuiMessageDataTypeUInt64;
  mValue._uint64 = value;
}

nuiMessageData::nuiMessageData(float value)
{
  mType = nuiMessageDataTypeFloat;
  mValue._float = value;
}

nuiMessageData::nuiMessageData(double value)
{
  mType = nuiMessageDataTypeDouble;
  mValue._double = value;
}

nuiMessageData::nuiMessageData(nuiMessageDataType type, void* data, size_t size)
: mType(type)
{
  NGL_ASSERT(mType < nuiMessageDataTypeLast);
  switch (mType) {
    case nuiMessageDataTypeBuffer:
    {
      if (!data)
      {
        mValue.Pointer._ptr = new uint8[size];
        mValue.Pointer._size = size;
        memcpy(mValue.Pointer._ptr, data, size);
      }
      else
      {
        mValue.Pointer._ptr = nullptr;
        mValue.Pointer._size = 0;
      }
    }break;

    case nuiMessageDataTypeString:
    {
      if (data)
      {
        mValue.Pointer._ptr = new uint8[size];
        mValue.Pointer._size = size;
        memcpy(mValue.Pointer._ptr, data, size);
      }
      else
      {
        mValue.Pointer._ptr = nullptr;
        mValue.Pointer._size = 0;
      }
    }break;


    default:
    {
      memcpy(&mValue, data, nui_internal_datasizes[mType]);
    }break;
  }

}

nuiMessageData::nuiMessageData(const nuiMessageData& rData)
: mType(rData.mType)
{
  NGL_ASSERT(mType < nuiMessageDataTypeLast);
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

nuiMessageData::nuiMessageData(nuiMessageData&& rData)
: mType(rData.mType)
{
  NGL_ASSERT(mType < nuiMessageDataTypeLast);
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

nuiMessageData& nuiMessageData::operator= (nuiMessageData&& rData)
{
  mType = rData.mType;
  NGL_ASSERT(mType < nuiMessageDataTypeLast);
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

nuiMessageData::~nuiMessageData()
{
  if (mType == nuiMessageDataTypeBuffer || mType == nuiMessageDataTypeString)
  {
    delete[] mValue.Pointer._ptr;
  }
}

