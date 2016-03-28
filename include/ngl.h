/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron

  licence: see nui3/LICENCE.TXT
*/

/*!
\file  ngl.h
\brief Common declarations

This file must be included \e first in every source file, in NGL own sources
and in NGL user application code.
*/

#pragma once

#include "nuiVersion.h"


#define USE_WCHAR
#define __NUI_NO_SOFTWARE__

#if (defined _DEBUG) || (defined DEBUG)
  #if !(defined _DEBUG_)
    #define _DEBUG_
  #endif
#endif

/*
 * Platform detection
 */

#if (defined __X86_64__) || (defined _M_X64) || (defined __ppc64__) || (defined __LP64__)
// 64bits platform
#define __NUI64__
#else
// 32bits platform
#define __NUI32__
#endif

/* MS Windows flavours
*/
#if defined (_WIN32) || defined (WIN32)
#  ifndef _WIN32_
#    define _WIN32_
#    define _WIN32_WINNT 0x0502
#  endif
#endif // _WIN32_

/* Unix flavours
*/
#ifdef __LINUX__
#define _LINUX_
#endif

#ifdef __FreeBSD__
#define _FREEBSD_
#endif

#ifdef __CYGWIN__
#define _CYGWIN_
#endif

#ifdef _MINUI3_
#define _UNIX_
#endif

/* Mac world
 */
#if (defined __APPLE__) && (!defined _MINUI3_)
  #define _MACOSX_

// Include conditionals
	#include <TargetConditionals.h>
  #define _CORE_FOUNDATION_
  #include <CoreFoundation/CoreFoundation.h>

	// Using UIKit for iPhone and iPhone simulator
	#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
    #define NUI_IOS
    #define _UIKIT_

  #elif (defined _CARBON_)
    #ifdef __GNUC__
      #define __MACHO__
      #define __NGL_MACHO__
    #endif // __GNUC__

  //#elif (defined _COCOA_)
  #else // Make Cocoa the default choice when compiling on a Mac, even when using gcc
    #ifndef _COCOA_
        #define _COCOA_
    #endif
    #define __MACHO__
    #define __NGL_MACHO__
    #if !__LP64__
      #include <CoreServices/CoreServices.h>
    #endif
  #endif
#endif

  #define __NGL_MACHO__


/*
 * Platform specific includes
 */

/* Win32
 */
#ifdef _WIN32_

  /* Automatically import DLL symbols when :
   * - the user did not explicitely required a static build
   * - AND we are not currently building NGL itself as a DLL
   */

#pragma warning(disable : 4355) // Disable 'this' : used in base member initializer list warning
  #if (_MSC_VER >= 1400) && (_MSC_VER < 1500)  // Is this VC8.0?
  # define _CRT_SECURE_NO_DEPRECATE
  # pragma warning( disable : 4996 )
  # define STDEXT stdext // Need special namespace for hash_maps...
  #elif (_MSC_VER >= 1300) && (_MSC_VER < 1400)  // Is this VC7.1?
  # define STDEXT stdext // Need special namespace for hash_maps...

  #elif (_MSC_VER >= 1400) && (_MSC_VER < 1500)  // Is this VC8?
    // Some missing math.h functions in VC7 2002
    inline float abs(float& a)
    {
      if (a < 0)
        return -a;
      return a;
    }

    inline float abs(float a)
    {
      if (a < 0)
        return -a;
      return a;
    }

    # ifdef NGL_STATIC_BUILD
    # endif // NGL_STATIC_BUILD
  #endif // VC7.1


  // Add the needed external libs
  #pragma comment( lib , "opengl32.lib" )
  #pragma comment( lib , "glu32.lib" )
  #pragma comment( lib , "winmm.lib" )
  #pragma comment( lib , "Ws2_32.lib" )
  #pragma comment( lib,  "winhttp.lib" )
  #pragma comment( lib, "wsock32.lib" )

  #define _WINSOCKAPI_  // Prevent inclusion of winsock.h in windows.h
  #define NODRAWTEXT // Prevent inclusion of DrawText defines in windows.h.

  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT _WIN32_WINNT_WS03
  #endif

  #define NTDDI_VERSION NTDDI_WS03 //< Minimum version of the system = WinXP.
  #include <windows.h>
  #include <malloc.h>
  #include <cstdio>

  #pragma warning(disable : 4786) // Disable the "debug name too long" warning which is really annoying with stlport
  #pragma warning(disable : 4251) // Disable the template export warning
  #pragma warning(disable : 4530) // Disable the exception disable but used in MSCV Stl warning.

