/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

/*!
\file  nui_all.h
\brief NUI / VC Precompiled headers files.

This file includes all nui needed headers to ease the creation of precompiled headers outside of the ngl project in VC.
*/

#ifndef __nui_h__
#define __nui_h__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wno-global-constructors"

#ifndef NGL_STATIC_BUILD
#define NGL_STATIC_BUILD // Fake the static build! Always!
#endif

#ifdef _WIN32_
#ifdef NGL_STATIC_BUILD
#define NUI_API
#else
#ifdef NUI_EXPORTS
#define __declspec(dllexport)
#else
#define __declspec(dllimport)
#endif
#endif
#else
#define NUI_API
#endif

#include "ngl.h"

#ifdef _WIN32_
//#include <crtdbg.h>
#endif // _WIN32_

#define NUI_USE_RENDER_THREAD 1
#define NUI_USE_LAYERS 0

//#define _TU_USE_STL 1

#define NUI_FONTDB_PATH "nuiFonts.db5"
#include "nui_all.h"

#pragma clang diagnostic pop

#endif // __nui_h__
