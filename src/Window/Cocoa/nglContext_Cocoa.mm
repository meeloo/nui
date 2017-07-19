#include "nui.h"
#include "nglWindow_Cocoa.h"

//#include "ngl_uikit.h"

//#define _DEBUG_CONTEXT_
/*
 * nglContextInfo
 */

bool nglContextInfo::Enum(uint Index, nglContextInfo& rInfo)
{
/*
  if (!nglContext::mEGLInitialized)
    return false;
  EGLDisplay display = eglGetDisplay(0);

  EGLint numConfigs;
  if (!eglGetConfigs(display, 0, 0, &numConfigs))
    return false;
  NGL_OUT("%d egl configs\n", numConfigs);
  if (Index >= numConfigs)
    return false;

  const uint maxConf = (Index+1);
  EGLConfig configs[maxConf];
  eglGetConfigs(display, configs, maxConf, &numConfigs);

  EGLint redBits;
  EGLint greenBits;
  EGLint blueBits;
  EGLint alphaBits;

  EGLint bufferSize;
  EGLint depthSize;
  EGLint stencilSize;

  EGLint caveat;
  EGLint native;

  EGLint level;
  EGLint surfaceType;
  EGLint visualType;
  EGLint visualID;

  eglGetConfigAttrib(display, configs[Index], EGL_RED_SIZE,    &redBits);
  eglGetConfigAttrib(display, configs[Index], EGL_GREEN_SIZE,  &greenBits);
  eglGetConfigAttrib(display, configs[Index], EGL_BLUE_SIZE,   &blueBits);
  eglGetConfigAttrib(display, configs[Index], EGL_ALPHA_SIZE,  &alphaBits);

  eglGetConfigAttrib(display, configs[Index], EGL_BUFFER_SIZE, &bufferSize);
  eglGetConfigAttrib(display, configs[Index], EGL_DEPTH_SIZE,  &depthSize);
  eglGetConfigAttrib(display, configs[Index], EGL_STENCIL_SIZE, &stencilSize);
  eglGetConfigAttrib(display, configs[Index], EGL_CONFIG_CAVEAT, &caveat);
  eglGetConfigAttrib(display, configs[Index], EGL_NATIVE_RENDERABLE, &native);

  eglGetConfigAttrib(display, configs[Index], EGL_LEVEL, &level);
  eglGetConfigAttrib(display, configs[Index], EGL_SURFACE_TYPE, &surfaceType);
  eglGetConfigAttrib(display, configs[Index], EGL_NATIVE_VISUAL_TYPE, &visualType);
  eglGetConfigAttrib(display, configs[Index], EGL_NATIVE_VISUAL_ID, &visualID);
  
  rInfo.FrameCnt=2; ///< Number of frame buffers (two means double-buffering)
  rInfo.FrameBitsR=redBits;
  rInfo.FrameBitsG=greenBits;
  rInfo.FrameBitsB=blueBits;
  rInfo.FrameBitsA=alphaBits;
  rInfo.DepthBits=depthSize;
  rInfo.StencilBits=stencilSize;
*/
/*
  rInfo.AccumBitsR=0;
  rInfo.AccumBitsG=0;
  rInfo.AccumBitsB=0;
  rInfo.AccumBitsA=0;
  rInfo.AuxCnt=0;
  rInfo.AABufferCnt=0;
  rInfo.AASampleCnt=0;
  rInfo.Stereo=0;
  rInfo.Offscreen=0;
  rInfo.RenderToTexture=0;
  rInfo.CopyOnSwap=0;
*/
  rInfo.TargetAPI = eTargetAPI_OpenGL2;
/*
  NGL_OUT(
    "config %d: rgba=%d:%d:%d:%d(%d), ds=%d:%d, caveat=%04x, native=%d, level=%d, surface=%04x, visual=%04x, id=%d\n",
    Index, redBits, greenBits, blueBits, alphaBits, bufferSize, depthSize, stencilSize, caveat, native, level, surfaceType, visualType, visualID
  );
*/
  return true;
}

const nglChar* gpContextErrorTable[] =
{
/*  0 */ "No error",
/*  1 */ "No context match your request",
/*  2 */ "GL rendering context creation failed",
/*  3 */ "Couldn't bind GL context to the system window",
         NULL
};

#if 0
const nglChar* gpEAGLErrorTable[] =
{
/* kEAGLErrorSuccess */           "No error",
/* kEAGLErrorInvalidParameter */  "Function parameter is not legal",
/* kEAGLErrorInvalidOperation */  "Requested operation is not legal",
/* kEAGLErrorOutOfMemory */       "Implementation ran out of memory",
                                  NULL
};
#endif

/*
 * nglContext
 */

nglContext::nglContext()
: mLock("nglContext::mLock")
{
  mpPainter = NULL;
  mScale = 1.0f;
  mScaleInv = 1.0f;
}

nglContext::~nglContext()
{
}

bool nglContext::GetContextInfo(nglContextInfo& rInfo) const
{
  rInfo = mContextInfo;
  return true;
}


const nglChar* nglContext::OnError (uint& rError) const
{
  return FetchError(gpContextErrorTable, NULL, rError);
}

// -------------------------
// TODO: Query Exts
// -------------------------

nglContext::GLExtFunc nglContext::LookupExtFunc (const char* pFuncName)
{
  return NULL;//(GLExtFunc)eaglGetProcAddress(pFuncName);
}

void nglContext::Build(const nglContextInfo& rInfo)
{
  mTargetAPI = rInfo.TargetAPI;
  InitPainter();
}

void* nglContext::GetMetalDevice() const // id <MTLDevice>
{
  return mMetalDevice;
}

void* nglContext::GetMetalCommandQueue() const // id <MTLCommandQueue>
{
  return mMetalCommandQueue;
}

void* nglContext::GetMetalDestinationTexture() const
{
  return mMetalDestinationTexture;
}

void* nglContext::GetMetalDrawable() const
{
  return mMetalDrawable;
}

void* nglContext::GetMetalCommandEncoder() const
{
  return mMetalCommandEncoder;
}

void* nglContext::GetMetalCommandBuffer() const
{
  return mMetalCommandBuffer;
}

void nglContext::AddMarker(const char* title)
{
  if (mMetalDevice)
  {
    [(id<MTLCommandEncoder>)mMetalCommandEncoder insertDebugSignpost:[NSString stringWithCString:title]];
  }
  else
  {
    glInsertEventMarkerEXT(0, title);
  }
}

void nglContext::StartMarkerGroup(const char* title)
{
  if (mMetalDevice)
  {
    [(id<MTLCommandEncoder>)mMetalCommandEncoder pushDebugGroup:[NSString stringWithCString:title]];
  }
  else
  {
    glPushGroupMarkerEXT(0, title);
  }
}

void nglContext::StopMarkerGroup()
{
  if (mMetalDevice)
  {
    [(id<MTLCommandEncoder>)mMetalCommandEncoder popDebugGroup];
  }
  else
  {
    glPopGroupMarkerEXT();
  }
}

