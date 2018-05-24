/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot

 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

#ifdef _METAL_

#import <Metal/Metal.h>
#ifdef _COCOA_
#import <QuartzCore/CAMetalLayer.h>
#endif


#ifndef __NUI_NO_GL__

//#define NUI_RETURN_IF_RENDERING_DISABLED return;
#define NUI_RETURN_IF_RENDERING_DISABLED
//#define NUI_USE_GL_VERTEX_BUFFER
#define NUI_COMPLEX_SHAPE_THRESHOLD 6

//#define NUI_USE_ANTIALIASING
#ifdef NUI_USE_ANTIALIASING
#define NUI_USE_MULTISAMPLE_AA
#endif

static const char* TextureVertexColor_VTX =
SHADER_STRING
(
using namespace metal;
               
struct Transforms
{
  float4x4 ModelViewMatrix;
  float4 Offset;
};

 struct InputVertex
{
  float4 mPosition  [[ attribute(0) ]];
  float4 mColor     [[ attribute(1) ]];
  float4 mNormal    [[ attribute(2) ]];
  float2 mTexCoord  [[ attribute(3) ]];
};

struct Vertex
{
  float4 Position [[position]];
  float4 Color;
  float2 TexCoord;
};
               
vertex Vertex vertex_main(
                          InputVertex vertex_in [[ stage_in ]],
                          constant Transforms &transforms [[buffer(0)]])
{
  Vertex vert;

  vert.TexCoord = vertex_in.mTexCoord;
  vert.Color = vertex_in.mColor;
  vert.Position = transforms.ModelViewMatrix * (vertex_in.mPosition + transforms.Offset);

  return vert;
}
               
fragment float4 fragment_main(
                              Vertex vert [[stage_in]],
                              constant Transforms &transforms [[buffer(0)]],
                              texture2d<float> texture [[texture(0)]],
                              sampler textureSampler [[sampler(0)]]
                              )
{
  return vert.Color * texture.sample(textureSampler, vert.TexCoord);
}
);

////////////////////////////////////////////////////////////////////////////////
static const char* TextureAlphaVertexColor_VTX =
SHADER_STRING
(
 using namespace metal;
 
 
 struct Transforms
{
  float4x4 ModelViewMatrix;
  float4 Offset;
};
 
 struct InputVertex
{
  float4 mPosition  [[ attribute(0) ]];
  float4 mColor     [[ attribute(1) ]];
  float4 mNormal    [[ attribute(2) ]];
  float2 mTexCoord  [[ attribute(3) ]];
};

 struct Vertex
{
  float4 Position [[position]];
  float4 Color;
  float2 TexCoord;
};
 
 vertex Vertex vertex_main(
                           InputVertex vertex_in [[ stage_in ]],
                           constant Transforms &transforms [[buffer(0)]],
                           uint vid [[vertex_id]])
{
  Vertex vert;
  
  vert.TexCoord = vertex_in.mTexCoord;
  vert.Color = vertex_in.mColor;
  vert.Position = transforms.ModelViewMatrix * (vertex_in.mPosition + transforms.Offset);

  return vert;
}
 
 fragment float4 fragment_main(
                               Vertex vert [[stage_in]],
                               constant Transforms &transforms [[buffer(0)]],
                               texture2d<float> texture [[texture(0)]],
                               sampler textureSampler [[sampler(0)]]
                               )
{
//  return float4(0,0,1,1);
  return vert.Color * texture.sample(textureSampler, vert.TexCoord)[3];
//  return texture.sample(textureSampler, vert.TexCoord)[0];
}
);

//////////////////////////////////////////////////////////////////////////////////
static const char* TextureDifuseColor_VTX =
SHADER_STRING
(
using namespace metal;
 
struct Transforms
{
  float4x4 ModelViewMatrix;
  float4 Offset;
  float4 DifuseColor;
};
 
 struct InputVertex
{
  float4 mPosition  [[ attribute(0) ]];
  float4 mColor     [[ attribute(1) ]];
  float4 mNormal    [[ attribute(2) ]];
  float2 mTexCoord  [[ attribute(3) ]];
};

 struct Vertex
{
  float4 Position [[position]];
  float2 TexCoord;
};
 
vertex Vertex vertex_main(
                          InputVertex vertex_in [[ stage_in ]],
                          constant Transforms &transforms [[buffer(0)]])
{
  Vertex vert;
  
  vert.TexCoord = vertex_in.mTexCoord;
  vert.Position = transforms.ModelViewMatrix * (vertex_in.mPosition + transforms.Offset);

  return vert;
}
 
fragment float4 fragment_main(
                              Vertex vert [[stage_in]],
                              constant Transforms &transforms [[buffer(0)]],
                              texture2d<float> texture [[texture(0)]],
                              sampler textureSampler [[sampler(0)]]
                              )
{
//  return float4(1,0,0,1);
  return transforms.DifuseColor * texture.sample(textureSampler, vert.TexCoord);
}

);

