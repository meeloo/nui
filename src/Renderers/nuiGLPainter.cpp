/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot

 licence: see nui3/LICENCE.TXT
 */


#include "nui.h"
#include "AAPrimitives.h"

#ifndef __NUI_NO_GL__

//#define NUI_RETURN_IF_RENDERING_DISABLED return;
#define NUI_RETURN_IF_RENDERING_DISABLED
//#define NUI_USE_GL_VERTEX_BUFFER
#define NUI_COMPLEX_SHAPE_THRESHOLD 6

//#define NUI_USE_ANTIALIASING
#ifdef NUI_USE_ANTIALIASING
#define NUI_USE_MULTISAMPLE_AA
#endif

#ifdef _UIKIT_
#define glDeleteVertexArrays glDeleteVertexArraysOES
#elif defined __APPLE__
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

#ifdef _UIKIT_
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#elif defined __APPLE__
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif


static const char* TextureVertexColor_VTX =
SHADER_STRING (
               attribute vec4 Position;
               attribute vec2 TexCoord;
               attribute vec4 Color;
               uniform mat4 SurfaceMatrix;
               uniform mat4 ModelViewMatrix;
               uniform mat4 ProjectionMatrix;
               uniform vec4 Offset;
               uniform vec2 TextureTranslate;
               uniform vec2 TextureScale;
               varying vec2 TexCoordVar;
               varying vec4 ColorVar;
               
               void main()
               {
                 TexCoordVar = TexCoord * TextureScale + TextureTranslate;
                 ColorVar = Color;
                 gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
               }
               );