//#include <winsock2.h>
  #ifdef _NO_NOT_USED_PARAM_WARNING_
  #pragma warning (disable: 4100) // Prevent the "unreferenced formal parameter" warning
  #pragma warning (disable: 4201) // Prevent the "parameter not used" warning
  #pragma warning (disable: 4711) // Prevent the "selected for automatic inline expansion" warning
  #endif

  #ifdef _MAYAPLUGIN_ // Maya includes try to redefine the bool type
    #define _BOOL
  #endif

  // Fixed size types
  typedef signed __int8    int8;
  typedef unsigned __int8  uint8;
  typedef __int16          int16;
  typedef unsigned __int16 uint16;
  typedef __int32          int32;
  typedef unsigned __int32 uint32;
  typedef __int64          int64;
  typedef unsigned __int64 uint64;
  typedef float            real32;
  typedef double           real64;

  extern uint32 WM_NGLTIMER;

  // Windows already use these macros for Unicode support
  // (we keep their expected behaviour)
  #undef _T
  #undef TEXT
#endif // _WIN32_


/* Unices
 */
#if defined(_LINUX_) || defined(_FREEBSD_) || defined(_CYGWIN_) || defined(_DARWIN_) || defined (_ANDROID_)
#define _UNIX_
#endif

#ifdef _UNIX_
  #ifndef _GNU_SOURCE
  #define _GNU_SOURCE
  #endif
  #include <stdlib.h>
  #include <sys/types.h>
  #include <ctype.h>

  typedef int8_t    int8;
  typedef int16_t   int16;
  typedef int32_t   int32;
  typedef int64_t   int64;
  typedef u_int8_t  uint8;
  typedef u_int16_t uint16;
  typedef u_int32_t uint32;
  typedef u_int64_t uint64;

  #if (!defined _NOGFX_) && (!defined _MINUI3_) && (!defined _ANDROID_)
  #define _X11_
  #endif
#endif // _UNIX_

/*
 MacOS/UIKit
 */
#ifdef _UIKIT_
  #ifndef __cplusplus
//  #if defined(_OBJC_)
		#include <UIKit/UIKit.h>
  #endif

  #include <stdlib.h>
  #include <stddef.h>
  #include <limits.h>

  typedef int8_t    int8;
  typedef int16_t   int16;
  typedef int32_t   int32;
  typedef int64_t   int64;
  typedef u_int8_t  uint8;
  typedef u_int16_t uint16;
  typedef u_int32_t uint32;
  typedef u_int64_t uint64;

#endif//_UIKIT_

/*
 MacOS/Cocoa
 */
#ifdef _COCOA_
  #ifndef __cplusplus
  //  #if defined(_OBJC_)
  #include <Cocoa/Cocoa.h>
  #include <CoreGraphics/CoreGraphics.h>
  #endif

  #include <ApplicationServices/ApplicationServices.h>

  #include <stdlib.h>
  #include <stddef.h>
  #include <limits.h>

  typedef int8_t    int8;
  typedef int16_t   int16;
  typedef int32_t   int32;
  typedef int64_t   int64;
  typedef u_int8_t  uint8;
  typedef u_int16_t uint16;
  typedef u_int32_t uint32;
  typedef u_int64_t uint64;


#endif //_COCOA_


/*
 MacOS/Carbon
 */
#ifdef _CARBON_
  #define __PRINTCORE__
  #define __PMAPPLICATION__
  #define __PMAPPLICATION_DEPRECATED__
  #include <Carbon/Carbon.h>
  #include <stdlib.h>
  #include <stddef.h>

  typedef int8_t    int8;
  typedef int16_t   int16;
  typedef int32_t   int32;
  typedef int64_t   int64;
  typedef u_int8_t  uint8;
  typedef u_int16_t uint16;
  typedef u_int32_t uint32;
  #if macintosh == 1
    typedef UInt64 uint64;
  #else
    typedef u_int64_t uint64;
  #endif

#endif // _CARBON_


/*
 * Common stuff for all platforms
 */

