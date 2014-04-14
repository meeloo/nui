/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiEvent_h__
#define __nuiEvent_h__

#include "nui.h"

class nuiEventSource;
class nuiVariant;

/// This is the basic event object used in the user level event management system. 
class nuiEvent
{
public:
  nuiEvent(int Type = -1);  
  nuiEvent(const nuiEvent& rEvent);  
  virtual ~nuiEvent();

  int GetType() const;
  const nuiEventSource* GetSource() const;
  void SetSource(nuiEventSource* pObj) const;

  uint32 GetArgCount() const;
  void AddArgument(const nuiVariant& rArg);
  const nuiVariant& operator[](uint32 index) const;
  
  mutable void* mpUser; // This is the user defined data. It is defined on a connection basis.
  
  void Cancel() const; ///< Stop propagating this event!
  bool IsCanceled() const;
private:
  int mType;
  mutable nuiEventSource* mpSource;
  std::vector<nuiVariant*> mArguments;
  mutable bool mCancel;
};

/// You are never supposed to use this class directly!!! NEVER EVER :). 
class nuiEventTargetBase
{
public:
  typedef nuiFastDelegate1<const nuiEvent&> Delegate;
  
  nuiEventTargetBase(void* pTarget);
  virtual ~nuiEventTargetBase();
  
  void DisconnectAll();
  bool OnEvent(const nuiEvent& rEvent);
  bool CallEvent(void* pTarget, nuiDelegateMemento pFunc, const nuiEvent& rEvent);
  void Connect(nuiEventSource& rSource, const nuiDelegateMemento& rTargetFunc, void* pUser = NULL);
  void DisconnectSource(nuiEventSource& rSource);
  void Disconnect(const nuiDelegateMemento& rTFunc);
  void Disconnect(nuiEventSource& rSource, const nuiDelegateMemento& rTFunc);
  
protected:
  class Link
  {
  public:
    nuiDelegateMemento  mTargetFunc;
    void* mpUser;
  };
  
  typedef std::vector<Link*> LinkList;
  typedef std::map<nuiEventSource*, LinkList> LinksMap;
  LinksMap mpLinks;
  
  void* mpTarget;
  
  nuiEventTargetBase(const nuiEventTargetBase& rTarget)
  {
    // There is NO way to copy a target properly. 
  }
};

/// Do not touch this either!
class nuiEventSource
{
public:  
  nuiEventSource();
  virtual ~nuiEventSource();

  virtual void Connect(nuiEventTargetBase* t);
  virtual void Disconnect(nuiEventTargetBase* t);
  void DisconnectAll();
  virtual bool SendEvent(const nuiEvent& rEvent = nuiEvent(0));
  virtual bool operator() (const nuiEvent& rEvent = nuiEvent(0));
  virtual bool operator() (const nuiVariant& rP0);
  virtual bool operator() (const nuiVariant& rP0, const nuiVariant& rP1);
  virtual bool operator() (const nuiVariant& rP0, const nuiVariant& rP1, const nuiVariant& rP2);
  virtual bool operator() (const nuiVariant& rP0, const nuiVariant& rP1, const nuiVariant& rP2, const nuiVariant& rP3);
  virtual bool operator() (const nuiVariant& rP0, const nuiVariant& rP1, const nuiVariant& rP2, const nuiVariant& rP3, const nuiVariant& rP4);
  virtual bool operator() (const nuiVariant& rP0, const nuiVariant& rP1, const nuiVariant& rP2, const nuiVariant& rP3, const nuiVariant& rP4, const nuiVariant& rP5);
  uint GetTargetCount() const;

  void Enable(bool Set = true)
  {
    mEnabled = Set;
  }

  void Disable()
  {
    Enable(false);
  }

  bool IsEnabled()
  {
    return mEnabled;
  }

protected:
  mutable std::vector<nuiEventTargetBase*> mpTargets;

private:
  // Restrict access to some methods & constructors:
  nuiEventSource(const nuiEventSource& rSource) ;
  bool mEnabled;
  mutable std::set<nuiEventTargetBase*> mGraveYard;
  mutable int32 mEnumerating = 0;

};