static const char* TextureVertexColor_FGT =
SHADER_STRING (
               uniform sampler2D texture;
               varying vec4 ColorVar;
               varying vec2 TexCoordVar;
               void main()
{
  gl_FragColor = ColorVar * texture2D(texture, TexCoordVar);
  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
               );

////////////////////////////////////////////////////////////////////////////////
static const char* TextureAlphaVertexColor_VTX =
SHADER_STRING (
               attribute vec4 Position;
               attribute vec2 TexCoord;
               attribute vec4 Color;
               uniform mat4 SurfaceMatrix;
               uniform mat4 ModelViewMatrix;
               uniform mat4 ProjectionMatrix;
               uniform vec4 Offset;
               uniform vec2 TextureTranslate;
               uniform vec2 TextureScale;
               varying vec2 TexCoordVar;
               varying vec4 ColorVar;
               void main()
{
  TexCoordVar = TexCoord * TextureScale + TextureTranslate;
  ColorVar = Color;
  gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
}
               );

static const char* TextureAlphaVertexColor_FGT =
SHADER_STRING (
               uniform sampler2D texture;
               varying vec4 ColorVar;
               varying vec2 TexCoordVar;
               void main()
{
  float v = texture2D(texture, TexCoordVar)[3];
  gl_FragColor = ColorVar * v;
  //gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
               );

//////////////////////////////////////////////////////////////////////////////////
static const char* TextureDifuseColor_VTX =
SHADER_STRING (
               attribute vec4 Position;
               attribute vec2 TexCoord;
               uniform mat4 SurfaceMatrix;
               uniform mat4 ModelViewMatrix;
               uniform mat4 ProjectionMatrix;
               uniform vec4 Offset;
               uniform vec2 TextureTranslate;
               uniform vec2 TextureScale;
               varying vec2 TexCoordVar;
               void main()
{
  TexCoordVar = TexCoord * TextureScale + TextureTranslate;
  gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
}
               );

static const char* TextureDifuseColor_FGT =
SHADER_STRING (
               uniform sampler2D texture;
               uniform vec4 DifuseColor;
               varying vec2 TexCoordVar;
               void main()
{
  gl_FragColor = DifuseColor * texture2D(texture, TexCoordVar);
  //gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
  //gl_FragColor = texture2D(texture, TexCoordVar);
}
               );

//////////////////////////////////////////////////////////////////////////////////
static const char* TextureAlphaDifuseColor_VTX =
SHADER_STRING (
               attribute vec4 Position;
               attribute vec2 TexCoord;
               uniform mat4 SurfaceMatrix;
               uniform mat4 ModelViewMatrix;
               uniform mat4 ProjectionMatrix;
               uniform vec4 Offset;
               uniform vec2 TextureTranslate;
               uniform vec2 TextureScale;
               varying vec2 TexCoordVar;
               void main()
{
  TexCoordVar = TexCoord * TextureScale + TextureTranslate;
  gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
}
               );

static const char* TextureAlphaDifuseColor_FGT =
SHADER_STRING (
               uniform sampler2D texture;
               uniform vec4 DifuseColor;
               varying vec2 TexCoordVar;
               void main()
{
  float v = texture2D(texture, TexCoordVar)[3];\
  gl_FragColor = DifuseColor * vec4(v, v, v, v);
}
               );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// No texture cases:
static const char* VertexColor_VTX =
SHADER_STRING (
               attribute vec4 Position;
               attribute vec4 Color;
               uniform mat4 SurfaceMatrix;
               uniform mat4 ModelViewMatrix;
               uniform mat4 ProjectionMatrix;
               uniform vec4 Offset;
               varying vec4 ColorVar;
               void main()
{
  ColorVar = Color;
  gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
}
               );

static const char* VertexColor_FGT =
SHADER_STRING (
               varying vec4 ColorVar;
               void main()
{
  gl_FragColor = ColorVar;
}
               );

//////////////////////////////////////////////////////////////////////////////////
static const char* DifuseColor_VTX =
SHADER_STRING (
               attribute vec4 Position;
               uniform mat4 SurfaceMatrix;
               uniform mat4 ModelViewMatrix;
               uniform mat4 ProjectionMatrix;
               uniform vec4 Offset;
               uniform vec4 DifuseColor;
               varying vec4 ColorVar;
               
               void main()
               {
                 ColorVar = DifuseColor;
                 gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
               }
               );

static const char* DifuseColor_FGT =
SHADER_STRING (
               uniform sampler2D texture;
               varying vec4 ColorVar;
               void main()
               {
                 gl_FragColor = ColorVar;
               }
               );

// Stats
static uint32 mins = 30000;
static uint32 maxs = 0;
static uint32 totalinframe = 0;
static uint32 total = 0;
static nglCriticalSection gStats(nglString(__FILE__).Add(":").Add(__LINE__).GetChars());


static int64 MakePOT(int64 v)
{
  uint i;
  nuiSize val = 1;
  for (i = 0; i < 32; i++)
  {
    if (v <= val)
    {
      v = val;
      break;
    }
    val *= 2;
  }
  return v;
}



void nuiGLLoadMatrix(const float* pMatrix)
{
  glLoadMatrixf(pMatrix);
}

void nuiGLLoadMatrix(const double* pMatrix)
{
#ifndef _OPENGL_ES_
  glLoadMatrixd(pMatrix);
#else
  NGL_ASSERT(!"no glLoadMatrixd in gles");
#endif
}



void nuiGLPainter::BlendFuncSeparate(GLenum src, GLenum dst, GLenum srcalpha, GLenum dstalpha)
{
  mSrcColor = src;
  mDstColor = dst;
  mSrcAlpha = srcalpha;
  mDstAlpha = dstalpha;
#ifndef _OPENGL_ES_
  if (mpContext->glBlendFuncSeparate)
  {
    mpContext->glBlendFuncSeparate(src, dst, srcalpha, dstalpha);
    mTwoPassBlend = false;
  }
  else
  {
    glBlendFunc(src, dst);
    if (src != srcalpha || dst != dstalpha)
      mTwoPassBlend = true;
    else
      mTwoPassBlend = false;
  }
#else
#if GL_OES_blend_equation_separate
  //    if (glBlendFuncSeparateOES)
  //    {
  //      glBlendFuncSeparateOES(src, dst, srcalpha, dstalpha);
  //      mTwoPassBlend = false;
  //    }
  //    else
  {
    glBlendFunc(src, dst);
    if (src != srcalpha || dst != dstalpha)
      mTwoPassBlend = true;
    else
      mTwoPassBlend = false;
  }
#else
  glBlendFunc(src, dst);
#endif
#endif
}


bool nuiGLPainter::CheckFramebufferStatus()
{
  //  return true;
#if 1
  GLint status = glCheckFramebufferStatusNUI(GL_FRAMEBUFFER_NUI);
  if (status == GL_FRAMEBUFFER_COMPLETE_NUI)
    return true;
#if defined(NGL_DEBUG)
  switch (status)
  {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_NUI:
    {
      NGL_OUT("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
    } break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_NUI:
    {
      NGL_OUT("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
    } break;
//    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_NUI:
//    {
//      NGL_OUT("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
//    } break;
//    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_NUI:
//    {
//      NGL_OUT("GL_FRAMEBUFFER_INCOMPLETE_FORMATS\n");
//    } break;
    case GL_FRAMEBUFFER_UNSUPPORTED_NUI:
    {
      NGL_OUT("GL_FRAMEBUFFER_UNSUPPORTED\n");
    } break;
    default:
    {
      NGL_OUT("GL Framebuffer incomplete, unknown error %d (0x%x)\n", status, status);
    } break;
  }
#endif
  return (status == GL_FRAMEBUFFER_COMPLETE_NUI);
#endif
}

uint32 nuiGLPainter::mActiveContexts = 0;

nuiGLPainter::nuiGLPainter(nglContext* pContext)
: nuiPainter(pContext), mRenderingCS("mRenderingCS")
{
  gpPainters.insert(this);

  nuiCheckForGLErrors();
  mCanRectangleTexture = 0;
  mTextureTarget = 0;
  mTwoPassBlend = false;
  mDefaultFramebuffer = 0;
  mDefaultRenderbuffer = 0;
  mForceApply = false;
  mClientVertex = false;
  mClientColor = false;
  mClientTexCoord = false;
  mMatrixChanged = true;
  mR = -1;
  mG = -1;
  mB = -1;
  mA = -1;
  mTexEnvMode = 0;
  mUseShaders = false;


  mpContext = pContext;
  if (mpContext)
  {
    mpContext->BeginSession();
    const char* ext0 = (const char*)glGetString(GL_EXTENSIONS);
    nglString exts(ext0);
    //NGL_OUT("Extensions: %s\n", exts.GetChars());


    mpContext->CheckExtension("GL_VERSION_1_2");
    nuiCheckForGLErrors();
    mpContext->CheckExtension("GL_VERSION_1_3");
    nuiCheckForGLErrors();
    mpContext->CheckExtension("GL_VERSION_1_4");
    nuiCheckForGLErrors();
    mpContext->CheckExtension("GL_VERSION_1_5");
    nuiCheckForGLErrors();
    mpContext->CheckExtension("GL_VERSION_2_0");
    nuiCheckForGLErrors();
    mpContext->CheckExtension("GL_ARB_vertex_buffer_object");
    nuiCheckForGLErrors();
    mpContext->CheckExtension("GL_EXT_framebuffer_object");
    nuiCheckForGLErrors();

    mpContext->CheckExtension("GL_ARB_framebuffer_object");
    nuiCheckForGLErrors();

    if (mpContext->CheckExtension("GL_ARB_texture_non_power_of_two")  || mpContext->CheckExtension("GL_APPLE_texture_2D_limited_npot"))
    {
      mCanRectangleTexture = 1;
    }
    else if (mpContext->CheckExtension("GL_EXT_texture_rectangle") || mpContext->CheckExtension("GL_ARB_texture_rectangle") || mpContext->CheckExtension("GL_NV_texture_rectangle"))
    {
      mCanRectangleTexture = 2;
    }
#ifdef _UIKIT_
    mCanRectangleTexture = 1;
#endif


    if (!mActiveContexts)
    {
      glAAInit();
      nuiCheckForGLErrors();
    }


    nuiCheckForGLErrors();
  }

#ifdef _OPENGL_ES_
  //  mDefaultFramebuffer = 0;
  //  mDefaultRenderbuffer = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING_NUI, &mDefaultFramebuffer);
  nuiCheckForGLErrors();
  glGetIntegerv(GL_RENDERBUFFER_BINDING_NUI, (GLint *) &mDefaultRenderbuffer);
  nuiCheckForGLErrors();
#endif

  mActiveContexts++;
  nuiCheckForGLErrors();
  
  nuiCheckForGLErrors();
  mUseShaders = true;
  
  mpShader_TextureVertexColor = nuiShaderProgram::GetProgram("TextureVertexColor");
  if (!mpShader_TextureVertexColor)
  {
    mpShader_TextureVertexColor = new nuiShaderProgram("TextureVertexColor");
    mpShader_TextureVertexColor->AddShader(eVertexShader, TextureVertexColor_VTX);
    mpShader_TextureVertexColor->AddShader(eFragmentShader, TextureVertexColor_FGT);
    mpShader_TextureVertexColor->Link();
    mpShader_TextureVertexColor->GetCurrentState()->Set("Offset", 0.0f, 0.0f);
    mpShader_TextureVertexColor->GetCurrentState()->Set("texture", 0);
  }
  nuiCheckForGLErrors();
  
  mpShader_TextureAlphaVertexColor = nuiShaderProgram::GetProgram("TextureAlphaVertexColor");
  if (!mpShader_TextureAlphaVertexColor)
  {
    mpShader_TextureAlphaVertexColor = new nuiShaderProgram("TextureAlphaVertexColor");
    mpShader_TextureAlphaVertexColor->AddShader(eVertexShader, TextureAlphaVertexColor_VTX);
    mpShader_TextureAlphaVertexColor->AddShader(eFragmentShader, TextureAlphaVertexColor_FGT);
    mpShader_TextureAlphaVertexColor->Link();
    mpShader_TextureAlphaVertexColor->GetCurrentState()->Set("Offset", 0.0f, 0.0f);
    mpShader_TextureAlphaVertexColor->GetCurrentState()->Set("texture", 0);
  }
  nuiCheckForGLErrors();
  
  mpShader_TextureDifuseColor = nuiShaderProgram::GetProgram("TextureDiffuseColor");
  if (!mpShader_TextureDifuseColor)
  {
    mpShader_TextureDifuseColor = new nuiShaderProgram("TextureDiffuseColor");
    mpShader_TextureDifuseColor->AddShader(eVertexShader, TextureDifuseColor_VTX);
    mpShader_TextureDifuseColor->AddShader(eFragmentShader, TextureDifuseColor_FGT);
    mpShader_TextureDifuseColor->Link();
    mpShader_TextureDifuseColor->GetCurrentState()->Set("DifuseColor", nuiColor(255, 255, 255, 255));
    mpShader_TextureDifuseColor->GetCurrentState()->Set("Offset", 0.0f, 0.0f);
    mpShader_TextureDifuseColor->GetCurrentState()->Set("texture", 0);
  }
  nuiCheckForGLErrors();
  
  mpShader_TextureAlphaDifuseColor = nuiShaderProgram::GetProgram("TextureAlphaDifuseColor");
  if (!mpShader_TextureAlphaDifuseColor)
  {
    mpShader_TextureAlphaDifuseColor = new nuiShaderProgram("TextureAlphaDifuseColor");
    mpShader_TextureAlphaDifuseColor->AddShader(eVertexShader, TextureAlphaDifuseColor_VTX);
    mpShader_TextureAlphaDifuseColor->AddShader(eFragmentShader, TextureAlphaDifuseColor_FGT);
    mpShader_TextureAlphaDifuseColor->Link();
    mpShader_TextureAlphaDifuseColor->GetCurrentState()->Set("DifuseColor", nuiColor(255, 255, 255, 255));
    mpShader_TextureAlphaDifuseColor->GetCurrentState()->Set("Offset", 0.0f, 0.0f);
    mpShader_TextureAlphaDifuseColor->GetCurrentState()->Set("texture", 0);
  }
  nuiCheckForGLErrors();
  
  mpShader_VertexColor = nuiShaderProgram::GetProgram("VertexColor");
  if (!mpShader_VertexColor)
  {
    mpShader_VertexColor = new nuiShaderProgram("VertexColor");
    mpShader_VertexColor->AddShader(eVertexShader, VertexColor_VTX);
    mpShader_VertexColor->AddShader(eFragmentShader, VertexColor_FGT);
    mpShader_VertexColor->Link();
    mpShader_VertexColor->GetCurrentState()->Set("Offset", 0.0f, 0.0f);
  }
  nuiCheckForGLErrors();
  
  mpShader_DifuseColor = nuiShaderProgram::GetProgram("DifuseColor");
  if (!mpShader_DifuseColor)
  {
    mpShader_DifuseColor = new nuiShaderProgram("DifuseColor");
    mpShader_DifuseColor->AddShader(eVertexShader, DifuseColor_VTX);
    mpShader_DifuseColor->AddShader(eFragmentShader, DifuseColor_FGT);
    mpShader_DifuseColor->Link();
    mpShader_DifuseColor->GetCurrentState()->Set("DifuseColor", nuiColor(255, 255, 255, 255));
    mpShader_DifuseColor->GetCurrentState()->Set("Offset", 0.0f, 0.0f);
  }
  nuiCheckForGLErrors();
  
  
  mpCurrentRenderArrayInfo = NULL;
  mpLastArray = NULL;

}

nuiGLPainter::~nuiGLPainter()
{
  gpPainters.erase(this);

  mActiveContexts--;
  if (mActiveContexts == 0)
    glAAExit();
  nglCriticalSectionGuard fag(mFrameArraysCS);
  for (auto pArray : mFrameArrays)
    pArray->Release();
  mFrameArrays.clear();

  mpShader_TextureVertexColor->Release();
  mpShader_TextureDifuseColor->Release();
  mpShader_TextureAlphaVertexColor->Release();
  mpShader_TextureAlphaDifuseColor->Release();
  mpShader_DifuseColor->Release();
  mpShader_VertexColor->Release();
}

void nuiGLPainter::SetViewport()
{
  //GetAngle(), GetCurrentWidth(), GetCurrentHeight(), mProjectionViewportStack.top(), mProjectionMatrixStack.top());
  GLint Angle = GetAngle();
  GLint Width = GetCurrentWidth();
  GLint Height = GetCurrentHeight();
  const nuiRect& rViewport(mProjectionViewportStack.top());
  const nuiMatrix& rMatrix = mProjectionMatrixStack.top();
  
  //NGL_DEBUG(NGL_OUT("nuiGLPainter::SetViewPort(%d, %d)\n", Width, Height);)
  
  uint32 x, y, w, h;
  
  nuiRect r(rViewport);
  //  if (Angle == 90 || Angle == 270)
  //  {
  //    uint32 tmp = Width;
  //    Width = Height;
  //    Height = tmp;
  //    r.Set(r.Top(), r.Left(), r.GetHeight(), r.GetWidth());
  //  }
  
  
  x = ToBelow(r.Left());
  w = ToBelow(r.GetWidth());
  y = Height - ToBelow(r.Bottom());
  h = ToBelow(r.GetHeight());
  
  {
    const float scale = mpContext->GetScale();
    x *= scale;
    y *= scale;
    w *= scale;
    h *= scale;
  }
  
  nuiCheckForGLErrors();
  glViewport(x, y, w, h);
  
  if (mpSurface)
  {
    mSurfaceMatrix.SetScaling(1.0f, -1.0f, 1.0f);
  }
  else
  {
    mSurfaceMatrix.SetIdentity();
  }
  
  float angle = GetAngle();
  //  if (angle != 0.0f)
  //  {
  //    mSurfaceMatrix.Rotate(angle, 0 ,0, 1);
  //  }
  
  nuiCheckForGLErrors();
}




void nuiGLPainter::StartRendering()
{
  nuiPainter::StartRendering();
  nglCriticalSectionGuard fag(mFrameArraysCS);
  for (auto pArray : mFrameArrays)
    pArray->Release();
  mFrameArrays.clear();

  ResetOpenGLState();
}

void nuiGLPainter::ResetOpenGLState()
{
  BeginSession();
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGLPainter::ResetOpenGLState");
#endif
  
  //NUI_RETURN_IF_RENDERING_DISABLED;
  nuiCheckForGLErrors();
  
  mScissorX = -1;
  mScissorY = -1;
  mScissorW = -1;
  mScissorH = -1;
  mScissorOn = false;
  
  
  SetViewport();
  
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_BLEND);
#ifndef _OPENGL_ES_
  glDisable(GL_ALPHA_TEST);
#endif
  glDisable(GL_CULL_FACE);
  
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  BlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  
  mMatrixChanged = true;
  mR = -1;
  mG = -1;
  mB = -1;
  mA = -1;
  mTexEnvMode = 0;
  
  //  mTextureTranslate = nglVector2f(0.0f, 0.0f);
  //  mTextureScale = nglVector2f(1, 1);
  
  mFinalState = nuiRenderState();
  mpState = &mDefaultState;
  mpLastArray = NULL;
  
  nuiCheckForGLErrors();
}



void nuiGLPainter::ApplyState(const nuiRenderState& rState, bool ForceApply)
{
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGLPainter::ApplyState()");
#endif

  //TEST_FBO_CREATION();
  NUI_RETURN_IF_RENDERING_DISABLED;
  //ForceApply = true;
  nuiCheckForGLErrors();

  // blending
  if (ForceApply || mFinalState.mBlending != rState.mBlending)
  {
    mFinalState.mBlending = rState.mBlending;
    if (mFinalState.mBlending)
    {
      glEnable(GL_BLEND);
    }
    else
    {
      glDisable(GL_BLEND);
    }
  }

  if (ForceApply || mFinalState.mBlendFunc != rState.mBlendFunc)
  {
    mFinalState.mBlendFunc = rState.mBlendFunc;
    GLenum src, dst;
    nuiGetBlendFuncFactors(rState.mBlendFunc, src, dst);
    BlendFuncSeparate(src, dst);
    nuiCheckForGLErrors();
  }

  if (ForceApply || mFinalState.mDepthTest != rState.mDepthTest)
  {
    mFinalState.mDepthTest = rState.mDepthTest;
    if (mFinalState.mDepthTest)
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);
  }

  if (ForceApply || mFinalState.mDepthWrite != rState.mDepthWrite)
  {
    mFinalState.mDepthWrite = rState.mDepthWrite;
    glDepthMask(mFinalState.mDepthWrite ? GL_TRUE : GL_FALSE);
  }

  if (ForceApply || mFinalState.mCulling != rState.mCulling)
  {
    mFinalState.mCulling = rState.mCulling;
    if (mFinalState.mCulling)
    {
      glEnable(GL_CULL_FACE);
    }
    else
    {
      glDisable(GL_CULL_FACE);
    }
  }

  if (ForceApply || mFinalState.mCullingMode != rState.mCullingMode)
  {
    mFinalState.mCullingMode = rState.mCullingMode;
    glCullFace(mFinalState.mCullingMode);
  }


  // We don't care about the font in the lower layer of rendering
  //nuiFont* mpFont;
  //

  for (int i = 0; i < NUI_MAX_TEXTURE_UNITS; i++)
  {
    ApplyTexture(rState, ForceApply, i);
  }

  if (ForceApply || mFinalState.mpShader != mpShader)
  {
    if (mpShader)
      mpShader->Acquire();
    if (mFinalState.mpShader)
      mFinalState.mpShader->Release();

    mFinalState.mpShader = mpShader;
#if DEBUG
    mpShader->Validate();
#endif
    glUseProgram(mpShader->GetProgram());
  }

  if (mpShaderState)
    mpShaderState->Acquire();
  if (mFinalState.mpShaderState)
    mFinalState.mpShaderState->Release();
  mFinalState.mpShaderState = mpShaderState;

  // Rendering buffers:
  if (ForceApply || mFinalState.mColorBuffer != rState.mColorBuffer)
  {
    mFinalState.mColorBuffer = rState.mColorBuffer;
    GLboolean m = mFinalState.mColorBuffer ? GL_TRUE : GL_FALSE;
    glColorMask(m, m, m, m);
    nuiCheckForGLErrors();
  }

  if (mClip.mEnabled || ForceApply)
  {
    uint32 width = GetCurrentWidth();
    uint32 height = GetCurrentHeight();

    nuiRect clip(mClip);

    int x,y,w,h;
    uint angle = (mpSurface && mpSurface->GetRenderToTexture()) ? 0 : mAngle;
    x = ToBelow(clip.Left());
    y = ToBelow(height - clip.Bottom());
    w = ToBelow(clip.GetWidth());
    h = ToBelow(clip.GetHeight());

    if (!mScissorOn || ForceApply)
    {
      glEnable(GL_SCISSOR_TEST);
      mScissorOn = true;
    }

    if (mScissorX != x || mScissorY != y || mScissorW != w || mScissorH != h || ForceApply)
    {
      mScissorX = x;
      mScissorY = y;
      mScissorW = w;
      mScissorH = h;

      if (mpSurface)
      {
        y = mpSurface->GetHeight() - (y + h);
      }

      {
        float scale = mpContext->GetScale();
        x *= scale;
        y *= scale;
        w *= scale;
        h *= scale;
      }
      
      if (mpSurface)
      {
//        NGL_OUT("%p scissor: %d %d %d %d (%s)\n", mpSurface, x, y, w, h, mpSurface->GetObjectName().GetChars());
      }
      glScissor(x, y, w, h);
    }
    nuiCheckForGLErrors();
  }
  else
  {
    if (mScissorOn || ForceApply)
    {
      glDisable(GL_SCISSOR_TEST);
      mScissorOn = false;
    }
  }

  mFinalState.mClearColor = rState.mClearColor;
  mFinalState.mStrokeColor = rState.mStrokeColor;
  mFinalState.mFillColor = rState.mFillColor;

  mForceApply = false;

  nuiCheckForGLErrors();
}

void nuiGLPainter::SetState(const nuiRenderState& rState, bool ForceApply)
{
  //TEST_FBO_CREATION();
  NUI_RETURN_IF_RENDERING_DISABLED;

  mpState = &rState;
  mForceApply |= ForceApply;
  //ApplyState(rState, ForceApply);
}

void nuiGLPainter::SetSize(uint32 w, uint32 h)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  NGL_DEBUG(NGL_LOG("painter", NGL_LOG_DEBUG, "nuiGLPainter::SetSize(%d, %d)\n", w, h);)
  mWidth = w;
  mHeight = h;
}

void nuiGLPainter::ApplyTexture(const nuiRenderState& rState, bool ForceApply, int slot)
{
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGLPainter::ApplTexture()");
#endif

  NGL_ASSERT(slot < NUI_MAX_TEXTURE_UNITS);
  NGL_ASSERT(slot >= 0);
  
  // 2D Textures:
  nglCriticalSectionGuard tg(mTexturesCS);
  auto it = mTextures.end();
  nuiTexture* pTexture = rState.mpTexture[slot];
  if (pTexture)
  {
    it = mTextures.find(pTexture);
    if (pTexture->GetTrace())
    {
      NGL_OUT("Applying texture %s\n", pTexture->GetSource().GetChars());
    }
 }

  bool uptodate = false;
  if (it != mTextures.end())
  {
    if (!it->second.mReload && it->second.mTexture >= 0)
    {
      uptodate = true;
    }
  }

  if (pTexture && !pTexture->IsUptoDate())
  {
    uptodate = false;
    if (it != mTextures.end())
    {
      it->second.mReload = true;
    }
  }

  if (ForceApply || (mFinalState.mpTexture[slot] != pTexture) || (mFinalState.mpTexture[slot] && !uptodate))
  {
    GLenum intarget = 0;
    GLenum outtarget = 0;

    if (mFinalState.mpTexture[slot])
    {
      if (mFinalState.mpTexture[slot]->GetTextureID())
        outtarget = mFinalState.mpTexture[slot]->GetTarget();
      else
        outtarget = GetTextureTarget(mFinalState.mpTexture[slot]->IsPowerOfTwo());

      nuiCheckForGLErrors();
      mFinalState.mpTexture[slot]->Release();
      nuiCheckForGLErrors();
    }

    //NGL_OUT("Change texture to 0x%x (%s)\n", pTexture, pTexture?pTexture->GetSource().GetChars() : nglString::Empty.GetChars());
    mFinalState.mpTexture[slot] = pTexture ;

    if (mFinalState.mpTexture[slot])
    {
      if (mFinalState.mpTexture[slot]->GetTextureID())
        intarget = mFinalState.mpTexture[slot]->GetTarget();
      else
        intarget = GetTextureTarget(mFinalState.mpTexture[slot]->IsPowerOfTwo());

      mFinalState.mpTexture[slot]->Acquire();
      mTextureTarget = intarget;

      UploadTexture(mFinalState.mpTexture[slot], slot);
      nuiCheckForGLErrors();
    }

    //NGL_OUT("Change texture type from 0x%x to 0x%x\n", outtarget, intarget);

    if (!mUseShaders)
    {
      if (intarget != outtarget)
      {
        // Texture Target has changed
        if (outtarget)
        {
//          NGL_OUT("disable outtarget 0x%x\n", outtarget);
          glDisable(outtarget);
          nuiCheckForGLErrors();
        }
        mFinalState.mTexturing = rState.mTexturing;
        if (intarget && mFinalState.mTexturing && mFinalState.mpTexture[slot])
        {
//          NGL_OUT("enable intarget 0x%x\n", intarget);
          glEnable(intarget);
          nuiCheckForGLErrors();
        }
      }
      else
      {
        // Texture Target have not changed
        if (mFinalState.mTexturing != rState.mTexturing) // Have texture on/off changed?
        {
          // Should enable or disable texturing
          mFinalState.mTexturing = rState.mTexturing;
          if (mFinalState.mTexturing)
          {
//            NGL_OUT("enable mTextureTarget 0x%x\n", mTextureTarget);
            glEnable(mTextureTarget);
            nuiCheckForGLErrors();
          }
          else
          {
//            NGL_OUT("disable mTextureTarget 0x%x\n", mTextureTarget);
            glDisable(mTextureTarget);
            nuiCheckForGLErrors();
          }
        }
      }
    }
  }
  else
  {
    mFinalState.mTexturing = rState.mTexturing;
  }
}



void nuiGLPainter::Clear(bool color, bool depth, bool stencil)
{
  mRenderOperations++;
  NUI_RETURN_IF_RENDERING_DISABLED;

  glClearColor(mpState->mClearColor.Red(),mpState->mClearColor.Green(),mpState->mClearColor.Blue(),mpState->mClearColor.Alpha());
#ifdef _OPENGL_ES_
    glClearDepthf(mFinalState.mClearDepth);
#else
    glClearDepth(mFinalState.mClearDepth);
#endif

  GLint v = 0;
  if (color)
    v |= GL_COLOR_BUFFER_BIT;
  if (depth)
    v |= GL_DEPTH_BUFFER_BIT;
  if (stencil)
    v |= GL_STENCIL_BUFFER_BIT;

  glClear(v);
  nuiCheckForGLErrors();
}

/*
 void nuiGLPainter::BlurRect(const nuiRect& rRect, uint Strength)
 {
 nuiRect Rect = rRect;
 if (mClippingRect.mEnabled)
 Rect.Intersect(mClippingRect,rRect);
 nuiRect size = Rect.Size();

 nuiTexture* pScratchPad = GetScratchPad(ToZero(size.GetWidth()), ToZero(size.GetHeight()));

 if (!pScratchPad)
 return;

 SetTexture(pScratchPad);

 glPushMatrix();
 glLoadIdentity();

 EnableBlending(true);
 EnableTexturing(true);
 SetBlendFunc(nuiBlendTransp);

 do
 {
 glCopyTexSubImage2D(
 GL_TEXTURE_2D, 0,
 0, 0,
 ToZero(rRect.mLeft), ToZero(mHeight) - 1 - ToZero(rRect.mTop) - ToZero(rRect.GetHeight()),
 ToZero(rRect.GetWidth()), ToZero(rRect.GetHeight())
 );

 SetFillColor(nuiColor(1,1,1,.15f));
 nuiRect rect = Rect;

 rect.Move(-1,-1);
 DrawImage(rect,size);
 rect.Move(1,0);
 DrawImage(rect,size);
 rect.Move(1,0);
 DrawImage(rect,size);

 rect.Move(-2,1);
 DrawImage(rect,size);
 rect.Move(1,0);
 DrawImage(rect,size);
 rect.Move(1,0);
 DrawImage(rect,size);

 rect.Move(-2,1);
 DrawImage(rect,size);
 rect.Move(0,1);
 DrawImage(rect,size);
 rect.Move(0,1);
 DrawImage(rect,size);
 } while ((long)(Strength--) > 0);

 EnableBlending(false);
 EnableTexturing(false);

 glPopMatrix();
 }
 */


#define LOGENUM(XXX) case XXX: { NGL_OUT("%s\n", #XXX); } break;

void nuiGLPainter::DrawArray(nuiRenderArray* pArray)
{
  mRenderOperations++;
  mBatches++;
  
  if (!mEnableDrawArray)
  {
    pArray->Release();
    return;
  }
  
  if (pArray->GetDebug())
  {
    NGL_OUT("Texturing %s (%p)\n", YESNO(mFinalState.mTexturing), mFinalState.mpTexture[0]);
    pArray->Dump();
  }
  
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGLPainter::DrawArray");
#endif

  static uint32 ops = 0;
  static uint32 skipped_ops = 0;
  {
    nglCriticalSectionGuard g(gStats);
    ops++;
  }

  const nuiMatrix& rM(mMatrixStack.top());
  float bounds[6];
  pArray->GetBounds(bounds);
  
  if (rM.Elt.M11 == 1.0f
      && rM.Elt.M22 == 1.0f
      && rM.Elt.M33 == 1.0f
      && rM.Elt.M44 == 1.0f
      
      && rM.Elt.M12 == 0.0f
      && rM.Elt.M13 == 0.0f
      //&& rM.Elt.M14 == 0.0f
      
      && rM.Elt.M21 == 0.0f
      && rM.Elt.M23 == 0.0f
      //&& rM.Elt.M24 == 0.0f
      
      && rM.Elt.M31 == 0.0f
      && rM.Elt.M32 == 0.0f
      && rM.Elt.M34 == 0.0f
      
      && rM.Elt.M41 == 0.0f
      && rM.Elt.M42 == 0.0f
      && rM.Elt.M43 == 0.0f)
  {
    bounds[0] += rM.Elt.M14;
    bounds[1] += rM.Elt.M24;
    //bounds[2] += rM.Elt.M34;
    bounds[3] += rM.Elt.M14;
    bounds[4] += rM.Elt.M24;
    //bounds[5] += rM.Elt.M34;
    
    if (
        (bounds[0] > mClip.Right()) ||
        (bounds[1] > mClip.Bottom()) ||
        (bounds[3] < mClip.Left()) ||
        (bounds[4] < mClip.Top())
        )
    {
      pArray->Release();
      {
        nglCriticalSectionGuard g(gStats);
        skipped_ops++;

      //      #ifdef _DEBUG
      //      if (!(skipped_ops % 100))
      //        printf("optim (%d / %d) - %2.2f%%\n", skipped_ops, ops, (float)skipped_ops * 100.0f / (float)ops);
      //      #endif
      }
      return;
    }
  }
  
  // Shader selection:
  mpShader = mpState->mpShader;
  mpShaderState = mpState->mpShaderState;
  
  if (mpShader == NULL)
  {
    nuiShaderProgram* pShader = NULL;
    if (pArray->IsArrayEnabled(nuiRenderArray::eColor))
    {
      // Vertex Colors is on
      if (pArray->IsArrayEnabled(nuiRenderArray::eTexCoord))
      {
        // texture on
        if (mpState->mpTexture[0]->GetPixelFormat() == eImagePixelAlpha)
          pShader = mpShader_TextureAlphaVertexColor;
        else
          pShader = mpShader_TextureVertexColor;
      }
      else
      {
        pShader = mpShader_VertexColor;
      }
    }
    else
    {
      // Vertex color off -> Difuse Color
      if (pArray->IsArrayEnabled(nuiRenderArray::eTexCoord) && mpState->mpTexture[0])
      {
        // texture on
        if (mpState->mpTexture[0]->GetPixelFormat() == eImagePixelAlpha)
          pShader = mpShader_TextureAlphaDifuseColor;
        else
          pShader = mpShader_TextureDifuseColor;
      }
      else
      {
        pShader = mpShader_DifuseColor;
      }
    }
    
    pShader->Acquire();
    mpShader = pShader;
    mpShaderState = pShader->GetCurrentState();
    mpShaderState->Acquire();
  }
  
  NGL_ASSERT(mpShader != NULL);
  
  ApplyState(*mpState, mForceApply);
  pArray->Acquire();
  {
    nglCriticalSectionGuard fag(mFrameArraysCS);
    mFrameArrays.push_back(pArray);
  }
  
  
  if (mFinalState.mpTexture && pArray->IsArrayEnabled(nuiRenderArray::eTexCoord))
  {
    if (mTextureScale[1] < 0)
    {
      NGL_ASSERT(mFinalState.mpTexture[0]->GetSurface() != NULL);
      //      printf("REVERSED SURFACE TEXTURE");
    }
    mFinalState.mpShaderState->SetTextureTranslate(mTextureTranslate, false);
    //mTextureScale = nglVector2f(1,1);
    mFinalState.mpShaderState->SetTextureScale(mTextureScale, false);
  }
  
  mFinalState.mpShaderState->SetSurfaceMatrix(mSurfaceMatrix, false);
  mFinalState.mpShaderState->SetProjectionMatrix(mProjectionMatrixStack.top(), false);
  mFinalState.mpShaderState->SetModelViewMatrix(mMatrixStack.top(), false);
  
  uint32 s = pArray->GetSize();

  {
  nglCriticalSectionGuard g(gStats);
  total += s;
  totalinframe += s;
  mins = MIN(mins, s);
  maxs = MAX(maxs, s);
  }

  if (!s)
  {
    pArray->Release();
    return;
  }
  
  if (mClip.GetWidth() <= 0 || mClip.GetHeight() <= 0)
  {
    pArray->Release();
    return;
  }
  
  mVertices += s;
  GLenum mode = pArray->GetMode();
  
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  if (mMatrixChanged)
  {
    mMatrixChanged = false;
  }
  
  
  bool NeedTranslateHack = pArray->IsShape() || ((mode == GL_POINTS || mode == GL_LINES || mode == GL_LINE_LOOP || mode == GL_LINE_STRIP) && !pArray->Is3DMesh());
  float hackX = 0;
  float hackY = 0;
  if (NeedTranslateHack)
  {
    const float ratio = mpContext->GetScaleInv() / 2.f;
#ifdef _UIKIT_
    hackX = ratio;
    hackY = ratio;
#else
    //    if (mAngle == 0)
    {
      hackX = ratio;
      hackY = ratio;
    }
    //    else if (mAngle == 90)
    //    {
    //      hackX = 0;
    //      hackY = ratio;
    //    }
    //    else if (mAngle == 180)
    //    {
    //      hackX = 0;
    //      hackY = 0;
    //    }
    //    else/*mAngle == 270*/
    //    {
    //      hackX = ratio;
    //      hackY = 0;
    //    }
#endif
  }
  
  if (mpSurface)
    mFinalState.mpShaderState->SetOffset(-hackX, -hackY, false);
  else
    mFinalState.mpShaderState->SetOffset(hackX, hackY, false);
  
  
  if (!pArray->IsArrayEnabled(nuiRenderArray::eColor))
  {
    nuiColor c;
    switch (pArray->GetMode())
    {
      case GL_POINTS:
      case GL_LINES:
      case GL_LINE_LOOP:
      case GL_LINE_STRIP:
        c = mFinalState.mStrokeColor;
        break;
        
      case GL_TRIANGLES:
      case GL_TRIANGLE_STRIP:
      case GL_TRIANGLE_FAN:
        c = mFinalState.mFillColor;
        break;
    }
    
    mR = c.Red();
    mG = c.Green();
    mB = c.Blue();
    mA = c.Alpha();
    
    mFinalState.mpShaderState->SetDifuseColor(nuiColor(mR, mG, mB, mA), false);
  }
  
  mFinalState.mpShader->SetState(*mFinalState.mpShaderState, true);
  
  if (pArray->IsStatic())
  {
    nglCriticalSectionGuard rag(mRenderArraysCS);
    auto it = mRenderArrays.find(pArray);
    if (it == mRenderArrays.end())
    {
      mRenderArrays[pArray] = RenderArrayInfo::Create(pArray);
      it = mRenderArrays.find(pArray);
    }
    NGL_ASSERT(it != mRenderArrays.end());
    mpCurrentRenderArrayInfo = it->second;
    RenderArrayInfo& rInfo(*it->second);
    
    if (mpLastArray != pArray)
    {
      SetVertexBuffersPointers(*pArray, rInfo);
      mpLastArray = pArray;
    }
  }
  else
  {
    if (mpLastArray != pArray)
    {
      if (mpCurrentRenderArrayInfo)
      {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        mpCurrentRenderArrayInfo = NULL;
      }
      SetVertexPointers(*pArray);
      mpLastArray = pArray;
    }
  }
  
  nuiCheckForGLErrors();
  
  if (mpSurface && mTwoPassBlend && mFinalState.mBlending)
  {
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    uint32 arraycount = pArray->GetIndexArrayCount();
    
    if (!arraycount)
    {
      glDrawArrays(mode, 0, s);
    }
    else
    {
      for (uint32 i = 0; i < arraycount; i++)
      {
        nuiRenderArray::IndexArray& array(pArray->GetIndexArray(i));
#if (defined _UIKIT_) || (defined _ANDROID_)
        glDrawElements(array.mMode, (GLsizei)array.mIndices.size(), GL_UNSIGNED_SHORT, &(array.mIndices[0]));
#else
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_INT, &(array.mIndices[0]));
#endif
      }
    }
    nuiCheckForGLErrors();
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glBlendFunc(mSrcAlpha, mDstAlpha);
    if (!arraycount)
    {
      glDrawArrays(mode, 0, s);
    }
    else
    {
      for (uint32 i = 0; i < arraycount; i++)
      {
        nuiRenderArray::IndexArray& array(pArray->GetIndexArray(i));
#if (defined _UIKIT_) || (defined _ANDROID_)
        glDrawElements(array.mMode, (GLsizei)array.mIndices.size(), GL_UNSIGNED_SHORT, &(array.mIndices[0]));
#else
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_INT, &(array.mIndices[0]));
#endif
      }
    }
    glBlendFunc(mSrcColor, mDstColor);
    nuiCheckForGLErrors();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    nuiCheckForGLErrors();
  }
  else
  {
    uint32 arraycount = pArray->GetIndexArrayCount();
    
    if (!arraycount)
    {
      glDrawArrays(mode, 0, s);
      nuiCheckForGLErrors();
    }
    else
    {
      for (uint32 i = 0; i < arraycount; i++)
      {
        nuiRenderArray::IndexArray& array(pArray->GetIndexArray(i));
#if (defined _UIKIT_) || (defined _ANDROID_)
        glDrawElements(array.mMode, (GLsizei)array.mIndices.size(), GL_UNSIGNED_SHORT, &(array.mIndices[0]));
#else
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_INT, &(array.mIndices[0]));
#endif
        nuiCheckForGLErrors();
      }
    }
  }
  
  glBindVertexArray(0);
  
  //  ResetVertexPointers(*pArray);
  pArray->Release();
  nuiCheckForGLErrors();
}


