/*
  NGL - C++ cross-platform framework for OpenGL based applications
  Copyright (C) 2000-2003 NGL Team

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "nui.h"
#include "nglKernel.h"
#include "nglContext.h"


/*
#define NGL_CONTEXT_ENONE      0
*/
#define NGL_CONTEXT_ENOX       1
#define NGL_CONTEXT_ENOGLX     2
#define NGL_CONTEXT_ENOMATCH   3
#define NGL_CONTEXT_EGLCTX     4
#define NGL_CONTEXT_EBIND      5

const nglChar* gpContextErrorTable[] =
{
/*  0 */ _T("No error"),
/*  1 */ _T("No X connection"),
/*  2 */ _T("GLX extension not found"),
/*  3 */ _T("No visual match your request"),
/*  4 */ _T("GL rendering context creation failed"),
/*  5 */ _T("Couldn't bind GL context to the system window"),
         NULL
};


/*
 * nglContextInfo
 */


bool nglContextInfo::Enum (uint Index, nglContextInfo& rInfo)
{
  return false;
}


#define ATTRIB_MAX 64
#define ATTRIB_PUSH1(a)   { if (pos >= ATTRIB_MAX) return NULL; attrib[pos++] = a; }
#define ATTRIB_PUSH2(a,b) { if (pos+1 >= ATTRIB_MAX) return NULL; attrib[pos++] = a; attrib[pos++] = b; }


/*
 * nglContext
 */

nglContext::nglContext()
{
  LOGI("nglContext::nglContext()");
  mpPainter = NULL;
  mScale = 1.0f;
  mScaleInv = 1.0f;

  mSurface = NULL;
  mContext = NULL;
  mDisplay = NULL;

  LOGI("nglContext::nglContext() OK");
}

nglContext::~nglContext()
{
}

bool nglContext::GetContextInfo(nglContextInfo& rInfo) const
{
  return false;
}

nglContext::GLExtFunc nglContext::LookupExtFunc (const char* pFuncName)
{
  return NULL;
}


const nglChar* nglContext::OnError (uint& rError) const
{
  return NULL;
}

/**
  * Initialize an EGL context for the current display.
  */
bool nglContext::Build(ANativeWindow* window) 
{
  LOGI("Init opengl context");
  // initialize OpenGL ES and EGL
  
  /*
    * Here specify the attributes of the desired configuration.
    * Below, we select an EGLConfig with at least 8 bits per color
    * component compatible with on-screen windows
    */
  const EGLint attribs[] = 
  {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    //EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
  };
  EGLint w, h, dummy, format;
  EGLint numConfigs;
  EGLConfig configs[10];
  
  mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  
  eglInitialize(mDisplay, 0, 0);
  
  /* Here, the application chooses the configuration it desires. In this
    * sample, we have a very simplified selection process, where we pick
    * the first EGLConfig that matches our criteria */
  eglChooseConfig(mDisplay, attribs, configs, 10, &numConfigs);
  LOGI("Init opengl num configs: %d", numConfigs);
  
  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  eglGetConfigAttrib(mDisplay, configs[0], EGL_NATIVE_VISUAL_ID, &format);
  LOGI("Init opengl config format: %d", format);
  
  ANativeWindow_setBuffersGeometry(window, 0, 0, format);
  
  mSurface = eglCreateWindowSurface(mDisplay, configs[0], window, NULL);
  LOGI("Init opengl surface: %p", mSurface);

  EGLint attribs2[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  mContext = eglCreateContext(mDisplay, configs[0], NULL, attribs2);
  LOGI("Init opengl context: %p", mContext);
  
  if (eglMakeCurrent(mDisplay, mSurface, mSurface, mContext) == EGL_FALSE) 
  {
    LOGI("Unable to eglMakeCurrent");
    return false;
  }
    
  nuiCheckForGLErrorsReal();
  // Initialize GL state.
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  nuiCheckForGLErrorsReal();
  glEnable(GL_CULL_FACE);
  nuiCheckForGLErrorsReal();
  glShadeModel(GL_SMOOTH);
  nuiCheckForGLErrorsReal();
  glDisable(GL_DEPTH_TEST);
  nuiCheckForGLErrorsReal();

  return true;
}


