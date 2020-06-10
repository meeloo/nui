/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/


#include "nui.h"

std::map<nglThread::ID,nglThread*> nglGlobalThreadMap;
nglCriticalSection nglGlobalThreadMapCS("nglThread_nglGlobalThreadMapCS");

const nglString& nglThread::GetName() const
{
  return mName;
}

void nglThread::SetAutoDelete(bool set)
{
  mAutoDelete = set;
}

bool nglThread::GetAutoDelete() const
{
  return mAutoDelete;
}

nglString nglThread::GetCurThreadName()
{
  nglThread* pThread = nglThread::GetCurThread();
  if (pThread)
  {
    return pThread->GetName();
  }
  
  nglString str;
  str.CFormat("%p", GetCurThreadID());
  return str;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetThreadFromGlobalList
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Get thread from list
//
nglThread* nglGetThreadFromGlobalList(nglThread::ID threadID)
{
  nglCriticalSectionGuard	guard(nglGlobalThreadMapCS);
  std::map<nglThread::ID,nglThread*>::const_iterator it = nglGlobalThreadMap.find(threadID);
  if (it == nglGlobalThreadMap.end())
    return NULL;
  return (it->second);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddThreadInGlobalList
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Add a thread in list
//
void nglAddThreadInGlobalList(nglThread* thread)
{
  nglCriticalSectionGuard	guard(nglGlobalThreadMapCS);
  nglGlobalThreadMap[thread->GetID()] = thread;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DelThreadFromGlobalList
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Delete a thread from list
//
bool nglDelThreadFromGlobalList(nglThread* thread)
{
  nglCriticalSectionGuard	guard(nglGlobalThreadMapCS);
  return (nglGlobalThreadMap.erase(thread->GetID()) != 0);
}


////////////////////////
// nglThreadDelegate
////////////////////////
nglThreadDelegate::nglThreadDelegate(const ThreadDelegate& rStartFunction, Priority priority, size_t StackSize)
: nglThread(priority, StackSize), mDelegate(rStartFunction)
{
}

nglThreadDelegate::nglThreadDelegate(const ThreadDelegate& rStartFunction, const nglString& rName, Priority priority, size_t StackSize)
: nglThread(rName, priority, StackSize), mDelegate(rStartFunction)
{
}

nglThreadDelegate::~nglThreadDelegate()
{
}

void nglThreadDelegate::OnStart()
{
  mDelegate();
}


////////////////////////
// nglThreadFunction
////////////////////////
nglThreadFunction::nglThreadFunction(std::function<void()> rStartFunction, Priority priority, size_t StackSize)
: nglThread("nglThreadFunction", priority, StackSize), mStartFunction(rStartFunction)
{
}

nglThreadFunction::nglThreadFunction(std::function<void()> rStartFunction, const nglString& rName, Priority priority, size_t StackSize)
: nglThread(rName, priority, StackSize), mStartFunction(rStartFunction)
{
}

nglThreadFunction::~nglThreadFunction()
{
}

void nglThreadFunction::OnStart()
{
    mStartFunction();
}