void nuiGLPainter::BeginSession()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  mpContext->MakeCurrent();
  nuiCheckForGLErrors();

  FinalizeSurfaces();
  FinalizeTextures();
  FinalizeRenderArrays();

  {
    nglCriticalSectionGuard g(gStats);
    totalinframe = 0;
  }
}

void nuiGLPainter::EndSession()
{
  // Bleh!
  NUI_RETURN_IF_RENDERING_DISABLED;
  //NGL_OUT("min = %d max = %d total in frame = %d total = %d\n", mins, maxs, totalinframe, total);
}


uint32 nuiGLPainter::GetRectangleTextureSupport() const
{
  return mCanRectangleTexture;
}

GLenum nuiGLPainter::GetTextureTarget(bool POT) const
{
  GLenum target = GL_TEXTURE_2D;

  if (!POT)
  {
    switch (GetRectangleTextureSupport())
    {
      case 0:
        break;
      case 1:
        break;
      case 2:
#ifndef _OPENGL_ES_
        target = GL_TEXTURE_RECTANGLE_ARB;
#endif
        break;
    }
  }

  return target;
}

nuiGLPainter::TextureInfo::TextureInfo()
{
  mReload = false;
  mTexture = -1;
}

void nuiGLPainter::UploadTexture(nuiTexture* pTexture, int slot)
{
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGLPainter::UploadTexture()");
#endif

  if (mActiveTextureSlot != slot)
  {
    glActiveTexture(GL_TEXTURE0 + slot);
    mActiveTextureSlot = slot;
  }

  nuiTexture* pProxy = pTexture->GetProxyTexture();
  if (pProxy)
    pTexture = pProxy;
  nuiSurface* pSurface = pTexture->GetSurface();

  float Width = pTexture->GetUnscaledWidth();
  float Height = pTexture->GetUnscaledHeight();
  GLenum target = GetTextureTarget(pTexture->IsPowerOfTwo());

  bool changedctx = false;

  nglCriticalSectionGuard tg(mTexturesCS);
  std::map<nuiTexture*, TextureInfo>::iterator it = mTextures.find(pTexture);
  if (it == mTextures.end())
    it = mTextures.insert(mTextures.begin(), std::pair<nuiTexture*, TextureInfo>(pTexture, TextureInfo()));
  NGL_ASSERT(it != mTextures.end());

  TextureInfo& info(it->second);

  GLint id = pTexture->GetTextureID();
  if (id)
  {
    info.mReload = false;
    info.mTexture = id;
    target = pTexture->GetTarget();
  }

  nuiCheckForGLErrors();

  if (!pTexture->IsPowerOfTwo())
  {
    switch (GetRectangleTextureSupport())
    {
      case 0:
        Width = pTexture->GetWidthPOT();
        Height = pTexture->GetHeightPOT();
        break;
      case 1:
      case 2:
        break;
    }
  }

  //NGL_OUT("Apply Target: 0x%x\n", target);
  nglImage* pImage = pTexture->GetImage();

  {
    bool firstload = false;
    bool reload = info.mReload;

    uint i;
    if (info.mTexture == (GLint)-1)
    { // Generate a texture
      //      if (mpSharedContext)
      //      {
      //        mpSharedContext->MakeCurrent();
      //        nuiCheckForGLErrors();
      //        changedctx = true;
      //      }

      glGenTextures(1, (GLuint*)&info.mTexture);

      //NGL_OUT("nuiGLPainter::UploadTexture 0x%x : '%s' / %d\n", pTexture, pTexture->GetSource().GetChars(), info.mTexture);
      nuiCheckForGLErrors();
      firstload = true;
      reload = true;
    }

    glBindTexture(target, info.mTexture);
    nuiCheckForGLErrors();

    if (!pSurface && !(pImage && pImage->GetPixelSize()) && !id)
      return;

    if (reload)
    {
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, pTexture->GetMinFilter());
      nuiCheckForGLErrors();
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, pTexture->GetMagFilter());
      nuiCheckForGLErrors();
      glTexParameteri(target, GL_TEXTURE_WRAP_S, pTexture->GetWrapS());
      nuiCheckForGLErrors();
      glTexParameteri(target, GL_TEXTURE_WRAP_T, pTexture->GetWrapT());
      nuiCheckForGLErrors();

      int type = 8;
      GLint pixelformat = 0;
      GLint internalPixelformat = 0;
      GLbyte* pBuffer = NULL;
      bool allocated = false;

      if (pImage)
      {
        type = pImage->GetBitDepth();
        pixelformat = pImage->GetPixelFormat();
        internalPixelformat = pImage->GetPixelFormat();
        pBuffer = (GLbyte*)pImage->GetBuffer();

        //#ifndef NUI_IOS
#if (!defined NUI_IOS) && (!defined _ANDROID_)
        if (pixelformat == GL_BGR)
          internalPixelformat = GL_RGB;
#endif

        if (!GetRectangleTextureSupport())
        {
          nuiCheckForGLErrors();
          if (!pTexture->IsPowerOfTwo())
          {
            pBuffer = (GLbyte*)malloc( (uint)(Width * Height * pImage->GetPixelSize()));
            if (!pBuffer)
              return;
            allocated = true;
            memset(pBuffer,0, (uint)(Width*Height * pImage->GetPixelSize()));

            for (i=0; i < pImage->GetHeight(); i++)
            {

              memcpy
              (
               pBuffer + (uint)(Width * i * pImage->GetPixelSize()),
               pImage->GetBuffer() + pImage->GetBytesPerLine()*i,
               pImage->GetBytesPerLine()
               );
            }
          }
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        nuiCheckForGLErrors();

        switch (type)
        {
          case 16:
          case 15:
            type = GL_UNSIGNED_SHORT_5_5_5_1;
            break;
          case 8:
          case 24:
          case 32:
            type = GL_UNSIGNED_BYTE;
            break;
        }

#if !defined(_OPENGL_ES_) && defined(_MACOSX_)
        glTexParameteri(target, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_CACHED_APPLE);
        glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
#endif
      }
      else
      {
        NGL_ASSERT(pSurface);
#if !defined(_OPENGL_ES_) && defined(_MACOSX_)
        internalPixelformat = pSurface->GetPixelFormat();
        if (internalPixelformat == GL_RGBA)
        {
          internalPixelformat = GL_RGBA;
          pixelformat = GL_BGRA;
          type = GL_UNSIGNED_INT_8_8_8_8_REV;
        }
        else if (internalPixelformat == GL_RGB)
        {
          internalPixelformat = GL_RGB;
          pixelformat = GL_BGR;
          type = GL_UNSIGNED_BYTE;
        }
        else
        {
          pixelformat = pSurface->GetPixelFormat();
          type = GL_UNSIGNED_BYTE;
        }
        glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
#else
        internalPixelformat = pSurface->GetPixelFormat();
        pixelformat = pSurface->GetPixelFormat();
        
//        internalPixelformat = GL_RGBA8;
//        pixelformat = GL_BGRA;
//        Width = 4 * Width;
//        Height = 4 * Height;
        type = GL_UNSIGNED_BYTE;
#endif
      }


#if (!defined _MACOSX_)
      if (!firstload)
      {
        glTexSubImage2D
        (
         target,
         0,
         0,0,
         (int)Width,
         (int)Height,
         pixelformat,
         type,
         pBuffer
         );
        nuiCheckForGLErrors();
        pTexture->ResetForceReload();
      }
      else
#endif
      {
#ifndef _OPENGL_ES_
        if (pTexture->GetAutoMipMap())
        {
#ifdef _MACOSX_
          glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
          nuiCheckForGLErrors();
#endif
          glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, (int)log2(Width));
          nuiCheckForGLErrors();
          gluBuild2DMipmaps(target, internalPixelformat, (int)Width, (int)Height, pixelformat, type, pBuffer);
          nuiCheckForGLErrors();
        }
        else
#endif
        {
          glTexImage2D(target, 0, internalPixelformat, (int)Width, (int)Height, 0, pixelformat, type, pBuffer);
          nuiCheckForGLErrors();
          pTexture->ResetForceReload();
          nuiCheckForGLErrors();
        }

#ifdef _UIKIT_
#ifdef NGL_DEBUG
        glLabelObjectEXT(GL_TEXTURE, info.mTexture, 0, pTexture->GetSource().GetChars());
#endif
#endif
      }

      info.mReload = false;

      if (allocated)
        free(pBuffer);

#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
      if (!pTexture->IsBufferRetained())
      {
        pTexture->ReleaseBuffer();
      }
#endif

    }
  }

  if (!mUseShaders)
  {
    if (pTexture->GetPixelFormat() == eImagePixelAlpha)
    {
      if (mTexEnvMode != GL_COMBINE)
      {
        mTexEnvMode = GL_COMBINE;
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mTexEnvMode);
      }
    }
    else
    {
      if (mTexEnvMode != pTexture->GetEnvMode())
      {
        mTexEnvMode = pTexture->GetEnvMode();
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mTexEnvMode);
      }
    }
  }
  else if (!mUseShaders && (mTexEnvMode != pTexture->GetEnvMode()))
  {
    mTexEnvMode = pTexture->GetEnvMode();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mTexEnvMode);
  }

  nuiCheckForGLErrors();

  if (changedctx)
  {
    mpContext->BeginSession();
    nuiCheckForGLErrors();
    if (mActiveTextureSlot != slot)
    {
      glActiveTexture(GL_TEXTURE0 + slot);
      mActiveTextureSlot = slot;
    }
    glBindTexture(target, info.mTexture);
    nuiCheckForGLErrors();
    if (mTexEnvMode != pTexture->GetEnvMode())
    {
      mTexEnvMode = pTexture->GetEnvMode();
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mTexEnvMode);
    }
    nuiCheckForGLErrors();
  }

  uint32 rectangle = GetRectangleTextureSupport();
  nuiCheckForGLErrors();

  double rx = 1;
  double ry = 1;
  if (rectangle != 1 || pTexture->GetTextureID())
  {
    rx = pTexture->GetUnscaledWidth() / Width;
    ry = pTexture->GetUnscaledHeight() / Height;
#ifndef _OPENGL_ES_
    if (target == GL_TEXTURE_RECTANGLE_ARB)
    {
      rx *= pTexture->GetUnscaledWidth();
      ry *= pTexture->GetUnscaledHeight();
    }
#endif
  }

  if (mUseShaders)
  {
//    if (pSurface)
//    {
//      mTextureTranslate = nglVector2f(0.0f, ry);
//      mTextureScale = nglVector2f(rx, -ry);
//    }
//    else
    {
      mTextureTranslate = nglVector2f(0.0f, 0.0f);
      mTextureScale = nglVector2f(rx, ry);
    }
  }
  else
  {
    glMatrixMode(GL_TEXTURE);
    nuiCheckForGLErrors();
    glLoadIdentity();
    nuiCheckForGLErrors();

    if (pSurface)
    {
      glTranslatef(0, ry, 0);
      ry = -ry;
    }

#ifndef _OPENGL_ES_
    glScaled(rx, ry, 1);
#else
    glScalef((float)rx, (float)ry, 1);
#endif
    nuiCheckForGLErrors();

    glMatrixMode(GL_MODELVIEW);
    nuiCheckForGLErrors();
  }

  nuiCheckForGLErrors();
}

