/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

class nuiRefCount
{
public:
  nuiRefCount() : mTrace(false), mRefCount(1), mPermanent(false)
  {
  }

  inline uint32 Acquire() const
  { 
    mRefCount++;
    if (mTrace)
    {
      NGL_OUT(_T("Acquire object %p (%d)\n"), this, mRefCount);
    }
    return mRefCount;
  }

  inline uint32 Release() const
  { 
    if (mRefCount < 1)
    {
      printf("Hmmm\n");
    }
    NGL_ASSERTR(mRefCount > 0, mRefCount);
    mRefCount--;

    if (mTrace)
    {
      NGL_OUT(_T("Release object %p (%d)\n"), this, mRefCount);
    }

    if (mRefCount == 0)
    {
      if (mTrace)
      {
        NGL_OUT(_T("Delete object %p\n"), this);
      }
      
      
      const_cast<nuiRefCount*>(this)->OnFinalize();
      delete this;
      return 0;
    }
    return mRefCount;
  }

  void SetTrace(bool set)
  {
    mTrace = set;
  }
  
  bool GetTrace() const
  {
    return mTrace;
  }
  
  uint32 GetRefCount() const
  {
    return mRefCount;
  }

  void SetPermanent(bool Permanent = true)
  {
    if (Permanent)
    {
      if (mPermanent)
        return;
      Acquire();
      mPermanent = true;
    }
    else
    {
      if (!mPermanent)
        return;
      mPermanent = false;
      Release();
    }
  }

  bool IsPermanent() const
  {
    return mPermanent;
  }

  virtual void OnFinalize() ///< This callback is called when we are about to delete this
  {
  }

protected:
  mutable bool mTrace;
  virtual ~nuiRefCount()
  {
    //NGL_ASSERT(mRefCount == 0);
  }
  
private:
  mutable uint32 mRefCount;
  bool mPermanent;

};

class nuiRefGuard : nuiNonCopyable
{
public:
  nuiRefGuard(const nuiRefCount* pRef)
  {
    mpRef = pRef;
    mpRef->Acquire();
  }
  
  nuiRefGuard(const nuiRefCount& rRef)
  {
    mpRef = &rRef;
    mpRef->Acquire();
  }
  
  
  ~nuiRefGuard()
  {
    mpRef->Release();
  }
  
private:
  const nuiRefCount* mpRef;
};


#define nuiAutoRef nuiRefGuard nui_local_auto_ref(this);

