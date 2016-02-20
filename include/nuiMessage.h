//
//  nuiMessage.h
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once
#include "nui.h"

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

