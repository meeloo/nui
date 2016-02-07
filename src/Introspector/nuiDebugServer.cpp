//
//  nuiDebugServer.cpp
//  nui3
//
//  Created by Sebastien Metrot on 02/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nuiDebugServer.h"


///////////////////////////////// MessageData
static uint8 datasizes[] = {
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
      memcpy(&mValue, data, datasizes[mType]);
      NGL_ASSERT(0);
    }break;
  }

}

nuiMessageData::nuiMessageData(const nuiMessageData& rData)
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

nuiMessageData::nuiMessageData(nuiMessageData&& rData)
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

nuiMessageData& nuiMessageData::operator= (nuiMessageData&& rData)
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

nuiMessageData::~nuiMessageData()
{
  if (mType == nuiMessageDataTypeBuffer || mType == nuiMessageDataTypeString)
  {
    delete[] mValue.Pointer._ptr;
  }
}



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

void FixEndianness(nuiMessageDataType type, std::vector<uint8>& rData)
{
  switch (type)
  {
    case nuiMessageDataTypeString:
    case nuiMessageDataTypeBuffer:
    case nuiMessageDataTypeInt8:
    case nuiMessageDataTypeUInt8:
    {
    } break;


    case nuiMessageDataTypeInt16:
    case nuiMessageDataTypeUInt16:
    {
      le16_to_cpu_s((uint8*)&rData[0], rData.size() / 2);
    } break;

    case nuiMessageDataTypeInt32:
    case nuiMessageDataTypeUInt32:
    case nuiMessageDataTypeFloat:
    {
      le32_to_cpu_s((uint64*)&rData[0], rData.size() / 4);
    } break;

    case nuiMessageDataTypeInt64:
    case nuiMessageDataTypeUInt64:
    case nuiMessageDataTypeDouble:
    {
      le64_to_cpu_s((uint64*)&rData[0], rData.size() / 8);
    } break;
  }
}

bool nuiMessageClient::Parse(std::vector<uint8>& rData)
{
  uint32 offset = 0;
  while (offset < rData.size())
  {
    switch (mState)
    {
      case Waiting:
      {
        mRemainingDataChunks = rData[offset];
        offset++;

        mCurrentChunkSize = 0;
        mState = ReadType;
      }break;

      case ReadType:
      {
        mType = (nuiMessageDataType)rData[offset];
        offset++;

        mCurrentChunkSize = datasizes[mType];

        switch (mType)
        {
          case nuiMessageDataTypeBuffer:
          case nuiMessageDataTypeString:
          {
            mState = ReadSize;
          }break;
            
          default:
          {
            mState = ReadData;
          }break;
        }
      } break;

      case ReadSize:
      {
        uint32 toread = MIN(mCurrentChunkSize, rData.size() - offset);
        for (uint32 i = 0; i < toread; i++)
        {
          mChunck.push_back(rData[offset+i]);
        }
        offset += toread;
        mCurrentChunkSize -= toread;
        if (mCurrentChunkSize == 0)
        {
          mCurrentChunkSize = le32_to_cpu(*(uint32*)&mChunck[0]); // We have the size of the data
          mState = ReadData;
        }
      }break;

      case ReadData:
      {
        uint32 toread = MIN(mCurrentChunkSize, rData.size() - offset);
        mChunck.resize(mChunck.size() + toread);
        memcpy(&mChunck[mChunck.size()], &rData[offset], toread);
        offset += toread;
        mCurrentChunkSize -= toread;

        if (mCurrentChunkSize == 0)
        {
          FixEndianness(mType, mChunck);
          mState = ReadType;
          switch (mType) {
            case nuiMessageDataTypeBuffer:
            {
              mpCurrentMessage->Add(&mChunck[0], mChunck.size());
            }break;

            case nuiMessageDataTypeString:
            {
              mpCurrentMessage->Add(nglString((nglChar*)&mChunck[0], mChunck.size(), eUTF8));
            }break;


            default:
            {
              nuiMessageData data(mType, &mChunck[0], mChunck.size());
              mpCurrentMessage->Add(data);
              NGL_ASSERT(0);
            }break;
          }
          mChunck.clear();
        }
        break;
      }

      default:
      {
        NGL_ASSERT(0);
      }
    }
  }
  return true;
}

///////////////////////////////// Debug Server
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
