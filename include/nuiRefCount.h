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
    mRefCount = 1;

//    static int64 refCreated = 1;
//    printf("REF CREATED\t[%lld]\n", refCreated++);
  }

  inline uint64 Acquire() const
  {
    uint64 val = ngl_atomic_inc(mRefCount);
    if (mTrace)
    {
      NGL_OUT("Acquire object %p (%d)\n", this, val);
    }
    OnAcquired();
    return val;
  }

  inline uint64 Release() const
  {
    uint64 val = ngl_atomic_read(mRefCount);
    if (val < 1)
    {
      printf("Hmmm\n");
      DumpInfos();
    }
    NGL_ASSERTR(val > 0, val);

    OnReleased();
    bool trace = mTrace;
    if (trace)
    {
      NGL_OUT("Release object %p (%d)\n", this, ngl_atomic_read(mRefCount));
    }

    if (mRefCount == 1)
    {
      if (trace)
      {
        NGL_OUT("Delete object %p %d\n", this, ngl_atomic_read(mRefCount));
      }

      const_cast<nuiRefCount*>(this)->OnFinalize();
      val = ngl_atomic_dec(mRefCount);

      if (trace)
      {
        DumpInfos();
        NGL_OUT("delete ref counted object: %p %s / %d / %d\n", this, typeid(this).name(), typeid(this).hash_code(), ngl_atomic_read(mRefCount));
      }

      delete this;
      return 0;
    }
    else
    {
      return ngl_atomic_dec(mRefCount);
    }
  }

  void SetTrace(bool set)
  {
    mTrace = set;
  }
  
  bool GetTrace() const
  {
    return mTrace;
  }
  
  uint64 GetRefCount() const
  {
    return ngl_atomic_read(mRefCount);
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

  virtual void OnAcquired() const ///< This callback is called when we are about to acquire this
  {
  }
  
  virtual void OnReleased() const ///< This callback is called when we are about to release this
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
//    static int64 refDeleted = 1;
//    printf("REF DELETED\t[%lld]\n", refDeleted++);
//    NGL_ASSERT(ngl_atomic_read(mRefCount) == 0);
  }
  
private:
  mutable nglAtomic mRefCount;
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
  const nuiRefCount* mpRef = nullptr;
};

template <class T>
T* nuiAutoRelease(T* pObj)
{
  pObj->AutoRelease();
  return pObj;
}


#define nuiAutoRef nuiRefGuard nui_local_auto_ref(this);


template <class Pointee>
class nuiNewRef
{
public:
  nuiNewRef(Pointee* pPointer)
  {
    mpPointer = pPointer;
  }

  nuiNewRef()
  {
    mpPointer = new Pointee();
  }

  template<typename T0>
  nuiNewRef(T0 p0)
  {
    mpPointer = new Pointee(p0);
  }

  template<typename T0, typename T1>
  nuiNewRef(T0 p0, T1 p1)
  {
    mpPointer = new Pointee(p0, p1);
  }

  template<typename T0, typename T1, typename T2>
  nuiNewRef(T0 p0, T1 p1, T2 p2)
  {
    mpPointer = new Pointee(p0, p1, p2);
  }

  template<typename T0, typename T1, typename T2, typename T3>
  nuiNewRef(T0 p0, T1 p1, T2 p2, T3 p3)
  {
    mpPointer = new Pointee(p0, p1, p2, p3);
  }

  template<typename T0, typename T1, typename T2, typename T3, typename T4>
  nuiNewRef(T0 p0, T1 p1, T2 p2, T3 p3, T4 p4)
  {
    mpPointer = new Pointee(p0, p1, p2, p3, p4);
  }

  ~nuiNewRef()
  {
    if (mpPointer)
      mpPointer->Release();
    mpPointer = nullptr;
  }
  
  operator bool() const
  {
    return mpPointer != nullptr;
  }
  
  bool operator==(Pointee* pPtr) const
  {
    return mpPointer == pPtr;
  }
  
  bool operator==(const nuiNewRef<Pointee>& ref) const
  {
    return mpPointer == ref.mpPointer;
  }

  operator Pointee* () const
  {
    return mpPointer;
  }
  
  Pointee* operator->() const
  {
    return mpPointer;
  }
  
  const Pointee& operator*() const
  {
    return *mpPointer;
  }

  Pointee* Ptr() const
  {
    return mpPointer;
  }
  

private:
  Pointee* mpPointer = nullptr;
};

template <class Pointee>
class nuiRef
{
public:
  nuiRef(Pointee* pPointer = nullptr)
  {
    mpPointer = pPointer;
    if (pPointer)
      pPointer->Acquire();
  }

  nuiRef(const nuiRef<Pointee>& ref)
  {
    mpPointer = ref.mpPointer;
    if (mpPointer)
      mpPointer->Acquire();
  }

  nuiRef(const nuiNewRef<Pointee>& ref)
  {
    mpPointer = ref.Ptr();
    if (mpPointer)
      mpPointer->Acquire();
  }

  ~nuiRef()
  {
    if (mpPointer)
      mpPointer->Release();
    mpPointer = nullptr;
  }

  nuiRef<Pointee>& operator = (const nuiRef& ref)
  {
    if (ref.mpPointer)
      ref.mpPointer->Acquire();
    if (mpPointer)
      mpPointer->Release();
    mpPointer = ref.mpPointer;

    return *this;
  }

  nuiRef<Pointee>& operator = (const nuiNewRef<Pointee>& ref)
  {
    Pointee* pPtr = ref.Ptr();
    if (pPtr)
      pPtr->Acquire();
    if (mpPointer)
      mpPointer->Release();
    mpPointer = pPtr;

    return *this;
  }
  
  nuiRef<Pointee>& operator = (Pointee* pPointer)
  {
    if (pPointer)
      pPointer->Acquire();
    if (mpPointer)
      mpPointer->Release();
    mpPointer = pPointer;

    return *this;
  }

  operator bool() const
  {
    return mpPointer != nullptr;
  }

  bool operator==(Pointee* pPtr) const
  {
    return mpPointer == pPtr;
  }

  bool operator==(const nuiRef<Pointee>& ref) const
  {
    return mpPointer == ref.Ptr();
  }

  bool operator==(const nuiNewRef<Pointee>& ref) const
  {
    return mpPointer == ref.Ptr();
  }

  bool operator!=(Pointee* pPtr) const
  {
    return mpPointer != pPtr;
  }

  bool operator!=(const nuiRef<Pointee>& ref) const
  {
    return mpPointer != ref.Ptr();
  }

  bool operator!=(const nuiNewRef<Pointee>& ref) const
  {
    return mpPointer != ref.Ptr();
  }

  bool operator<(Pointee* pPtr) const
  {
    return mpPointer < pPtr;
  }

  bool operator<(const nuiRef<Pointee>& ref) const
  {
    return mpPointer < ref.Ptr();
  }

  bool operator<(const nuiNewRef<Pointee>& ref) const
  {
    return mpPointer < ref.Ptr();
  }
  

  operator Pointee* () const
  {
    return mpPointer;
  }

  Pointee* operator->() const
  {
    return mpPointer;
  }

  const Pointee& operator*() const
  {
    return *mpPointer;
  }

  Pointee* Ptr() const
  {
    return mpPointer;
  }


private:
  Pointee* mpPointer = nullptr;
};

