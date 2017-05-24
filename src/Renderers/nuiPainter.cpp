/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

///////////////////////////////////
// nuiPainter implementation:

static const nuiClipper nuiMaxClipper(nuiRect(-10000000, -10000000, 20000000, 20000000), false);
std::set<nuiPainter*> nuiPainter::gpPainters;

nuiPainter::nuiPainter(nglContext* pContext)
{
  ResetStats();
  mWidth = 0;
  mHeight = 0;
  mpClippingStack.push(nuiMaxClipper);
  mMatrixStack.push(nuiMatrix());
  mProjectionMatrixStack.push(nuiMatrix());
  mProjectionViewportStack.push(nuiRect());

  mDummyMode = false;
  mpSurface = NULL;

  mAngle=0;

  mEnableDrawArray = true;
}

nuiPainter::~nuiPainter() 
{
}


void nuiPainter::StartRendering()
{
  int32 w = GetCurrentWidth(), h = GetCurrentHeight();
  nuiMatrix m;
  m.Translate(-1.0f, 1.0f, 0.0f);
  m.Scale(2.0f/(float)w, -2.0f/(float)h, 1.0f);


  mpClippingStack = std::stack<nuiClipper>();
  mMatrixStack = std::stack<nglMatrixf>();
  mProjectionMatrixStack = std::stack<nglMatrixf>();
  mProjectionViewportStack = std::stack<nuiRect>();

  mpClippingStack.push(nuiMaxClipper);
  mMatrixStack.push(nuiMatrix());
  mProjectionMatrixStack.push(m);
  mProjectionViewportStack.push(nuiRect(0, 0, w, h));
}

void nuiPainter::PushMatrix()
{
  NGL_ASSERT(!mMatrixStack.empty());
  mMatrixStack.push(mMatrixStack.top());
}

void nuiPainter::PopMatrix()
{
  NGL_ASSERT(!mMatrixStack.empty());
  mMatrixStack.pop();
}

void nuiPainter::LoadMatrix(const nuiMatrix& Matrix)
{
  NGL_ASSERT(!mMatrixStack.empty());
  mMatrixStack.top() = Matrix;
}

void nuiPainter::MultMatrix(const nuiMatrix& Matrix)
{
  NGL_ASSERT(!mMatrixStack.empty());
  mMatrixStack.top() *= Matrix;
}

const nuiMatrix& nuiPainter::GetMatrix() const
{
  NGL_ASSERT(!mMatrixStack.empty());
  return mMatrixStack.top();
}

///////
void nuiPainter::PushProjectionMatrix()
{
  NGL_ASSERT(!mProjectionMatrixStack.empty());
  mProjectionMatrixStack.push(mProjectionMatrixStack.top());
  mProjectionViewportStack.push(mProjectionViewportStack.top());
}

void nuiPainter::PopProjectionMatrix()
{
  NGL_ASSERT(!mProjectionMatrixStack.empty());
  mProjectionMatrixStack.pop();
  mProjectionViewportStack.pop();
}

void nuiPainter::LoadProjectionMatrix(const nuiRect& rViewport, const nuiMatrix& rMatrix)
{
  NGL_ASSERT(!mProjectionMatrixStack.empty());
  NGL_ASSERT(rViewport.Left() >= 0);
  NGL_ASSERT(rViewport.Top() >= 0);
  mProjectionMatrixStack.top() = rMatrix;

  nuiMatrix LocalMatrix(mMatrixStack.empty()?nuiMatrix():mMatrixStack.top());
  nuiVector vec1(rViewport.Left(), rViewport.Top(), 0.0f);
  nuiVector vec2(rViewport.Right(), rViewport.Bottom(), 0.0f);
  vec1 = LocalMatrix * vec1;
  vec2 = LocalMatrix * vec2;
  mProjectionViewportStack.top().Set(vec1[0], vec1[1], vec2[0], vec2[1], false);
  NGL_ASSERT(mProjectionViewportStack.top().Left() >= 0);
  NGL_ASSERT(mProjectionViewportStack.top().Top() >= 0);
}

void nuiPainter::MultProjectionMatrix(const nuiMatrix& rMatrix)
{
  NGL_ASSERT(!mProjectionMatrixStack.empty());
  mProjectionMatrixStack.top() *= rMatrix;
  NGL_ASSERT(mProjectionMatrixStack.top().GetTranslation()[0] >= 0);
  NGL_ASSERT(mProjectionMatrixStack.top().GetTranslation()[1] >= 0);
}

const nuiMatrix& nuiPainter::GetProjectionMatrix() const
{
  NGL_ASSERT(!mProjectionMatrixStack.empty());
  return mProjectionMatrixStack.top();
}

const nuiRect& nuiPainter::GetProjectionViewport() const
{
  NGL_ASSERT(!mProjectionMatrixStack.empty());
  return mProjectionViewportStack.top();
}


// Clipping using Scissor :
void nuiPainter::PushClipping()
{
  NGL_ASSERT(!mpClippingStack.empty());
  mpClippingStack.push(mpClippingStack.top());
//  NGL_OUT("Push Clip (%s)\n", mpClippingStack.top().GetValue().GetChars());
}

void nuiPainter::PopClipping()
{
  NGL_ASSERT(!mpClippingStack.empty());
  mpClippingStack.pop();
//  NGL_OUT("Pop Clip to %s\n", mpClippingStack.top().GetValue().GetChars());
  NGL_ASSERT(!mpClippingStack.empty());
}

