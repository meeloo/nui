/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

using namespace std;


/* Instance is in <platform>/nglApplication.cpp
 */
extern const nglChar* gpApplicationErrorTable[];


/*
 * Command line
 */

bool nglApplication::ParseDefaultArgs()
{
  bool result = true;
  int i = 0;

  while (i < GetArgCount())
  {
    nglString arg = GetArg(i);

    if (arg == "--log-level")
    {
      // Check --log-level parameter consistency
      if (((i+1) >= GetArgCount()) ||      // not enough args
          (!GetArg(i+1).Compare("-", 0, 1)))  // this can't be a parameter
      {
        result = false;
        i++;
      }
      else
      {
        // Aliases are coma-separated
        std::vector<nglString> aliases;
        GetArg(i+1).Tokenize (aliases, _T(','));

        std::vector<nglString>::iterator alias;
        for (alias = aliases.begin(); alias != aliases.end(); ++alias)
        {
          int verbose = NGL_LOG_DEFAULT;
          int colon = (*alias).Find (_T(':'));
          if (colon >= 0)
          {
            // Separate alias name and option in 'alias:opt'
            nglString opt = (*alias).Extract (colon + 1);
            (*alias).Delete (colon);

            if (opt == "error")   verbose = NGL_LOG_ERROR; else
            if (opt == "warning") verbose = NGL_LOG_WARNING; else
            if (opt == "info")    verbose = NGL_LOG_INFO; else
            if (opt == "debug")   verbose = NGL_LOG_DEBUG; else
              verbose = opt.GetInt();
          }

          GetLog().SetLevel ((*alias).GetChars(), verbose);
        }

        // Remove parsed arguments (ie. they are not exposed to the user via GetArg())
        DelArg(i, 2);
      }
    }
    else
    if (arg == "--log-output")
    {
      // Check --log-output parameter consistency
      if (((i+1) >= GetArgCount()) ||      // not enough args
          (!GetArg(i+1).Compare("-", 0, 1)))  // this can't be a parameter
      {
        result = false;
        i++;
      }
      else
      {
        // Aliases are coma-separated
        std::vector<nglString> aliases;
        GetArg(i+1).Tokenize (aliases, _T(','));

        std::vector<nglString>::iterator alias;
        for (alias = aliases.begin(); alias != aliases.end(); ++alias)
        {
          int colon = (*alias).Find (_T(':'));

          if (colon >= 0)
          {
            // Separate alias name and option in 'alias:opt'
            nglString opt = (*alias).Extract (colon + 1);
            (*alias).Delete (colon);

            if ((*alias) == "con")
            {
              if ((opt == "on")  || (opt == "yes") || (opt == "1"))
                GetLog().UseConsole (true);
              else
              if ((opt == "off") || (opt == "no")  || (opt == "0"))
                GetLog().UseConsole (false);
            }
            else
            if (((*alias) == "file") || ((*alias) == "file+"))
            {
              nglOFile* ostream = new nglOFile (nglPath (opt), (*alias)[4] == _T('+') ? eOFileAppend : eOFileCreate);
              if (ostream)
              {
                if (ostream->GetState() != eStreamReady || !GetLog().AddOutput (ostream))
                  delete ostream;
                else
                  ostream->AutoFlush (true);
              }
            }
          }
        }

        // Remove parsed arguments (ie. they are not exposed to the user via GetArg())
        DelArg(i, 2);
      }
    }
    else
    if (arg == "--no-debug")
    {
      SetDebug(false);
      DelArg(i);
    }
    else
    if (arg == "--help" || arg == "-h")
    {
      PrintUsage();
      // Keep this (very) common option for user's sake
      // DelArg(i);
      i++;
    }
    else
      i++;
  }

  return result;
}

void nglApplication::PrintUsage()
{
  NGL_OUT("NGL default options :\n" \
    "\n" \
    "  --log-level all[:<level>],domain[:<level>],...   Set per-domain log levels\n" \
    "  --log-output con:<yes|no>,file:<path>,...        Control/add log outputs\n" \
    "  --no-debug                                       Don't run NGL_DEBUG() statements\n" \
    "  --help, -h                                       This help\n\n");
}


/*
 * Idle event management
 */

#ifdef __APPLE__

#ifdef _UIKIT_
///< #TODO?

#elif (defined _COCOA_)
///< #TODO?

#elif defined(__MACHO__)

pascal void TimerAction (EventLoopTimerRef  theTimer, EventLoopIdleTimerMessage inState, void* userData)
{
  if (inState == kEventLoopIdleTimerIdling)
    ((nglApplication*)App)->CallOnIdle();
}
#elif defined __CFM__ // CFM
pascal void TimerAction (EventLoopTimerRef  theTimer, void* userData)
{
  ((nglApplication*)App)->CallOnIdle ();
}
#elif defined __CFM_CLASSIC__ // CFM Classic
#endif
#endif

void nglApplication::SetIdle (bool UseIdle)
{
#ifdef __APPLE__

# ifdef _UIKIT_

///< #TODO?

#elif (defined _COCOA_) //COCOA

# else //CARBON?

  EventLoopRef       mainLoop;
  EventLoopTimerRef  theTimer;
  
  mainLoop = GetMainEventLoop();
  if (UseIdle && !mUseIdle)
  {
    #ifdef __CFM__
      EventLoopTimerUPP  timerUPP;
      timerUPP = NewEventLoopTimerUPP(&TimerAction);
      
      InstallEventLoopTimer (mainLoop,
                             0,
                             0.000001,
                             timerUPP,
                             (void*)this,
                             &mIdleTimer);
      
    #else // MACHO
      InstallEventLoopIdleTimer (mainLoop,
                                 0,
                                 kEventDurationMillisecond/4,
                                 TimerAction,
                                 NULL,
                                 &mIdleTimer);
    #endif
  }
  else if (mIdleTimer)
  {
    RemoveEventLoopTimer(mIdleTimer);
    mIdleTimer = NULL;
  }
#endif//!_UIKIT_
#endif//_APPLE_

  
  mUseIdle = UseIdle;
#if (defined _UNIX_) && (!defined _ANDROID_)
  if (UseIdle)
    mFlags |= nglEvent::Idle;
  else
    mFlags &= ~nglEvent::Idle;
#endif
}

bool nglApplication::GetIdle()
{
  return mUseIdle;
}

void nglApplication::OnIdle(nglTime Elapsed)
{
  /* We don't log anything, even in debug mode.
   * This would be obviously way too verbose in most cases.
   */
}

void nglApplication::CallOnIdle()
{
  nglTime now;
  nglTime elapsed(0);

  if (mLastIdleCall != 0.0f)
    elapsed = now - mLastIdleCall;
  OnIdle (elapsed);
  mLastIdleCall = now;
}

/*
 * From nglError
 */

const nglChar* nglApplication::OnError (uint& rError) const
{
  return FetchError(gpApplicationErrorTable, nglKernel::OnError(rError), rError);
}


bool nglApplication::OnQuitRequested()
{
  return true;
}

void nglApplication::OpenDocuments(std::list<nglPath>& filePaths)
{
}

bool nglApplication::IsQuitRequested() const
{
  return mExitPosted;
}

void nglApplication::OnDeactivation()
{
  Quit(0);
}



