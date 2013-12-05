/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"


nuiStopWatch::nuiStopWatch(const nglString& LogTitle)
: mLogTitle(LogTitle), mOutputToLog(false)
{
  
}

nuiStopWatch::~nuiStopWatch()
{
  double seconds = 0.0f;
  {
    nglTime DestructionTime;
    double seconds = (double)DestructionTime - (double)mCreationTime;

    if (mOutputToLog)
    {
      NGL_LOG("StopWatch", NGL_LOG_DEBUG, "%s\t\t%lf sec (%f Hz)\n", mLogTitle.GetChars(), seconds, 1.0f / seconds);
    }
    else
    {
      App->TimedPrint("%s\t\t%lf sec (%f Hz)\n", mLogTitle.GetChars(), seconds, 1.0f / seconds);
    }

  }
  
  nglTime ref = mCreationTime;
  
  while (mIntermediatePoints.size())
  {
    std::pair<nglString, nglTime> point = mIntermediatePoints.front();
    nglString str = point.first;
    nglTime time = point.second;
    nglTime diff = time - ref;
    double sec = diff.GetValue();
    
    nglString log("\t");
    log.Add(str).Add("\t").Add(sec).Add(" s\t(").Add(100.0 * sec / seconds).Add("%%)");
    if (mOutputToLog)
    {
      NGL_LOG("StopWatch", NGL_LOG_DEBUG, "%s", log.GetChars());
    }
    else
    {
      App->TimedPrint("%s", log.GetChars());
    }
    
    ref = time;
    mIntermediatePoints.pop_front();
  }
}

void nuiStopWatch::AddIntermediate(const nglString& title, bool immediate)
{
  nglTime now;
  if (!immediate)
  {
    mLastTime = nglTime();
    std::pair<nglString, nglTime> point = std::make_pair(title, now);
    mIntermediatePoints.push_back(point);
    return;
  }

  double sec = (double)mLastTime - (double)now;
  nglString log("\t");
  log.Add(title).Add("\t").Add(sec).Add(" s");
  if (mOutputToLog)
  {
    NGL_LOG("StopWatch", NGL_LOG_DEBUG, "%s", log.GetChars());
  }
  else
  {
    App->TimedPrint("%s\n", log.GetChars());
  }

}

double nuiStopWatch::GetElapsedTime() const
{
  nglTime now;
  nglTime diff = now - mCreationTime;
  double elapsed = diff.GetValue();
  return elapsed;
}
