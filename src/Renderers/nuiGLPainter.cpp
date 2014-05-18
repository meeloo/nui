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

static uint32 mins = 30000;
static uint32 maxs = 0;
static uint32 totalinframe = 0;
static uint32 total = 0;

nuiGLPainter::nuiGLPainter(nglContext* pContext)
: nuiPainter(pContext)
{
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
    //NGL_OUT(_T("Extensions: %s\n"), exts.GetChars());


    mpContext->CheckExtension(_T("GL_VERSION_1_2"));
    nuiCheckForGLErrors();
    mpContext->CheckExtension(_T("GL_VERSION_1_3"));
    nuiCheckForGLErrors();
    mpContext->CheckExtension(_T("GL_VERSION_1_4"));
    nuiCheckForGLErrors();
    mpContext->CheckExtension(_T("GL_VERSION_1_5"));
    nuiCheckForGLErrors();
    mpContext->CheckExtension(_T("GL_VERSION_2_0"));
    nuiCheckForGLErrors();
    mpContext->CheckExtension(_T("GL_ARB_vertex_buffer_object"));
    nuiCheckForGLErrors();
    mpContext->CheckExtension(_T("GL_EXT_framebuffer_object"));
    nuiCheckForGLErrors();

    mpContext->CheckExtension(_T("GL_ARB_framebuffer_object"));
    nuiCheckForGLErrors();

    if (mpContext->CheckExtension("GL_ARB_texture_non_power_of_two")  || mpContext->CheckExtension("GL_APPLE_texture_2D_limited_npot"))
    {
      mCanRectangleTexture = 1;
    }
    else if (mpContext->CheckExtension(_T("GL_EXT_texture_rectangle")) || mpContext->CheckExtension(_T("GL_ARB_texture_rectangle")) || mpContext->CheckExtension(_T("GL_NV_texture_rectangle")))
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
}

nuiGLPainter::~nuiGLPainter()
{
  mActiveContexts--;
  if (mActiveContexts == 0)
    glAAExit();
  for (auto pArray : mFrameArrays)
    pArray->Release();
  mFrameArrays.clear();
}

void nuiGLPainter::SetViewport()
{
  GLuint Angle = GetAngle();
  GLuint Width = GetCurrentWidth();
  GLuint Height = GetCurrentHeight();
  const nuiRect& rViewport(mProjectionViewportStack.top());
  const nuiMatrix& rMatrix = mProjectionMatrixStack.top();

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

  const float scale = mpContext->GetScale();

  //NGL_OUT("set projection matrix (%d %d - %d %d)\n", x, y, w, h);
  //if (!mpSurface)
  {
    x *= scale;
    y *= scale;
    w *= scale;
    h *= scale;
  }

//  if (mViewPort[0] != x || mViewPort[1] != y || mViewPort[2] != w || mViewPort[3] != h)
  nuiCheckForGLErrors();
  glViewport(x, y, w, h);

  nuiCheckForGLErrors();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glScalef(scale, scale, 1.0f);
//  if (Angle != 0.0f)
//  {
//    glRotatef(Angle, 0.f,0.f,1.f);
//    glMultMatrixf(rMatrix.Array);
//  }
//  else
  {
    glLoadMatrixf(rMatrix.Array);
  }

  nuiCheckForGLErrors();

  glMatrixMode (GL_MODELVIEW);
  nuiCheckForGLErrors();
}

void nuiGLPainter::StartRendering()
{
  nuiPainter::StartRendering();
  for (auto pArray : mFrameArrays)
    pArray->Release();
  mFrameArrays.clear();

  ResetOpenGLState();
}

void nuiGLPainter::ResetOpenGLState()
{
  BeginSession();
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGLPainter::ResetOpenGLState()");
#endif

  //NUI_RETURN_IF_RENDERING_DISABLED;
  nuiCheckForGLErrors();

  mScissorX = -1;
  mScissorY = -1;
  mScissorW = -1;
  mScissorH = -1;
  mScissorOn = false;

  SetViewport();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_TEXTURE_2D);

  mTextureTarget = 0;
#ifndef _OPENGL_ES_
  if (mCanRectangleTexture == 2)
  {
    glDisable(GL_TEXTURE_RECTANGLE_ARB);
  }
