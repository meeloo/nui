/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiMessageQueue_h__
#define __nuiMessageQueue_h__

#include "nui.h"
#include "nglCriticalSection.h"
#include "nglSyncEvent.h"

class nuiNotification;
class nuiTask;

/// implements a message queue for multi-threaded communication
/// see nuiTest, MessageQueueWindow, for an application example
template <class Message>
class nuiProtectedQueue
{
public : 

  nuiProtectedQueue(const char* queue_name = "basic queue")
  : mQueueCS(queue_name)
  {
  }


  ~nuiProtectedQueue()
  {
  }

  bool Post(Message* message)
  {
    nglCriticalSectionGuard guard(mQueueCS);
    message->Acquire();
    mQueue.push(message);

    // unlock thread waiting to read the message
    mSyncEvent.Set ();
    return true;
  }

  // Don't forget to ->Release the message you get from this method!
  Message* Get(uint32 time)
  {
    // wait for an incoming message
    if (time > 0)
      mSyncEvent.Wait(time);

    mQueueCS.Lock();
    if (mQueue.empty())
    {
      mQueueCS.Unlock();
      return NULL;
    }
    Message* message = mQueue.front();
    mQueue.pop();

    // no more messages. next call to Get will block 'til another message is posted.
    if (mQueue.empty())
      mSyncEvent.Reset();
    mQueueCS.Unlock();
    
    return message;
  }

private : 

  nglCriticalSection mQueueCS;
  std::queue<Message*> mQueue;
  nglSyncEvent mSyncEvent;
};

typedef nuiProtectedQueue<nuiNotification> nuiMessageQueue;
typedef nuiProtectedQueue<nuiTask> nuiTaskQueue;

#endif
