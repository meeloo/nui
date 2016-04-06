/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

const nglLog::StampFlags nglLog::NoStamp     = 0;
const nglLog::StampFlags nglLog::TimeStamp   = 1 << 0;
const nglLog::StampFlags nglLog::DateStamp   = 1 << 1;
const nglLog::StampFlags nglLog::DomainStamp = 1 << 2;

#ifndef NGL_DISABLE_LOG
#define NGL_DISABLE_LOG 0
#endif

/*
 * Life cycle
 */

nglLog::nglLog (bool UseConsole)
{
  mDefaultLevel = NGL_LOG_DEFAULT;
  mStampFlags = DomainStamp;
  mUseConsole = UseConsole;
  mDomainFormat = "%s: ";
  mDomainFormatLen = 0;
}

nglLog::~nglLog ()
{
  mLock.LockWrite();
  mOutputList.clear();
  mDomainList.clear();
  mLock.UnlockWrite();
}

/*
 * Formatting flags
 */

void nglLog::SetFlags (nglLog::StampFlags Flags)
{
  mStampFlags = Flags;
}

nglLog::StampFlags nglLog::GetFlags()
{
  return mStampFlags;
}


/*
 * Output management
 */

void nglLog::UseConsole(bool Use)
{
  mUseConsole = Use;
}

bool nglLog::AddOutput (nglOStream* pStream)
{
  #if NGL_DISABLE_LOG
  return false;
  #endif

  if (!pStream)
    return false;

  mLock.LockWrite();
  mOutputList.push_back(pStream);
  mLock.UnlockWrite();
  return true;
}

bool nglLog::DelOutput (nglOStream* pStream)
{
  #if NGL_DISABLE_LOG
  return false;
  #endif

  if (!pStream)
    return false;

  OutputList::iterator output;

  mLock.LockWrite();
  for (output = mOutputList.begin(); output != mOutputList.end(); ++output)
  {
    if (*output == pStream)
    {
      mOutputList.erase(output);
      mLock.UnlockWrite();
      return true;
    }
  }
  mLock.UnlockWrite();
  return false;
}


/*
 * Domain management
 */

int nglLog::GetLevel (const nglChar* pDomain)
{
  #if NGL_DISABLE_LOG
  return 0;
  #endif

  Domain* slot = LookupDomain(pDomain);

  return slot ? slot->Level : 0;
}

void nglLog::SetLevel (const nglChar* pDomain, int Level)
{
  #if NGL_DISABLE_LOG
  return;
  #endif

  // NGL_OUT("nglLog::SetLevel(%s, %d)", pDomain, Level);
  if (!pDomain)
    return;

  if (strcmp(pDomain, "all") == 0)
  {
    // NGL_OUT("nglLog::SetLevel setting ALL LEVELS");
    mLock.LockRead();
    DomainList::iterator dom = mDomainList.begin();
    DomainList::iterator end = mDomainList.end();

    for (; dom != end; ++dom)
    {
      // NGL_OUT("nglLog::SetLevel setting %s to %d", dom->Name.GetChars(), Level);
      ngl_atomic_set(dom->Level, Level);
    }
    mLock.UnlockRead();

    mDefaultLevel = Level;
    return;
  }

  Domain* slot = LookupDomain(pDomain);

  if (slot)
  {
    ngl_atomic_set(slot->Level, Level);
  }
}


/*
 * Output functions
 */

void nglLog::Log (const nglChar* pDomain, int Level, const nglChar* pText, ...)
{
  #if NGL_DISABLE_LOG
  return;
  #endif

  if (pText == NULL)
    return;

  va_list args;
  va_start (args, pText);

  Logv (pDomain, Level, pText, args);

  va_end (args);
}

