//
//  nuiMessageParser.cpp
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nui.h"

#define NUI_DEBUG_MESSAGE_PARSER 0
#define NUI_DUMP_MESSAGES 1

extern uint8 nui_internal_datasizes[];

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
      __unused void* p = le16_to_cpu_s((uint8*)&rData[0], rData.size() / 2);
    } break;

    case nuiMessageDataTypeInt32:
    case nuiMessageDataTypeUInt32:
    case nuiMessageDataTypeFloat:
    {
      __unused void* p = le32_to_cpu_s((uint64*)&rData[0], rData.size() / 4);
    } break;

    case nuiMessageDataTypeInt64:
    case nuiMessageDataTypeUInt64:
    case nuiMessageDataTypeDouble:
    {
      __unused void* p = le64_to_cpu_s((uint64*)&rData[0], rData.size() / 8);
    } break;

    default:
      NGL_ASSERT(0);
  }
}

nuiMessageParser::nuiMessageParser()
{

}

nuiMessageParser::~nuiMessageParser()
{
  delete mpCurrentMessage;
}

void nuiMessageParser::Parse(const std::vector<uint8>& rData, std::function<bool(nuiMessage*)> onNewMessage)
{
  if (!rData.size())
    return;

  if (!mpCurrentMessage)
    mpCurrentMessage = new nuiMessage();

  uint32 offset = 0;
  while (offset < rData.size())
  {
    switch (mState)
    {
      case Waiting:
      {
#if NUI_DEBUG_MESSAGE_PARSER
        NGL_OUT("Wait\n");
#endif
        mRemainingDataChunks = rData[offset];
        offset++;

        mCurrentChunkSize = 0;
        mState = ReadType;
      }break;

      case ReadType:
      {
        mType = (nuiMessageDataType)rData[offset];
#if NUI_DEBUG_MESSAGE_PARSER
        NGL_OUT("Read Type %d\n", mType);
#endif
        offset++;

        mCurrentChunkSize = nui_internal_datasizes[mType];

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
#if NUI_DEBUG_MESSAGE_PARSER
          NGL_OUT("Read Size %d\n", mCurrentChunkSize);
#endif
          mChunck.clear();
          mState = ReadData;
        }
      }break;

      case ReadData:
      {
        uint32 toread = MIN(mCurrentChunkSize, rData.size() - offset);
        size_t pos = mChunck.size();
        mChunck.resize(mChunck.size() + toread);
        memcpy(&mChunck[pos], &rData[offset], toread);
        offset += toread;
        mCurrentChunkSize -= toread;
#if NUI_DEBUG_MESSAGE_PARSER
        NGL_OUT("Read Data %d\n", toread);
#endif

        if (mCurrentChunkSize == 0)
        {
#if NUI_DEBUG_MESSAGE_PARSER
          NGL_OUT("  Done Reading Data\n");
#endif
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
            }break;
          }
          mChunck.clear();

          mRemainingDataChunks--;
          if (!mRemainingDataChunks)
          {
#if NUI_DEBUG_MESSAGE_PARSER || NUI_DUMP_MESSAGES
            NGL_OUT("Done Reading Message: %s\n", mpCurrentMessage->GetDescription().GetChars());
#endif
            if (!onNewMessage(mpCurrentMessage))
            {
              delete mpCurrentMessage;
              mpCurrentMessage = nullptr;
              return;
            }
            delete mpCurrentMessage;
            mpCurrentMessage = new nuiMessage();
            mState = Waiting;

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
}

static void add(std::vector<uint8>& rOut, uint8* pointer, size_t len)
{
  for (size_t i = 0; i < len; i++)
    rOut.push_back(pointer[i]);
}


std::vector<uint8> nuiMessageParser::Build(const nuiMessage& rMessage)
{
  std::vector<uint8> data;
  size_t size = rMessage.GetSize();
  uint8 count = (uint8)size;
  add(data, &count, 1);

  for (size_t index = 0; index < size; index++)
  {
    if (!Build(rMessage.GetData(index), data))
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
      uint32 size = cpu_to_le32(rData.mValue.Pointer._size);
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