//! Shortcut and uniformisation with fixed-size NGL integers
typedef unsigned int uint;

//! File offset and size type
/*!
This type is used for file adressing (size and offsets). It is currently
set to a 32 bit signed integer (can address 2GB files on filesystems which
allow it).
*/
#ifdef _WIN32_
  typedef SSIZE_T nglSize;
  typedef LONGLONG nglFileOffset;
  typedef LONGLONG nglFileSize;
#else
  #include <unistd.h>
  #include <stddef.h>
  typedef ssize_t nglSize;
  typedef off_t   nglFileOffset;
  typedef off_t nglFileSize;
#endif // _WIN32_

#ifdef _T
  // _T is defined under MacOSX
  #undef _T
#endif

typedef char nglChar;
typedef wchar_t nglUChar;
/*!< Portable char type
  See nglString for more info
*/
//#define _T(x) L##x
#define _T(x) x
/*!< Portable string declaration macro
  Use for literals to support both <i>char</i> and <i>wide char</i> modes :
  \code
  const nglChar* my_string_value = "Example";
  nglString my_string = my_string_value;
  nglString my_string("Example");
  \endcode
*/

#define TEXT(x) _T(x)
/*!< Portable string declaration macro
  An alternative to the _T macro
*/


/* Handy macros
 */
#if (!defined _BEOS_)
#ifndef MIN
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#  define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#endif

#ifdef UNUSED
#  undef UNUSED
#endif
#define UNUSED(x)    ((x) = (x))

/* Multitouch support is now always on
 */
#define _MULTI_TOUCHES_

/* At last, include GL and GLU headers if necessary
 */
#ifndef _NOGFX_

#   define __NUI_NO_GLES__

# ifdef _ANDROID_

#    define _OPENGL_ES_
// Make our GLES Painter available, and disable other Painters...
#    define  __NUI_NO_GLES__
#    define __NUI_NO_SOFTWARE__
#    define __NUI_NO_D3D__
#    undef __NUI_NO_GL__

    // Disable Anti-Aliasing
    //#    define __NUI_NO_AA__

#   include <EGL/egl.h>
#   include <GLES/gl.h>
#   include <GLES/glext.h>

#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#   include <GLES2/gl2platform.h>

    // Fake GLU for OpenGLES
    typedef double GLdouble;
    typedef double GLclampd;
/* TessWinding */
#   define GLU_TESS_WINDING_ODD               100130
#   define GLU_TESS_WINDING_NONZERO           100131
#   define GLU_TESS_WINDING_POSITIVE          100132
#   define GLU_TESS_WINDING_NEGATIVE          100133
#   define GLU_TESS_WINDING_ABS_GEQ_TWO       100134

#  elif defined _UIKIT_
#    define _OPENGL_ES_
// Make our GLES Painter available, and disable other Painters...
#    define  __NUI_NO_GLES__
#    define __NUI_NO_SOFTWARE__
#    define __NUI_NO_D3D__
#    undef __NUI_NO_GL__

// Disable Anti-Aliasing
//#    define __NUI_NO_AA__

#    if defined(_OBJC_)
#     include <OpenGLES/EAGL.h>
#     include <OpenGLES/EAGLDisplay.h>
#    endif
#    include <OpenGLES/ES1/gl.h>
#    include <OpenGLES/ES1/glext.h>
#    include <OpenGLES/ES2/gl.h>
#    include <OpenGLES/ES2/glext.h>

// Fake GLU for OpenGLES
    typedef double GLdouble;
    typedef double GLclampd;
    /* TessWinding */
    #define GLU_TESS_WINDING_ODD               100130
    #define GLU_TESS_WINDING_NONZERO           100131
    #define GLU_TESS_WINDING_POSITIVE          100132
    #define GLU_TESS_WINDING_NEGATIVE          100133
    #define GLU_TESS_WINDING_ABS_GEQ_TWO       100134

#  elif (defined _COCOA_)
#    define _OPENGL_
// Make our GL and Software Painters available, and disable other Painters...
#    undef __NUI_NO_GLES__
#    define __NUI_NO_D3D__
#    undef __NUI_NO_GL__


#    if defined(_OBJC_)
#     include <OpenGL/OpenGL.h>
#    endif
#    include <OpenGL/gl.h>
#    include "nui_GL/glext.h"
//#    include <OpenGL/glext.h>
#    include <OpenGL/glu.h>