void nglLog::Logv (const nglChar* pDomain, int Level, const nglChar* pText, va_list Args)
{
  #if NGL_DISABLE_LOG
  return;
  #endif

  if (pText == NULL)
    return;

  Domain* dom = LookupDomain(pDomain);
  if (!dom)
  {
    // NGL_OUT("LookupDomain %s failed", pDomain);
    return;
  }

  if (dom->Level == NGL_LOG_NEVER)
    return;

  if ((Level > dom->Level) && (dom->Level != NGL_LOG_ALWAYS))
    return;

  // Update log item counter
  ngl_atomic_inc(dom->Count);

  // Get time stamp (if necessary)
  nglTimeInfo stamp;

  if (mStampFlags & (TimeStamp | DateStamp))
  {
    nglTime now;
    now.GetLocalTime (stamp);
  }

  {
    nglCriticalSectionGuard guard(mCS);
    // Compose mPrefix
    mPrefix.Wipe();
    if (mStampFlags & DateStamp)
    {
      mBody.Format("%.2d/%.2d/%.2d ", stamp.Year - 100, stamp.Month, stamp.Day);
      mPrefix += mBody;
    }
    if (mStampFlags & TimeStamp)
    {
      mBody.Format("%.2d:%.2d:%.2d ", stamp.Hours, stamp.Minutes, stamp.Seconds);
      mPrefix += mBody;
    }
    if (mStampFlags & DomainStamp)
    {
      const nglChar* dom_name = dom->Name.GetChars();

      if (dom->Count == 1)
      {
        // On first display from this domain, adjust domain display width
        uint32 dom_len = strlen(dom_name);

        if (dom_len > mDomainFormatLen)
        {
          mDomainFormatLen = dom_len;
          mDomainFormat.Format("%%-%ds: ", mDomainFormatLen);
        }
      }

      mBody.Format(mDomainFormat.GetChars(), dom_name);
      mPrefix += mBody;
    }

    mOutputBuffer.Formatv(pText, Args);
    mOutputBuffer.TrimRight(_T('\n'));

    if (mOutputBuffer.Find(_T('\n')) == -1)
    {
      // Single line, display immediatly
      mBody = mPrefix;
      mBody += mOutputBuffer;
      mBody += _T('\n');
      Output (mBody);
    }
    else
    {
      // Multiple lines, display individually
      std::vector<nglString> lines;
      std::vector<nglString>::iterator line;

      mOutputBuffer.Tokenize(lines, _T('\n'));
      for (line = lines.begin(); line != lines.end(); ++line)
      {
        mBody = mPrefix;
        mBody += *line;
        mBody.TrimRight(_T('\n'));
        mBody += _T('\n');
        Output (mBody);
      }
    }
  }
}

void nglLog::Dump (int Level) const
{
  #if NGL_DISABLE_LOG
  return;
  #endif

  mLock.LockRead();
  DomainList::const_iterator dom = mDomainList.begin();
  DomainList::const_iterator end = mDomainList.end();

  nglString text = "# Log domains usage statistics :\n";
  nglString format;
  Output(text);

  for (; dom != end; dom++)
  {
    format.Format("#   %s %%d\n", mDomainFormat.GetChars());
    text.Format(format.GetChars(), (*dom).Name.GetChars(), (*dom).Count);
    Output(text);
  }
  mLock.UnlockRead();
}


/*
 * Internal classes
 */

nglLog::Domain* nglLog::LookupDomain (const nglChar* pName)
{
  #if NGL_DISABLE_LOG
  return NULL;
  #endif

  // Sanity check
  if (!pName)
    return NULL;

  mLock.LockRead();
  // Search in domain list
  DomainList::iterator dom = mDomainList.begin();
  DomainList::iterator end = mDomainList.end();

  for (; dom != end; dom++)
  {
    if (dom->Name == pName)
    {
      Domain* pDom = &(*dom);
      mLock.UnlockRead();
      return pDom;
    }
  }
  mLock.UnlockRead();

  // Not found ? Create it.
  mLock.LockWrite();
  mDomainList.push_back(Domain(pName, mDefaultLevel));
  Domain* pDom = &mDomainList.back();
  mLock.UnlockWrite();
  return pDom;
}

void nglLog::Output (const nglString& rText) const
{
  #if NGL_DISABLE_LOG
  return;
  #endif

  if (mUseConsole)
  {
    NGL_OUT(rText);
  }

  OutputList::const_iterator out;
  for (out = mOutputList.begin(); out != mOutputList.end(); out++)
    (*out)->WriteText (rText);
}
