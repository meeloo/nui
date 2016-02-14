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

///////////////////////////////// Message
nuiMessage::nuiMessage()
{

}

nuiMessage::~nuiMessage()
{

}

nglString nuiMessage::GetDescription() const
{
  nglString tmp("[ ");

  int i = 0;
  for (const auto& data : mData)
  {
    if (i)
      tmp.Add(", ");
    switch (data.mType)
    {
      case nuiMessageDataTypeString:
      {
        nglString str((nglChar*)data.mValue.Pointer._ptr, data.mValue.Pointer._size);
        tmp.Add("\"").Add(str).Add("\"");
      } break;
      case nuiMessageDataTypeBuffer:
        tmp.Add("<").Add(data.mValue.Pointer._ptr).Add(" - ").Add((uint64)data.mValue.Pointer._size).Add(">");
        break;
      case nuiMessageDataTypeInt8:
        tmp.Add("<i8> ").Add(data.mValue._int8); break;
      case nuiMessageDataTypeInt16:
        tmp.Add("<i16> ").Add(data.mValue._int16); break;
      case nuiMessageDataTypeInt32:
        tmp.Add("<i32> ").Add(data.mValue._int32); break;
      case nuiMessageDataTypeInt64:
        tmp.Add("<i64> ").Add(data.mValue._int64); break;
      case nuiMessageDataTypeUInt8:
        tmp.Add("<u8> ").Add(data.mValue._uint8); break;
      case nuiMessageDataTypeUInt16:
        tmp.Add("<u16> ").Add(data.mValue._uint16); break;
      case nuiMessageDataTypeUInt32:
        tmp.Add("<u32> ").Add(data.mValue._uint32); break;
      case nuiMessageDataTypeUInt64:
        tmp.Add("<u64> ").Add(data.mValue._uint64); break;
      case nuiMessageDataTypeFloat:
        tmp.Add("<f> ").Add(data.mValue._float); break;
      case nuiMessageDataTypeDouble:
        tmp.Add("<d> ").Add(data.mValue._double); break;
      default:
        tmp.Add("???"); break;
    }
    i++;
  }
  tmp.Add(" ]");

  return tmp;
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

nuiMessage* nuiMessageParser::Parse(const std::vector<uint8>& rData)
{
  if (!mpCurrentMessage)
    mpCurrentMessage = new nuiMessage();
  
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
          
          mRemainingDataChunks--;
          if (!mRemainingDataChunks)
          {
            nuiMessage* pMessage = mpCurrentMessage;
            mpCurrentMessage = nullptr;
            return pMessage;
          }
        }
        break;
      }
        
      default:
      {
        NGL_ASSERT(0);
      }
    }
  }
  nuiMessage* pReturn = mpCurrentMessage;
  mpCurrentMessage = nullptr;
  return pReturn;
}

static void add(std::vector<uint8>& rOut, uint8* pointer, size_t len)
{
  for (size_t i = 0; i < len; i++)
    rOut.push_back(pointer[i]);
}


std::vector<uint8> nuiMessageParser::Build(nuiMessage* pMessage)
{
  std::vector<uint8> data;
  size_t size = pMessage->GetSize();
  uint8 count = (uint8)size;
  add(data, &count, 1);
  
  for (size_t index = 0; index < size; index++)
  {
    if (!Build(pMessage->GetData(index), data))
      return std::vector<uint8>();
  }
  return data;

}


bool nuiMessageParser::Build(const nuiMessageData& rData, std::vector<uint8>& rOut)
{
  uint8 type = rData.mType;
  rOut.push_back(type);
  
  switch (rData.mType)
  {
    case nuiMessageDataTypeString:
    case nuiMessageDataTypeBuffer:
    {
      uint32 size = cpu_to_le32(rData.mValue._int32);
      add(rOut,(uint8*)&size, sizeof(size));
      add(rOut, rData.mValue.Pointer._ptr, rData.mValue.Pointer._size);
    } break;
    case nuiMessageDataTypeInt8:
    {
      add(rOut, (uint8*)&rData.mValue._int8, 1);
    } break;
    case nuiMessageDataTypeInt16:
    {
      uint16 tmp = cpu_to_le16(rData.mValue._int16);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeInt32:
    {
      uint32 tmp = cpu_to_le32(rData.mValue._int32);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeInt64:
    {
      uint64 tmp = cpu_to_le64(rData.mValue._int64);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeUInt8:
    {
      add(rOut, (uint8*)&rData.mValue._uint8, 1);
    } break;
    case nuiMessageDataTypeUInt16:
    {
      uint16 tmp = cpu_to_le16(rData.mValue._uint16);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeUInt32:
    {
      uint32 tmp = cpu_to_le32(rData.mValue._uint32);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeUInt64:
    {
      uint64 tmp = cpu_to_le64(rData.mValue._uint64);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeFloat:
    {
      uint32 tmp = cpu_to_le32(rData.mValue._uint32);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
    case nuiMessageDataTypeDouble:
    {
      uint64 tmp = cpu_to_le32(rData.mValue._uint64);
      add(rOut, (uint8*)&tmp, sizeof(tmp));
    } break;
      
    default:
    {
      NGL_ASSERT(0); // Data type not handled
      return false;
    } break;
      
  }
  return true;
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

