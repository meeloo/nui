/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

#include "ucdata.h"

#if (defined _UNIX_)
#include <signal.h>
#endif

#ifdef _UIKIT_
#import <UIKit/UIkit.h>
#endif

#ifdef _COCOA_
#import <Cocoa/Cocoa.h>
#endif

/* Defined in <platform>/nglKernel.cpp
 */
extern const nglChar* gpKernelErrorTable[];

/* The kernel global instance is referenced here (and only here)
 */
nglKernel * App = NULL;

nglTime nglKernel::mStartTime;


/*
 * Life cycle
 */

void nglKernel::Quit (int Code)
{
  // Pure nglKernel instance : does not handle event loop, can't quit by its own will.
}

void nglKernel::AddExit (ExitFunc Func)
{
  mExitFuncs.push_front(Func);
}

void nglKernel::DelExit (ExitFunc Func)
{
  mExitFuncs.remove(Func);
}


/*
 * Runtime debugging
 */

// bool nglKernel::GetDebug() [inlined in header]

void nglKernel::SetDebug (bool Debug)
{
  mDebug = Debug;
}


/*
 * Logging and output
 */

nglLog& nglKernel::GetLog()
{
  if (!mpLog)
  {
    mpLog = new nglLog();
  }
  return *mpLog;
}


nglConsole& nglKernel::GetConsole()
{
  //#HACH This is a hack to have NGL_OUT working event when nuiInit hasn't been called yet
  if (!this)
  {
    return *(nglConsole*)NULL;
  }

  if (!mpCon)
  {
    mpCon = new nglConsole();
    mOwnCon = true;
  }
  return *mpCon;
}

void nglKernel::SetConsole (nglConsole* pConsole)
{
  if (mpCon && mOwnCon)
    delete mpCon;
  mpCon = pConsole;
  mOwnCon = false;
}

/*
 * Application context
 */

const nglPath& nglKernel::GetPath()
{
  return mPath;
}

const nglString& nglKernel::GetName()
{
  return mName;
}

int nglKernel::GetArgCount()
{
  return (int)mArgs.size();
}

const nglString& nglKernel::GetArg (int Index)
{
  return (Index < (int)mArgs.size()) ? mArgs[Index] : nglString::Null;
}


#ifndef _MINUI3_
/* Clipboard (platform specific)
 *
nglString GetClipboard();
bool SetClipboard(const nglString& rString);
 */
nglClipBoard& nglKernel::GetClipBoard()
{
  return mClipboard;
}

/*
 * DataTypesRegistry
 */
nglDataTypesRegistry& nglKernel::GetDataTypesRegistry()
{
  return mDataTypesRegistry;
}
#endif

/*
 * User callbacks
 */

// Init/Exit:
void nglKernel::OnInit()
{
}

void nglKernel::OnExit(int Code)
{
}

// Device management:
void nglKernel::OnDeviceAdded(const nglDeviceInfo* pDeviceInfo)
{
}

void nglKernel::OnDeviceRemoved(const nglDeviceInfo* pDeviceInfo)
{
}


/*
 * Life cycle (private)
 */

void nglKernel::IncRef()
{
  mRefCount++;
}

void nglKernel::DecRef()
{
  NGL_ASSERT(mRefCount > 0);

  mRefCount--;
  if (mRefCount == 0)
    delete this;
}

void SendSomeMail();

void nglKernel::Init()
{
  //SendSomeMail();
  mRefCount = 0;
  App = this;
#ifdef _DEBUG_
  mDebug = true;
#else
  mDebug = false;
#endif
  mpLog = NULL;
  mpCon = NULL;
  mOwnCon = false;
  mActive = true;
}

void nglKernel::Exit(int32 ExitCode)
{
#ifndef _MINUI3_
  nuiMainWindow::DestroyAllWindows();
#endif
  mKernelEventSink.DisconnectAll();
  nglVolume::UnmountAll();
  nuiAnimation::ReleaseTimer();

  ExitFuncList::iterator func_i;

  for (func_i = mExitFuncs.begin(); func_i != mExitFuncs.end(); ++func_i)
  {
    ExitFunc func;

    func = *func_i;
    if (func)
      (*func)();
  }
  mExitFuncs.clear();

#ifdef NGL_STRING_STATS
  nglString::DumpStats(NGL_LOG_INFO);
#endif

  if (mpLog)
  {
    delete mpLog;
    mpLog = NULL;
  }

  if (mpCon && mOwnCon)
  {
    delete mpCon;
    mpCon = NULL;
  }

  nglString::ReleaseStringConvs();
}

