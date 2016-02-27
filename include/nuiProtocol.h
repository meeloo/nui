/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

class nuiProtocolFunctionBase
{
public:
  nuiProtocolFunctionBase(const nglString& rMethodName) : mName(rMethodName) {}
  virtual ~nuiProtocolFunctionBase() {}
  
  virtual bool IsCompatible(const nuiMessage& rMessage) const = 0;
  virtual void Call(const nuiMessage& rMessage) const = 0;
  
  const nglString& GetName() const { return mName; }
  virtual nglString GetDescription() const = 0;
protected:
  nglString mName;
};

class nuiProtocolFunction0 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction0(const nglString& rMethodName, std::function<void()> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 1)
      return 0;
    
    return ((nglString)rMessage.GetData(0)) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction();
  }
  
  nglString GetDescription() const
  {
    nglString desc;
    desc.Add(mName).Add("()");
    return desc;
  }

private:
  std::function<void()> mFunction;
};

template <typename T1>
class nuiProtocolFunction1 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction1(const nglString& rMethodName, std::function<void(T1)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 2)
      return 0;
    if (rMessage.GetData(1).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    return ((nglString)rMessage.GetData(0)) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1));
  }
  
  nglString GetDescription() const
  {
    nglString desc;
    desc.Add(mName).Add("(");
    desc.Add(nuiMessageDataTypeTrait<T1>::GetName());
    desc.Add(")");
    return desc;
  }
private:
  std::function<void(T1)> mFunction;
};

template <typename T1, typename T2>
class nuiProtocolFunction2 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction2(const nglString& rMethodName, std::function<void(T1,T2)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 3)
      return 0;
    if (rMessage.GetData(1).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    if (rMessage.GetData(2).mType != nuiMessageDataTypeTrait<T2>::GetType())
      return false;
    return ((nglString)rMessage.GetData(0)) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2));
  }
  
  nglString GetDescription() const
  {
    nglString desc;
    desc.Add(mName).Add("(");
    desc.Add(nuiMessageDataTypeTrait<T1>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T2>::GetName());
    desc.Add(")");
    return desc;
  }
private:
  std::function<void(T1,T2)> mFunction;
};


template <typename T1, typename T2, typename T3>
class nuiProtocolFunction3 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction3(const nglString& rMethodName, std::function<void(T1,T2,T3)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 4)
      return 0;
    if (rMessage.GetData(1).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    if (rMessage.GetData(2).mType != nuiMessageDataTypeTrait<T2>::GetType())
      return false;
    if (rMessage.GetData(3).mType != nuiMessageDataTypeTrait<T3>::GetType())
      return false;
    return ((nglString)rMessage.GetData(0)) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2), rMessage.GetData(3));
  }
  
  nglString GetDescription() const
  {
    nglString desc;
    desc.Add(mName).Add("(");
    desc.Add(nuiMessageDataTypeTrait<T1>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T2>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T3>::GetName());
    desc.Add(")");
    return desc;
  }
private:
  std::function<void(T1,T2,T3)> mFunction;
};


template <typename T1, typename T2, typename T3, typename T4>
class nuiProtocolFunction4 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction4(const nglString& rMethodName, std::function<void(T1,T2,T3,T4)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 5)
      return 0;
    if (rMessage.GetData(1).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    if (rMessage.GetData(2).mType != nuiMessageDataTypeTrait<T2>::GetType())
      return false;
    if (rMessage.GetData(3).mType != nuiMessageDataTypeTrait<T3>::GetType())
      return false;
    if (rMessage.GetData(4).mType != nuiMessageDataTypeTrait<T4>::GetType())
      return false;
    return ((nglString)rMessage.GetData(0)) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2), rMessage.GetData(3), rMessage.GetData(4));
  }
  
  nglString GetDescription() const
  {
    nglString desc;
    desc.Add(mName).Add("(");
    desc.Add(nuiMessageDataTypeTrait<T1>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T2>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T3>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T4>::GetName());
    desc.Add(")");
    return desc;
  }
private:
  std::function<void(T1,T2,T3,T4)> mFunction;
};


template <typename T1, typename T2, typename T3, typename T4, typename T5>
class nuiProtocolFunction5 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction5(const nglString& rMethodName, std::function<void(T1,T2,T3,T4,T5)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 6)
      return 0;
    if (rMessage.GetData(1).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    if (rMessage.GetData(2).mType != nuiMessageDataTypeTrait<T2>::GetType())
      return false;
    if (rMessage.GetData(3).mType != nuiMessageDataTypeTrait<T3>::GetType())
      return false;
    if (rMessage.GetData(4).mType != nuiMessageDataTypeTrait<T4>::GetType())
      return false;
    if (rMessage.GetData(5).mType != nuiMessageDataTypeTrait<T5>::GetType())
      return false;
    return ((nglString)rMessage.GetData(0)) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2), rMessage.GetData(3), rMessage.GetData(4), rMessage.GetData(5));
  }
  
  nglString GetDescription() const
  {
    nglString desc;
    desc.Add(mName).Add("(");
    desc.Add(nuiMessageDataTypeTrait<T1>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T2>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T3>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T4>::GetName()).Add(", ");
    desc.Add(nuiMessageDataTypeTrait<T5>::GetName());
    desc.Add(")");
    return desc;
  }
