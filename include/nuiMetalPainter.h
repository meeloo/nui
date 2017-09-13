/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiMetalPainter_h__
#define __nuiMetalPainter_h__

//#include "nui.h"
#include "nuiDrawContext.h"

#ifndef __NUI_NO_GL__

class nuiMetalDebugGuard
{
public:
  nuiMetalDebugGuard(nglContext* pContext, const nglString& rString) : mpContext(pContext)
  {
#if defined DEBUG
    mpContext->StartMarkerGroup(rString.GetChars());
#endif
  }

  nuiMetalDebugGuard(nglContext* pContext, const char* pString) : mpContext(pContext)
  {
#if defined DEBUG
    mpContext->StartMarkerGroup(pString);
#endif
  }

  ~nuiMetalDebugGuard()
  {
#if defined DEBUG
    mpContext->StopMarkerGroup();
#endif
  }
  
  nglContext* mpContext = nullptr;
};

class nuiMetalPainter : public nuiPainter
{
public:
  nuiMetalPainter(nglContext* pContext);
  virtual ~nuiMetalPainter();
  
  virtual void StartRendering();
  virtual void SetState(const nuiRenderState& rState, bool ForceApply = false);
  virtual void DrawArray(nuiRenderArray* pArray);
  virtual void Clear(bool color, bool depth, bool stencil);
  virtual void BeginSession();
  virtual void EndSession();
  virtual void LoadMatrix(const nuiMatrix& rMatrix);
  virtual void MultMatrix(const nuiMatrix& rMatrix);
  virtual void PushMatrix();
  virtual void PopMatrix();
  virtual void LoadProjectionMatrix(const nuiRect& rViewport, const nuiMatrix& rMatrix);
  virtual void MultProjectionMatrix(const nuiMatrix& rMatrix);
  virtual void PushProjectionMatrix();
  virtual void PopProjectionMatrix();

  virtual void DestroyTexture(nuiTexture* pTexture);

  virtual void DestroySurface(nuiSurface* pSurface);

  virtual void DestroyRenderArray(nuiRenderArray* pArray);

  virtual nglImage* CreateImageFromGPUTexture(const nuiTexture* pTexture) const;
  
protected:
  class TextureInfo
  {
  public:
    bool mReload = false;
    void* mTexture = nullptr;
    void* mSampler = nullptr;
  };

  virtual void ResetMetalState();
  void SetSurface(nuiSurface* pSurface);
  void CreateSurface(nuiSurface* pSurface);

  void ApplyState(const nuiRenderState& rState);
  void ApplyScissor(const nuiRenderState& rState);
  void ApplyTexture(const nuiRenderState& rState, int slot);

  void UploadTexture(nuiTexture* pTexture, int slot);
  
  virtual void SetViewport();

  static uint32 mActiveContexts;
  nglContext* mpContext;
  nuiRenderState mFinalState;

  static std::map<nuiTexture*, TextureInfo> mTextures;
  static std::vector<nuiRenderArray*> mFrameArrays;
  static nglCriticalSection mTexturesCS;
  static nglCriticalSection mFrameArraysCS;
  static nglCriticalSection mRenderArraysCS;

  nglCriticalSection mRenderingCS;

  int64 dummy = 0;
  bool mScissorIsFlat = false;
  bool mTwoPassBlend;
  GLenum mSrcColor;
  GLenum mDstColor;
  GLenum mSrcAlpha;
  GLenum mDstAlpha;

  // Local cache:
  bool mClientVertex;
  bool mClientColor;
  bool mClientTexCoord;
  bool mMatrixChanged;
  float mR;
  float mG;
  float mB;
  float mA;
  GLenum mTexEnvMode;

  bool mUseShaders;

  // Only used for shaders:
  nglVector2f mTextureTranslate;
  nglVector2f mTextureScale;

  nuiShaderProgram* mpShader = nullptr;
  nuiShaderState* mpShaderState = nullptr;
  
  nuiShaderProgram* mpShader_TextureVertexColor;
  nuiShaderProgram* mpShader_TextureAlphaVertexColor;
  nuiShaderProgram* mpShader_TextureDifuseColor;
  nuiShaderProgram* mpShader_TextureAlphaDifuseColor;
  nuiShaderProgram* mpShader_DifuseColor;
  nuiShaderProgram* mpShader_VertexColor;
  nuiShaderProgram* mpShader_ClearColor;
  void* mpClearColor_pipelineDescriptor;
  
  nuiMatrix mSurfaceMatrix;
  
  void FinalizeSurfaces();
  void FinalizeTextures();
  void FinalizeRenderArrays();
  void _DestroySurface(nuiSurface* pSurface);
  void _DestroyTexture(nuiTexture* pTexture);
  void _DestroyRenderArray(nuiRenderArray* pArray);
  std::vector<nuiSurface*> mDestroyedSurfaces;
  std::vector<nuiTexture*> mDestroyedTextures;

  bool mViewportChanged = true;
  
  void* mDrawable = nullptr;
  void* mBackBuffer = nullptr;
  
  virtual int32 GetCurrentWidth() const;
  virtual int32 GetCurrentHeight() const;

};

#endif //   #ifndef __NUI_NO_GL__

#endif //__nuiMetalPainter_h__