//////////////////////////////////////////////////////////////////////////////////
static const char* TextureAlphaDifuseColor_VTX =
SHADER_STRING
(
using namespace metal;
 
struct Transforms
{
  float4x4 ModelViewMatrix;
  float4 Offset;
  float4 DifuseColor;
};
 
 struct InputVertex
{
  float4 mPosition  [[ attribute(0) ]];
  float4 mColor     [[ attribute(1) ]];
  float4 mNormal    [[ attribute(2) ]];
  float2 mTexCoord  [[ attribute(3) ]];
};

struct Vertex
{
  float4 Position [[position]];
  float2 TexCoord;
};
 
vertex Vertex vertex_main(
                          InputVertex vertex_in [[ stage_in ]],
                          constant Transforms &transforms [[buffer(0)]])
{
  Vertex vert;
  
  vert.TexCoord = vertex_in.mTexCoord;
  vert.Position = transforms.ModelViewMatrix * (vertex_in.mPosition + transforms.Offset);

  return vert;
}
 
fragment float4 fragment_main(
                              Vertex vert [[stage_in]],
                              constant Transforms &transforms [[buffer(0)]],
                              texture2d<float> texture [[texture(0)]],
                              sampler textureSampler [[sampler(0)]]
                              )
{
//  return float4(0,1,0,1);
  float v = texture.sample(textureSampler, vert.TexCoord)[3];
  return transforms.DifuseColor * float4(v, v, v, v);
}
 
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// No texture cases:
static const char* VertexColor_VTX =
SHADER_STRING
(
using namespace metal;

struct Transforms
{
  float4x4 ModelViewMatrix;
  float4 Offset;
};
               
 struct InputVertex
{
  float4 mPosition  [[ attribute(0) ]];
  float4 mColor     [[ attribute(1) ]];
  float4 mNormal    [[ attribute(2) ]];
  float2 mTexCoord  [[ attribute(3) ]];
};
 
struct Vertex
{
  float4 Position [[position]];
  float4 Color;
};
               
vertex Vertex vertex_main(
                          InputVertex vertex_in [[ stage_in ]],
                          constant Transforms &transforms [[buffer(0)]]
                          )
{
  Vertex vert;
  
  vert.Color = vertex_in.mColor;
  vert.Position = transforms.ModelViewMatrix * (vertex_in.mPosition + transforms.Offset);

  return vert;
}
               
fragment float4 fragment_main(Vertex vert [[stage_in]])
{
//  return float4(1,1,0,1);
  return vert.Color;
}
               
);

//////////////////////////////////////////////////////////////////////////////////
static const char* DifuseColor_VTX =
SHADER_STRING
(
 using namespace metal;
 
 struct Transforms
{
  float4x4 ModelViewMatrix;
  float4 Offset;
  float4 DifuseColor;
};
 
 struct InputVertex
{
  float4 mPosition  [[ attribute(0) ]];
  float4 mColor     [[ attribute(1) ]];
  float4 mNormal    [[ attribute(2) ]];
  float2 mTexCoord  [[ attribute(3) ]];
};
 
 struct Vertex
{
  float4 Position [[position]];
};
 
 vertex Vertex vertex_main(
                           InputVertex vertex_in [[ stage_in ]],
                           constant Transforms &transforms [[buffer(0)]])
{
  Vertex vert;
  
  vert.Position = transforms.ModelViewMatrix * (vertex_in.mPosition + transforms.Offset);

  return vert;
}
 
 fragment float4 fragment_main(
                               Vertex vert [[stage_in]],
                               constant Transforms &transforms [[buffer(0)]]
                               )
{
//  return float4(1,0,1,1);
  return transforms.DifuseColor;
}
 
 );

//////////////////////////////////////////////////////////////////////////////////
static const char* ClearColor_VTX =
SHADER_STRING
(
using namespace metal;
 
struct Transforms
{
  float4 ClearColor;
};
 
 struct Vertex
{
  float4 Position [[position]];
};
 
 vertex Vertex vertex_main(
                           uint vid [[vertex_id]]
                           )
{
  Vertex vert;
  vert.Position = float4((vid % 2) * 2.0 - 1.0,
                (vid / 2) * 2.0 - 1.0,
                0,
                1.0);
  
  return vert;
}
 
 fragment float4 fragment_main(
                               Vertex vert [[stage_in]],
                               constant Transforms &transforms [[buffer(0)]]
                               )
{
  return transforms.ClearColor;
}
 
);


// Stats
static uint32 mins = 30000;
static uint32 maxs = 0;
static uint32 totalinframe = 0;
static uint32 total = 0;
static nglCriticalSection gStats(nglString(__FILE__).Add(":").Add(__LINE__).GetChars());

template <typename T>
static T MakePOT(T v)
{
  uint i;
  double val = 1;
  for (i = 0; i < 64; i++)
  {
    if (v <= val)
    {
      v = ToBelow(val);
      break;
    }
    val *= 2;
  }
  return v;
}



uint32 nuiMetalPainter::mActiveContexts = 0;

nglString MakeLines(const nglString& rString)
{
  nglString t(rString);
  t.Replace(";", ";\n");
  t.Replace("{", "{\n");
  t.Replace("}", "}\n");
  return t;
}

nuiMetalPainter::nuiMetalPainter(nglContext* pContext)
: nuiPainter(pContext), mRenderingCS("mRenderingCS")
{
  gpPainters.insert(this);

  mClientVertex = false;
  mClientColor = false;
  mClientTexCoord = false;
  mMatrixChanged = true;
  mR = -1;
  mG = -1;
  mB = -1;
  mA = -1;
  mTexEnvMode = 0;

  mpContext = pContext;
  mActiveContexts++;

  mpShader_TextureVertexColor = nuiShaderProgram::GetProgram(mpContext, "TextureVertexColor");
  if (!mpShader_TextureVertexColor)
  {
    mpShader_TextureVertexColor = new nuiShaderProgram(mpContext, "TextureVertexColor");
    mpShader_TextureVertexColor->AddShader(eMetalShader, MakeLines(TextureVertexColor_VTX));
    mpShader_TextureVertexColor->Link();
  }

  mpShader_TextureAlphaVertexColor = nuiShaderProgram::GetProgram(mpContext, "TextureAlphaVertexColor");
  if (!mpShader_TextureAlphaVertexColor)
  {
    mpShader_TextureAlphaVertexColor = new nuiShaderProgram(mpContext, "TextureAlphaVertexColor");
    mpShader_TextureAlphaVertexColor->AddShader(eMetalShader, MakeLines(TextureAlphaVertexColor_VTX));
    mpShader_TextureAlphaVertexColor->Link();
  }

  mpShader_TextureDifuseColor = nuiShaderProgram::GetProgram(mpContext, "TextureDiffuseColor");
  if (!mpShader_TextureDifuseColor)
  {
    mpShader_TextureDifuseColor = new nuiShaderProgram(mpContext, "TextureDiffuseColor");
    mpShader_TextureDifuseColor->AddShader(eMetalShader, MakeLines(TextureDifuseColor_VTX));
    mpShader_TextureDifuseColor->Link();
//    mpShader_TextureDifuseColor->GetCurrentState()->Set("DifuseColor", nuiColor(255, 255, 255, 255));
  }

  mpShader_TextureAlphaDifuseColor = nuiShaderProgram::GetProgram(mpContext, "TextureAlphaDifuseColor");
  if (!mpShader_TextureAlphaDifuseColor)
  {
    mpShader_TextureAlphaDifuseColor = new nuiShaderProgram(mpContext, "TextureAlphaDifuseColor");
    mpShader_TextureAlphaDifuseColor->AddShader(eMetalShader, MakeLines(TextureAlphaDifuseColor_VTX));
    mpShader_TextureAlphaDifuseColor->Link();
//    mpShader_TextureAlphaDifuseColor->GetCurrentState()->Set("DifuseColor", nuiColor(255, 255, 255, 255));
  }

  mpShader_VertexColor = nuiShaderProgram::GetProgram(mpContext, "VertexColor");
  if (!mpShader_VertexColor)
  {
    mpShader_VertexColor = new nuiShaderProgram(mpContext, "VertexColor");
    mpShader_VertexColor->AddShader(eMetalShader, MakeLines(VertexColor_VTX));
    mpShader_VertexColor->Link();
  }

  mpShader_DifuseColor = nuiShaderProgram::GetProgram(mpContext, "DifuseColor");
  if (!mpShader_DifuseColor)
  {
    mpShader_DifuseColor = new nuiShaderProgram(mpContext, "DifuseColor");
    mpShader_DifuseColor->AddShader(eMetalShader, MakeLines(DifuseColor_VTX));
    mpShader_DifuseColor->Link();
  }
  
  mpShader_ClearColor = nuiShaderProgram::GetProgram(mpContext, "ClearColor");
  if (!mpShader_ClearColor)
  {
    mpShader_ClearColor = new nuiShaderProgram(mpContext, "ClearColor");
    mpShader_ClearColor->AddShader(eMetalShader, MakeLines(ClearColor_VTX));
    mpShader_ClearColor->Link();

  }

  {
    // Create the local clear color pipelineDesc once and for all for this painter
    nuiRenderState state(*mpState);
    state.mBlending = false;
    MTLRenderPipelineDescriptor* pipelineDesc = (__bridge MTLRenderPipelineDescriptor*) mpShader_ClearColor->NewMetalPipelineDescriptor(state);
    
    MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
    vertexDescriptor.layouts[0].stride = 0;
    vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionConstant;
    
    pipelineDesc.vertexDescriptor = vertexDescriptor;
    NSError* err = nil;
    id<MTLDevice> device = (__bridge id<MTLDevice>)mpContext->GetMetalDevice();
    NGL_ASSERT(device != nil);
    id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&err];
    NGL_ASSERT(pipelineState != nil);
    if (err)
    {
      NGL_ASSERT(0);
      NGL_OUT("Error creating MTLRenderPipelineState: %s\n", err.localizedDescription.UTF8String);
    }
    mpClearColor_pipelineState = (void*)CFBridgingRetain(pipelineState);
  }
}