void nuiGLPainter::DestroyTexture(nuiTexture* pTexture)
{
  nglCriticalSectionGuard g(mRenderingCS);
  mDestroyedTextures.push_back(pTexture);
}

void nuiGLPainter::FinalizeTextures()
{
  for (auto texture: mDestroyedTextures)
    _DestroyTexture(texture);
  mDestroyedTextures.clear();
}

void nuiGLPainter::_DestroyTexture(nuiTexture* pTexture)
{
  nglCriticalSectionGuard tg(mTexturesCS);
  std::map<nuiTexture*, TextureInfo>::iterator it = mTextures.find(pTexture);
  if (it == mTextures.end())
    return;
  NGL_ASSERT(it != mTextures.end());

  TextureInfo info(it->second);
  mTextures.erase(it);

  if (info.mTexture <= 0)
  {
    return;
  }
  //NGL_OUT("nuiGLPainter::DestroyTexture 0x%x : '%s' / %d\n", pTexture, pTexture->GetSource().GetChars(), info.mTexture);

  glDeleteTextures(1, (GLuint*)&info.mTexture);
}

nuiGLPainter::FramebufferInfo::FramebufferInfo()
{
  mReload = true;
  mFramebuffer = 0;
  mRenderbuffer = 0;
  mTexture = 0;
  mDepthbuffer = 0;
  mStencilbuffer = 0;
}