void nglKernel::SetName(const nglString& rName)
{
  mName = rName;
}

void nglKernel::SetPath(const nglPath& rPath)
{
  mPath = rPath;
}

void nglKernel::ParseCmdLine (char* pCmdLine)
{
  enum { skipping, reading_token, reading_quoted_token } state = skipping;
  nglString arg;
  char* p = pCmdLine;
  char* token_p = NULL;
  bool eos;

  do
  {
    eos = (*p == 0);

    switch (state)
    {
      case skipping:
        if (*p == '"') { state = reading_quoted_token; token_p = p + 1; } else
        if (*p != ' ') { state = reading_token; token_p = p; }
        /* else keep skipping */
        break;
      case reading_token:
      case reading_quoted_token:
        if (eos ||
            (state == reading_token && *p == ' ') ||
            (state == reading_quoted_token && *p == '"'))
        {
          *p = 0;
          arg = token_p;
          mArgs.push_back (arg);
          state = skipping;
        }
        break;
    }
    p++;
  }
  while (!eos);
}

void nglKernel::AddArg (const nglString& rArg)
{
  uint size = mArgs.size();

  mArgs.resize(size + 1);
  mArgs[size++] = rArg;
}

void nglKernel::DelArg (int Pos, int Count)
{
  if (Count > 1)
    mArgs.erase (mArgs.begin() + Pos, mArgs.begin() + Pos + Count);
  else
    mArgs.erase (mArgs.begin() + Pos);
}


/*
 * Internal callbacks
 */

const nglChar* nglKernel::OnError (uint& rError) const
{
  return FetchError(gpKernelErrorTable, NULL, rError);
}

void nglKernel::TimedPrint(const char* Format, ...) const
{
  va_list args;

  va_start(args, Format);
  TimedPrintv(Format, args);
  va_end(args);
}

void nglKernel::TimedPrintv(const char* Format, va_list Args) const
{
  double now = nglTime();
  double duration = now - (double)mStartTime;
  nglString fmt;
  fmt.Formatv(Format, Args);
  printf("%f: %s\n", duration, fmt.GetChars());
}

void nglKernel::CallOnInit()
{
  mMainThreadID = nglThread::GetCurThreadID();
  
  double now = nglTime();
  ucdata_init_static();
  double then = nglTime();

  //TimedPrint("ucdata_init_static took %f seconds\n", then - now);

#ifndef _MINUI3_
  NGL_DEBUG( NGL_LOG("kernel", NGL_LOG_INFO, "Init (%d parameter%s)", GetArgCount(), (GetArgCount() > 1) ? "s" : _T("")); )
  nglVolume* pResources = new nuiNativeResourceVolume();
  nglVolume::Mount(pResources);
#endif

  nuiTimer* pTimer = nuiAnimation::AcquireTimer();
  mKernelEventSink.Connect(pTimer->Tick, &nglKernel::ProcessMessages);
  mpNotificationManager = new nuiNotificationManager();

  //TimedPrint("before nglKernel::OnInit\n");
  OnInit();
  //TimedPrint("after nglKernel::OnInit\n");
}

void nglKernel::CallOnExit(int Code)
{
  NGL_DEBUG( NGL_LOG("kernel", NGL_LOG_INFO, "Exit (code: %d)", Code); )
  OnExit (Code);
  nuiAnimation::ReleaseTimer();
}

void nglKernel::CallOnWillExit()
{
  OnWillExit();
}

void nglKernel::OnWillExit()
{

}

void nglKernel::CallOnActivation()
{
  mActive = true;
  OnActivation();
}

void nglKernel::CallOnDeactivation()
{
  mActive = false;
  OnDeactivation();
}

void nglKernel::OnActivation()
{
}

void nglKernel::OnDeactivation()
{
}

bool nglKernel::IsActive() const
{
  return mActive;
}

