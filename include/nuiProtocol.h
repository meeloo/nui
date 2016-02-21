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
  
protected:
  nglString mName;
};

template <typename RT>
class nuiProtocolFunction0 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction0(const nglString& rMethodName, std::function<RT()> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 1)
      return 0;
    
    return rMessage.GetData(0) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction();
  }
  
private:
  std::function<RT()> mFunction;
};

template <typename RT, typename T1>
class nuiProtocolFunction1 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction1(const nglString& rMethodName, std::function<RT(T1)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 2)
      return 0;
    if (rMessage.GetData(0).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    return rMessage.GetData(1) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1));
  }
  
private:
  std::function<RT(T1)> mFunction;
};

template <typename RT, typename T1, typename T2>
class nuiProtocolFunction2 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction2(const nglString& rMethodName, std::function<RT(T1,T2)> rFunction)
  : nuiProtocolFunctionBase(rMethodName), mFunction(rFunction) { }
  
  bool IsCompatible(const nuiMessage& rMessage) const
  {
    if (rMessage.GetSize() != 3)
      return 0;
    if (rMessage.GetData(1).mType != nuiMessageDataTypeTrait<T1>::GetType())
      return false;
    if (rMessage.GetData(2).mType != nuiMessageDataTypeTrait<T2>::GetType())
      return false;
    return rMessage.GetData(0) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2));
  }
  
private:
  std::function<RT(T1,T2)> mFunction;
};


template <typename RT, typename T1, typename T2, typename T3>
class nuiProtocolFunction3 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction3(const nglString& rMethodName, std::function<RT(T1,T2,T3)> rFunction)
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
    return rMessage.GetData(0) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2), rMessage.GetData(3));
  }
  
private:
  std::function<RT(T1,T2,T3)> mFunction;
};


template <typename RT, typename T1, typename T2, typename T3, typename T4>
class nuiProtocolFunction4 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction4(const nglString& rMethodName, std::function<RT(T1,T2,T3,T4)> rFunction)
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
    return rMessage.GetData(0) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2), rMessage.GetData(3), rMessage.GetData(4));
  }
  
private:
  std::function<RT(T1,T2,T3,T4)> mFunction;
};


template <typename RT, typename T1, typename T2, typename T3, typename T4, typename T5>
class nuiProtocolFunction5 : public nuiProtocolFunctionBase
{
public:
  nuiProtocolFunction5(const nglString& rMethodName, std::function<RT(T1,T2,T3,T4,T5)> rFunction)
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
    return rMessage.GetData(0) == mName;
  }
  
  void Call(const nuiMessage& rMessage) const
  {
    NGL_ASSERT(IsCompatible(rMessage));
    mFunction(rMessage.GetData(1), rMessage.GetData(2), rMessage.GetData(3), rMessage.GetData(4), rMessage.GetData(5));
  }
  
private:
  std::function<RT(T1,T2,T3,T4,T5)> mFunction;
};


class nuiProtocol : public nuiMessageClient
{
public:
  nuiProtocol(nuiTCPClient* pTCPClient)
  : nuiMessageClient(pTCPClient)
  {
  }
  
  template <typename RT>
  void AddMethod(const nglString& rMethodName, std::function<RT()> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction0<RT>(rMethodName, rFunction);
  }

  template <typename RT, typename T1>
  void AddMethod(const nglString& rMethodName, std::function<RT(T1)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction1<RT,T1>(rMethodName, rFunction);
  }

  template <typename RT, typename T1, typename T2>
  void AddMethod(const nglString& rMethodName, std::function<RT(T1,T2)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction2<RT,T1,T2>(rMethodName, rFunction);
  }

  template <typename RT, typename T1, typename T2, typename T3>
  void AddMethod(const nglString& rMethodName, std::function<RT(T1,T2,T3)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction3<RT,T1,T2,T3>(rMethodName, rFunction);
  }

  template <typename RT, typename T1, typename T2, typename T3, typename T4>
  void AddMethod(const nglString& rMethodName, std::function<RT(T1,T2,T3,T4)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction4<RT,T1,T2,T3,T4>(rMethodName, rFunction);
  }

  template <typename RT, typename T1, typename T2, typename T3, typename T4, typename T5>
  void AddMethod(const nglString& rMethodName, std::function<RT(T1,T2,T3,T4,T5)> rFunction)
  {
    NGL_ASSERT(mMethods.find(rMethodName) == mMethods.end());
    mMethods[rMethodName] = new nuiProtocolFunction5<RT,T1,T2,T3,T4,T5>(rMethodName, rFunction);
  }

  void HandleMessages()
  {
    mRunning = true;
    std::vector<uint8> data;
    while (mRunning)
    {
      nuiMessage* pMessage = Read();
      nglString name(pMessage->GetData(0));
      auto it = mMethods.find(name);
      if (it != mMethods.end())
      {
        nuiProtocolFunctionBase* pFunction = it->second;
        pFunction->Call(*pMessage);
      }
      delete pMessage;
    }
  }
  
  void StopHandlingMessage()
  {
    mRunning = false;
  }
  
private:
  std::map<nglString, nuiProtocolFunctionBase*> mMethods;
  bool mRunning = false;
};

