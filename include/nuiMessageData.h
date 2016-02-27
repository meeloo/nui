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

  operator nglString () const { return nglString((nglChar*)mValue.Pointer._ptr, (int32)mValue.Pointer._size); }
  operator int8() const { return mValue._int8; }
  operator int16() const { return mValue._int16; }
  operator int32() const { return mValue._int32; }
  operator int64() const { return mValue._int64; }
  operator uint8() const { return mValue._uint8; }
  operator uint16() const { return mValue._uint16; }
  operator uint32() const { return mValue._uint32; }
  operator uint64() const { return mValue._uint64; }
  operator float() const { return mValue._float; }
  operator double() const { return mValue._double; }
};

template <typename T>
struct nuiMessageDataTypeTrait
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeLast;
  }
  
  static nglString GetName()
  {
    return "???";
  }
};

template <>
struct nuiMessageDataTypeTrait<nglString>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeString;
  }
  
  static nglString GetName()
  {
    return "String";
  }
};

template <>
struct nuiMessageDataTypeTrait<const nglString&>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeString;
  }
  
  static nglString GetName()
  {
    return "String";
  }
};

template <>
struct nuiMessageDataTypeTrait<int8>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeInt8;
  }
  
  static nglString GetName()
  {
    return "int8";
  }
};

template <>
struct nuiMessageDataTypeTrait<int16>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeInt16;
  }
  
  static nglString GetName()
  {
    return "int16";
  }
};

template <>
struct nuiMessageDataTypeTrait<int32>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeInt32;
  }
  
  static nglString GetName()
  {
    return "int32";
  }
};

template <>
struct nuiMessageDataTypeTrait<int64>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeInt64;
  }
  
  static nglString GetName()
  {
    return "int64";
  }
};

template <>
struct nuiMessageDataTypeTrait<uint8>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeUInt8;
  }
  
  static nglString GetName()
  {
    return "uint8";
  }
};

template <>
struct nuiMessageDataTypeTrait<uint16>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeUInt16;
  }
  
  static nglString GetName()
  {
    return "uint16";
  }
};

template <>
struct nuiMessageDataTypeTrait<uint32>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeUInt32;
  }
  
  static nglString GetName()
  {
    return "uint32";
  }
};

template <>
struct nuiMessageDataTypeTrait<uint64>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeUInt64;
  }
  
  static nglString GetName()
  {
    return "uint64";
  }
};

template <>
struct nuiMessageDataTypeTrait<float>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeFloat;
  }
  
  static nglString GetName()
  {
    return "float";
  }
};

template <>
struct nuiMessageDataTypeTrait<double>
{
  static nuiMessageDataType GetType()
  {
    return nuiMessageDataTypeDouble;
  }
  
  static nglString GetName()
  {
    return "double";
  }
};