void nuiGLPainter::DestroySurface(nuiSurface* pSurface)
{
  nglCriticalSectionGuard g(mRenderingCS);
  mDestroyedSurfaces.push_back(pSurface);
}

void nuiGLPainter::FinalizeSurfaces()
{
  for (auto surface : mDestroyedSurfaces)
    _DestroySurface(surface);
  mDestroyedSurfaces.clear();
}

void nuiGLPainter::_DestroySurface(nuiSurface* pSurface)
{
  nglCriticalSectionGuard fbg(mFramebuffersCS);
  std::map<nuiSurface*, FramebufferInfo>::iterator it = mFramebuffers.find(pSurface);
  
  if (it == mFramebuffers.end())
  {
    return;
  }
  FramebufferInfo info = it->second;

  glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, 0);
  NGL_ASSERT(info.mFramebuffer > 0);
  glDeleteFramebuffersNUI(1, (GLuint*)&info.mFramebuffer);
  if (info.mRenderbuffer > 0)
    glDeleteRenderbuffersNUI(1, (GLuint*)&info.mRenderbuffer);
  if (info.mDepthbuffer > 0)
    glDeleteRenderbuffersNUI(1, (GLuint*)&info.mDepthbuffer);
  if (info.mStencilbuffer > 0)
    glDeleteRenderbuffersNUI(1, (GLuint*)&info.mStencilbuffer);