#endif

  glDisable(GL_STENCIL_TEST);
  glDisable(GL_BLEND);
#ifndef _OPENGL_ES_
  glDisable(GL_ALPHA_TEST);
#endif
  glDisable(GL_CULL_FACE);

  glEnable(GL_MULTISAMPLE);

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  BlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);

  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_ALPHA);
 	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);


  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  mClientVertex = false;
  mClientColor = false;
  mClientTexCoord = false;
  mMatrixChanged = true;
  mR = -1;
  mG = -1;
  mB = -1;
  mA = -1;
  mTexEnvMode = 0;

  mFinalState = nuiRenderState();
  mpState = &mDefaultState;
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

    //NGL_OUT(_T("Change texture to 0x%x (%s)\n"), pTexture, pTexture?pTexture->GetSource().GetChars() : nglString::Empty.GetChars());
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

    //NGL_OUT(_T("Change texture type from 0x%x to 0x%x\n"), outtarget, intarget);

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


#define LOGENUM(XXX) case XXX: { NGL_OUT(_T("%s\n"), #XXX); } break;

void nuiGLPainter::DrawArray(nuiRenderArray* pArray)
{
  mRenderOperations++;
  mBatches++;

  //glDisable(GL_TEXTURE_2D);
  //glEnable(GL_TEXTURE_2D);
  //glEnable(GL_TEXTURE_2D);

  if (!mEnableDrawArray)
  {
    pArray->Release();
    return;
  }

#ifdef DEBUG
  nuiGLDebugGuard guard("nuiGLPainter::DrawArray");
#endif

  static uint32 ops = 0;
  static uint32 skipped_ops = 0;

  ops++;
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
      skipped_ops++;

      //      #ifdef _DEBUG
      //      if (!(skipped_ops % 100))
      //        NGL_OUT("optim (%d / %d) - %2.2f%%\n", skipped_ops, ops, (float)skipped_ops * 100.0f / (float)ops);
      //      #endif

      return;
    }
  }
  //  else
  //  {
  //    nglVectorf v1(bounds[0], bounds[1], 0); // topleft(x, y)
  //    nglVectorf v2(bounds[3], bounds[4], 0); // bottomright(x, y)
  //    v1 = rM * v1;
  //    v2 = rM * v2;
  //
  //    if (
  //        (v1[0] > mClip.Right()) ||
  //        (v1[1] > mClip.Bottom()) ||
  //        (v2[0] < mClip.Left()) ||
  //        (v2[1] < mClip.Top())
  //       )
  //    {
  //      return;
  //    }
  //  }

  pArray->Acquire();
  mFrameArrays.push_back(pArray);

  uint32 s = pArray->GetSize();

  total += s;
  totalinframe += s;
  mins = MIN(mins, s);
  maxs = MAX(maxs, s);
  //NGL_OUT("DA (%d) min = %d  max = %d\n", s, mins, maxs);

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

  ApplyState(*mpState, mForceApply);

  mVertices += s;
  GLenum mode = pArray->GetMode();
  //NGL_OUT(_T("GL Array Mode: %d   vertice count %d\n"), mode, size);

  /*
   switch (pArray->GetMode())
   {
   LOGENUM(GL_POINTS);
   LOGENUM(GL_LINES);
   LOGENUM(GL_LINE_LOOP);
   LOGENUM(GL_LINE_STRIP);
   //LOGENUM(GL_TRIANGLES);
   LOGENUM(GL_TRIANGLE_STRIP);
   LOGENUM(GL_TRIANGLE_FAN);
   //LOGENUM(GL_QUADS);
   LOGENUM(GL_QUAD_STRIP);
   LOGENUM(GL_POLYGON);
   }
   */


  NUI_RETURN_IF_RENDERING_DISABLED;

  if (mMatrixChanged)
  {
    nuiGLLoadMatrix(mMatrixStack.top().Array);
    mMatrixChanged = false;
  }


  bool NeedTranslateHack = pArray->IsShape() || ((mode == GL_POINTS || mode == GL_LINES || mode == GL_LINE_LOOP || mode == GL_LINE_STRIP) && !pArray->Is3DMesh());
  float hackX = 0;
  float hackY = 0;
  if (NeedTranslateHack)
  {
    //    const float ratio=0.5f;
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
    glTranslatef(hackX, hackY, 0);
  }



