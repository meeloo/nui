/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


nglReaderWriterLock::nglReaderWriterLock()
: mCS("nglReaderWriterLock::mCS"), mCSWriter("nglReaderWriterLock::mCSWriter")
{
  mReaders = 0;
  mWriter = 0;
}

nglThread::ID nglReaderWriterLock::GetWriter() const
{
  nglCriticalSectionGuard g(mCSWriter);
  return mWriter;
}

void nglReaderWriterLock::SetWriter(nglThread::ID value)
{
  nglCriticalSectionGuard g(mCSWriter);
  mWriter = value;
}


void nglReaderWriterLock::LockRead()
{ 
  bool ok = false;
  do 
  {
    {
      nglCriticalSectionGuard guard(mCS);

      if (!GetWriter())
      {
        ngl_atomic_inc(mReaders);
        ok = true;
      }
    }

    // If the writer changes after the test but before the wait we are fucked
    while (GetWriter())
      mWaitForRead.Wait(1);

  } while (!ok);
}

bool nglReaderWriterLock::TryLockRead()
{ 
  bool ok = false;
  {
    nglCriticalSectionGuard guard(mCS);
    
    if (!GetWriter())
    {
      ngl_atomic_inc(mReaders);
      ok = true;
    }
  }

  return ok;  
}

void nglReaderWriterLock::UnlockRead()
{
  nglCriticalSectionGuard guard(mCS);

  ngl_atomic_dec(mReaders);
  if (!ngl_atomic_read(mReaders))
    mWaitForWrite.WakeOne();
}

void nglReaderWriterLock::LockWrite()
{
  do 
  {
    {
      nglCriticalSectionGuard guard(mCS);

      if (!GetWriter() && !ngl_atomic_read(mReaders))
      {
        SetWriter(nglThread::GetCurThreadID());
      }
    }

    nglThread::ID threadid = GetWriter();
    while (threadid && threadid != nglThread::GetCurThreadID())
    {
      mWaitForWrite.Wait(1);
      threadid = GetWriter();
    }

  } while (GetWriter() != nglThread::GetCurThreadID());
}

bool nglReaderWriterLock::TryLockWrite()
{
  {
    nglCriticalSectionGuard guard(mCS);
    
    if (!GetWriter() && !ngl_atomic_read(mReaders))
    {
      SetWriter(nglThread::GetCurThreadID());
    }
  }
  
  SetWriter(nglThread::GetCurThreadID());
  return mWriter;
}


void nglReaderWriterLock::UnlockWrite()
{
  nglCriticalSectionGuard guard(mCS);

  SetWriter(NULL);
  if (!ngl_atomic_read(mReaders))
    mWaitForWrite.WakeOne();
  else
    mWaitForRead.WakeAll();
}



