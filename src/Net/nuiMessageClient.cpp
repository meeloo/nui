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
  size_t size = rMessage.GetSize();
  uint8 count = (uint8)size;
  if (1 != mpTCPClient->Send(&count, 1))
    return false;

  for (size_t index = 0; index < size; index++)
  {
    if (!Post(rMessage.GetData(index)))
      return false;
  }
  return true;
}

bool nuiMessageClient::Post(const nuiMessageData& rData)
{
  uint8 type = rData.mType;
  if (1 != mpTCPClient->Send(&type, 1))
    return false;

  switch (rData.mType)
  {
    case nuiMessageDataTypeString:
    case nuiMessageDataTypeBuffer:
    {
      uint32 size = cpu_to_le32(rData.mValue._int32);
      if (!(sizeof(size) == mpTCPClient->Send((uint8*)&size, sizeof(size))))
      {
        return false;
      }
      return rData.mValue.Pointer._size == mpTCPClient->Send(rData.mValue.Pointer._ptr, rData.mValue.Pointer._size);
    } break;
    case nuiMessageDataTypeInt8:
    {
      if (1 != mpTCPClient->Send((uint8*)&rData.mValue._int8, 1))
        return false;
    } break;
    case nuiMessageDataTypeInt16:
    {
      uint16 tmp = cpu_to_le16(rData.mValue._int16);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeInt32:
    {
      uint32 tmp = cpu_to_le32(rData.mValue._int32);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeInt64:
    {
      uint64 tmp = cpu_to_le64(rData.mValue._int64);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeUInt8:
    {
      if (1 != mpTCPClient->Send((uint8*)&rData.mValue._uint8, 1))
        return false;
    } break;
    case nuiMessageDataTypeUInt16:
    {
      uint16 tmp = cpu_to_le16(rData.mValue._uint16);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeUInt32:
    {
      uint32 tmp = cpu_to_le32(rData.mValue._uint32);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeUInt64:
    {
      uint64 tmp = cpu_to_le64(rData.mValue._uint64);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeFloat:
    {
      uint32 tmp = cpu_to_le32(rData.mValue._uint32);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;
    case nuiMessageDataTypeDouble:
    {
      uint64 tmp = cpu_to_le32(rData.mValue._uint64);
      return (sizeof(tmp) == mpTCPClient->Send((uint8*)&tmp, sizeof(tmp)));
    } break;

    default:
    {
      NGL_ASSERT(0); // Data type not handled
      return false;
    } break;

  }
  return false;
}

nuiMessage* nuiMessageClient::Read()
{
  return nullptr;
}