//  if (pArray->IsArrayEnabled(nuiRenderArray::eVertex))
//  {
//    if (!mClientVertex)
      glEnableClientState(GL_VERTEX_ARRAY);
    mClientVertex = true;
    glVertexPointer(3, GL_FLOAT, sizeof(nuiRenderArray::Vertex), &pArray->GetVertices()[0].mX);
    nuiCheckForGLErrors();
//  }
//  else
//  {
//    if (mClientVertex)
//      glDisableClientState(GL_VERTEX_ARRAY);
//    mClientVertex = false;
//  }

  float r = mR, g = mG, b = mB, a = mA;
  if (pArray->IsArrayEnabled(nuiRenderArray::eColor))
  {
    if (!mClientColor)
      glEnableClientState(GL_COLOR_ARRAY);
    mClientColor = true;
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(nuiRenderArray::Vertex), &pArray->GetVertices()[0].mR);
    nuiCheckForGLErrors();
  }
  else
  {
    if (mClientColor)
      glDisableClientState(GL_COLOR_ARRAY);
    mClientColor = false;

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
#ifndef _OPENGL_ES_
      case GL_QUADS:
      case GL_QUAD_STRIP:
      case GL_POLYGON:
#endif
        c = mFinalState.mFillColor;
        break;
    }

    r = c.Red();
    g = c.Green();
    b = c.Blue();
    a = c.Alpha();
    nuiCheckForGLErrors();
  }

  if (mR != r || mG != g || mB != b || mA != a)
  {
    glColor4f(r, g, b, a);
    mR = r;
    mG = g;
    mB = b;
    mA = a;
  }

  if (pArray->IsArrayEnabled(nuiRenderArray::eTexCoord))
  {
    if (!mClientTexCoord)
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    mClientTexCoord = true;
    glTexCoordPointer(2, GL_FLOAT, sizeof(nuiRenderArray::Vertex), &pArray->GetVertices()[0].mTX);
    nuiCheckForGLErrors();
  }
  else
  {
    if (mClientTexCoord)
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    mClientTexCoord = false;
  }

  /*
   if (pArray->IsArrayEnabled(nuiRenderArray::eNormal))
   {
   glEnableClientState(GL_NORMAL_ARRAY);
   glNormalPointer(GL_FLOAT, sizeof(nuiRenderArray::Vertex)-12, &pArray->GetVertices()[0].mNX);
   nuiCheckForGLErrors();
   }
   else
   glDisableClientState(GL_NORMAL_ARRAY);
   */

  /*
   if (pArray->IsArrayEnabled(nuiRenderArray::eEdgeFlag))
   {
   glEnableClientState(GL_EDGE_FLAG_ARRAY);
   glEdgeFlagPointer(sizeof(nuiRenderArray::Vertex), &pArray->GetVertices()[0].mEdgeFlag);
   nuiCheckForGLErrors();
   }
   else
   glDisableClientState(GL_EDGE_FLAG_ARRAY);
   */

  nuiCheckForGLErrors();

  if (mpSurface && mTwoPassBlend)
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
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_SHORT, &(array.mIndices[0]));
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
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_SHORT, &(array.mIndices[0]));
#else
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_INT, &(array.mIndices[0]));
#endif
      }
    }
    glBlendFunc(mSrcColor, mDstColor);
    nuiCheckForGLErrors();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
  else
  {
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
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_SHORT, &(array.mIndices[0]));
#else
        glDrawElements(array.mMode, array.mIndices.size(), GL_UNSIGNED_INT, &(array.mIndices[0]));
#endif
      }
    }
    nuiCheckForGLErrors();
  }


  {
    if (NeedTranslateHack)
      glTranslatef(-hackX, -hackY, 0);
  }

  pArray->Release();
  nuiCheckForGLErrors();
}

void nuiGLPainter::BeginSession()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  mpContext->MakeCurrent();
  nuiCheckForGLErrors();

  totalinframe = 0;
}

