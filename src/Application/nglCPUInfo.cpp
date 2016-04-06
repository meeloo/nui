/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


/* CPU family can be set at build time
 */
#if (defined i386) || (defined _M_IX86) || (defined _M_IX86) || (defined __THW_INTEL)
nglCPUInfo::Family nglCPUInfo::mFamily = eIA32;
#elif (defined __ia64__) || (defined _IA64) || (defined __IA64__) || (defined _M_IA64)
nglCPUInfo::Family nglCPUInfo::mFamily = eIA64;
#elif (defined __powerpc) || (defined __powerpc__) || (defined __POWERPC__) || (defined __ppc__) || (defined _M_PPC) || (defined __PPC) || (defined __PPC__)
nglCPUInfo::Family nglCPUInfo::mFamily = ePPC;
#elif (defined __mips__) || (defined __mips) || (defined __MIPS__)
nglCPUInfo::Family nglCPUInfo::mFamily = eMIPS;
#elif (defined __alpha__) || (defined __alpha) || (defined M_ALPHA)
nglCPUInfo::Family nglCPUInfo::mFamily = eAlpha;
#else
nglCPUInfo::Family nglCPUInfo::mFamily = eUnknown;
#endif

uint nglCPUInfo::mCount   = 0; // 0 CPU means we do not have any info on the current architecture
bool nglCPUInfo::mMMX     = false;
bool nglCPUInfo::mSSE     = false;
bool nglCPUInfo::mSSE2    = false;
bool nglCPUInfo::m3DNow   = false;
bool nglCPUInfo::mAltivec = false;


nglCPUInfo::Family nglCPUInfo::GetFamily()
{
  FillCPUInfo();
  return mFamily;
}

uint nglCPUInfo::GetCount()
{
  FillCPUInfo();
  return mCount;
}

bool nglCPUInfo::HasMMX()
{
  FillCPUInfo();
  return mMMX;
}

bool nglCPUInfo::HasSSE()
{
  FillCPUInfo();
  return mSSE;
}

bool nglCPUInfo::HasSSE2()
{
  FillCPUInfo();
  return mSSE2;
}

bool nglCPUInfo::Has3DNow()
{
  FillCPUInfo();
  return m3DNow;
}

bool nglCPUInfo::HasAltivec()
{
  FillCPUInfo();
  return mAltivec;
}

nglString nglCPUInfo::Dump()
{
  nglString text, buffer;
  uint count = GetCount();

  if (count == 0)
  {
    text = "no host CPU information available";
    return text;
  }

  switch (GetFamily())
  {
    case eUnknown: text += "unknown"; break;
    case eIA32   : text += "IA32"; break;
    case eIA64   : text += "IA64"; break;
    case ePPC    : text += "PPC"; break;
    case eAlpha  : text += "Alpha"; break;
    case eMIPS   : text += "MIPS"; break;
  }

  if (count > 1)
  {
    buffer.Format(" x %d", count);
    text += " x %d";
  }

  buffer.Format("%s%s%s%s%s",
    HasMMX()     ? " MMX" : _T(""),
    HasSSE()     ? " SSE" : _T(""),
    HasSSE2()    ? " SSE2" : _T(""),
    Has3DNow()   ? " 3DNow" : _T(""),
    HasAltivec() ? " Altivec" : _T(""));
  if (buffer.GetLength())
  {
    text += " with";
    text += buffer;
  }

  return text;
}

#ifndef _WIN32_
void nglCPUInfo::FillCPUInfo()
{
}
#endif // _WIN32_