#  elif defined _CARBON_
#    undef __NUI_NO_GLES__
#    define __NUI_NO_D3D__
#    undef __NUI_NO_GL__
#    if defined(__MWERKS__)
#      if macintosh == 0
#        include <AGL/agl.h>
#        include <OpenGL/glu.h>
#      else
#        include <agl.h>
#        include <glu.h>
#      endif
#    else
#      include <AGL/agl.h>
#      include <OpenGL/glu.h>
#      include <OpenGL/OpenGL.h>
#      include "nui_GL/glext.h"
#    endif

#  elif defined _ANDROID_
#   include <GLES/gl.h>
#   include <GL/glu.h>
#   include <GLES/glext.h>
#   include <GLES/glew.h>

// Dummy BGR flags. Beware! GLES on Android doesn't seem to handle BGR!!!
#   define GL_BGR  0x80E0
#   define GL_BGRA 0x80E1


#  elif !defined _MINUI3_

#define GL_GLEXT_PROTOTYPES
#    include <GL/gl.h>
//#    include <GL/glu.h>
#    include "nui_GL/glext.h"
#  endif
#  ifdef _WIN32_
     /* The latest glext.h file can be fetched from :
      *   http://oss.sgi.com/projects/ogl-sample/registry/index.html
      */
#    include "nui_GL/glext.h"
//#    include <GL/glext.h>
#    include "nui_GL/wglext.h"
#    include <d3d9.h>
#  endif

/* Glue for lagging OpenGL implementations (taken from glext.h revision 24)
 */
#ifndef GL_VERSION_2_0
/* GL type for program/shader text */
typedef char GLchar;
#endif
#if (!defined GL_VERSION_1_5) && (!defined GL_OES_VERSION_1_1) && (!defined _ANDROID_)
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
#endif

#ifndef GL_ARB_vertex_buffer_object
/* GL types for handling large vertex buffer objects */
  typedef ptrdiff_t GLintptrARB;
  typedef ptrdiff_t GLsizeiptrARB;
#endif

#ifndef GL_ARB_shader_objects
/* GL types for handling shader object handles and characters */
typedef char GLcharARB;    /* native character */
typedef unsigned int GLhandleARB;  /* shader object handle */
#endif

#ifndef GL_NV_half_float
  typedef unsigned short GLhalfNV;
#endif

// This linkage macro should be defined by <GL/gl.h> if needed
#ifndef APIENTRY
  #define APIENTRY
#endif

#endif // _NOGFX_

// Do we need to define STDEXT to the standard std namespace? Only VC7.1 currently needs this.
#ifndef STDEXT
  #define STDEXT std
#endif

// Architecture Detection:
// x86 / ia32
#if (defined i386) || (defined __i386__) || (defined __i386) || (defined _M_IX86) || (defined _X86_) || (defined __THW_INTEL)
  #define _NGL_X86_
#endif

// ia64
#if (defined __ia64__) || (defined _IA64) || (defined __IA64__) || (defined _M_IA64)
  #define _NGL_IA64_
#endif

  // ia64
#if (defined __amd64__) || (defined __amd64) || (defined __x86_64__) || (defined __x86_64) || (defined _M_X64)
  #define _NGL_X64_
#endif

// PowerPC
#if (defined __powerpc) || (defined __powerpc__) || (defined __POWERPC__) || (defined __ppc__) || (defined _M_PPC) || (defined __PPC) || (defined __PPC__)
  #define _NGL_PPC_
#endif

#ifdef _WIN32_
#define NGL_CONFIG_H <ngl_config.win32.h>
#elif (defined _CARBON_)
#define NGL_CONFIG_H <ngl_config.mac.h>
#elif (defined _UIKIT_)
#define NGL_CONFIG_H <ngl_config.uikit.h>
#elif (defined _COCOA_)
#define NGL_CONFIG_H <ngl_config.cocoa.h>
#else
#define NGL_CONFIG_H <ngl_config.tux.h>
#endif

#ifdef _ANDROID_
#include "android/log.h"
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/window.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <android/configuration.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/obb.h>
#include <android/rect.h>
#include <android/storage_manager.h>
//#include <android/tts.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "nui", __VA_ARGS__))
#else
#define LOGI
#endif


#include "ngl_all.h"


