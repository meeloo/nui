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
  nuiMetalDebugGuard(const nglString& rString)
  {
#if defined DEBUG && defined _UIKIT_
    mpContext->StartMarkerGroup(rString.GetChars());
#endif
  }

  nuiMetalDebugGuard(const char* pString)
  {
#if defined DEBUG && defined _UIKIT_
    mpContext->StartMarkerGroup(pString);
#endif
  }

  ~nuiMetalDebugGuard()
  {
#if defined DEBUG && defined _UIKIT_
    mpContext->StopMarkerGroup();
#endif
  }
};

class nuiMetalPainter : public nuiPainter
{
public:
  nuiMetalPainter(nglContext* pContext);
  virtual ~nuiMetalPainter();
  
  virtual void SetSize(uint32 sizex, uint32 sizey);
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

  class RenderArrayInfo
  {
  public:

    static RenderArrayInfo* Create(nuiRenderArray* pRenderArray);
    static void Recycle(nuiMetalPainter::RenderArrayInfo* pInfo);

    void BindVertices() const;
    void BindStream(int index) const;
    void BindIndices(int index) const;
    void Draw() const;

    std::map<nuiShaderProgram*, GLint> mVAOs;
  private:
    RenderArrayInfo(nuiRenderArray* pRenderArray);
    ~RenderArrayInfo();
    void Destroy();
    void Rebind(nuiRenderArray* pRenderArray);

    nuiRenderArray* mpRenderArray;
    void* mpVertexBuffer;
    std::vector<GLuint> mIndexBuffers;
    std::vector<GLuint> mStreamBuffers;

    static nglCriticalSection mHeapCS;
    static std::list<RenderArrayInfo*> mHeap;
  };

  virtual void ResetMetalState();
  void SetSurface(nuiSurface* pSurface);
  void CreateSurface(nuiSurface* pSurface);

  void ApplyState(const nuiRenderState& rState);
  void ApplyTexture(const nuiRenderState& rState, int slot);

  void UploadTexture(nuiTexture* pTexture, int slot);
  
  virtual void SetViewport();

  static uint32 mActiveContexts;
  nglContext* mpContext;
  nuiRenderState mFinalState;

  static std::map<nuiTexture*, TextureInfo> mTextures;
  static std::vector<nuiRenderArray*> mFrameArrays;
  static std::map<nuiRenderArray*, RenderArrayInfo*> mRenderArrays;
  static nglCriticalSection mTexturesCS;
  static nglCriticalSection mFrameArraysCS;
  static nglCriticalSection mRenderArraysCS;

  nglCriticalSection mRenderingCS;

  int64 dummy = 0;
  int32 mScissorX;
  int32 mScissorY;
  int32 mScissorW;
  int32 mScissorH;
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

  GLint mDefaultFramebuffer;
  GLint mDefaultRenderbuffer;
  uint32 mOriginalWidth;
  uint32 mOriginalHeight;

  nuiShaderProgram* mpShader = nullptr;
  nuiShaderState* mpShaderState = nullptr;

  // Immediate mode setup:
  void SetVertexPointers(const nuiRenderArray& rArray);
  
  //  VBO Setup:
  void SetVertexBuffersPointers(const nuiRenderArray& rArray, RenderArrayInfo& rInfo);
  void SetStreamBuffersPointers(const nuiRenderArray& rArray, const RenderArrayInfo& rInfo, int index);
  
  // Reset vertices modes
  void ResetVertexPointers(const nuiRenderArray& rArray);
  
  
  nuiShaderProgram* mpShader_TextureVertexColor;
  nuiShaderProgram* mpShader_TextureAlphaVertexColor;
  nuiShaderProgram* mpShader_TextureDifuseColor;
  nuiShaderProgram* mpShader_TextureAlphaDifuseColor;
  nuiShaderProgram* mpShader_DifuseColor;
  nuiShaderProgram* mpShader_VertexColor;
  nuiShaderProgram* mpShader_ClearColor;
  
  nuiMatrix mSurfaceMatrix;
  
  RenderArrayInfo* mpCurrentRenderArrayInfo;
  nuiRenderArray* mpLastArray;
    
  void FinalizeSurfaces();
  void FinalizeTextures();
  void FinalizeRenderArrays();
  void _DestroySurface(nuiSurface* pSurface);
  void _DestroyTexture(nuiTexture* pTexture);
  void _DestroyRenderArray(nuiRenderArray* pArray);
  std::vector<nuiSurface*> mDestroyedSurfaces;
  std::vector<nuiTexture*> mDestroyedTextures;
  std::vector<nuiRenderArray*> mDestroyedRenderArrays;

  bool mViewportChanged = true;
  
  void* mDrawable = nullptr;
  void* mBackBuffer = nullptr;
};

#endif //   #ifndef __NUI_NO_GL__

#endif //__nuiMetalPainter_h__