nuiMetalPainter::~nuiMetalPainter()
{
  gpPainters.erase(this);

  mActiveContexts--;

  mpShader_TextureVertexColor->Release();
  mpShader_TextureDifuseColor->Release();
  mpShader_TextureAlphaVertexColor->Release();
  mpShader_TextureAlphaDifuseColor->Release();
  mpShader_DifuseColor->Release();
  mpShader_VertexColor->Release();
}

void nuiMetalPainter::SetViewport()
{
//  if (!mViewportChanged)
//    return;
  
  GLint Width = GetCurrentWidth();
  GLint Height = GetCurrentHeight();
  const nuiRect& rViewport(mProjectionViewportStack.top());
  NGL_ASSERT(rViewport.Left() >= 0);
  NGL_ASSERT(rViewport.Top() >= 0);

  int32 x, y, w, h;
  
  nuiRect r(rViewport);
  if (r.GetWidth() == 0 || r.GetHeight() == 0)
    r.SetSize((float)Width, (float)Height);
//  NGL_DEBUG(NGL_OUT("nuiMetalPainter::SetViewPort(%d, %d) (%s)\n", Width, Height, r.GetValue().GetChars());)


  x = ToNearest(r.Left());
  y = Height - ToNearest(r.Bottom());
//  NGL_ASSERT(x >= 0);
//  NGL_ASSERT(y >= 0);

//  x = nuiClamp(x, 0, Width - 1);
//  y = nuiClamp(y, 0, Height - 1);

  w = ToNearest(r.GetWidth());
  h = ToNearest(r.GetHeight());

//  w = nuiClamp(w, 0, Width - x);
//  h = nuiClamp(h, 0, Height - y);

  {
    const float scale = mpContext->GetScale();
    x *= scale;
    y *= scale;
    w *= scale;
    h *= scale;
  }

//  NGL_ASSERT(x >= 0);
//  NGL_ASSERT(y >= 0);
//  Set Metal ViewPort to (x, y, w, h);
  id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)GetRenderCommandEncoder();
  NGL_ASSERT(encoder != nil);
  MTLViewport viewport = { (double)x, (double)y, (double)w, (double)h, (double)-1, (double)1 };
  if (memcmp(&mViewport[0], &viewport, sizeof(viewport)) != 0)
  {
    NGL_ASSERT(sizeof(mViewport) == sizeof(viewport));
    memcpy(&mViewport[0], &viewport, sizeof(viewport));
    [encoder setViewport:viewport];
  }
  
//  NGL_DEBUG(NGL_OUT("nuiMetalPainter::SetViewPort Actual(%d, %d, %d, %d)\n", x, y, w, h);)

  mViewportChanged = false;
}




void nuiMetalPainter::StartRendering()
{
  nuiPainter::StartRendering();
  
  ResetMetalState();
}

void nuiMetalPainter::ResetMetalState()
{
  BeginSession();
#ifdef DEBUG
  nuiMetalDebugGuard g(mpContext, "nuiMetalPainter::ResetMetalState");
#endif
  
  mMatrixChanged = true;
  mR = -1;
  mG = -1;
  mB = -1;
  mA = -1;
  mTexEnvMode = 0;
  
  mScissorIsFlat = false;
  mFinalState = nuiRenderState();
  mpState = &mDefaultState;
}



void nuiMetalPainter::ApplyState(const nuiRenderState& rState)
{
#ifdef DEBUG
  nuiMetalDebugGuard g(mpContext, "nuiMetalPainter::ApplyState()");
#endif

  //TEST_FBO_CREATION();
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)GetRenderCommandEncoder();
  NGL_ASSERT(encoder != nil);

  // blending
  if (mFinalState.mBlending != rState.mBlending)
  {
    mFinalState.mBlending = rState.mBlending;
    if (mFinalState.mBlending)
    {
//      glEnable(GL_BLEND);
    }
    else
    {
//      glDisable(GL_BLEND);
    }
  }

//  if (mFinalState.mDepthTest != rState.mDepthTest)
//  {
//    mFinalState.mDepthTest = rState.mDepthTest;
//    if (mFinalState.mDepthTest)
//      glEnable(GL_DEPTH_TEST);
//    else
//      glDisable(GL_DEPTH_TEST);
//  }

//  if (mFinalState.mDepthWrite != rState.mDepthWrite)
//  {
//    mFinalState.mDepthWrite = rState.mDepthWrite;
//    glDepthMask(mFinalState.mDepthWrite ? GL_TRUE : GL_FALSE);
//  }

  if (mFinalState.mCulling != rState.mCulling || mFinalState.mCullingMode != rState.mCullingMode)
  {
    mFinalState.mCulling = rState.mCulling;
    mFinalState.mCullingMode = rState.mCullingMode;
    if (mFinalState.mCulling)
    {
      switch (mFinalState.mCullingMode)
      {
        case eCullingFront:
          [encoder setCullMode:MTLCullModeFront];
          break;
          
        case eCullingBack:
          [encoder setCullMode:MTLCullModeBack];
          break;
        
        case eCullingBoth:
          [encoder setCullMode:MTLCullModeNone];
          break;
      }
    }
    else
    {
      [encoder setCullMode:MTLCullModeNone];
    }
  }
  

  // We don't care about the font in the lower layer of rendering
  //nuiFont* mpFont;
  //

  for (int i = 0; i < NUI_MAX_TEXTURE_UNITS; i++)
  {
    ApplyTexture(rState, i);
  }
  
  if (mFinalState.mpShader != mpShader)
  {
    if (mpShader)
      mpShader->Acquire();
    if (mFinalState.mpShader)
      mFinalState.mpShader->Release();

    mFinalState.mpShader = mpShader;
#if DEBUG
    mpShader->Validate();
#endif
//    glUseProgram(mpShader->GetProgram());
  }

  if (mpShaderState)
    mpShaderState->Acquire();
  if (mFinalState.mpShaderState)
    mFinalState.mpShaderState->Release();
  mFinalState.mpShaderState = mpShaderState;

  // Rendering buffers:
  if (mFinalState.mColorBuffer != rState.mColorBuffer)
  {
    mFinalState.mColorBuffer = rState.mColorBuffer;
//    GLboolean m = mFinalState.mColorBuffer ? GL_TRUE : GL_FALSE;
//    glColorMask(m, m, m, m);
  }

  mFinalState.mClearColor = rState.mClearColor;
  mFinalState.mStrokeColor = rState.mStrokeColor;
  mFinalState.mFillColor = rState.mFillColor;
}

