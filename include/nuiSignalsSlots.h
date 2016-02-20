#ifndef SIGNALS_SLOTS_H
#define SIGNALS_SLOTS_H

#include "nui.h"
#include "nuiFastDelegate.h"

class nuiSlotsSink;

class nuiSignal
{
  friend class nuiSlotsSink;
public:
  virtual ~nuiSignal()
  {
  }

  void Disable()
  {
    mDisabled = true;
  }
  
  void Enable()
  {
    mDisabled = false;
  }
  
protected:
  nuiSignal()
  {
    mDisabled = false;
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &slot) = 0;

  void RemoveConnection(nuiSlotsSink &rSink, const nuiDelegateMemento &rSlot);
  void AddConnection(nuiSlotsSink &rSink, const nuiDelegateMemento &rSlot);

  bool mDisabled;

  static void UpdateStats(size_t v)
  {
#ifdef _DEBUG_
    static size_t ref = 0;
    
    if (v > ref)
    {
      ref = v;
      
      NGL_LOG(_T("nuiSignal"), NGL_LOG_DEBUG, _T("nuiSignal new max connection count: %d\n"), ref);
    }
#endif
  }
  
};

// N=0
template<class RetType = detail::DefaultVoid>
class nuiSignal0 : public nuiSignal, private nuiNonCopyable 
{
  friend class nuiSlotsSink;
public:
  typedef typename detail::DefaultVoidToVoid<RetType>::type DesiredRetType;
  typedef nuiFastDelegate0<DesiredRetType> Slot;
  typedef std::function<DesiredRetType()> Function;

  nuiSignal0()
  {
  }

  ~nuiSignal0()
  {
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    for(; it != end; ++it)
    {
      RemoveConnection(*(*it).second, (*it).first.GetMemento());
    }
  }

  void operator()() const
  {
    if (mDisabled)
      return;
      
    //#FIXME: temp hack to avoid modification of the map (connection/deconnection of slots) during iteration
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();

    std::vector<Slot> tmp(mSlots.size());
    for(uint i = 0; it != end; ++it, i++)
      tmp[i] = (*it).first;
    
    for(uint i = 0; i < tmp.size(); i++)
      tmp[i]();

    auto functions(mFunctions);
    for (auto fun : functions)
      fun();
  }

  bool IsConnected ()
  {
    return (!mSlots.empty() || !mFunctions.empty());
  }

  void Connect(const Function &function)
  {
    mFunctions.push_back(function);
  }

  void Disconnect(const Function &function)
  {
    auto it = mFunctions.begin();
    auto end = mFunctions.end();
    while (it != end)
    {
      if ((*it) == function)
      {
        mFunctions.erase(it);
        break;
      }
      ++it;
    }
  }

  
private:
  void Connect(nuiSlotsSink &rSink, const Slot &rSlot)
  {
    mSlots.push_back(std::make_pair(rSlot, &rSink));
    AddConnection(rSink, rSlot.GetMemento());
    UpdateStats(mSlots.size());
  }