//  NGL_OUT("nuiGLPainter::DestroySurface %p (fbo %d tx %d)\n", pSurface, info.mFramebuffer, info.mTexture);
  mFramebuffers.erase(it);
}

#define NUI_ALLOCATE_TMP_SURFACE_TEXTURE 1

void nuiGLPainter::CreateSurface(nuiSurface* pSurface)
{
  //NGL_OUT("nuiGLPainter::CreateSurface %p\n", pSurface);
#if NUI_ALLOCATE_TMP_SURFACE_TEXTURE
  char* tmp = nullptr;
#endif

  NGL_ASSERT(pSurface != nullptr);
  NGL_ASSERT(this != nullptr);

  GLint DefaultFrameBuffer = 0;
  GLint DefaultRenderBuffer = 0;
  
  glGetIntegerv(GL_FRAMEBUFFER_BINDING_NUI, &DefaultFrameBuffer);
  nuiCheckForGLErrors();
  glGetIntegerv(GL_RENDERBUFFER_BINDING_NUI, (GLint *) &DefaultRenderBuffer);
  nuiCheckForGLErrors();

  nglCriticalSectionGuard fbg(mFramebuffersCS);
  std::map<nuiSurface*, FramebufferInfo>::const_iterator it = mFramebuffers.find(pSurface);
  bool create = (it == mFramebuffers.end()) ? true : false;
  if (!create)
    return;

  GLint width = (GLint)pSurface->GetWidth();
  GLint height = (GLint)pSurface->GetHeight();

  float scale = mpContext->GetScale();
  width *= scale;
  height *= scale;

  nuiTexture* pTexture = pSurface->GetTexture();

#if defined DEBUG && defined _UIKIT_
  if (pTexture)
    glPushGroupMarkerEXT(0, pTexture->GetSource().GetChars());
  else
    glPushGroupMarkerEXT(0, pSurface->GetObjectName().GetChars());
  nuiCheckForGLErrors();
#endif

  if (pTexture && !pTexture->IsPowerOfTwo())
  {
    switch (GetRectangleTextureSupport())
    {
      case 0:
        width  = MakePOT(width);
        height = MakePOT(height);
        break;
      case 1:
      case 2:
        break;
    }
  }

  FramebufferInfo info;

  if (pTexture && pSurface->GetRenderToTexture())
  {
    if (mActiveTextureSlot != 0)
    {
      glActiveTexture(GL_TEXTURE0);
      mActiveTextureSlot = 0;
    }
    mFinalState.mpTexture[0] = nullptr;
    
    nglCriticalSectionGuard tg(mTexturesCS);
    std::map<nuiTexture*, TextureInfo>::iterator it = mTextures.find(pTexture);
    if (it == mTextures.end())
      it = mTextures.insert(mTextures.begin(), std::pair<nuiTexture*, TextureInfo>(pTexture, TextureInfo()));
    NGL_ASSERT(it != mTextures.end());
    
    TextureInfo& tinfo(it->second);
    
    if (tinfo.mTexture > 0)
    {
      glDeleteTextures(1, (GLuint*)&tinfo.mTexture);
      nuiCheckForGLErrors();
      tinfo.mTexture = -1;
    }
    
    glGenTextures(1, (GLuint*)&tinfo.mTexture);
    info.mTexture = tinfo.mTexture;
    
    nuiCheckForGLErrors();
    glBindTexture(GL_TEXTURE_2D, tinfo.mTexture);
    nuiCheckForGLErrors();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    nuiCheckForGLErrors();
    
    NGL_ASSERT(width > 0);
    NGL_ASSERT(height > 0);

#if NUI_ALLOCATE_TMP_SURFACE_TEXTURE
    tmp = (char*)malloc(width * height * 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#endif
    nuiCheckForGLErrors();
    glBindTexture(GL_TEXTURE_2D, 0);
    nuiCheckForGLErrors();
    
//    NGL_OUT("init color surface texture %d x %d --> %d\n", width, height, tinfo.mTexture);
    
  }


  glGenFramebuffersNUI(1, (GLuint*)&info.mFramebuffer);
//  NGL_OUT("glGenFramebuffersNUI %p '%s' -> %d (%d x %d)\n", pSurface, pSurface->GetObjectName().GetChars(), info.mFramebuffer, pSurface->GetWidth(), pSurface->GetHeight());
  NGL_ASSERT(info.mFramebuffer);
  nuiCheckForGLErrors();

#ifdef _UIKIT_
#ifdef NGL_DEBUG
//      if (pTexture)
//        glLabelObjectEXT(GL_BUFFER_OBJECT_EXT, info.mFramebuffer, 0, pTexture->GetSource().GetChars());
//      else
//        glLabelObjectEXT(GL_BUFFER_OBJECT_EXT, info.mFramebuffer, 0, pSurface->GetObjectName().GetChars());
//      nuiCheckForGLErrors();
#endif
#endif

  glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, info.mFramebuffer);
  nuiCheckForGLErrors();
//  glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, 0);
//  nuiCheckForGLErrors();

  ///< We definetly need a color attachement, either a texture, or a renderbuffer
  if (pTexture && pSurface->GetRenderToTexture())
  {
    glFramebufferTexture2DNUI(GL_FRAMEBUFFER_NUI, GL_COLOR_ATTACHMENT0_NUI, GL_TEXTURE_2D, info.mTexture, 0);
    nuiCheckForGLErrors();
    
//    NGL_OUT("init color surface texture %d x %d --> %d\n", width, height, info.mTexture);
  }
  else
  {
    glGenRenderbuffersNUI(1, (GLuint*)&info.mRenderbuffer);
    nuiCheckForGLErrors();
    
    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, info.mRenderbuffer);
    nuiCheckForGLErrors();
    
    GLint pixelformat = pSurface->GetPixelFormat();
    GLint internalPixelformat = pSurface->GetPixelFormat();
    internalPixelformat = GL_RGBA;
#if !defined(_OPENGL_ES_) && defined(_MACOSX_)
    if (internalPixelformat == GL_RGBA)
    {
      pixelformat = GL_BGRA;
    }
    else if (internalPixelformat == GL_RGB)
    {
      pixelformat = GL_BGR;
    }
#else
#ifdef _UIKIT_
    pixelformat = GL_RGBA8_OES;
#endif
#endif
    
    glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI, pixelformat, width, height);
    nuiCheckForGLErrors();
    
    GLint w, h;
    glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_WIDTH_NUI, &w);
    glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_HEIGHT_NUI, &h);
    
    NGL_ASSERT(w == width);
    NGL_ASSERT(h == height);
    
    NGL_OUT("init color surface %d x %d (asked %d x %d)\n", w, h, width, height);
    
    
    glFramebufferRenderbufferNUI(GL_FRAMEBUFFER_NUI,
                                 GL_COLOR_ATTACHMENT0_NUI,
                                 GL_RENDERBUFFER_NUI,
                                 info.mRenderbuffer);
    //NGL_OUT("surface render buffer -> %d\n", info.mRenderbuffer);
    nuiCheckForGLErrors();
  }
  

  ///< Do we need a depth buffer
  if (pSurface->GetDepth())
  {
    glGenRenderbuffersNUI(1, (GLuint*)&info.mDepthbuffer);
    nuiCheckForGLErrors();
    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, info.mDepthbuffer);
    nuiCheckForGLErrors();

    
#ifdef _OPENGL_ES_
    if (pSurface->GetStencil())
    {
      glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI,
                               GL_DEPTH24_STENCIL8_OES,
                               width, height);
      nuiCheckForGLErrors();

      GLint w, h;
      glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_WIDTH_NUI, &w);
      glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_HEIGHT_NUI, &h);

      NGL_ASSERT(w == width);
      NGL_ASSERT(h == height);

      NGL_OUT("init depth surface %d x %d (asked %d x %d)\n", w, h, width, height);
    }
    else
#endif
    {
      int32 depth = pSurface->GetDepth();
#ifndef _OPENGL_ES_
      if (depth <= 16)
        glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI, GL_DEPTH_COMPONENT16, width, height);
      else if (depth <= 24)
        glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI, GL_DEPTH_COMPONENT24, width, height);
      else
        glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI, GL_DEPTH_COMPONENT32, width, height);
#else
      if (depth <= 16)
        glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI, GL_DEPTH_COMPONENT16, width, height);
      else
        glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI, GL_DEPTH_COMPONENT24, width, height);
#endif
      nuiCheckForGLErrors();

      GLint w, h;
      glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_WIDTH_NUI, &w);
      glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_HEIGHT_NUI, &h);

      NGL_ASSERT(w == width);
      NGL_ASSERT(h == height);

      NGL_OUT("init depth surface %d x %d (asked %d x %d)\n", w, h, width, height);
      
    }

    nuiCheckForGLErrors();

    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, 0);
    nuiCheckForGLErrors();

    glFramebufferRenderbufferNUI(GL_FRAMEBUFFER_NUI,
                                 GL_DEPTH_ATTACHMENT_NUI,
                                 GL_RENDERBUFFER_NUI,
                                 info.mDepthbuffer);
    nuiCheckForGLErrors();

#ifdef _OPENGL_ES_
    if (pSurface->GetStencil())
    {
      glFramebufferRenderbufferNUI(GL_FRAMEBUFFER_NUI,
                                   GL_STENCIL_ATTACHMENT_NUI,
                                   GL_RENDERBUFFER_NUI,
                                   info.mDepthbuffer);
    }