void nuiMetalPainter::ApplyScissor(const nuiRenderState& rState)
{
  id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)GetRenderCommandEncoder();
  NGL_ASSERT(encoder != nil);

  float scale = mpContext->GetScale();
  int32 width = GetCurrentWidth();
  int32 height = GetCurrentHeight();
  int x,y,w,h;
  
  if (mpClippingStack.top().mEnabled)
  {
    
    nuiRect clip(mpClippingStack.top());
    
    x = ToNearest(clip.Left());
    y = ToNearest(clip.Top());
    w = ToNearest(clip.GetWidth());
    h = ToNearest(clip.GetHeight());
  }
  else
  {
    x = 0;
    y = 0;
    w = width;
    h = height;
  }
  
  if (x < 0)
  {
    w += x;
    x = 0;
  }
  
  if (y < 0)
  {
    h += y;
    y = 0;
  }
  
  if (x + w > width)
  {
    w = width - x;
  }
  
  if (y + h > height)
  {
    h = height - y;
  }
  
  mScissorIsFlat = (w == 0 || h == 0);
  if (!mScissorIsFlat)
  {
    
    x *= scale;
    y *= scale;
    w *= scale;
    h *= scale;

    MTLScissorRect rect = {(NSUInteger)x, (NSUInteger)y, (NSUInteger)w, (NSUInteger)h};
    if (memcmp(&mScissorRect, &rect, sizeof(rect)) != 0)
    {
      NGL_ASSERT(sizeof(mScissorRect) == sizeof(rect));
      memcpy(&mScissorRect[0], &rect, sizeof(rect));
      [encoder setScissorRect:rect];
    }
  }
}

void nuiMetalPainter::SetState(const nuiRenderState& rState, bool ForceApply)
{
  NUI_RETURN_IF_RENDERING_DISABLED;

  mpState = &rState;
}

void nuiMetalPainter::ApplyTexture(const nuiRenderState& rState, int slot)
{
#ifdef DEBUG
  nuiMetalDebugGuard g(mpContext, "nuiMetalPainter::ApplTexture()");
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
    if (!it->second.mReload && it->second.mTexture != nullptr)
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

  if ((mFinalState.mpTexture[slot] != pTexture) || (mFinalState.mpTexture[slot] && !uptodate))
  {
    if (mFinalState.mpTexture[slot])
    {
      mFinalState.mpTexture[slot]->Release();
    }

    //NGL_OUT("Change texture to 0x%x (%s)\n", pTexture, pTexture?pTexture->GetSource().GetChars() : nglString::Empty.GetChars());
    mFinalState.mpTexture[slot] = pTexture ;

    if (mFinalState.mpTexture[slot])
    {
      mFinalState.mpTexture[slot]->Acquire();
      UploadTexture(mFinalState.mpTexture[slot], slot);
    }

    //NGL_OUT("Change texture type from 0x%x to 0x%x\n", outtarget, intarget);
  }
  else
  {
    mFinalState.mTexturing = rState.mTexturing;
  }
}



void nuiMetalPainter::Clear(bool color, bool depth, bool stencil)
{
  mRenderOperations++;
  NUI_RETURN_IF_RENDERING_DISABLED;

  if (mScissorIsFlat)
    return;
  
  SetViewport();  
  ApplyScissor(*mpState);
  mFinalState.mClearColor = mpState->mClearColor;
  
  float c[4] = { mFinalState.mClearColor.Red(), mFinalState.mClearColor.Green(), mFinalState.mClearColor.Blue(), mFinalState.mClearColor.Alpha() };
  id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)GetRenderCommandEncoder();
  NGL_ASSERT(encoder != nil);
  id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)mpClearColor_pipelineState;
  [encoder setRenderPipelineState:pipelineState];
  [encoder setFragmentBytes:c length:4*4 atIndex:0];
  [encoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

//#ifdef _OPENGL_ES_
//    glClearDepthf(mFinalState.mClearDepth);
//#else
//    glClearDepth(mFinalState.mClearDepth);
//#endif

//  GLint v = 0;
//  if (color)
//    v |= GL_COLOR_BUFFER_BIT;
//  if (depth)
//    v |= GL_DEPTH_BUFFER_BIT;
//  if (stencil)
//    v |= GL_STENCIL_BUFFER_BIT;

//  glClear(v);
}

MTLPrimitiveType nuiMTLPrimitiveTypeFromGL(GLenum type)
{
  switch (type)
  {
    case GL_POINTS:
      return MTLPrimitiveTypePoint;
    case GL_LINES:
      return MTLPrimitiveTypeLine;
    case GL_LINE_STRIP:
      return MTLPrimitiveTypeLineStrip;
    case GL_TRIANGLES:
      return MTLPrimitiveTypeTriangle;
    case GL_TRIANGLE_FAN:
      NGL_ASSERT(0); // Fans not supported in Metal
      break;
    case GL_TRIANGLE_STRIP:
      return MTLPrimitiveTypeTriangleStrip;
  }
  
  return MTLPrimitiveTypePoint;
}

#define LOGENUM(XXX) case XXX: { NGL_OUT("%s\n", #XXX); } break;

