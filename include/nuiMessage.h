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

  nuiMessage(const nglString& rName) { Add(rName); }

  template <typename T1>
  nuiMessage(const nglString& rName, T1 p1) { Add(rName); Add(p1); }

  template <typename T1, typename T2>
  nuiMessage(const nglString& rName, T1 p1, T2 p2) { Add(rName); Add(p1); Add(p2);}

  template <typename T1, typename T2, typename T3>
  nuiMessage(const nglString& rName, T1 p1, T2 p2, T3 p3) { Add(rName); Add(p1); Add(p2); Add(p3);}

  template <typename T1, typename T2, typename T3, typename T4>
  nuiMessage(const nglString& rName, T1 p1, T2 p2, T3 p3, T4 p4) { Add(rName); Add(p1); Add(p2); Add(p3); Add(p4); }

  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  nuiMessage(const nglString& rName, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5) { Add(rName); Add(p1); Add(p2); Add(p3); Add(p4); Add(p5); }

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