void nuiGLPainter::EndSession()
{
  // Bleh!
  NUI_RETURN_IF_RENDERING_DISABLED;
  //NGL_OUT("min = %d max = %d total in frame = %d total = %d\n", mins, maxs, totalinframe, total);
}

void nuiGLPainter::LoadMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;

  nuiPainter::LoadMatrix(rMatrix);
  //nuiGLLoadMatrix(rMatrix.Array);
  //nuiGLLoadMatrix(mMatrixStack.top().Array);

  mMatrixChanged = true;

  nuiCheckForGLErrors();
}

void nuiGLPainter::MultMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;

  nuiPainter::MultMatrix(rMatrix);
  LoadMatrix(mMatrixStack.top());
  nuiCheckForGLErrors();
}

void nuiGLPainter::PushMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;

  nuiPainter::PushMatrix();
  LoadMatrix(mMatrixStack.top());
  nuiCheckForGLErrors();
}

void nuiGLPainter::PopMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;

  nuiPainter::PopMatrix();
  LoadMatrix(mMatrixStack.top());
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

  //NGL_OUT(_T("Apply Target: 0x%x\n"), target);
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

      //NGL_OUT(_T("nuiGLPainter::UploadTexture 0x%x : '%s' / %d\n"), pTexture, pTexture->GetSource().GetChars(), info.mTexture);
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
          pTexture->ResetForceReload();
        }
        nuiCheckForGLErrors();

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
  std::map<nuiTexture*, TextureInfo>::iterator it = mTextures.find(pTexture);
  if (it == mTextures.end())
    return;
  NGL_ASSERT(it != mTextures.end());

  TextureInfo& info(it->second);
  if (info.mTexture <= 0)
    return;
  //NGL_OUT(_T("nuiGLPainter::DestroyTexture 0x%x : '%s' / %d\n"), pTexture, pTexture->GetSource().GetChars(), info.mTexture);

  mpContext->BeginSession();
  glDeleteTextures(1, (GLuint*)&info.mTexture);
  mTextures.erase(it);
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
  std::map<nuiSurface*, FramebufferInfo>::iterator it = mFramebuffers.find(pSurface);
  if (it == mFramebuffers.end())
  {
    return;
  }
  FramebufferInfo info = it->second;

  NGL_ASSERT(info.mFramebuffer > 0);
  glDeleteFramebuffersNUI(1, (GLuint*)&info.mFramebuffer);
  if (info.mRenderbuffer > 0)
    glDeleteRenderbuffersNUI(1, (GLuint*)&info.mRenderbuffer);
  if (info.mDepthbuffer > 0)
    glDeleteRenderbuffersNUI(1, (GLuint*)&info.mDepthbuffer);
  if (info.mStencilbuffer > 0)
    glDeleteRenderbuffersNUI(1, (GLuint*)&info.mStencilbuffer);

  mFramebuffers.erase(it);
}