void nuiMetalPainter::DrawArray(nuiRenderArray* pArray)
{
  mRenderOperations++;
  mBatches++;
  
  if (!mEnableDrawArray)
  {
    pArray->Release();
    return;
  }
  
  ApplyScissor(*mpState);

  if (mScissorIsFlat)
  {
    pArray->Release();
    return;
  }

  SetViewport();

  if (pArray->GetDebug())
  {
    NGL_OUT("Texturing %s (%p)\n", YESNO(mFinalState.mTexturing), mFinalState.mpTexture[0]);
    pArray->Dump();
  }
  
#ifdef DEBUG
  nuiMetalDebugGuard g(mpContext, "nuiMetalPainter::DrawArray");
#endif

  static uint32 ops = 0;
  static uint32 skipped_ops = 0;
  {
    nglCriticalSectionGuard g(gStats);
    ops++;
  }

  const nuiMatrix& rM(GetMatrix());
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

    nuiClipper clip = mpClippingStack.top();
    if (clip.mEnabled && (
        (bounds[0] > clip.Right()) ||
        (bounds[1] > clip.Bottom()) ||
        (bounds[3] < clip.Left()) ||
        (bounds[4] < clip.Top())
        ))
    {
      pArray->Release();
      {
        nglCriticalSectionGuard g(gStats);
        skipped_ops++;

#ifdef _DEBUG
        if (!(skipped_ops % 100))
          printf("optim (%d / %d) - %2.2f%%\n", skipped_ops, ops, (float)skipped_ops * 100.0f / (float)ops);
#endif
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
  }
  if (!mpShaderState)
    mpShaderState = mpShader->NewState();
  else
    mpShaderState->Acquire();

  NGL_ASSERT(mpShader != NULL);
  
  ApplyState(*mpState);
  pArray->Acquire();
  
  mFinalState.mpShaderState->SetModelViewMatrix(GetProjectionMatrix() * GetMatrix());
  
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
    mpShaderState->Release();
    mpShaderState = nullptr;
    return;
  }
  
  if (mpClippingStack.top().GetWidth() <= 0 || mpClippingStack.top().GetHeight() <= 0)
  {
    pArray->Release();
    mpShaderState->Release();
    mpShaderState = nullptr;
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
    hackX = ratio;
    hackY = ratio;
  }
  
  if (mpSurface)
    mFinalState.mpShaderState->SetOffset(hackX, -hackY);
  else
    mFinalState.mpShaderState->SetOffset(hackX, hackY);
  
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
    
    mFinalState.mpShaderState->SetDifuseColor(nuiColor(mR, mG, mB, mA));
  }
  
  // Metal Specific part:
  id<MTLDevice> device = (__bridge id<MTLDevice>)mpContext->GetMetalDevice();
  NGL_ASSERT(device != nil);
  id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)GetRenderCommandEncoder();
  NGL_ASSERT(encoder != nil);
  // Uniforms
  {
    const void* stateData = mpShaderState->GetStateData();
    size_t stateDataSize = mpShaderState->GetStateDataSize();
    [encoder setVertexBytes:stateData length:stateDataSize atIndex:0];
    [encoder setFragmentBytes:stateData length:stateDataSize atIndex:0];
  }

  std::vector<void*> vertexArray; //  The MTLBuffer caches for the vertices and indices of this nuiRenderArray
  {
    
    // Vertex Data:
    {
      nglCriticalSectionGuard g(mRenderArraysCS);
      auto arrayInfoIt = mRenderArrays.find(pArray);
      if (arrayInfoIt != mRenderArrays.end())
      {
        vertexArray = arrayInfoIt->second;
      }
      else
      {
        size_t vertexcount = pArray->GetSize();
        size_t vertexsize = sizeof(nuiRenderArray::Vertex);
//        id<MTLBuffer> vertices = [device newBufferWithBytes:&pArray->GetVertex(0).mX length:vertexsize*vertexcount options:MTLResourceStorageModePrivate];
        id<MTLBuffer> vertices = [device newBufferWithBytes:&pArray->GetVertex(0).mX length:vertexsize*vertexcount options:MTLResourceStorageModeShared];
        NGL_ASSERT(vertices != nil);
        vertexArray.push_back((void*)CFBridgingRetain(vertices));
        
        for (size_t i = 0; i < pArray->GetIndexArrayCount(); i++)
        {
          auto& array(pArray->GetIndexArray(i));
#if (defined _UIKIT_)
          id<MTLBuffer> indexes = [device newBufferWithBytes:&(array.mIndices[0]) length:array.mIndices.size()*2 options:MTLResourceStorageModePrivate];
#else
          id<MTLBuffer> indexes = [device newBufferWithBytes:&(array.mIndices[0]) length:array.mIndices.size()*4 options:MTLResourceStorageModePrivate];
#endif
          NGL_ASSERT(indexes != nil);
          vertexArray.push_back((void*)CFBridgingRetain(indexes));
        }
        mRenderArrays[pArray] = vertexArray;
      }

    }

    id<MTLBuffer> vertices = (__bridge id<MTLBuffer>)vertexArray[0];
    NGL_ASSERT(vertices != nil);
    [encoder setVertexBuffer:vertices offset:0 atIndex:1];

    // Create the pipeline state from all the gathered informations:
    id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)mFinalState.mpShader->NewMetalPipelineState(mFinalState);
    NGL_ASSERT(pipelineState != nil);

    [encoder setRenderPipelineState:pipelineState];
  }
  
  // Set Textures:
  for (uint i = 0; i < 8; i++)
  {
    nuiTexture* pTexture = mFinalState.mpTexture[i];
    nuiTexture* pProxy = nullptr;
    id<MTLTexture> texture = nil;
    id<MTLSamplerState> sampler = nil;
    
    if (pTexture)
    {
     pProxy = pTexture->GetProxyTexture();
      if (pProxy)
        pTexture = pProxy;

      {
        nglCriticalSectionGuard tg(mTexturesCS);
        auto it = mTextures.find(pTexture);
        NGL_ASSERT(it != mTextures.end());
        texture = (__bridge id<MTLTexture>)it->second.mTexture;
        sampler = (__bridge id<MTLSamplerState>)it->second.mSampler;
      }
    }
//    NGL_OUT("Setting texture[%d] = %p / %p (%p%s)\n", i, texture, sampler, pTexture, (pProxy?" [Proxy]":""));
    NGL_ASSERT((pTexture != nullptr && texture != nil && sampler != nil) || (pTexture == nullptr && texture == nil && sampler == nil));
    [encoder setFragmentTexture:texture atIndex:i];
    [encoder setFragmentSamplerState:sampler atIndex:i];
  }
  
  
  uint32 arraycount = pArray->GetIndexArrayCount();
  
  if (!arraycount)
  {
    MTLPrimitiveType primitiveType = nuiMTLPrimitiveTypeFromGL(mode);
    [encoder drawPrimitives:primitiveType vertexStart:0 vertexCount:s];
    mDrawOperationsOnCurrentSurface++;
  }
  else
  {
    for (uint32 i = 0; i < arraycount; i++)
    {
      nuiRenderArray::IndexArray& array(pArray->GetIndexArray(i));
      MTLPrimitiveType primitiveType = nuiMTLPrimitiveTypeFromGL(array.mMode);
      id<MTLBuffer> indexes = (__bridge id<MTLBuffer>)vertexArray[1 + i];
      NGL_ASSERT(indexes != nil);
#if (defined _UIKIT_)
      MTLIndexType indexType = MTLIndexTypeUInt16;
#else
      MTLIndexType indexType = MTLIndexTypeUInt32;
#endif
      [encoder drawIndexedPrimitives:primitiveType indexCount:array.mIndices.size() indexType:indexType indexBuffer:indexes indexBufferOffset:0];
      mDrawOperationsOnCurrentSurface++;
    }
  }

  pArray->Release();
  mpShaderState->Release();
  mpShaderState = nullptr;
}


void nuiMetalPainter::BeginSession()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  mpContext->MakeCurrent();

  FinalizeSurfaces();
  FinalizeTextures();
  FinalizeRenderArrays();

  {
    nglCriticalSectionGuard g(gStats);
    totalinframe = 0;
  }
}

void nuiMetalPainter::EndSession()
{
  // Bleh!
  NUI_RETURN_IF_RENDERING_DISABLED;
  //NGL_OUT("min = %d max = %d total in frame = %d total = %d\n", mins, maxs, totalinframe, total);
}


