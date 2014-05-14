/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#pragma once

#include "nuiFastDelegate.h"
#include "nuiRefCount.h"


template <class Param>
class DeRefConst
{
public:
  typedef Param Type;
};

template <class Param>
class DeRefConst<const Param&>
{
public:
  typedef Param Type;
};

template <class Param>
class RefConst
{
public:
  typedef const Param& Type;
};

template <class Param>
class RefConst<const Param&>
{
public:
  typedef const Param& Type;
};





class nuiTask : public nuiRefCount
{
public:
  nuiTask()
  : mCanceled(false)
  {
  }
  
  virtual ~nuiTask()
  {
  }
  
  void Cancel()
  {
    mCanceled = true;
  }
  
  bool IsCanceled() const
  {
    return mCanceled;
  }
  
  void Run() const
  {
    if (mCanceled)
      return;
    Execute();
  }
  
protected:
  virtual void Execute() const = 0;
  bool mCanceled;
};


// No Param:
template <class Ret>
class nuiTask0 : public nuiTask
{
public:
  typedef nuiFastDelegate0<Ret> Delegate;
  nuiTask0(const Delegate& rDelegate)
  : mDelegate(rDelegate)
  {    
  }
  
  
protected:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate();
  }
};

#define DEFPARAM(N,TYPE) \
  public:\
    TYPE GetParam##N() const  { return mP##N; }\
    void SetParam##N(TYPE P)  { mP##N = P; }\
  protected:\
    TYPE mP##N;

// 1 Param
template <class P0, class Ret>
class nuiTask1 : public nuiTask
{
public:
  typedef nuiFastDelegate1<P0, Ret> Delegate;
  nuiTask1(const Delegate& rDelegate, typename RefConst<P0>::Type rP0)
  : mP0(rP0), mDelegate(rDelegate)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0);
  }
};

// 2 Params
template <class P0, class P1, class Ret>
class nuiTask2 : public nuiTask
{
public:
  typedef nuiFastDelegate2<P0, P1, Ret> Delegate;
  nuiTask2(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1);
  }
};

// 3 Params
template <class P0, class P1, class P2, class Ret>
class nuiTask3 : public nuiTask
{
public:
  typedef nuiFastDelegate3<P0, P1, P2, Ret> Delegate;
  nuiTask3(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1, typename RefConst<P2>::Type rP2)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1), mP2(rP2)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);
  DEFPARAM(2, typename DeRefConst<P2>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1, mP2);
  }
};

// 4 Params
template <class P0, class P1, class P2, class P3, class Ret>
class nuiTask4 : public nuiTask
{
public:
  typedef nuiFastDelegate4<P0, P1, P2, P3, Ret> Delegate;
  nuiTask4(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1, typename RefConst<P2>::Type rP2, typename RefConst<P3>::Type rP3)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1), mP2(rP2), mP3(rP3)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);
  DEFPARAM(2, typename DeRefConst<P2>::Type);
  DEFPARAM(3, typename DeRefConst<P3>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1, mP2, mP3);
  }
};


// 5 Params
template <class P0, class P1, class P2, class P3, class P4, class Ret>
class nuiTask5 : public nuiTask
{
public:
  typedef nuiFastDelegate5<P0, P1, P2, P3, P4, Ret> Delegate;
  nuiTask5(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1, typename RefConst<P2>::Type rP2, typename RefConst<P3>::Type rP3, typename RefConst<P4>::Type rP4)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1), mP2(rP2), mP3(rP3), mP4(rP4)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);
  DEFPARAM(2, typename DeRefConst<P2>::Type);
  DEFPARAM(3, typename DeRefConst<P3>::Type);
  DEFPARAM(4, typename DeRefConst<P4>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1, mP2, mP3, mP4);
  }
};



// 6 Params
template <class P0, class P1, class P2, class P3, class P4, class P5, class Ret>
class nuiTask6 : public nuiTask
{
public:
  typedef nuiFastDelegate6<P0, P1, P2, P3, P4, P5, Ret> Delegate;
  nuiTask6(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1, typename RefConst<P2>::Type rP2, typename RefConst<P3>::Type rP3, typename RefConst<P4>::Type rP4, typename RefConst<P5>::Type rP5)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1), mP2(rP2), mP3(rP3), mP4(rP4), mP5(rP5)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);
  DEFPARAM(2, typename DeRefConst<P2>::Type);
  DEFPARAM(3, typename DeRefConst<P3>::Type);
  DEFPARAM(4, typename DeRefConst<P4>::Type);
  DEFPARAM(5, typename DeRefConst<P5>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1, mP2, mP3, mP4, mP5);
  }
};