void nuiGLPainter::SetSurface(nuiSurface* pSurface)
{
  //NGL_OUT("nuiGLPainter::SetSurface %p\n", pSurface);
  if (pSurface)
  {
    if (mpSurface)
    {
      mpSurfaceStack.push(mpSurface);
    }
  }
  else
  {
    if (!mpSurfaceStack.empty())
    {
      pSurface = mpSurfaceStack.top();
      mpSurfaceStack.pop();
    }
  }

  if (mpSurface == pSurface)
    return;

  if (!mpSurface && pSurface)
  {
//#ifdef _OPENGL_ES_
//    glGetIntegerv(GL_FRAMEBUFFER_BINDING_NUI, &mDefaultFramebuffer);
//    nuiCheckForGLErrors();
//    glGetIntegerv(GL_RENDERBUFFER_BINDING_NUI, (GLint *) &mDefaultRenderbuffer);
//    nuiCheckForGLErrors();
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


    FramebufferInfo info;

    if (create)
    {
      glGenFramebuffersNUI(1, (GLuint*)&info.mFramebuffer);
//      NGL_OUT("glGenFramebuffersNUI '%s' -> %d\n", pSurface->GetObjectName().GetChars(), info.mFramebuffer);
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
      glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, 0);
      nuiCheckForGLErrors();

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

      ///< We definetly need a color attachement, either a texture, or a renderbuffer
      if (pTexture && pSurface->GetRenderToTexture())
      {
        GLint oldTexture;
        if (mActiveTextureSlot != 0)
        {
          glActiveTexture(GL_TEXTURE0);
          mActiveTextureSlot = 0;
        }
        glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *) &oldTexture);

        UploadTexture(pTexture, 0);
        nuiCheckForGLErrors();

        std::map<nuiTexture*, TextureInfo>::iterator tex_it = mTextures.find(pTexture);
        NGL_ASSERT(tex_it != mTextures.end());
        TextureInfo& tex_info(tex_it->second);

        glBindTexture(GL_TEXTURE_2D, 0);

        nuiCheckForGLErrors();

        glFramebufferTexture2DNUI(GL_FRAMEBUFFER_NUI,
                                  GL_COLOR_ATTACHMENT0_NUI,
                                  GetTextureTarget(pTexture->IsPowerOfTwo()),
                                  tex_info.mTexture,
                                  0);
        //NGL_OUT("surface texture -> %d\n", tex_info.mTexture);
        nuiCheckForGLErrors();
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

#ifdef NGL_DEBUG
      CheckFramebufferStatus();
#endif
      nuiCheckForGLErrors();
      mFramebuffers[pSurface] = info;
    }
    else
    {
      /// !create
      info = it->second;
      glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, info.mFramebuffer);
      nuiCheckForGLErrors();
      //NGL_OUT("glBindFramebufferNUI -> %d\n", info.mFramebuffer);
      if (info.mRenderbuffer >= 0)
      {
        glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, info.mRenderbuffer);
        nuiCheckForGLErrors();
        //NGL_OUT("glBindRenderbufferNUI -> %d\n", info.mRenderbuffer);
      }

      nuiCheckForGLErrors();
#ifdef NGL_DEBUG
      CheckFramebufferStatus();
#endif
    }
  }
  else
  {
    /// !pSurface
    glBindFramebufferNUI(GL_FRAMEBUFFER_NUI, mDefaultFramebuffer);
    nuiCheckForGLErrors();
//    NGL_OUT("UNBIND glBindFramebufferNUI -> %d\n", mDefaultFramebuffer);
    glBindRenderbufferNUI(GL_RENDERBUFFER_NUI, mDefaultRenderbuffer);
//    NGL_OUT("UNBIND glBindRenderbufferNUI -> %d\n", mDefaultRenderbuffer);
    nuiCheckForGLErrors();

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

std::list<nuiGLPainter::RenderArrayInfo*> nuiGLPainter::RenderArrayInfo::mHeap;
nuiGLPainter::RenderArrayInfo* nuiGLPainter::RenderArrayInfo::Create(nuiRenderArray* pRenderArray)
{
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
    glDeleteVertexArrays(1, (GLuint*)&vao.second);
    nuiCheckForGLErrors();
  }
  mVAOs.clear();

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
  mHeap.push_back(this);
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


void nuiGLPainter::DestroyRenderArray(nuiRenderArray* pArray)
{
}



bool nuiCheckForGLErrorsReal()
{
  GLenum err = GL_NO_ERROR;
#if 1 // Globally enable/disable OpenGL error checking
  //#ifdef NGL_DEBUG
  err = glGetError();
  if (err == GL_NO_ERROR)
    return true;

//  App->GetLog().SetLevel("nuiGLPainter", 1000);
  switch (err)
  {
    case GL_INVALID_ENUM:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("An unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag."));
      break;
    case GL_INVALID_VALUE:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("A numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag."));
      break;
    case GL_INVALID_OPERATION:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("The specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag."));
      break;
    case GL_STACK_OVERFLOW:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("This function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag."));
      break;
    case GL_STACK_UNDERFLOW:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("This function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag."));
      break;
    case GL_OUT_OF_MEMORY:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("There is not enough memory left to execute the function. The state of OpenGL is undefined, except for the state of the error flags, after this error is recorded."));
      break;
    default:
      NGL_LOG(_T("nuiGLPainter"), NGL_LOG_ERROR, _T("Unkown error %d 0x%x."), err, err);
      break;
  }
  //#endif
  NGL_ASSERT(0);
#endif

  return true;
}

#endif //   #ifndef __NUI_NO_GL__