///< Memory-warning methods (only availble on iPhone OS/UIKit)
void nglKernel::OnMemoryWarning()
{
}

void nglKernel::CallOnMemoryWarning()
{
	OnMemoryWarning();
}



char* nglCrashEmail = NULL;
void nglKernel::SetCrashReportEmail(const nglString& rEmail)
{
  nglCrashEmail = rEmail.Export();
}


void nglKernel::ProcessMessages(const nuiEvent& rEvent)
{
  {
    nuiTask* pTask = NULL;
    while ((pTask = mMainQueue.Get(0)))
    {
      pTask->Run();
      pTask->Release();
    }
  }

  {
    nuiNotification* pNotif;
    while ((pNotif = Get(0)))
    {
      nuiCommand* pCommand = NULL;
      nuiGetTokenValue<nuiCommand*>(pNotif->GetToken(), pCommand);
      if (pCommand)
        pCommand->Do();
      pNotif->Release();
    }

    mpNotificationManager->BroadcastQueuedNotifications();

  }

  nuiRefCount::PurgeAutoReleasePoolForCurrentThread();
}

nuiTaskQueue& nglKernel::GetMainQueue()
{
  return mMainQueue;
}

nuiTaskQueue& nuiGetMainQueue()
{
  return App->GetMainQueue();
}



void nglKernel::PostNotification(nuiNotification* pNotification)
{
  mpNotificationManager->PostNotification(pNotification);
}

void nglKernel::BroadcastNotification(const nuiNotification& rNotification)
{
  mpNotificationManager->BroadcastNotification(rNotification);
}

void nglKernel::RegisterObserver(const nglString& rNotificationName, nuiNotificationObserver* pObserver)
{
  mpNotificationManager->RegisterObserver(rNotificationName, pObserver);
}

void nglKernel::UnregisterObserver(nuiNotificationObserver* pObserver, const nglString& rNotificationName)
{
  mpNotificationManager->UnregisterObserver(pObserver, rNotificationName);
}

#if ((defined _UNIX_) || (defined _MINUI3_) || (defined _COCOA_) || (defined _CARBON_)) && !(defined _ANDROID_)
#include <execinfo.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <cxxabi.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

void nglDumpStackTrace()
{
  void * array[25];
  int nSize = backtrace(array, 25);
  char ** symbols = backtrace_symbols(array, nSize);

  for (int i = 0; i < nSize; i++)
  {
    int status;
    char *realname;
    std::string current = symbols[i];
    size_t start = current.find("(");
    size_t end = current.find("+");
    realname = NULL;
    if (start != std::string::npos && end != std::string::npos)
    {
      std::string symbol = current.substr(start+1, end-start-1);
      realname = abi::__cxa_demangle(symbol.c_str(), 0, 0, &status);
    }
    if (realname != NULL)
      syslog(LOG_ERR, "[%d] %s (%p)\n", i, realname, array[i]);
    else
      syslog(LOG_ERR, "[%d] %s (%p)\n", i, symbols[i], array[i]);
    free(realname);
  }

  free(symbols);
}

void nglKernel::CatchSignal (int Signal, void (*pHandler)(int))
{
  struct sigaction act;

  act.sa_handler = pHandler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = (Signal == SIGCHLD) ? SA_NOCLDSTOP : 0;
  sigaction (Signal, &act, NULL);
}
#endif

nglThread::ID nglKernel::GetMainThreadID() const
{
  return mMainThreadID;
}

void nglKernel::DidReceiveNotification(const std::map<nglString, nglString>& infos)
{
  NGL_OUT("Received notification:\n");
  for (const auto& item : infos)
  {
    NGL_OUT("\t%s -> %s\n", item.first.GetChars(), item.second.GetChars());
  }
}

void nglKernel::DidRegisterForRemoteNotifications(const std::vector<uint8>& deviceToken)
{
  NGL_OUT("DidRegisterForRemoteNotifications (token size: %d)\n", deviceToken.size());
}

void nglKernel::RegisterForRemoteNotifications(int32 types)
{
#ifdef _UIKIT_
  [[UIApplication sharedApplication] registerForRemoteNotificationTypes: types];
#endif

#ifdef _COCOA_
  [[NSApplication sharedApplication] registerForRemoteNotificationTypes: types];
#endif

}
