/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

class nuiRefCount
{
public:
  nuiRefCount()
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
      DumpInfos();
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

      if (mTrace)
      {
        DumpInfos();
        NGL_OUT("delete ref counted object: %p %s / %d\n", this, typeid(this).name(), typeid(this).hash_code());
      }

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

  void AutoRelease()
  {
    if (!mAutoReleased)
    {
//      printf("Autorelease %p\n:", this);
//      DumpInfos();
      get_tls_pool().push_back(this);
      mAutoReleased = true;
    }
  }

  static void PurgeAutoReleasePoolForCurrentThread()
  {
    std::vector<nuiRefCount*>& pool(get_tls_pool());
    for (auto item : pool)
    {
      item->Release();
    }

    pool.clear();
  }
protected:
  mutable bool mTrace = false;
  virtual ~nuiRefCount()
  {
    //NGL_ASSERT(mRefCount == 0);
  }
  
private:
  mutable uint32 mRefCount = 1;
  bool mPermanent = false;
  bool mAutoReleased = false;

  static pthread_key_t key;

  static void destroy_pool(void* p)
  {
    std::vector<nuiRefCount*>* pPool = (std::vector<nuiRefCount*>*)p;
    delete pPool;
  }

  static void create_key()
  {
    pthread_key_create(&key, destroy_pool);
  }

  static std::vector<nuiRefCount*>& get_tls_pool()
  {
    static pthread_once_t key_once = PTHREAD_ONCE_INIT;

    pthread_once(&key_once, create_key);

    std::vector<nuiRefCount*>* pPool = nullptr;
    if ((pPool = (std::vector<nuiRefCount*>*)pthread_getspecific(key)) == nullptr)
    {
      pPool = new std::vector<nuiRefCount*>;
      pthread_setspecific(key, pPool);
    }

    NGL_ASSERT(pPool != nullptr);
    return *pPool;
  }

  virtual void DumpInfos() const;
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

template <class T>
T* nuiAutoRelease(T* pObj)
{
  pObj->AutoRelease();
  return pObj;
}


#define nuiAutoRef nuiRefGuard nui_local_auto_ref(this);

