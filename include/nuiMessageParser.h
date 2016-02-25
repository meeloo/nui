//
//  nuiMessageParser.hpp
//  nui3
//
//  Created by Sebastien Metrot on 20/02/16.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once
#include "nui.h"

class nuiMessageParser
{
public:
  nuiMessageParser();
  virtual ~nuiMessageParser();
  void Parse(const std::vector<uint8>& rData, std::function<void(nuiMessage*)> onNewMessage);
  std::vector<uint8> Build(const nuiMessage& rMessage);
  bool Build(const nuiMessageData& rData, std::vector<uint8>& rOut);
private:
  enum State
  {
    Waiting,
    ReadType,
    ReadSize,
    ReadData
  };
  State mState = Waiting;
  uint32 mRemainingDataChunks = 0;
  uint32 mCurrentChunkSize = 0;
  nuiMessageDataType mType;
  std::vector<uint8> mChunck;
  nuiMessage* mpCurrentMessage = nullptr;
};