void nuiPainter::Clip(const nuiRect& rRect)
{
  nuiVector p1(rRect.mLeft, rRect.mTop, 0.0f),p2(rRect.mRight, rRect.mBottom, 0.0f);
  nuiMatrix m(GetMatrix());
  p1 = m * p1;
  p2 = m * p2;
  nuiRect l(p1[0], p1[1], p2[0], p2[1], false);
  if (mpClippingStack.top().mEnabled)
  {
  /*bool res = (unused)*/ mpClippingStack.top().Intersect(mpClippingStack.top(),l);
  }
  else
  {
    /*bool res = (unused)*/ mpClippingStack.top() = l;
  }
//  NGL_OUT("Clip %s -> %s\n", l.GetValue().GetChars(), mpClippingStack.top().GetValue().GetChars());
}

void nuiPainter::ResetClipRect()
{
  NGL_ASSERT(!mpClippingStack.empty());
  int32 w = GetCurrentWidth(), h = GetCurrentHeight();
  mpClippingStack.top().Set(0, 0, w, h);
//  NGL_OUT("Reset Clip to %s\n", mpClippingStack.top().GetValue().GetChars());
}

void nuiPainter::EnableClipping(bool set)
{
  NGL_ASSERT(!mpClippingStack.empty());
  mpClippingStack.top().mEnabled = set;
}

bool nuiPainter::GetClipRect(nuiRect& rRect, bool LocalRect) const
{
  if (mpClippingStack.top().mEnabled)
  {
    rRect = mpClippingStack.top();
  }
  else
  {
    rRect.Set(0, 0, GetCurrentWidth(), GetCurrentHeight());
  }
  
  if (LocalRect)
  {
    // Transform the rect with the inverse of the current matrix
    nglMatrixf m(GetMatrix());
    m.InvertHomogenous();
    nglVectorf v1(rRect.Left(), rRect.Top(), 0, 1);
    nglVectorf v2(rRect.Right(), rRect.Bottom(), 0, 1);
    v1 = m * v1;
    v2 = m * v2;
    rRect.Set(v1[0], v1[1], v2[0], v2[1], false);
  }
  return mpClippingStack.top().mEnabled;
}


void nuiPainter::ResetStats()
{
  mRenderOperations = 0;
  mVertices = 0;
  mBatches = 0;
}

uint32 nuiPainter::GetRenderOperations() const
{
  return mRenderOperations;
}

uint32 nuiPainter::GetVertices() const
{
  return mVertices;
}

uint32 nuiPainter::GetBatches() const
{
  return mBatches;
}

size_t nuiPainter::GetClipStackSize() const
{
  return mpClippingStack.size();
}


uint32 nuiPainter::GetRectangleTextureSupport() const
{
  return 0;
}

void nuiPainter::DEBUG_EnableDrawArray(bool set) const
{
  mEnableDrawArray = set;
}

bool nuiPainter::DEBUG_GetEnableDrawArray() const
{
  return mEnableDrawArray;
}

uint32 nuiPainter::mNeedTextureBackingStore = 0;

void nuiPainter::AddNeedTextureBackingStore()
{
  mNeedTextureBackingStore++;
  if (mNeedTextureBackingStore)
  {
    nuiTexture::RetainBuffers(true);
  }
}

void nuiPainter::DelNeedTextureBackingStore()
{
  mNeedTextureBackingStore--;
  if (!mNeedTextureBackingStore)
  {
    nuiTexture::RetainBuffers(false);
  }
}

void nuiPainter::SetSurface(nuiSurface* pSurface)
{
//  NGL_OUT("nuiPainter::SetSurface %p\n", pSurface);
  
  if (pSurface)
    pSurface->Acquire();
  if (mpSurface)
    mpSurface->Release();
  
  mpSurface = pSurface;
}

void nuiPainter::CreateSurface(nuiSurface* pSurface)
{
//  NGL_OUT("nuiPainter::SetSurface %p\n", pSurface);
  if (pSurface)
    pSurface->Acquire();
  if (mpSurface)
    mpSurface->Release();
  
  mpSurface = pSurface;
}


nuiSurface* nuiPainter::GetSurface() const
{
  return mpSurface;
}


void nuiPainter::GetSize(uint32& rX, uint32& rY) const
{
  rX = GetCurrentWidth();
  rY = GetCurrentHeight();
}

const nuiRenderState& nuiPainter::GetState() const
{
  return *mpState;
}

void nuiPainter::SetDummyMode(bool set)
{
  mDummyMode = set;
}

bool nuiPainter::GetDummyMode() const
{
  return mDummyMode;
}

void nuiPainter::SetAngle(int32 Angle)
{
  Angle = Angle % 360;
  while (Angle < 0)
    Angle += 360;
  mAngle = Angle;
}

int32 nuiPainter::GetAngle() const
{
  if (mpSurface)
    return 0;
  return mAngle;
}

int32 nuiPainter::GetCurrentWidth() const
{
  if (mpSurface)
    return mpSurface->GetWidth();
  return mWidth;
}

int32 nuiPainter::GetCurrentHeight() const
{
  if (mpSurface)
    return mpSurface->GetHeight();
  return mHeight;
}


// Debug:
void nuiPainter::AddBreakPoint()
{
  // do nothing by default, this is only used to debug defered rendering (i.e. nuiMetaPainter).
}

void nuiPainter::AddPrint(const char* str)
{

}

void nuiPainter::BroadcastDestroyTexture(nuiTexture* pTexture)
{
  for (auto pPainter : gpPainters)
  {
    pPainter->DestroyTexture(pTexture);
  }
}

void nuiPainter::BroadcastDestroySurface(nuiSurface* pSurface)
{
  for (auto pPainter : gpPainters)
  {
    pPainter->DestroySurface(pSurface);
  }
}

void nuiPainter::BroadcastDestroyRenderArray(nuiRenderArray* pArray)
{
  for (auto pPainter : gpPainters)
  {
    pPainter->DestroyRenderArray(pArray);
  }
}