private:
  std::function<void(T1,T2,T3,T4,T5)> mFunction;
};


class nuiProtocol : public nuiMessageClient
{
public:
  nuiProtocol(nuiTCPClient* pTCPClient)
  : nuiMessageClient(pTCPClient)
  {
  }
  
  void AddMethod(nuiProtocolFunctionBase* pProtocolFunction)
  {
    NGL_ASSERT(mMethods.find(pProtocolFunction->GetName()) == mMethods.end());
    mMethods[pProtocolFunction->GetName()] = pProtocolFunction;
  }
  
  void AddMethod(const nglString& rMethodName, std::function<void()> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction0(rMethodName, rFunction);
  }

  template <typename T1>
  void AddMethod(const nglString& rMethodName, std::function<void(T1)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction1<T1>(rMethodName, rFunction);
  }

  template <typename T1, typename T2>
  void AddMethod(const nglString& rMethodName, std::function<void(T1,T2)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction2<T1,T2>(rMethodName, rFunction);
  }

  template <typename T1, typename T2, typename T3>
  void AddMethod(const nglString& rMethodName, std::function<void(T1,T2,T3)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction3<T1,T2,T3>(rMethodName, rFunction);
  }

  template <typename T1, typename T2, typename T3, typename T4>
  void AddMethod(const nglString& rMethodName, std::function<void(T1,T2,T3,T4)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction4<T1,T2,T3,T4>(rMethodName, rFunction);
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  void AddMethod(const nglString& rMethodName, std::function<void(T1,T2,T3,T4,T5)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction5<T1,T2,T3,T4,T5>(rMethodName, rFunction);
  }

  void HandleMessages()
  {
    HandleMessages([](nuiMessage& msg, nuiProtocolFunctionBase* pBase)
                   {
                     NGL_OUT("Protocol Error: received message\n%s\nis incompatible with prototype\n%s\n", msg.GetDescription().GetChars(), pBase->GetDescription().GetChars());
                   
                     return false;
                   });
  }

  void HandleMessages(std::function<bool(nuiMessage&, nuiProtocolFunctionBase*)> errorHandler)
  {
    Read([&](nuiMessage* pMessage){
      if (pMessage->GetSize())
      {
        nglString name(pMessage->GetData(0));
        auto it = mMethods.find(name);
        if (it != mMethods.end())
        {
          nuiProtocolFunctionBase* pFunction = it->second;
          if (pFunction->IsCompatible(*pMessage))
          {
            pFunction->Call(*pMessage);
          }
          else
          {
            if (errorHandler)
            {
              if (!errorHandler(*pMessage, pFunction))
              {
                delete pMessage;
                return false;
              }
            }
          }
        }
      }
      delete pMessage;
      return true;
    });
  }
  
  void StopHandlingMessage()
  {
    mRunning = false;
  }

private:
  std::map<nglString, nuiProtocolFunctionBase*> mMethods;
  bool mRunning = false;
};

// nui_make_function was lifted from http://stackoverflow.com/questions/27825559/why-is-there-no-stdmake-function
// in order to have simpler protocol declarations:

// For generic types that are functors, delegate to its 'operator()'
template <typename T>
struct function_traits
: public function_traits<decltype(&T::operator())>
{};

// for pointers to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> {
  enum { arity = sizeof...(Args) };
  typedef std::function<ReturnType (Args...)> f_type;
};

// for pointers to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) > {
  enum { arity = sizeof...(Args) };
  typedef std::function<ReturnType (Args...)> f_type;
};

// for function pointers
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>  {
  enum { arity = sizeof...(Args) };
  typedef std::function<ReturnType (Args...)> f_type;
};

template <typename L>
static typename function_traits<L>::f_type nui_make_function(L l){
  return (typename function_traits<L>::f_type)(l);
}

//handles bind & multiple function call operator()'s
template<typename ReturnType, typename... Args, class T>
auto nui_make_function(T&& t)
-> std::function<decltype(ReturnType(t(std::declval<Args>()...)))(Args...)>
{return {std::forward<T>(t)};}

//handles explicit overloads
template<typename ReturnType, typename... Args>
auto nui_make_function(ReturnType(*p)(Args...))
-> std::function<ReturnType(Args...)> {
  return {p};
}

//handles explicit overloads
template<typename ReturnType, typename... Args, typename ClassType>
auto nui_make_function(ReturnType(ClassType::*p)(Args...))
-> std::function<ReturnType(Args...)> {
  return {p};
}