// 7 Params
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class Ret>
class nuiTask7 : public nuiTask
{
public:
  typedef nuiFastDelegate7<P0, P1, P2, P3, P4, P5, P6, Ret> Delegate;
  nuiTask7(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1, typename RefConst<P2>::Type rP2, typename RefConst<P3>::Type rP3, typename RefConst<P4>::Type rP4, typename RefConst<P5>::Type rP5, typename RefConst<P6>::Type rP6)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1), mP2(rP2), mP3(rP3), mP4(rP4), mP5(rP5), mP6(rP6)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);
  DEFPARAM(2, typename DeRefConst<P2>::Type);
  DEFPARAM(3, typename DeRefConst<P3>::Type);
  DEFPARAM(4, typename DeRefConst<P4>::Type);
  DEFPARAM(5, typename DeRefConst<P5>::Type);
  DEFPARAM(6, typename DeRefConst<P6>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1, mP2, mP3, mP4, mP5, mP6);
  }
};



// 8 Params
template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class Ret>
class nuiTask8 : public nuiTask
{
public:
  typedef nuiFastDelegate8<P0, P1, P2, P3, P4, P5, P6, P7, Ret> Delegate;
  nuiTask8(const Delegate& rDelegate, typename RefConst<P0>::Type rP0, typename RefConst<P1>::Type rP1, typename RefConst<P2>::Type rP2, typename RefConst<P3>::Type rP3, typename RefConst<P4>::Type rP4, typename RefConst<P5>::Type rP5, typename RefConst<P6>::Type rP6, typename RefConst<P7>::Type rP7)
  : mDelegate(rDelegate), mP0(rP0), mP1(rP1), mP2(rP2), mP3(rP3), mP4(rP4), mP5(rP5), mP6(rP6), mP7(rP7)
  {    
  }
  
  DEFPARAM(0, typename DeRefConst<P0>::Type);
  DEFPARAM(1, typename DeRefConst<P1>::Type);
  DEFPARAM(2, typename DeRefConst<P2>::Type);
  DEFPARAM(3, typename DeRefConst<P3>::Type);
  DEFPARAM(4, typename DeRefConst<P4>::Type);
  DEFPARAM(5, typename DeRefConst<P5>::Type);
  DEFPARAM(6, typename DeRefConst<P6>::Type);
  DEFPARAM(7, typename DeRefConst<P7>::Type);

private:
  Delegate mDelegate;
  virtual void Execute() const
  {
    mDelegate(mP0, mP1, mP2, mP3, mP4, mP5, mP6, mP7);
  }
};





// Task creation shortcurts:

// No Param
template <class RetType>
nuiTask0<RetType>* nuiMakeTask(RetType (*func)())
{ 
	return new nuiTask0<RetType>(func);
}

template <class X, class Y, class RetType>
nuiTask0<RetType>* nuiMakeTask(Y* x, RetType (X::*func)())
{ 
	return new nuiTask0<RetType>(nuiMakeDelegate(x, func));
}

template <class X, class Y, class RetType>
nuiTask0<RetType>* nuiMakeTask(Y* x, RetType (X::*func)() const)
{ 
	return new nuiTask0<RetType>(nuiMakeDelegate(x, func));
}


// 1 Param
template <class Param0, class RetType>
nuiTask1<Param0, RetType>* nuiMakeTask(RetType (*func)(Param0 p0), typename RefConst<Param0>::Type P0)
{ 
	return new nuiTask1<Param0, RetType>(func, P0);
}

template <class X, class Y, class Param0, class RetType>
nuiTask1<Param0, RetType>* nuiMakeTask(Y* x, RetType (X::*func)(Param0 p0), typename RefConst<Param0>::Type  P0)
{ 
	return new nuiTask1<Param0, RetType>(nuiMakeDelegate(x, func), P0);
}

template <class X, class Y, class Param0, class RetType>
nuiTask1<Param0, RetType>* nuiMakeTask(Y* x, RetType (X::*func)(Param0 p0) const, typename RefConst<Param0>::Type  P0)
{ 
	return new nuiTask1<Param0, RetType>(nuiMakeDelegate(x, func), P0);
}



// 2 Params
template <class Param0, class Param1, class RetType>
nuiTask2<Param0, Param1, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1),
                                               Param0 P0, Param1 P1)
{ 
	return new nuiTask1<Param1, RetType>(func, P0, P1);
}

template <class X, class Y, class Param0, class Param1, class RetType>
nuiTask2<Param0, Param1, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1),
                                               Param0 P0, Param1 P1)
{ 
	return new nuiTask2<Param0, Param1, RetType>(nuiMakeDelegate(x, func), P0, P1);
}

template <class X, class Y, class Param0, class Param1, class RetType>
nuiTask2<Param0, Param1, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1) const,
                                               Param0 P0, Param1 P1)
{ 
	return new nuiTask2<Param0, Param1, RetType>(nuiMakeDelegate(x, func), P0, P1);
}