static inline MTLSamplerMinMagFilter nuiGLToMetaltextureFilter(GLenum val)
{
  switch (val)
  {
    case GL_NEAREST_MIPMAP_NEAREST:
      return MTLSamplerMinMagFilterNearest;
    case GL_LINEAR_MIPMAP_NEAREST:
      return MTLSamplerMinMagFilterLinear;
    case GL_NEAREST_MIPMAP_LINEAR:
      return MTLSamplerMinMagFilterNearest;
    case GL_LINEAR_MIPMAP_LINEAR:
      return MTLSamplerMinMagFilterLinear;
  }
  
  return MTLSamplerMinMagFilterLinear;
}

static inline MTLSamplerAddressMode nuiGLToMetaltextureAddressMode(GLenum val)
{
  switch (val)
  {
    case GL_CLAMP_TO_EDGE:
      return MTLSamplerAddressModeClampToEdge;
#ifdef _COCOA_
    case GL_CLAMP_TO_BORDER:
      return MTLSamplerAddressModeClampToBorderColor;
#endif
    case GL_REPEAT:
      return MTLSamplerAddressModeRepeat;
    case GL_MIRRORED_REPEAT:
      return MTLSamplerAddressModeMirrorRepeat;
  }
  
  return MTLSamplerAddressModeClampToEdge;
}

void nuiMetalPainter::UploadTexture(nuiTexture* pTexture, int slot)
{
#ifdef DEBUG
  nuiMetalDebugGuard g(mpContext, "nuiMetalPainter::UploadTexture()");
#endif

  id<MTLDevice> device = (__bridge id<MTLDevice>)mpContext->GetMetalDevice();
  NGL_ASSERT(device != nil);
  nuiTexture* pProxy = pTexture->GetProxyTexture();
  if (pProxy)
    pTexture = pProxy;
  nuiSurface* pSurface = pTexture->GetSurface();

  float Width = pTexture->GetUnscaledWidth();
  float Height = pTexture->GetUnscaledHeight();

  nglCriticalSectionGuard tg(mTexturesCS);
  auto it = mTextures.find(pTexture);
  if (it == mTextures.end())
    it = mTextures.insert(mTextures.begin(), std::pair<nuiTexture*, TextureInfo>(pTexture, TextureInfo()));
  NGL_ASSERT(it != mTextures.end());

  TextureInfo& info(it->second);

  void* _id = (void*)pTexture->GetTextureID();
  if (_id)
  {
    info.mReload = false;
    info.mTexture = _id;
    MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
    samplerDescriptor.minFilter = nuiGLToMetaltextureFilter(pTexture->GetMinFilter());
    samplerDescriptor.magFilter = nuiGLToMetaltextureFilter(pTexture->GetMagFilter());
    samplerDescriptor.sAddressMode = nuiGLToMetaltextureAddressMode(pTexture->GetWrapS());
    samplerDescriptor.tAddressMode = nuiGLToMetaltextureAddressMode(pTexture->GetWrapT());
    
    id<MTLSamplerState> samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
    NGL_ASSERT(samplerState != nil);
    info.mSampler = (void*)CFBridgingRetain(samplerState);
  }

  //NGL_OUT("Apply Target: 0x%x\n", target);
  nglImage* pImage = pTexture->GetImage();

  {
    bool firstload = false;
    bool reload = info.mReload;

    if (info.mTexture == nullptr)
    { // Generate a texture
      //NGL_OUT("nuiMetalPainter::UploadTexture 0x%x : '%s' / %d\n", pTexture, pTexture->GetSource().GetChars(), info.mTexture);
      firstload = true;
      reload = true;
    }

    if (!pSurface && !(pImage && pImage->GetPixelSize()) && !_id)
      return;

    if (reload)
    {
      uint32 type = 8;
      MTLPixelFormat mtlPixelFormat = MTLPixelFormatInvalid;
      GLint pixelformat = 0;
      uint8* pBuffer = NULL;
      uint8* pNewBuffer = nullptr;
      uint32 bytesPerLine = 0;
      
      if (pImage)
      {
        type = pImage->GetBitDepth();
        pixelformat = pImage->GetPixelFormat();
        pBuffer = (uint8*)pImage->GetBuffer();
        bytesPerLine = pTexture->GetImage()->GetBytesPerLine();
        
        //#ifndef NUI_IOS
        switch (type)
        {
          case 8:
            mtlPixelFormat = MTLPixelFormatA8Unorm;
            break;
          case 16:
//            mtlPixelFormat = MTLPixelFormatB5G6R5Unorm;
            NGL_ASSERT(0);
            break;
          case 15:
//            mtlPixelFormat = MTLPixelFormatBGR5A1Unorm;
            NGL_ASSERT(0);
            break;
          case 24:
            { //  Crude convertion from 24 to 32 bits images:
              if (pBuffer)
              {
                uint32 count = (uint32)ToNearest(Width) * (uint32)ToNearest(Height);
                pNewBuffer = new uint8[4 * count];
                for (uint32 i = 0; i < count; i++)
                {
                  pNewBuffer[i * 4 + 0] = pBuffer[i * 3 + 0];
                  pNewBuffer[i * 4 + 1] = pBuffer[i * 3 + 1];
                  pNewBuffer[i * 4 + 2] = pBuffer[i * 3 + 2];
                  pNewBuffer[i * 4 + 3] = 255;
                }
                pBuffer = pNewBuffer;
                bytesPerLine = bytesPerLine + bytesPerLine / 3;
              }
            }
            mtlPixelFormat = MTLPixelFormatRGBA8Unorm;
            break;
          case 32:
            mtlPixelFormat = MTLPixelFormatRGBA8Unorm;
            break;
            
          default:
            NGL_ASSERT(0);
        }
      }
      else
      {
        NGL_ASSERT(pSurface);
        mtlPixelFormat = MTLPixelFormatBGRA8Unorm;
      }


      {
        // Handle mipmaps and pTexture->GetAutoMipMap()
          pTexture->ResetForceReload();

        // Give a name to the texture for Metal Debugging
        
        id<MTLTexture> texture = (__bridge id<MTLTexture>)info.mTexture;
        if (!texture)
        {
          MTLTextureDescriptor* descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:mtlPixelFormat
                                                                                                width:(uint32)ToNearest(Width) height:(uint32)ToNearest(Height) mipmapped:pTexture->GetAutoMipMap()];
          descriptor.usage = pSurface ? (MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead) : MTLTextureUsageShaderRead;
//          descriptor.resourceOptions = MTLResourceStorageModePrivate;
//          descriptor.storageMode = MTLStorageModePrivate;
          texture = [device newTextureWithDescriptor:descriptor];
          CFStringRef str = pTexture->GetSource().ToCFString();
          texture.label = (__bridge NSString*)str;
          CFRelease(str);
        }
        
        MTLRegion region = MTLRegionMake2D(0, 0, (uint32)ToNearest(Width), (uint32)ToNearest(Height));
        if (pBuffer)
        {
          NGL_ASSERT(!pSurface);
          [texture replaceRegion:region mipmapLevel:0 withBytes:pBuffer bytesPerRow:bytesPerLine];
        }
        else
        {
          NGL_ASSERT(pSurface);
        }
        info.mTexture = (void*)CFBridgingRetain(texture);
        MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
        samplerDescriptor.minFilter = nuiGLToMetaltextureFilter(pTexture->GetMinFilter());
        samplerDescriptor.magFilter = nuiGLToMetaltextureFilter(pTexture->GetMagFilter());
        samplerDescriptor.sAddressMode = nuiGLToMetaltextureAddressMode(pTexture->GetWrapS());
        samplerDescriptor.tAddressMode = nuiGLToMetaltextureAddressMode(pTexture->GetWrapT());

        id<MTLSamplerState> samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
        NGL_ASSERT(samplerState != nil);
        info.mSampler = (void*)CFBridgingRetain(samplerState);
      }

      info.mReload = false;

#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
      if (!pTexture->IsBufferRetained())
      {
        pTexture->ReleaseBuffer();
      }
#endif
      
      if (pNewBuffer)
        delete[] pNewBuffer;
    }
  }

  NGL_ASSERT((info.mTexture != nil && info.mSampler != nil));
}