/// This template is the basis of the event reception system of nui.
template <class T> class nuiEventSink : public nuiEventTargetBase
{
private:
  typedef void (T::*TargetFunc)(const nuiEvent&);
public:
  nuiEventSink(T* pTarget)
  : nuiEventTargetBase((void*)pTarget)
  {
  }

  virtual ~nuiEventSink()
  {
  }

  void Connect(nuiEventSource& rSource, void (*pTargetFunc)(const nuiEvent&), void* pUser=NULL)
  {
    nuiEventTargetBase::Connect(rSource, nuiFastDelegate1<const nuiEvent&>(pTargetFunc).GetMemento(), pUser);
  }
  
  void Connect(nuiEventSource& rSource, TargetFunc pTargetFunc, void* pUser=NULL)
  {
    assert(nuiEventTargetBase::mpTarget != NULL);
    nuiEventTargetBase::Connect(rSource, nuiMakeDelegate((T*)mpTarget, pTargetFunc).GetMemento(), pUser);
  }
  
  void Disconnect(void (*pTargetFunc)(const nuiEvent&))
  {
    nuiEventTargetBase::Disconnect(nuiFastDelegate1<const nuiEvent&>(pTargetFunc).GetMemento());
  }
  
  void Disconnect(TargetFunc pTargetFunc)
  {
    assert(nuiEventTargetBase::mpTarget != NULL);
    nuiEventTargetBase::Disconnect(nuiMakeDelegate((T*)mpTarget, pTargetFunc).GetMemento());
  }
  
  void Disconnect(nuiEventSource& rSource, TargetFunc pTargetFunc)
  {
    assert(nuiEventTargetBase::mpTarget != NULL);
    nuiEventTargetBase::Disconnect(rSource, nuiMakeDelegate((T*)mpTarget, pTargetFunc).GetMemento());
  }
  
private:
  nuiEventSink(const nuiEventSink& rSink)
  {
    // No way to use the copy constructor. 
  }
};

/// This is a helper class that implements a simple event source with a fixed type.
template <int eventtype = 0> class nuiSimpleEventSource : public nuiEventSource
{
public:
  nuiSimpleEventSource()
  {
  }

  virtual ~nuiSimpleEventSource()
  {
  }

  virtual bool operator() ()
  {
    if (IsEnabled())
    {
      nuiEvent e(eventtype); 
      return SendEvent(e);
    }

    return false;
  }

  virtual bool operator() (const nuiEvent& rEvent)
  {
    if (IsEnabled())
    {
      return SendEvent(rEvent);
    }
    return false;
  }

private:
  nuiSimpleEventSource(const nuiSimpleEventSource& rSource)
  {
    // No way to implement this correctly. 
  }
};

class nuiRelayEventSource : public nuiSimpleEventSource<0>
{
public:
  nuiRelayEventSource()
  : mEventSink(this)
  {
  }

  virtual ~nuiRelayEventSource()
  {
  }

  virtual bool operator() ()
  {
    if (IsEnabled())
    {
      nuiEvent e(0);
      return SendEvent(e);
    }

    return false;
  }

  virtual bool operator() (const nuiEvent& rEvent)
  {
    if (IsEnabled())
    {
      return SendEvent(rEvent);
    }
    return false;
  }

  void Connect(nuiEventSource& rSource, void* pUser = NULL)
  {
    mEventSink.Connect(rSource, &nuiRelayEventSource::RelayEvent, pUser);
  }

private:
  nuiRelayEventSource(const nuiRelayEventSource& rSource)
  : mEventSink(this)
  {
    // No way to implement this correctly.
  }

  void RelayEvent(const nuiEvent& rEvent)
  {
    (*this)(rEvent);
  }

  nuiEventSink<nuiRelayEventSource> mEventSink;
};



#endif // __nuiEvent_h__