#endif
    nuiCheckForGLErrors();
  }

  ///< Do we need a stencil buffer
#ifndef _OPENGL_ES_
  if (pSurface->GetStencil())
  {
    NGL_ASSERT(!"Stencil attachement not supported");
    glGenRenderbuffersNUI(1, (GLuint*)&info.mStencilbuffer);
    nuiCheckForGLErrors();

    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, info.mStencilbuffer);
    nuiCheckForGLErrors();

    glRenderbufferStorageNUI(GL_RENDERBUFFER_NUI,
                             GL_STENCIL_INDEX,
                             width, height);
    nuiCheckForGLErrors();
    GLint w, h;
    glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_WIDTH_NUI, &w);
    glGetRenderbufferParameterivNUI(GL_RENDERBUFFER_NUI, GL_RENDERBUFFER_HEIGHT_NUI, &h);

    NGL_ASSERT(w == width);
    NGL_ASSERT(h == height);

    NGL_OUT("init stencil surface %d x %d (asked %d x %d)\n", w, h, width, height);

    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, 0);
    nuiCheckForGLErrors();

    glFramebufferRenderbufferNUI(GL_FRAMEBUFFER_NUI,
                                 GL_STENCIL_ATTACHMENT_NUI,
                                 GL_RENDERBUFFER_NUI,
                                 info.mStencilbuffer);
    nuiCheckForGLErrors();
  }
#endif

#ifdef NGL_DEBUG
  CheckFramebufferStatus();
#endif
  nuiCheckForGLErrors();
  {
    nglCriticalSectionGuard fbg(mFramebuffersCS);
    mFramebuffers[pSurface] = info;
  }

  glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, DefaultFrameBuffer);
  nuiCheckForGLErrors();
  //NGL_OUT("glBindFramebufferNUI -> %d\n", DefaultFrameBuffer);
  if (DefaultRenderBuffer >= 0)
  {
    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, DefaultRenderBuffer);
    nuiCheckForGLErrors();
    //NGL_OUT("glBindRenderbufferNUI -> %d\n", DefaultRenderBuffer);
  }

  nuiCheckForGLErrors();
#ifdef NGL_DEBUG
  CheckFramebufferStatus();
#endif

#if NUI_ALLOCATE_TMP_SURFACE_TEXTURE
  if (tmp)
    free(tmp);
#endif
}

void nuiGLPainter::SetSurface(nuiSurface* pSurface)
{
  //NGL_OUT("nuiGLPainter::SetSurface %p\n", pSurface);
  if (!mpSurface && pSurface)
  {
    //#ifdef _OPENGL_ES_
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_NUI, &mDefaultFramebuffer);
    nuiCheckForGLErrors();
    glGetIntegerv(GL_RENDERBUFFER_BINDING_NUI, (GLint *) &mDefaultRenderbuffer);
    nuiCheckForGLErrors();
    //#endif
    
  }
  
  if (pSurface)
    pSurface->Acquire();
  
  if (mpSurface)
  {
    mpSurface->Release();
#if defined DEBUG && defined _UIKIT_
    glPopGroupMarkerEXT();
#endif
  }
  mpSurface = pSurface;
  
  if (pSurface)
  {
    nglCriticalSectionGuard fbg(mFramebuffersCS);
    std::map<nuiSurface*, FramebufferInfo>::const_iterator it = mFramebuffers.find(pSurface);
    bool create = (it == mFramebuffers.end()) ? true : false;
    
    GLint width = (GLint)pSurface->GetWidth();
    GLint height = (GLint)pSurface->GetHeight();
    
    float scale = mpContext->GetScale();
    width *= scale;
    height *= scale;
    
    nuiTexture* pTexture = pSurface->GetTexture();
    
#if defined DEBUG && defined _UIKIT_
    if (pTexture)
      glPushGroupMarkerEXT(0, pTexture->GetSource().GetChars());
    else
      glPushGroupMarkerEXT(0, pSurface->GetObjectName().GetChars());
    nuiCheckForGLErrors();
#endif
    
    if (pTexture && !pTexture->IsPowerOfTwo())
    {
      switch (GetRectangleTextureSupport())
      {
        case 0:
          width  = MakePOT(width);
          height = MakePOT(height);
          break;
        case 1:
        case 2:
          break;
      }
    }
    
    
    
    if (create)
    {
      CreateSurface(pSurface);
      it = mFramebuffers.find(pSurface);
    }

    const FramebufferInfo& info(it->second);
    glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, info.mFramebuffer);
    nuiCheckForGLErrors();
    //NGL_OUT("glBindFramebufferNUI -> %d\n", info.mFramebuffer);
//    if (info.mRenderbuffer >= 0)
//    {
//      glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, info.mRenderbuffer);
//      nuiCheckForGLErrors();
//      //NGL_OUT("glBindRenderbufferNUI -> %d\n", info.mRenderbuffer);
//    }
    
    nuiCheckForGLErrors();
#ifdef NGL_DEBUG
    CheckFramebufferStatus();
#endif
  }
  else
  {
    /// !pSurface
    glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, mDefaultFramebuffer);
    nuiCheckForGLErrors();
    //    NGL_OUT("UNBIND glBindFramebufferNUI -> %d\n", mDefaultFramebuffer);
//    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, mDefaultRenderbuffer);
//    //    NGL_OUT("UNBIND glBindRenderbufferNUI -> %d\n", mDefaultRenderbuffer);
//    nuiCheckForGLErrors();
    
#if defined _UIKIT_
    //    glResolveMultisampleFramebufferAPPLE();
#endif
    
    nuiCheckForGLErrors();
#ifdef NGL_DEBUG
    CheckFramebufferStatus();
#endif
  }
  
  ResetOpenGLState();
}

nglCriticalSection nuiGLPainter::RenderArrayInfo::mHeapCS;
std::list<nuiGLPainter::RenderArrayInfo*> nuiGLPainter::RenderArrayInfo::mHeap;

nuiGLPainter::RenderArrayInfo* nuiGLPainter::RenderArrayInfo::Create(nuiRenderArray* pRenderArray)
{
  nglCriticalSectionGuard g(mHeapCS);
  if (!mHeap.empty())
  {
    RenderArrayInfo* pInfo = mHeap.front();
    mHeap.pop_front();
    pInfo->Rebind(pRenderArray);
    return pInfo;
  }
  else
  {
    return new RenderArrayInfo(pRenderArray);
  }
}

void nuiGLPainter::RenderArrayInfo::Recycle(nuiGLPainter::RenderArrayInfo* pInfo)
{
  pInfo->Destroy();
}


nuiGLPainter::RenderArrayInfo::RenderArrayInfo(nuiRenderArray* pRenderArray)
{
  mpRenderArray = NULL;
  mVertexBuffer = -1;

  if (pRenderArray)
    Rebind(pRenderArray);
}

nuiGLPainter::RenderArrayInfo::~RenderArrayInfo()
{
  Destroy();
}


void nuiGLPainter::RenderArrayInfo::Rebind(nuiRenderArray* pRenderArray)
{
  mpRenderArray = pRenderArray;

  int32 count = pRenderArray->GetSize();
  NGL_ASSERT(mVertexBuffer == -1);
  glGenBuffers(1, &mVertexBuffer);
  nuiCheckForGLErrors();
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  nuiCheckForGLErrors();
  glBufferData(GL_ARRAY_BUFFER, sizeof(nuiRenderArray::Vertex) * count, &pRenderArray->GetVertices()[0], GL_STATIC_DRAW);
  nuiCheckForGLErrors();

  if (pRenderArray->GetIndexArrayCount() > 0)
  {
    uint32 indexcount = pRenderArray->GetIndexArrayCount();
    mIndexBuffers.resize(indexcount);

    for (uint32 i = 0; i < indexcount; i++)
    {
      glGenBuffers(1, &mIndexBuffers[i]);
      nuiCheckForGLErrors();
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[i]);
      nuiCheckForGLErrors();
      const nuiRenderArray::IndexArray& indices(pRenderArray->GetIndexArray(i));
  #ifdef _UIKIT_
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.mIndices.size() * sizeof(GLushort), &indices.mIndices[0], GL_STATIC_DRAW);
  #else
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.mIndices.size() * sizeof(GLuint), &indices.mIndices[0], GL_STATIC_DRAW);
  #endif
      nuiCheckForGLErrors();
    }
  }

  int streamcount = pRenderArray->GetStreamCount();
  mStreamBuffers.resize(streamcount);
  for (int i = 0; i < streamcount; i++)
  {
    const nuiRenderArray::StreamDesc& stream(pRenderArray->GetStream(i));

    glGenBuffers(1, &mStreamBuffers[i]);
    nuiCheckForGLErrors();
    glBindBuffer(GL_ARRAY_BUFFER, mStreamBuffers[i]);
    nuiCheckForGLErrors();
    int32 s = 1;
    switch (stream.mType)
    {
      case nuiRenderArray::eFloat: s = 4; break;
      case nuiRenderArray::eInt: s = 4; break;
      case nuiRenderArray::eByte: s = 1; break;
    }
    glBufferData(GL_ARRAY_BUFFER, count * stream.mCount * s, stream.mData.mpFloats, GL_STATIC_DRAW);
    nuiCheckForGLErrors();
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void nuiGLPainter::RenderArrayInfo::Destroy()
{
  if ((GLint)mVertexBuffer == -1)
    return;

  for (auto vao : mVAOs)
  {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, (GLuint*)&vao.second);
    nuiCheckForGLErrors();
  }
  mVAOs.clear();

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &mVertexBuffer);
  nuiCheckForGLErrors();
  mVertexBuffer = -1;

  for (uint32 i = 0; i < mIndexBuffers.size(); i++)
  {
    glDeleteBuffers(1, &mIndexBuffers[i]);
    nuiCheckForGLErrors();
  }
  mIndexBuffers.clear();

  for (uint32 i = 0; i < mStreamBuffers.size(); i++)
  {
    glDeleteBuffers(1, &mStreamBuffers[i]);
    nuiCheckForGLErrors();
  }

  mStreamBuffers.clear();
  {
    nglCriticalSectionGuard g(mHeapCS);
    mHeap.push_back(this);
  }
}

void nuiGLPainter::RenderArrayInfo::BindVertices() const
{
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  nuiCheckForGLErrors();
}

void nuiGLPainter::RenderArrayInfo::BindStream(int index) const
{
  glBindBuffer(GL_ARRAY_BUFFER, mStreamBuffers[index]);
  nuiCheckForGLErrors();
}

void nuiGLPainter::RenderArrayInfo::BindIndices(int index) const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[index]);
  nuiCheckForGLErrors();
}

void nuiGLPainter::RenderArrayInfo::Draw() const
{
  BindVertices();

  if (mpRenderArray->GetIndexArrayCount() > 0)
  {
    for (uint32 i = 0; i < mIndexBuffers.size(); i++)
    {
      const nuiRenderArray::IndexArray& array(mpRenderArray->GetIndexArray(i));
      BindIndices(i);
      
#ifdef _UIKIT_
      glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_SHORT, (void*)0);
#else
      glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_INT, (void*)0);
#endif
      nuiCheckForGLErrors();
    }
  }
  else
  {

  }
}