void nuiMetalPainter::DestroyTexture(nuiTexture* pTexture)
{
  nglCriticalSectionGuard g(mRenderingCS);
  mDestroyedTextures.push_back(pTexture);
}

void nuiMetalPainter::FinalizeTextures()
{
  for (auto texture: mDestroyedTextures)
    _DestroyTexture(texture);
  mDestroyedTextures.clear();
}

void nuiMetalPainter::_DestroyTexture(nuiTexture* pTexture)
{
  nglCriticalSectionGuard tg(mTexturesCS);
  auto it = mTextures.find(pTexture);
  if (it == mTextures.end())
    return;
  NGL_ASSERT(it != mTextures.end());

  TextureInfo info(it->second);
  mTextures.erase(it);

  //NGL_OUT("nuiMetalPainter::DestroyTexture 0x%x : '%s' / %d\n", pTexture, pTexture->GetSource().GetChars(), info.mTexture);

  // TODO Actually Delete the Texture
  if (info.mTexture)
    CFRelease(info.mTexture);
  if (info.mSampler)
    CFRelease(info.mSampler);
  info.mTexture = nullptr;
  info.mSampler = nullptr;
}

void nuiMetalPainter::DestroySurface(nuiSurface* pSurface)
{
  nglCriticalSectionGuard g(mRenderingCS);
  mDestroyedSurfaces.push_back(pSurface);
}

void nuiMetalPainter::FinalizeSurfaces()
{
  for (auto surface : mDestroyedSurfaces)
    _DestroySurface(surface);
  mDestroyedSurfaces.clear();
}

void nuiMetalPainter::_DestroySurface(nuiSurface* pSurface)
{
}

void nuiMetalPainter::CreateSurface(nuiSurface* pSurface)
{
  NGL_ASSERT(pSurface);
  nuiTexture* pTexture = pSurface->GetTexture();
  NGL_ASSERT(pTexture);
  UploadTexture(pTexture, 0);
}

void nuiMetalPainter::SetSurface(nuiSurface* pSurface)
{
//  NGL_OUT("nuiMetalPainter::SetSurface %p\n", pSurface);
  mViewportChanged = true;

  if (pSurface)
  {
    pSurface->Acquire();
  }
  
  // Print stats:
//  if (mpSurface && !pSurface)
//  {
//    NGL_OUT("DrawOperations On Surface %s = %d\n", mpSurface->GetObjectName().GetChars(), (int)mDrawOperationsOnCurrentSurface);
//  }
  
  if (mpSurface)
  {
    mpSurface->Release();
  }
  mpSurface = pSurface;
  
  if (pSurface)
  {
    GLint width = (GLint)pSurface->GetWidth();
    GLint height = (GLint)pSurface->GetHeight();

    float scale = mpContext->GetScale();
    width *= scale;
    height *= scale;
    
    nuiTexture* pTexture = pSurface->GetTexture();
    
#if defined DEBUG && defined _UIKIT_
    if (pTexture)
      mpContext->StartMarkerGroup(pTexture->GetSource().GetChars());
    else
      mpContext->StartMarkerGroup(pSurface->GetObjectName().GetChars());
#endif
    
    //NGL_OUT("nuiMetalPainter::CreateSurface %p\n", pSurface);
    NGL_ASSERT(pSurface != nullptr);
    
#if defined DEBUG && defined _UIKIT_
    if (pTexture)
      mpContext->StartMarkerGroup(pTexture->GetSource().GetChars());
    else
      mpContext->StartMarkerGroup(pSurface->GetObjectName().GetChars());
#endif
    
    if (pTexture && pSurface->GetRenderToTexture())
    {
      mFinalState.mpTexture[0] = nullptr;
      
      nglCriticalSectionGuard tg(mTexturesCS);
      auto it = mTextures.find(pTexture);
      if (it == mTextures.end())
        it = mTextures.insert(mTextures.begin(), std::pair<nuiTexture*, TextureInfo>(pTexture, TextureInfo()));
      NGL_ASSERT(it != mTextures.end());
      
      NGL_ASSERT(width > 0);
      NGL_ASSERT(height > 0);
      
      UploadTexture(pTexture, 0);
      
      //    NGL_OUT("init color surface texture %d x %d --> %d\n", width, height, tinfo.mTexture);
    }
    
    auto tit = mTextures.find(pTexture);
    
    MTLRenderPassDescriptor *passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    //  We definetly need a color attachement, either a texture, or a renderbuffer
    if (pTexture && pSurface->GetRenderToTexture())
    {
      passDescriptor.colorAttachments[0].texture = (__bridge id<MTLTexture>)tit->second.mTexture;
      passDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
      passDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
      passDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(mpState->mClearColor.Red(),mpState->mClearColor.Green(),mpState->mClearColor.Blue(),mpState->mClearColor.Alpha());
      
      
      //    NGL_OUT("init color surface texture %d x %d --> %d\n", width, height, info.mTexture);
    }
    else
    {
      GLint pixelformat = pSurface->GetPixelFormat();
      GLint internalPixelformat = pSurface->GetPixelFormat();
      internalPixelformat = GL_RGBA;
      
      //NGL_OUT("surface render buffer -> %d\n", info.mRenderbuffer);
    }
    
    
    if (pSurface->GetDepth())
    {
      NGL_ASSERT(0);
    }
    
    if (pSurface->GetStencil())
    {
      NGL_ASSERT(0);
    }

    id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)mpContext->GetMetalCommandBuffer();
    NGL_ASSERT(commandBuffer != nil);
    
    mpContext->SetCurrentMetalCommandEncoder(nullptr);
    if (mpBlitEncoder)
    {
      id<MTLBlitCommandEncoder> blitter = (__bridge id<MTLBlitCommandEncoder>)mpBlitEncoder;
      [blitter endEncoding];
      CFBridgingRelease(mpBlitEncoder);
      mpBlitEncoder = nullptr;
    }

    id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:passDescriptor];
    NGL_ASSERT(commandEncoder != nil);
    commandEncoder.label = [NSString stringWithUTF8String:pSurface->GetObjectName().GetChars()];
    mpContext->SetCurrentMetalCommandEncoder((__bridge void*)commandEncoder);

    // Reset stats:
    mDrawOperationsOnCurrentSurface = 0;
  }
  else
  {
    /// !pSurface
    mpContext->SetCurrentMetalCommandEncoder(nullptr);
  }
}