  void Disconnect(nuiSlotsSink &rSink, const Slot &rSlot)
  {
    RemoveConnection(rSink, rSlot.GetMemento());
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      if ((*it).first == rSlot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &rSlot_)
  {
    Slot slot;
    slot.SetMemento(rSlot_);
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      const Slot& rS(it->first);
      if (rS == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  std::vector< std::pair<Slot, nuiSlotsSink*> > mSlots;
  std::vector< Function > mFunctions;
};

// N=1
template<typename Param1, typename RetType=detail::DefaultVoid>
class nuiSignal1 : public nuiSignal, nuiNonCopyable  
{
  friend class nuiSlotsSink;
public:
  typedef typename detail::DefaultVoidToVoid<RetType>::type DesiredRetType;
  typedef nuiFastDelegate1<Param1, DesiredRetType> Slot;
  typedef std::function<DesiredRetType(Param1)> Function;

  nuiSignal1()
  {
  }

  ~nuiSignal1()
  {
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    for(; it != end; ++it)
    {
      RemoveConnection(*(*it).second, (*it).first.GetMemento());
    }
  }

  void operator()(Param1 param1) const
  {
    if (mDisabled)
      return;

    //#FIXME: temp hack to avoid modification of the map (connection/deconnection of slots) during iteration
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    std::vector<Slot> tmp(mSlots.size());
    for(uint i = 0; it != end; ++it, i++)
      tmp[i] = (*it).first;
    
    for(uint i = 0; i < tmp.size(); i++)
      tmp[i](param1);

    auto functions(mFunctions);
    for (auto fun : functions)
      fun(param1);
  }

  bool IsConnected ()
  {
    return (!mSlots.empty() || !mFunctions.empty());
  }

  void Connect(const Function &function)
  {
    mFunctions.push_back(function);
  }

  void Disconnect(const Function &function)
  {
    auto it = mFunctions.begin();
    auto end = mFunctions.end();
    while (it != end)
    {
      if ((*it) == function)
      {
        mFunctions.erase(it);
        break;
      }
      ++it;
    }
  }

private:	
  void Connect(nuiSlotsSink &sink, const Slot &slot)
  {
    mSlots.push_back(std::make_pair(slot, &sink));
    AddConnection(sink, slot.GetMemento());
    UpdateStats(mSlots.size());
  }

  void Disconnect(nuiSlotsSink &sink, const Slot &slot)
  {
    RemoveConnection(sink, slot.GetMemento());
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      if ((*it).first == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &slot_)
  {
    Slot slot;
    slot.SetMemento(slot_);
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      const Slot& rS(it->first);
      if (rS == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  std::vector< std::pair<Slot, nuiSlotsSink*> > mSlots;
  std::vector< Function > mFunctions;
};

// N=2
template<class Param1, class Param2, class RetType=detail::DefaultVoid>
class nuiSignal2 : public nuiSignal, nuiNonCopyable 
{
  friend class nuiSlotsSink;
public:
  typedef typename detail::DefaultVoidToVoid<RetType>::type DesiredRetType;
  typedef nuiFastDelegate2<Param1, Param2, DesiredRetType> Slot;
  typedef std::function<DesiredRetType(Param1, Param2)> Function;

  nuiSignal2()
  {
  }

  ~nuiSignal2()
  {
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    for(; it != end; ++it)
    {
      RemoveConnection(*(*it).second, (*it).first.GetMemento());
    }
  }

  void operator()(Param1 param1, Param2 param2) const
  {
    if (mDisabled)
      return;

    //#FIXME: temp hack to avoid modification of the map (connection/deconnection of slots) during iteration
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    std::vector<Slot> tmp(mSlots.size());
    for(uint i = 0; it != end; ++it, i++)
      tmp[i] = (*it).first;
    
    for(uint i = 0; i < tmp.size(); i++)
      tmp[i](param1, param2);

    auto functions(mFunctions);
    for (auto fun : functions)
      fun(param1, param2);
  }


  bool IsConnected ()
  {
    return (!mSlots.empty() || !mFunctions.empty());
  }

  void Connect(const Function &function)
  {
    mFunctions.push_back(function);
  }

  void Disconnect(const Function &function)
  {
    auto it = mFunctions.begin();
    auto end = mFunctions.end();
    while (it != end)
    {
      if ((*it) == function)
      {
        mFunctions.erase(it);
        break;
      }
      ++it;
    }
  }
  
  

private:	
  void Connect(nuiSlotsSink &sink, const Slot &slot)
  {
    mSlots.push_back(std::make_pair(slot, &sink));
    AddConnection(sink, slot.GetMemento());
    UpdateStats(mSlots.size());
  }

  void Disconnect(nuiSlotsSink &sink, const Slot &slot)
  {
    RemoveConnection(sink, slot.GetMemento());
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      if ((*it).first == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &slot_)
  {
    Slot slot;
    slot.SetMemento(slot_);
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      const Slot& rS(it->first);
      if (rS == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  std::vector< std::pair<Slot, nuiSlotsSink*> > mSlots;
  std::vector< Function > mFunctions;
};

// N=3
template<class Param1, class Param2, class Param3, class RetType=detail::DefaultVoid>
class nuiSignal3 : public nuiSignal, nuiNonCopyable  
{
  friend class nuiSlotsSink;
public:
  typedef typename detail::DefaultVoidToVoid<RetType>::type DesiredRetType;
  typedef nuiFastDelegate3<Param1, Param2, Param3, DesiredRetType> Slot;
  typedef std::function<DesiredRetType(Param1, Param2, Param3)> Function;

  nuiSignal3()
  {
  }

  ~nuiSignal3()
  {
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    for(; it != end; ++it)
    {
      RemoveConnection(*(*it).second, (*it).first.GetMemento());
    }
  }

  void operator()(Param1 param1, Param2 param2, Param3 param3) const
  {
    if (mDisabled)
      return;

    //#FIXME: temp hack to avoid modification of the map (connection/deconnection of slots) during iteration
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    std::vector<Slot> tmp(mSlots.size());
    for(uint i = 0; it != end; ++it, i++)
      tmp[i] = (*it).first;
    
    for(uint i = 0; i < tmp.size(); i++)
      tmp[i](param1, param2, param3);

    auto functions(mFunctions);
    for (auto fun : functions)
      fun(param1, param2, param3);
  }

  bool IsConnected ()
  {
    return (!mSlots.empty() || !mFunctions.empty());
  }

  void Connect(const Function &function)
  {
    mFunctions.push_back(function);
  }

  void Disconnect(const Function &function)
  {
    auto it = mFunctions.begin();
    auto end = mFunctions.end();
    while (it != end)
    {
      if ((*it) == function)
      {
        mFunctions.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  void Connect(nuiSlotsSink &sink, const Slot &slot)
  {
    mSlots.push_back(std::make_pair(slot, &sink));
    AddConnection(sink, slot.GetMemento());
    UpdateStats(mSlots.size());
  }

  void Disconnect(nuiSlotsSink &sink, const Slot &slot)
  {
    RemoveConnection(sink, slot.GetMemento());
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      if ((*it).first == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &slot_)
  {
    Slot slot;
    slot.SetMemento(slot_);
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      const Slot& rS(it->first);
      if (rS == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  std::vector< std::pair<Slot, nuiSlotsSink*> > mSlots;
  std::vector< Function > mFunctions;
};


// N=4
template<class Param1, class Param2, class Param3, class Param4, class RetType=detail::DefaultVoid>
class nuiSignal4 : public nuiSignal, nuiNonCopyable
{
  friend class nuiSlotsSink;
public:
  typedef typename detail::DefaultVoidToVoid<RetType>::type DesiredRetType;
  typedef nuiFastDelegate4<Param1, Param2, Param3, Param4, DesiredRetType> Slot;
  typedef std::function<DesiredRetType(Param1, Param2, Param3, Param4)> Function;

  nuiSignal4()
  {
  }

  ~nuiSignal4()
  {
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    for(; it != end; ++it)
    {
      RemoveConnection(*(*it).second, (*it).first.GetMemento());
    }
  }

  void operator()(Param1 param1, Param2 param2, Param3 param3, Param4 param4) const
  {
    if (mDisabled)
      return;

    //#FIXME: temp hack to avoid modification of the map (connection/deconnection of slots) during iteration
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    std::vector<Slot> tmp(mSlots.size());
    for(uint i = 0; it != end; ++it, i++)
      tmp[i] = (*it).first;

    for(uint i = 0; i < tmp.size(); i++)
      tmp[i](param1, param2, param3, param4);

    auto functions(mFunctions);
    for (auto fun : functions)
      fun(param1, param2, param3, param4);
  }

  bool IsConnected ()
  {
    return (!mSlots.empty() || !mFunctions.empty());
  }

  void Connect(const Function &function)
  {
    mFunctions.push_back(function);
  }

  void Disconnect(const Function &function)
  {
    auto it = mFunctions.begin();
    auto end = mFunctions.end();
    while (it != end)
    {
      if ((*it) == function)
      {
        mFunctions.erase(it);
        break;
      }
      ++it;
    }
  }


private:
  void Connect(nuiSlotsSink &sink, const Slot &slot)
  {
    mSlots.push_back(std::make_pair(slot, &sink));
    AddConnection(sink, slot.GetMemento());
    UpdateStats(mSlots.size());
  }

  void Disconnect(nuiSlotsSink &sink, const Slot &slot)
  {
    RemoveConnection(sink, slot.GetMemento());
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      if ((*it).first == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &slot_)
  {
    Slot slot;
    slot.SetMemento(slot_);
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      const Slot& rS(it->first);
      if (rS == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  std::vector< std::pair<Slot, nuiSlotsSink*> > mSlots;
  std::vector< Function > mFunctions;
};


// N=5
template<class Param1, class Param2, class Param3, class Param4, class Param5, class RetType=detail::DefaultVoid>
class nuiSignal5 : public nuiSignal, nuiNonCopyable
{
  friend class nuiSlotsSink;
public:
  typedef typename detail::DefaultVoidToVoid<RetType>::type DesiredRetType;
  typedef nuiFastDelegate5<Param1, Param2, Param3, Param4, Param5, DesiredRetType> Slot;
  typedef std::function<DesiredRetType(Param1, Param2, Param3, Param4, Param5)> Function;

  nuiSignal5()
  {
  }

  ~nuiSignal5()
  {
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    for(; it != end; ++it)
    {
      RemoveConnection(*(*it).second, (*it).first.GetMemento());
    }
  }

  void operator()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5) const
  {
    if (mDisabled)
      return;

    //#FIXME: temp hack to avoid modification of the map (connection/deconnection of slots) during iteration
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::const_iterator end = mSlots.end();
    std::vector<Slot> tmp(mSlots.size());
    for(uint i = 0; it != end; ++it, i++)
      tmp[i] = (*it).first;

      for(uint i = 0; i < tmp.size(); i++)
        tmp[i](param1, param2, param3, param4, param5);

    auto functions(mFunctions);
    for (auto fun : functions)
      fun(param1, param2, param3, param4, param5);
  }

  bool IsConnected ()
  {
    return (!mSlots.empty() || !mFunctions.empty());
  }

  void Connect(const Function &function)
  {
    mFunctions.push_back(function);
  }

  void Disconnect(const Function &function)
  {
    auto it = mFunctions.begin();
    auto end = mFunctions.end();
    while (it != end)
    {
      if ((*it) == function)
      {
        mFunctions.erase(it);
        break;
      }
      ++it;
    }
  }
  
  

private:
  void Connect(nuiSlotsSink &sink, const Slot &slot)
  {
    mSlots.push_back(std::make_pair(slot, &sink));
    AddConnection(sink, slot.GetMemento());
    UpdateStats(mSlots.size());
  }

  void Disconnect(nuiSlotsSink &sink, const Slot &slot)
  {
    RemoveConnection(sink, slot.GetMemento());
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      if ((*it).first == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

  virtual void DisconnectInternal(const nuiDelegateMemento &slot_)
  {
    Slot slot;
    slot.SetMemento(slot_);
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator it = mSlots.begin();
    typename std::vector< std::pair<Slot, nuiSlotsSink*> >::iterator end = mSlots.end();
    while (it != end)
    {
      const Slot& rS(it->first);
      if (rS == slot)
      {
        mSlots.erase(it);
        break;
      }
      ++it;
    }
  }

private:
  std::vector< std::pair<Slot, nuiSlotsSink*> > mSlots;
  std::vector< Function > mFunctions;
};

class nuiSlotsSink
{
  friend class nuiSignal;
public:
  nuiSlotsSink();
  ~nuiSlotsSink();

  void DisconnectAll();
  
  // N=0
  template<class RetType>
  void Connect(nuiSignal0<RetType> &signal, const typename nuiSignal0<RetType>::Slot &slot)
  {
    signal.Connect(*this, slot);
  }
  template<class RetType>
  void Disconnect(nuiSignal0<RetType> &signal, const typename nuiSignal0<RetType>::Slot &slot)
  {
    signal.Disconnect(*this, slot);
  }

  // N=1
  template<class Param1, class RetType>
  void Connect(nuiSignal1<Param1, RetType> &signal, const typename nuiSignal1<Param1, RetType>::Slot &slot)
  {
    signal.Connect(*this, slot);
  }
  template<class Param1, class RetType>
  void Disconnect(nuiSignal1<Param1, RetType> &signal, const typename nuiSignal1<Param1, RetType>::Slot &slot)
  {
    signal.Disconnect(*this, slot);
  }

  // N=2
  template<class Param1, class Param2, class RetType>
  void Connect(nuiSignal2<Param1, Param2, RetType> &signal, const typename nuiSignal2<Param1, Param2, RetType>::Slot &slot)
  {
    signal.Connect(*this, slot);
  }
  template<class Param1, class Param2, class RetType>
  void Disconnect(nuiSignal2<Param1, Param2, RetType> &signal, const typename nuiSignal2<Param1, Param2, RetType>::Slot &slot)
  {
    signal.Disconnect(*this, slot);
  }

  // N=3
  template<class Param1, class Param2, class Param3, class RetType>
  void Connect(nuiSignal3<Param1, Param2, Param3, RetType> &signal, const typename nuiSignal3<Param1, Param2, Param3, RetType>::Slot &slot)
  {
    signal.Connect(*this, slot);
  }
  template<class Param1, class Param2, class Param3, class RetType>
  void Disconnect(nuiSignal3<Param1, Param2, Param3, RetType> &signal, const typename nuiSignal3<Param1, Param2, Param3, RetType>::Slot &slot)
  {
    signal.Disconnect(*this, slot);
  }

  // N=4
  template<class Param1, class Param2, class Param3, class Param4, class RetType>
  void Connect(nuiSignal4<Param1, Param2, Param3, Param4, RetType> &signal, const typename nuiSignal4<Param1, Param2, Param3, Param4, RetType>::Slot &slot)
  {
    signal.Connect(*this, slot);
  }
  template<class Param1, class Param2, class Param3, class Param4, class RetType>
  void Disconnect(nuiSignal4<Param1, Param2, Param3, Param4, RetType> &signal, const typename nuiSignal4<Param1, Param2, Param3, Param4, RetType>::Slot &slot)
  {
    signal.Disconnect(*this, slot);
  }

  // N=5
  template<class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
  void Connect(nuiSignal5<Param1, Param2, Param3, Param4, Param5, RetType> &signal, const typename nuiSignal5<Param1, Param2, Param3, Param4, Param5, RetType>::Slot &slot)
  {
    signal.Connect(*this, slot);
  }
  template<class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
  void Disconnect(nuiSignal5<Param1, Param2, Param3, Param4, Param5, RetType> &signal, const typename nuiSignal5<Param1, Param2, Param3, Param4, Param5, RetType>::Slot &slot)
  {
    signal.Disconnect(*this, slot);
  }

private:
  void AddConnection(nuiSignal* pSignal, const nuiDelegateMemento &slot);
  void RemoveConnection(nuiSignal* pSignal, const nuiDelegateMemento &slot);

private:
  std::multimap<nuiSignal*, nuiDelegateMemento> mConnections;
};


#endif // SIGNALS_SLOTS_H
