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

static uint32 mins = 30000;
static uint32 maxs = 0;
static uint32 totalinframe = 0;
static uint32 total = 0;

nuiGL2Painter::nuiGL2Painter(nglContext* pContext)
: nuiGLPainter(pContext)
{
  nuiCheckForGLErrors();
  mUseShaders = true;

  mpShader_TextureVertexColor = nuiShaderProgram::GetProgram("TextureVertexColor");
  if (!mpShader_TextureVertexColor)
  {
    mpShader_TextureVertexColor = new nuiShaderProgram("TextureVertexColor");
    mpShader_TextureVertexColor->Acquire();
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
    mpShader_TextureAlphaVertexColor->Acquire();
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
    mpShader_TextureDifuseColor->Acquire();
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
    mpShader_TextureAlphaDifuseColor->Acquire();
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
    mpShader_VertexColor->Acquire();
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
    mpShader_DifuseColor->Acquire();
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

nuiGL2Painter::~nuiGL2Painter()
{
  mpShader_TextureVertexColor->Release();
  mpShader_TextureDifuseColor->Release();
  mpShader_TextureAlphaVertexColor->Release();
  mpShader_TextureAlphaDifuseColor->Release();
  mpShader_DifuseColor->Release();
  mpShader_VertexColor->Release();
}

void nuiGL2Painter::SetViewport()
{
  //GetAngle(), GetCurrentWidth(), GetCurrentHeight(), mProjectionViewportStack.top(), mProjectionMatrixStack.top());
  GLint Angle = GetAngle();
  GLint Width = GetCurrentWidth();
  GLint Height = GetCurrentHeight();
  const nuiRect& rViewport(mProjectionViewportStack.top());
  const nuiMatrix& rMatrix = mProjectionMatrixStack.top();

  //NGL_DEBUG(NGL_OUT("nuiGL2Painter::SetViewPort(%d, %d)\n", Width, Height);)

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

void nuiGL2Painter::ResetOpenGLState()
{
  BeginSession();
#ifdef DEBUG
  nuiGLDebugGuard g("nuiGL2Painter::ResetOpenGLState");
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

#define LOGENUM(XXX) case XXX: { NGL_OUT(_T("%s\n"), #XXX); } break;

void nuiGL2Painter::DrawArray(nuiRenderArray* pArray)
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
  nuiGLDebugGuard g("nuiGL2Painter::DrawArray");
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
      //        printf("optim (%d / %d) - %2.2f%%\n", skipped_ops, ops, (float)skipped_ops * 100.0f / (float)ops);
      //      #endif
      
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
      if (pArray->IsArrayEnabled(nuiRenderArray::eTexCoord))
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
  mFrameArrays.push_back(pArray);



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
  
  total += s;
  totalinframe += s;
  mins = MIN(mins, s);
  maxs = MAX(maxs, s);

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

void nuiGL2Painter::LoadMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::LoadMatrix(rMatrix);
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGL2Painter::MultMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::MultMatrix(rMatrix);
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGL2Painter::PushMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PushMatrix();
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGL2Painter::PopMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PopMatrix();
  mMatrixChanged = true;
  nuiCheckForGLErrors();
}

void nuiGL2Painter::SetVertexPointers(const nuiRenderArray& rArray)
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

void nuiGL2Painter::SetVertexBuffersPointers(const nuiRenderArray& rArray, RenderArrayInfo& rInfo)
{
  static int64 created = 0;
  static int64 bound = 0;
  total++;
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
    NGL_OUT("VAO/VBO %lld created %lld bound (%f cache hit)\n", created, bound, (float)bound / (float)created);
  }

  NGL_ASSERT(it != rInfo.mVAOs.end());

  GLint vao = it->second;
  glBindVertexArray(vao);
  nuiCheckForGLErrors();

}

void nuiGL2Painter::SetStreamBuffersPointers(const nuiRenderArray& rArray, const RenderArrayInfo& rInfo, int index)
{
  rInfo.BindStream(index);
  const nuiRenderArray::StreamDesc& rDesc(rArray.GetStream(index));
  glVertexAttribPointer(rDesc.mStreamID, rDesc.mCount, rDesc.mType, rDesc.mNormalize ? GL_TRUE : GL_FALSE, 0, NULL);
  nuiCheckForGLErrors();
  glEnableVertexAttribArray(rDesc.mStreamID);
  nuiCheckForGLErrors();
}


void nuiGL2Painter::ResetVertexPointers(const nuiRenderArray& rArray)
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


void nuiGL2Painter::DestroyRenderArray(nuiRenderArray* pArray)
{
  auto it = mRenderArrays.find(pArray);
  if (it == mRenderArrays.end())
    return; // This render array was not stored here

  RenderArrayInfo* info(it->second);

  RenderArrayInfo::Recycle(info);

  mRenderArrays.erase(it);
}


#endif //   #ifndef __NUI_NO_GL__