// 3 Params
template <class Param0, class Param1, class Param2, class RetType>
nuiTask3<Param0, Param1, Param2, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1, Param2 p2),
                                                       Param0 P0, Param1 P1, Param2 P2)
{ 
	return new nuiTask3<Param0, Param1, Param2, RetType>(func, P0, P1, P2);
}

template <class X, class Y, class Param0, class Param1, class Param2, class RetType>
nuiTask3<Param0, Param1, Param2, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2),
                                                       Param0 P0, Param1 P1, Param2 P2)
{ 
	return new nuiTask3<Param0, Param1, Param2, RetType>(nuiMakeDelegate(x, func), P0, P1, P2);
}

template <class X, class Y, class Param0, class Param1, class Param2, class RetType>
nuiTask3<Param0, Param1, Param2, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2) const,
                                                       Param0 P0, Param1 P1, Param2 P2)
{ 
	return new nuiTask3<Param0, Param1, Param2, RetType>(nuiMakeDelegate(x, func), P0, P1, P2);
}

// 4 Params
template <class Param0, class Param1, class Param2, class Param3, class RetType>
nuiTask4<Param0, Param1, Param2, Param3, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3)
{ 
	return new nuiTask4<Param0, Param1, Param2, Param3, RetType>(func, P0, P1, P2, P3);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class RetType>
nuiTask4<Param0, Param1, Param2, Param3, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3)
{ 
	return new nuiTask4<Param0, Param1, Param2, Param3, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class RetType>
nuiTask4<Param0, Param1, Param2, Param3, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3) const,
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3)
{ 
	return new nuiTask4<Param0, Param1, Param2, Param3, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3);
}

// 5 Params
template <class Param0, class Param1, class Param2, class Param3, class Param4, class RetType>
nuiTask5<Param0, Param1, Param2, Param3, Param4, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4)
{ 
	return new nuiTask5<Param0, Param1, Param2, Param3, Param4, RetType>(func, P0, P1, P2, P3, P4);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class RetType>
nuiTask5<Param0, Param1, Param2, Param3, Param4, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4)
{ 
	return new nuiTask5<Param0, Param1, Param2, Param3, Param4, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class RetType>
nuiTask5<Param0, Param1, Param2, Param3, Param4, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4) const,
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4)
{ 
	return new nuiTask5<Param0, Param1, Param2, Param3, Param4, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4);
}

// 6 Params
template <class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
nuiTask6<Param0, Param1, Param2, Param3, Param4, Param5, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5)
{ 
	return new nuiTask6<Param0, Param1, Param2, Param3, Param4, Param5, RetType>(func, P0, P1, P2, P3, P4, P5);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
nuiTask6<Param0, Param1, Param2, Param3, Param4, Param5, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5)
{ 
	return new nuiTask6<Param0, Param1, Param2, Param3, Param4, Param5, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4, P5);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
nuiTask6<Param0, Param1, Param2, Param3, Param4, Param5, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const,
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5)
{ 
	return new nuiTask6<Param0, Param1, Param2, Param3, Param4, Param5, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4, P5);
}

// 7 Params
template <class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class RetType>
nuiTask7<Param0, Param1, Param2, Param3, Param4, Param5, Param6, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6)
{ 
	return new nuiTask7<Param0, Param1, Param2, Param3, Param4, Param5, Param6, RetType>(func, P0, P1, P2, P3, P4, P5, P6);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class RetType>
nuiTask7<Param0, Param1, Param2, Param3, Param4, Param5, Param6, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6)
{ 
	return new nuiTask7<Param0, Param1, Param2, Param3, Param4, Param5, Param6, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4, P5, P6);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class RetType>
nuiTask7<Param0, Param1, Param2, Param3, Param4, Param5, Param6, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const,
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6)
{ 
	return new nuiTask7<Param0, Param1, Param2, Param3, Param4, Param5, Param6, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4, P5, P6);
}


// 8 Params
template <class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class RetType>
nuiTask8<Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>* nuiMakeTask(RetType (*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7)
{ 
	return new nuiTask8<Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>(func, P0, P1, P2, P3, P4, P5, P6, P7);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class RetType>
nuiTask8<Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7),
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7)
{ 
	return new nuiTask8<Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4, P5, P6, P7);
}

template <class X, class Y, class Param0, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class RetType>
nuiTask8<Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>* nuiMakeTask(Y*x, RetType (X::*func)(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const,
                                       Param0 P0, Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7)
{ 
	return new nuiTask8<Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>(nuiMakeDelegate(x, func), P0, P1, P2, P3, P4, P5, P6, P7);
}



#undef DEFPARAM