void nuiMetalPainter::LoadMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::LoadMatrix(rMatrix);
  mMatrixChanged = true;
}

void nuiMetalPainter::MultMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::MultMatrix(rMatrix);
  mMatrixChanged = true;
}

void nuiMetalPainter::PushMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PushMatrix();
  mMatrixChanged = true;
}

void nuiMetalPainter::PopMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PopMatrix();
  mMatrixChanged = true;
}

void nuiMetalPainter::LoadProjectionMatrix(const nuiRect& rViewport, const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::LoadProjectionMatrix(rViewport, rMatrix);
  mViewportChanged = true;
}

void nuiMetalPainter::MultProjectionMatrix(const nuiMatrix& rMatrix)
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::MultProjectionMatrix(rMatrix);
  mViewportChanged = true;
}

void nuiMetalPainter::PushProjectionMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PushProjectionMatrix();
  mViewportChanged = true;
}

void nuiMetalPainter::PopProjectionMatrix()
{
  NUI_RETURN_IF_RENDERING_DISABLED;
  
  nuiPainter::PopProjectionMatrix();
  mViewportChanged = true;
}


void nuiMetalPainter::DestroyRenderArray(nuiRenderArray* pArray)
{
  nglCriticalSectionGuard g(mRenderArraysCS);
  auto it = mRenderArrays.find(pArray);
  if (it != mRenderArrays.end())
  {
    for (size_t i = 0; i < it->second.size(); i++)
      CFBridgingRelease(it->second[i]);
    mRenderArrays.erase(it);
  }
}

void nuiMetalPainter::FinalizeRenderArrays()
{
  nglCriticalSectionGuard g(mRenderingCS);
}

nglImage* nuiMetalPainter::CreateImageFromGPUTexture(const nuiTexture* pTexture) const
{
  return nullptr;
}


std::unordered_map<nuiTexture*, nuiMetalPainter::TextureInfo> nuiMetalPainter::mTextures;
nglCriticalSection nuiMetalPainter::mTexturesCS("mTexturesCS");

std::unordered_map<nuiRenderArray*, std::vector<void*> > nuiMetalPainter::mRenderArrays;
nglCriticalSection nuiMetalPainter::mRenderArraysCS("mRenderArraysCS");


int32 nuiMetalPainter::GetCurrentWidth() const
{
  if (mpSurface)
    return mpSurface->GetWidth();
  return mpContext->GetWidth();
}

int32 nuiMetalPainter::GetCurrentHeight() const
{
  if (mpSurface)
    return mpSurface->GetHeight();
  return mpContext->GetHeight();
}

void* nuiMetalPainter::GetRenderCommandEncoder() const
{
  if (mpBlitEncoder)
  {
    id<MTLBlitCommandEncoder> blitter = (__bridge id<MTLBlitCommandEncoder>)mpBlitEncoder;
    [blitter endEncoding];
    CFBridgingRelease(mpBlitEncoder);
    mpBlitEncoder = nullptr;
  }
  
  void* encoder = mpContext->GetCurrentMetalCommandEncoder();
  if (!encoder)
  {
    encoder = mpContext->GetDrawableMetalCommandEncoder(true);
  }

  return encoder;
}

void* nuiMetalPainter::GetBlitCommandEncoder() const
{
  if (mpBlitEncoder)
    return mpBlitEncoder;
  
  void* encoder = mpContext->GetCurrentMetalCommandEncoder();
  if (encoder)
  {
    mpContext->SetCurrentMetalCommandEncoder(nullptr);
  }

  id<MTLCommandBuffer> buffer = (__bridge id<MTLCommandBuffer>)mpContext->GetMetalCommandBuffer();
  id<MTLBlitCommandEncoder> blitter = [buffer blitCommandEncoder];
  mpBlitEncoder = (void*)CFBridgingRetain(blitter);
  return mpBlitEncoder;
}


void nuiMetalPainter::CacheRenderArray(nuiRenderArray* pArray)
{
  id<MTLBlitCommandEncoder> blitter = (__bridge id<MTLBlitCommandEncoder>)GetBlitCommandEncoder();
  id<MTLDevice> device = (__bridge id<MTLDevice>)mpContext->GetMetalDevice();

  std::vector<void*> vertexArray; //  The MTLBuffer caches for the vertices and indices of this nuiRenderArray
  // Vertex Data:
  {
    nglCriticalSectionGuard g(mRenderArraysCS);
    auto arrayInfoIt = mRenderArrays.find(pArray);
    if (arrayInfoIt == mRenderArrays.end())
    {
      size_t vertexcount = pArray->GetSize();
      size_t vertexsize = sizeof(nuiRenderArray::Vertex);
      size_t size = vertexcount * vertexsize;
      if (!size)
        return;
      //        id<MTLBuffer> vertices = [device newBufferWithBytes:&pArray->GetVertex(0).mX length:size options:MTLResourceStorageModePrivate];
      id<MTLBuffer> vertices = [device newBufferWithBytes:&pArray->GetVertex(0).mX length:size options:MTLResourceStorageModeShared];
      NGL_ASSERT(vertices != nil);

      id<MTLBuffer> destination = [device newBufferWithLength:size options:MTLResourceStorageModePrivate];
      [blitter copyFromBuffer:vertices sourceOffset:0 toBuffer:destination destinationOffset:0 size:size];
      NGL_ASSERT(destination != nil);

      vertexArray.push_back((void*)CFBridgingRetain(destination));
      
      for (size_t i = 0; i < pArray->GetIndexArrayCount(); i++)
      {
        auto& array(pArray->GetIndexArray(i));
#if (defined _UIKIT_)
        size_t length = array.mIndices.size()*2;
        id<MTLBuffer> indexes = [device newBufferWithBytes:&(array.mIndices[0]) length:length options:MTLResourceStorageModePrivate];
#else
        size_t length = array.mIndices.size()*4;
        id<MTLBuffer> indexes = [device newBufferWithBytes:&(array.mIndices[0]) length:length options:MTLResourceStorageModePrivate];
#endif
        NGL_ASSERT(indexes != nil);

        id<MTLBuffer> destindexes = [device newBufferWithLength:length options:MTLResourceStorageModePrivate];
        [blitter copyFromBuffer:vertices sourceOffset:0 toBuffer:destindexes destinationOffset:0 size:length];
        NGL_ASSERT(destindexes != nil);

        vertexArray.push_back((void*)CFBridgingRetain(destindexes));
      }
      mRenderArrays[pArray] = vertexArray;
    }
    
  }
  
}


#endif //   #ifndef __NUI_NO_GL__

#endif // _METAL_
