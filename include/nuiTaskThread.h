//
//  nuiTaskThread.h
//  nui3
//
//  Created by Sébastien Métrot on 28/11/13.
//  Copyright (c) 2013 libNUI. All rights reserved.
//

#pragma once

class nuiTaskThread : public nglThread
{
public:
  nuiTaskThread(nuiTaskQueue* pQueue = NULL, Priority priority = Normal, size_t StackSize = 0)
  : nglThread(priority, StackSize), mpQueue(pQueue), mOwnQueue(false), mDone(false)
  {
    Init();
  }

  nuiTaskThread(const nglString& rName, nuiTaskQueue* pQueue = NULL, Priority priority = Normal, size_t StackSize = 0)
  : nglThread(rName, priority, StackSize), mpQueue(pQueue), mOwnQueue(false), mDone(false)
  {
    Init();
  }

  void Init()
  {
    if (!mpQueue)
    {
      mpQueue = new nuiTaskQueue();
      mOwnQueue = true;
    }
  }

  ~nuiTaskThread()
  {
    if (mOwnQueue)
      delete mpQueue;
  }

  void OnStart()
  {
    NGL_ASSERT(mpQueue != NULL);
    while (!mDone)
    {
      nuiTask* pTask = mpQueue->Get(10000);
      if (pTask)
      {
        pTask->Run();
        pTask->Release();
      }
    }
  }

  void Stop()
  {
    mpQueue->Post(nuiMakeTask(this, &nuiTaskThread::ExecDone));
    Join();
  }

  nuiTaskQueue& GetQueue()
  {
    NGL_ASSERT(mpQueue != NULL);
    return *mpQueue;
  }
private:
  void ExecDone()
  {
    mDone = true;
  }

  nuiTaskQueue* mpQueue;
  bool mOwnQueue;
  bool mDone;
};