bool nuiCheckForGLErrorsReal()
{
  GLenum err = GL_NO_ERROR;
#if 1 // Globally enable/disable OpenGL error checking
  //#ifdef NGL_DEBUG
  err = glGetError();
  if (err == GL_NO_ERROR)
    return true;

  App->GetLog().SetLevel("nuiGLPainter", 1000);
#if TARGET_OS_IPHONE
  switch (err)
  {
    case 0:
      break;
      
    case GL_INVALID_ENUM:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "An unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag.");
      break;
    case GL_INVALID_VALUE:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "A numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag.");
      break;
    case GL_INVALID_OPERATION:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "The specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag.");
      break;
    case GL_STACK_OVERFLOW:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "This function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag.");
      break;
    case GL_STACK_UNDERFLOW:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "This function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag.");
      break;
    case GL_OUT_OF_MEMORY:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "There is not enough memory left to execute the function. The state of OpenGL is undefined, except for the state of the error flags, after this error is recorded.");
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "Attempt to read from or write/render to a framebuffer that is not complete.");
      break;
#ifdef GL_CONTEXT_LOST
    case GL_CONTEXT_LOST:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "OpenGL context has been lost, due to a graphics card reset.");
      break;
#endif
          
    default:
      NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "Unkown error %d 0x%x.", err, err);
      break;
  }
#else
  if (err)
  {
    NGL_LOG("nuiGLPainter", NGL_LOG_ERROR, "OpenGL error %d/0x%x %s", err, err, (char *)gluErrorString(err));
  }
  
#endif
  return false;
#endif

  return true;
}

void nuiGLPainter::LoadMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::LoadMatrix(rMatrix);
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGLPainter::MultMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::MultMatrix(rMatrix);
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGLPainter::PushMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PushMatrix();
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGLPainter::PopMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PopMatrix();
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGLPainter::LoadProjectionMatrix(const nuiRect& rViewport, const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::LoadProjectionMatrix(rViewport, rMatrix);
  
  SetViewport();
}

void nuiGLPainter::MultProjectionMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::MultProjectionMatrix(rMatrix);
  
  SetViewport();
}

void nuiGLPainter::PushProjectionMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PushProjectionMatrix();
}

void nuiGLPainter::PopProjectionMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PopProjectionMatrix();
  SetViewport();
}



void nuiGLPainter::SetVertexPointers(const nuiRenderArray& rArray)
{
  nuiShaderProgram* pPgm = mFinalState.mpShader;
  GLint Position = pPgm->GetVAPositionLocation();
  GLint TexCoord = pPgm->GetVATexCoordLocation();
  GLint Color = pPgm->GetVAColorLocation();
  GLint Normal = pPgm->GetVANormalLocation();
  
  if (Position != -1)
  {
    glEnableVertexAttribArray(Position);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(nuiRenderArray::Vertex), &rArray.GetVertices()[0].mX);
  }
  else
  {
    //glDisableVertexAttribArray(Position);
  }
  
  if (TexCoord != -1)
  {
    glEnableVertexAttribArray(TexCoord);
    glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(nuiRenderArray::Vertex), &rArray.GetVertices()[0].mTX);
  }
  else
  {
    //glDisableVertexAttribArray(TexCoord);
  }
  
  if (Color != -1)
  {
    glEnableVertexAttribArray(Color);
    glVertexAttribPointer(Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(nuiRenderArray::Vertex), &rArray.GetVertices()[0].mR);
  }
  else
  {
    //glDisableVertexAttribArray(Color);
  }
  
  if (Normal != -1)
  {
    glEnableVertexAttribArray(Normal);
    glVertexAttribPointer(Normal, 3, GL_FLOAT, GL_FALSE, sizeof(nuiRenderArray::Vertex), &rArray.GetVertices()[0].mNX);
  }
  else
  {
    //glDisableVertexAttribArray(Normal);
  }
  
  int stream_count = rArray.GetStreamCount();
  for (int i = 0; i < stream_count; i++)
  {
    const nuiRenderArray::StreamDesc& rDesc(rArray.GetStream(i));
    glEnableVertexAttribArray(rDesc.mStreamID);
    glVertexAttribPointer(rDesc.mStreamID, rDesc.mCount, rDesc.mType, rDesc.mNormalize ? GL_TRUE : GL_FALSE, 0, rDesc.mData.mpFloats);
  }
}

void nuiGLPainter::SetVertexBuffersPointers(const nuiRenderArray& rArray, RenderArrayInfo& rInfo)
{
  static int64 created = 0;
  static int64 bound = 0;
  {
    nglCriticalSectionGuard g(gStats);
    total++;
  }
  nuiShaderProgram* pPgm = mFinalState.mpShader;
  
  // Look for VAO:
  auto it = rInfo.mVAOs.find(pPgm);
  if (it == rInfo.mVAOs.end())
  {
    created++;
    
    // Create this VAO:
    GLint vao = 0;
    glGenVertexArrays(1, (GLuint*)&vao);
    nuiCheckForGLErrors();
    rInfo.mVAOs[pPgm] = vao;
    
    glBindVertexArray(vao);
    nuiCheckForGLErrors();
    
    GLint Position = pPgm->GetVAPositionLocation();
    GLint TexCoord = pPgm->GetVATexCoordLocation();
    GLint Color = pPgm->GetVAColorLocation();
    GLint Normal = pPgm->GetVANormalLocation();
    
    rInfo.BindVertices();
    if (Position != -1)
    {
      glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(nuiRenderArray::Vertex), (void*)offsetof(nuiRenderArray::Vertex, mX));
      nuiCheckForGLErrors();
      glEnableVertexAttribArray(Position);
      nuiCheckForGLErrors();
    }
    else
    {
      //glDisableVertexAttribArray(Position);
    }
    
    if (TexCoord != -1)
    {
      glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(nuiRenderArray::Vertex), (void*)offsetof(nuiRenderArray::Vertex, mTX));
      nuiCheckForGLErrors();
      glEnableVertexAttribArray(TexCoord);
      nuiCheckForGLErrors();
    }
    else
    {
      //glDisableVertexAttribArray(TexCoord);
    }
    
    if (Color != -1)
    {
      glVertexAttribPointer(Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(nuiRenderArray::Vertex), (void*)offsetof(nuiRenderArray::Vertex, mR));
      nuiCheckForGLErrors();
      glEnableVertexAttribArray(Color);
      nuiCheckForGLErrors();
    }
    else
    {
      //glDisableVertexAttribArray(Color);
    }
    
    if (Normal != -1)
    {
      glVertexAttribPointer(Normal, 3, GL_FLOAT, GL_FALSE, sizeof(nuiRenderArray::Vertex), (void*)offsetof(nuiRenderArray::Vertex, mNX));
      nuiCheckForGLErrors();
      glEnableVertexAttribArray(Normal);
      nuiCheckForGLErrors();
    }
    else
    {
      //glDisableVertexAttribArray(Normal);
    }
    
    for (int i = 0; i < rArray.GetStreamCount(); i++)
    {
      SetStreamBuffersPointers(rArray, rInfo, i);
    }
    
    return;
  }
  
  bound++;
  
  if (bound % 10000 == 0)
  {
    NGL_DEBUG(NGL_OUT("VAO/VBO %lld created %lld bound (%f cache hit)\n", created, bound, (float)bound / (float)created););
  }
  
  NGL_ASSERT(it != rInfo.mVAOs.end());
  
  GLint vao = it->second;
  glBindVertexArray(vao);
  nuiCheckForGLErrors();
  
}

void nuiGLPainter::SetStreamBuffersPointers(const nuiRenderArray& rArray, const RenderArrayInfo& rInfo, int index)
{
  rInfo.BindStream(index);
  const nuiRenderArray::StreamDesc& rDesc(rArray.GetStream(index));
  glVertexAttribPointer(rDesc.mStreamID, rDesc.mCount, rDesc.mType, rDesc.mNormalize ? GL_TRUE : GL_FALSE, 0, NULL);
  nuiCheckForGLErrors();
  glEnableVertexAttribArray(rDesc.mStreamID);
  nuiCheckForGLErrors();
}


void nuiGLPainter::ResetVertexPointers(const nuiRenderArray& rArray)
{
  nuiShaderProgram* pPgm = mFinalState.mpShader;
  GLint Position = pPgm->GetVAPositionLocation();
  GLint TexCoord = pPgm->GetVATexCoordLocation();
  GLint Color = pPgm->GetVAColorLocation();
  GLint Normal = pPgm->GetVANormalLocation();
  
  if (Position != -1)
  {
    glDisableVertexAttribArray(Position);
    nuiCheckForGLErrors();
  }
  
  if (TexCoord != -1)
  {
    glDisableVertexAttribArray(TexCoord);
    nuiCheckForGLErrors();
  }
  
  if (Color != -1)
  {
    glDisableVertexAttribArray(Color);
    nuiCheckForGLErrors();
  }
  
  if (Normal != -1)
  {
    glDisableVertexAttribArray(Normal);
    nuiCheckForGLErrors();
  }
  
  int stream_count = rArray.GetStreamCount();
  for (int i = 0; i < stream_count; i++)
  {
    const nuiRenderArray::StreamDesc& rDesc(rArray.GetStream(i));
    glDisableVertexAttribArray(rDesc.mStreamID);
    nuiCheckForGLErrors();
  }
}



#ifdef glDeleteBuffersARB
#undef glDeleteBuffersARB
#endif

void nuiGLPainter::DestroyRenderArray(nuiRenderArray* pArray)
{
  nglCriticalSectionGuard g(mRenderingCS);
  mDestroyedRenderArrays.push_back(pArray);
}

void nuiGLPainter::FinalizeRenderArrays()
{
  nglCriticalSectionGuard g(mRenderingCS);
  for (auto array : mDestroyedRenderArrays)
    _DestroyRenderArray(array);
  mDestroyedRenderArrays.clear();
}

void nuiGLPainter::_DestroyRenderArray(nuiRenderArray* pArray)
{
  nglCriticalSectionGuard rag(mRenderArraysCS);
  auto it = mRenderArrays.find(pArray);
  if (it == mRenderArrays.end())
    return; // This render array was not stored here
  
  RenderArrayInfo* info(it->second);
  
  RenderArrayInfo::Recycle(info);
  
  mRenderArrays.erase(it);
}



std::map<nuiTexture*, nuiGLPainter::TextureInfo> nuiGLPainter::mTextures;
std::map<nuiSurface*, nuiGLPainter::FramebufferInfo> nuiGLPainter::mFramebuffers;
std::vector<nuiRenderArray*> nuiGLPainter::mFrameArrays;
std::map<nuiRenderArray*, nuiGLPainter::RenderArrayInfo*> nuiGLPainter::mRenderArrays;
nglCriticalSection nuiGLPainter::mTexturesCS("mTexturesCS");
nglCriticalSection nuiGLPainter::mFramebuffersCS("mFramebuffersCS");
nglCriticalSection nuiGLPainter::mFrameArraysCS("mFrameArraysCS");
nglCriticalSection nuiGLPainter::mRenderArraysCS("mRenderArraysCS");


#endif //   #ifndef __NUI_NO_GL__
