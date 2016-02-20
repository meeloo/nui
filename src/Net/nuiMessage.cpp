//
//  nuiMessage.cpp
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#include "nui.h"

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

