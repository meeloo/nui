
/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


/****************************************************************************
 *
 * Constructor / Destructor
 *
 ****************************************************************************/
nuiDrawContext::nuiDrawContext(const nuiRect& rRect)
{
  mWidth = rRect.GetWidth();
  mHeight = rRect.GetHeight();
  mDebug = false;

  mClipOffsetX = mClipOffsetY = 0;

  mPermitAntialising = true;

  mpPainter = NULL;
  mpAATexture = nuiTexture::GetAATexture();
  
  mStateChanges = 1;
}

nuiDrawContext::~nuiDrawContext()
{
  SetTexture(NULL);
  SetShader(NULL, NULL);
  SetFont(NULL);
  if (mpAATexture)
    mpAATexture->Release();

  mpPainter = NULL;
  while (!mpRenderStateStack.empty())
  {
    PopState();
  }
}

void nuiDrawContext::StartRendering()
{ 
  mpPainter->StartRendering(); 
}

void nuiDrawContext::BeginSession()
{
  mpPainter->BeginSession();
}

void nuiDrawContext::EndSession()
{
  mpPainter->EndSession();
}

void nuiDrawContext::StopRendering()
{
  SetTexture(NULL);
  SetShader(NULL, NULL);
}


void nuiDrawContext::SetPainter(nuiPainter* pPainter)
{
//  pPainter->SetSize(GetWidth(), GetHeight());
  mpPainter = pPainter;
}

nuiPainter* nuiDrawContext::GetPainter() const
{
  return mpPainter;
}

void nuiDrawContext::SetState(const nuiRenderState& rState)
{
  mpPainter->SetState(rState);
}

void nuiDrawContext::AddBreakPoint()
{
  mpPainter->AddBreakPoint();
}

void nuiDrawContext::AddPrint(const char* str)
{
  mpPainter->AddPrint(str);
}

/****************************************************************************
 *
 * Clipping Support
 *
 ****************************************************************************/

// Clipping using Scissor :
void nuiDrawContext::PushClipping()
{
  mpPainter->PushClipping();
}

void nuiDrawContext::PopClipping()
{
  mpPainter->PopClipping();
}

bool nuiDrawContext::Clip(const nuiRect& rRect)
{
  nuiRect r;
  mpPainter->GetClipRect(r, true);
//  if (r == rRect)
//    return true;
  
  mpPainter->Clip(rRect);
  mStateChanges++;
  return true;
}

bool nuiDrawContext::ResetClipRect()
{
  mpPainter->ResetClipRect();
  mStateChanges++;
  return true;
}

bool nuiDrawContext::EnableClipping(bool set)
{
//  nuiRect r;
//  if (mpPainter->GetClipRect(r, false) == set)
//    return true;
  mpPainter->EnableClipping(set);
  mStateChanges++;
  return true;
}

bool nuiDrawContext::GetClipRect(nuiRect& rRect, bool LocalRect) const
{
  return mpPainter->GetClipRect(rRect, LocalRect);
}

/****************************************************************************
 *
 * Render state manipulation
 *
 ****************************************************************************/
void nuiDrawContext::PushState()
{
  nuiRenderState *pState = new nuiRenderState(mCurrentState);
  mpRenderStateStack.push(pState);
  //mStateChanges++;
}

void nuiDrawContext::PopState()
{
  nuiRenderState* pState = mpRenderStateStack.top();
  if (!(*pState == mCurrentState))
  {
    mStateChanges++;
    mCurrentState = *pState;
  }
  mpRenderStateStack.pop();
  delete pState;
}

const nuiRenderState& nuiDrawContext::GetState() const
{
  return mCurrentState;
}

bool nuiDrawContext::ResetState()
{
  static nuiRenderState Dummy;
  if (!(Dummy == mCurrentState))
  {
    mStateChanges++;
    mCurrentState = Dummy;
  }
  
  return true;
}

void nuiDrawContext::EnableBlending(bool val)       
{
  if (mCurrentState.mBlending != val)
  {
    mCurrentState.mBlending = val; 
    mStateChanges++;
  }
}

void nuiDrawContext::EnableTexturing(bool val)      
{ 
  if (mCurrentState.mTexturing != val)
  {
    mCurrentState.mTexturing = val;
    mStateChanges++;
  }
}

#if 0
void nuiGetBlendFuncFactors(nuiBlendFunc Func, GLenum& src, GLenum& dst)
{
  GLenum sfactor, dfactor;
  switch (Func)
  {
  default:
  case nuiBlendSource:        sfactor = GL_ONE;                   dfactor = GL_ZERO;                 break;
  case nuiBlendTransp:        sfactor = GL_SRC_ALPHA;             dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
  case nuiBlendClear:         sfactor = GL_ZERO;                  dfactor = GL_ZERO;                 break;
  case nuiBlendDest:          sfactor = GL_ZERO;                  dfactor = GL_ONE;                  break;
  case nuiBlendOver:          sfactor = GL_ONE;                   dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
  case nuiBlendOverRev:       sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_ONE;                  break;
  case nuiBlendIn:            sfactor = GL_DST_ALPHA;             dfactor = GL_ZERO;                 break;
  case nuiBlendInRev:         sfactor = GL_ZERO;                  dfactor = GL_SRC_ALPHA;            break;
  case nuiBlendOut:           sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_ZERO;                 break;
  case nuiBlendOutRev:        sfactor = GL_ZERO;                  dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
  case nuiBlendTop:           sfactor = GL_DST_ALPHA;             dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
  case nuiBlendTopRev:        sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_SRC_ALPHA;            break;
  case nuiBlendXOR:           sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
  case nuiBlendAdd:           sfactor = GL_ONE;                   dfactor = GL_ONE;                  break;
  case nuiBlendSaturate:      sfactor = GL_SRC_ALPHA_SATURATE;    dfactor = GL_ONE;                  break;

  case nuiBlendTranspClear:   sfactor = GL_SRC_ALPHA;             dfactor = GL_ZERO;                 break;
  case nuiBlendTranspInRev:   sfactor = GL_SRC_ALPHA;             dfactor = GL_SRC_ALPHA;            break;
  case nuiBlendTranspAdd:     sfactor = GL_SRC_ALPHA;             dfactor = GL_ONE;                  break;
  }
  src = sfactor;
  dst = dfactor;
}
#endif

// Premultiplied Alpha Blending rules:
void nuiGetBlendFuncFactors(nuiBlendFunc Func, GLenum& src, GLenum& dst)
{
  GLenum sfactor, dfactor;
  switch (Func)
  {
    default:
    case nuiBlendSource:        sfactor = GL_ONE;                   dfactor = GL_ZERO;                 break;
    case nuiBlendTransp:        sfactor = GL_ONE;                   dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
    case nuiBlendClear:         sfactor = GL_ZERO;                  dfactor = GL_ZERO;                 break;
    case nuiBlendDest:          sfactor = GL_ZERO;                  dfactor = GL_ONE;                  break;
    case nuiBlendOver:          sfactor = GL_ONE;                   dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
    case nuiBlendOverRev:       sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_ONE;                  break;
    case nuiBlendIn:            sfactor = GL_DST_ALPHA;             dfactor = GL_ONE;                  break;
    case nuiBlendInRev:         sfactor = GL_ZERO;                  dfactor = GL_SRC_ALPHA;            break;
    case nuiBlendOut:           sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_ZERO;                 break;
    case nuiBlendOutRev:        sfactor = GL_ZERO;                  dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
    case nuiBlendTop:           sfactor = GL_DST_ALPHA;             dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
    case nuiBlendTopRev:        sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_SRC_ALPHA;            break;
    case nuiBlendXOR:           sfactor = GL_ONE_MINUS_DST_ALPHA;   dfactor = GL_SRC_ALPHA;            break;
    case nuiBlendAdd:           sfactor = GL_ONE;                   dfactor = GL_ONE;                  break;
//    case nuiBlendSaturate:      sfactor = GL_SRC_ALPHA_SATURATE;    dfactor = GL_ONE;                  break;
    case nuiBlendSaturate:      sfactor = GL_DST_COLOR;             dfactor = GL_ONE_MINUS_SRC_ALPHA;  break;
      
    case nuiBlendTranspClear:   sfactor = GL_SRC_ALPHA;             dfactor = GL_ZERO;                 break;
    case nuiBlendTranspInRev:   sfactor = GL_SRC_ALPHA;             dfactor = GL_SRC_ALPHA;            break;
    case nuiBlendTranspAdd:     sfactor = GL_SRC_ALPHA;             dfactor = GL_ONE;                  break;
  }
  src = sfactor;
  dst = dfactor;
}


void nuiDrawContext::SetBlendFunc(nuiBlendFunc Func)
{
  if (mCurrentState.mBlendFunc != Func)
  {
    mCurrentState.mBlendFunc = Func;
    mStateChanges++;
  }
}

/****************************************************************************
 *
 * Texture manipulation
 *
 ****************************************************************************/

void nuiDrawContext::SetTexture(nuiTexture* pTex, int slot)
{
  nuiTexture* pOld = mCurrentState.mpTexture[slot];
  if (pTex == pOld)
    return;
  
  mCurrentState.mpTexture[slot] = pTex ;
  if (pTex)
  {
    pTex->CheckValid();
    pTex->Acquire();
  }
  if (pOld)
    pOld->Release();
  mStateChanges++;
}

bool nuiDrawContext::IsTextureCurrent(nuiTexture* pTex, int slot) const
{
  return mCurrentState.mpTexture[slot] == pTex;
}

nuiTexture* nuiDrawContext::GetTexture(int slot) const
{ 
  return mCurrentState.mpTexture[slot];
}

/****************************************************************************
 *
 * Shader manipulation
 *
 ****************************************************************************/

void nuiDrawContext::SetShader(nuiShaderProgram* pShader, nuiShaderState* pShaderState)
{
  nuiShaderProgram* pOld = mCurrentState.mpShader;
  if (pShader == pOld)
    return;

  mCurrentState.mpShader = pShader ;
  if (pShader)
  {
    //pShader->CheckValid();
    pShader->Acquire();

    if (!pShaderState)
    {
      pShaderState = pShader->GetCurrentState();
    }

    SetShaderState(pShaderState);
  }
  if (pOld)
    pOld->Release();
  mStateChanges++;
}

bool nuiDrawContext::IsShaderCurrent(nuiShaderProgram* pShader) const
{
  return mCurrentState.mpShader == pShader;
}

nuiShaderProgram* nuiDrawContext::GetShader() const
{
  return mCurrentState.mpShader;
}

void nuiDrawContext::SetShaderState(nuiShaderState* pState)
{
  if (pState)
    pState->Acquire();
  if (mCurrentState.mpShaderState)
    mCurrentState.mpShaderState->Release();
  mCurrentState.mpShaderState = pState;
  mStateChanges++;
}

nuiShaderState* nuiDrawContext::GetShaderState() const
{
  return mCurrentState.mpShaderState;
}

/****************************************************************************
 *
 * Brush manipulation
 *
 ****************************************************************************/

void nuiDrawContext::SetFillColor(const nuiColor& rColor)
{
  if (!(mCurrentState.mFillColor == rColor))
  {
    mCurrentState.mFillColor = rColor;
    mStateChanges++;
  }
}

void nuiDrawContext::SetStrokeColor(const nuiColor& rColor)
{
  if (!(mCurrentState.mStrokeColor == rColor))
  {
    mCurrentState.mStrokeColor = rColor;
    mStateChanges++;
  }
}

const nuiColor& nuiDrawContext::GetFillColor() const
{
  return mCurrentState.mFillColor;
}

const nuiColor& nuiDrawContext::GetStrokeColor() const
{
  return mCurrentState.mStrokeColor;
}

void nuiDrawContext::SetLineWidth(nuiSize Width)
{
  if (Width < 0.0)
    Width = -Width;
  if (Width == 0.0f)
    Width = 0.0001f;
  mCurrentState.mLineWidth = Width;
//  mStateChanges++;
}

void nuiDrawContext::SetLineJoin(nuiLineJoin join)
{
  mCurrentState.mLineJoin = join;
}

nuiLineJoin nuiDrawContext::GetLineJoin()const
{
  return mCurrentState.mLineJoin;
}

void nuiDrawContext::SetLineCap(nuiLineCap cap)
{
  mCurrentState.mLineCap = cap;
}

nuiLineCap nuiDrawContext::GetLineCap() const
{
  return mCurrentState.mLineCap;
}

void nuiDrawContext::SetMiterLimit(float limit)
{
  mCurrentState.mMitterLimit = limit;
}

float nuiDrawContext::GetMiterLimit() const
{
  return mCurrentState.mMitterLimit;
}

void nuiDrawContext::EnableAntialiasing(bool set)
{
  if (mCurrentState.mAntialiasing != set)
  {
    mCurrentState.mAntialiasing = set;
    mStateChanges++;
  }
}

bool nuiDrawContext::GetAntialiasing() const
{
  return mCurrentState.mAntialiasing;
}

void nuiDrawContext::SetWinding(nuiShape::Winding Rule)
{
  mCurrentState.mWinding = Rule;
//  mStateChanges++;
}

nuiShape::Winding nuiDrawContext::GetWinding() const
{
  return mCurrentState.mWinding;
}

/****************************************************************************
 *
 * Drawing function
 *
 ****************************************************************************/
void nuiDrawContext::SetClearColor(const nuiColor& ClearColor)
{
  if (!(mCurrentState.mClearColor == ClearColor))
  {
    mCurrentState.mClearColor = ClearColor;
    mStateChanges++;
  }
}

void nuiDrawContext::Clear(bool color, bool depth, bool stencil)
{ 
  if (mStateChanges)
    mpPainter->SetState(mCurrentState);
  mStateChanges = 0;
  mpPainter->Clear(color, depth, stencil);
}

void nuiDrawContext::DrawShape(nuiShape* pShape, nuiShapeMode Mode, float Quality)
{
  NGL_ASSERT(pShape != NULL);
  PushState();
  switch (Mode)
  {
  case eStrokeShape:
    {
      nuiRenderObject* pObject = pShape->Outline(Quality, mCurrentState.mLineWidth, mCurrentState.mLineJoin, mCurrentState.mLineCap, mCurrentState.mMitterLimit);
      if (!pObject)
        return;
      SetFillColor(GetStrokeColor());
      //SetTexture(mpAATexture);
      //EnableTexturing(true);
      EnableBlending(true);
      SetBlendFunc(nuiBlendTransp);//GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      DrawObject(*pObject);
      delete pObject;
    }
    break;
  case eFillShape:
    {
      nuiTessellator* pTess = new nuiTessellator(pShape);
      pTess->SetFill(true);
      nuiRenderObject* pObject = pTess->Generate(Quality);
      if (pObject)
        DrawObject(*pObject);
      delete pObject;
      delete pTess;
    }
    break;
  case eStrokeAndFillShape:
    {
      {
        nuiRenderObject* pObject = pShape->Fill(Quality);
        if (pObject)
          DrawObject(*pObject);
        delete pObject;
      }

      {
        nuiRenderObject* pObject = pShape->Outline(Quality, mCurrentState.mLineWidth, mCurrentState.mLineJoin, mCurrentState.mLineCap, mCurrentState.mMitterLimit);
        SetFillColor(GetStrokeColor());
        //SetTexture(mpAATexture);
        //EnableTexturing(true);
        EnableBlending(true);
        SetBlendFunc(nuiBlendTransp);//GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (pObject)
          DrawObject(*pObject);
        delete pObject;
      }
    }
    break;
  case eDefault: //?
    break;
  }
  PopState();
}



/****************************************************************************
 *
 * Font manipulation
 *
 ****************************************************************************/

bool nuiDrawContext::SetFont (nuiFont* pFont, bool AlreadyAcquired)
{
  nuiFont* pOld = mCurrentState.mpFont;
  if (pOld == pFont)
    return true;
  
  mCurrentState.mpFont = pFont;
  if (mCurrentState.mpFont && !AlreadyAcquired)
    mCurrentState.mpFont->Acquire();
  if (pOld)
    pOld->Release();
  return true;
}

nuiFont* nuiDrawContext::GetFont() const
{
  return mCurrentState.mpFont;
}

void nuiDrawContext::SetTextColor(const nuiColor& rColor)
{
  mCurrentState.mTextColor = rColor;
}

nuiColor nuiDrawContext::GetTextColor() const
{
  return mCurrentState.mTextColor;
}

void nuiDrawContext::DrawText(nuiSize x, nuiSize y, const nglString& rString, bool AlignGlyphPixels)
{
  mCurrentState.mpFont->Print(this,x,y,rString, AlignGlyphPixels);
}

void nuiDrawContext::DrawText(nuiSize x, nuiSize y, const nuiTextLayout& rLayout, bool AlignGlyphPixels)
{
  rLayout.Print(this, x, y, AlignGlyphPixels);
  //mCurrentState.mpFont->Print(this,x,y,rLayout, AlignGlyphPixels);
}

void nuiDrawContext::PermitAntialiasing(bool Set)
{
  mPermitAntialising = Set;
}

bool nuiDrawContext::IsAntialiasingPermited() const
{
  return mPermitAntialising;
}

void nuiDrawContext::DrawImage(const nuiRect& rDest, const nuiRect& rSource)
{
  DrawImageQuad(rDest.mLeft, rDest.mTop, 
                rDest.mRight, rDest.mTop, 
                rDest.mRight, rDest.mBottom, 
                rDest.mLeft, rDest.mBottom, 
                rSource);
}

void nuiDrawContext::DrawImageQuad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, const nuiRect& rSource)
{
  bool texturing = mCurrentState.mTexturing;
  if (!texturing)
    EnableTexturing(true);

  nuiSize tx0,tx1,tx2,tx3;
  nuiSize ty0,ty1,ty2,ty3;

  tx0 = rSource.mLeft;
  ty0 = rSource.mTop;

  tx1 = rSource.mRight;
  ty1 = rSource.mTop;
  
  tx2 = rSource.mRight;
  ty2 = rSource.mBottom;
  
  tx3 = rSource.mLeft;
  ty3 = rSource.mBottom;
  
  mCurrentState.mpTexture[0]->ImageToTextureCoord(tx0, ty0);
  mCurrentState.mpTexture[0]->ImageToTextureCoord(tx1, ty1);
  mCurrentState.mpTexture[0]->ImageToTextureCoord(tx2, ty2);
  mCurrentState.mpTexture[0]->ImageToTextureCoord(tx3, ty3);

  nuiRenderArray* pArray = new nuiRenderArray(GL_TRIANGLE_STRIP);
  pArray->Reserve(4);
  pArray->EnableArray(nuiRenderArray::eVertex, true);
  pArray->EnableArray(nuiRenderArray::eTexCoord, true);
  pArray->EnableArray(nuiRenderArray::eColor, true);

  // 1
  pArray->SetTexCoords(tx0,ty0); 
  pArray->SetVertex(x0, y0);
  pArray->SetColor(mCurrentState.mFillColor);
  pArray->PushVertex();

  pArray->SetTexCoords(tx3,ty3); 
  pArray->SetVertex(x3, y3);
  pArray->PushVertex();

  pArray->SetTexCoords(tx1,ty1); 
  pArray->SetVertex(x1, y1);
  pArray->PushVertex();
  
  // 2
  pArray->SetTexCoords(tx2,ty2); 
  pArray->SetVertex(x2, y2);
  pArray->PushVertex();
  
  
  DrawArray(pArray);

  if (!texturing)
    EnableTexturing(texturing);
}

void nuiDrawContext::DrawGradient(const nuiGradient& rGradient, const nuiRect& rEnclosingRect, const nuiVector2& rP1, const nuiVector2& rP2)
{
  DrawGradient(rGradient, rEnclosingRect, rP1[0], rP1[1], rP2[0], rP2[1]);
}

static void IntersectLines(float x0,float y0,float x1,float y1, 
                           float x2,float y2,float x3,float y3,
                           float& xi,float& yi)
{
  x1 += x0;
  y1 += y0;
  x3 += x2;
  y3 += y2;

  float a1,b1,c1, // constants of linear equations
    a2,b2,c2,
    det_inv,  // the inverse of the determinant of the coefficient
    m1,m2;    // the slopes of each line

  // compute slopes, note the kludge for infinity, however, this will
  // be close enough

  if ((x1-x0)!=0)
    m1 = (y1-y0)/(x1-x0);
  else
    m1 = (float)1e+10;   // close enough to infinity

  if ((x3-x2)!=0)
    m2 = (y3-y2)/(x3-x2);
  else
    m2 = (float)1e+10;   // close enough to infinity

  // compute constants
  a1 = m1;
  a2 = m2;

  b1 = -1;
  b2 = -1;

  c1 = (y0-m1*x0);
  c2 = (y2-m2*x2);

  // compute the inverse of the determinate

  if (m1 >= (float)1e+10 || m1 >= (float)1e+10)
    det_inv = 0;
  else
    det_inv = 1/(a1*b2 - a2*b1);

  // use Kramers rule to compute xi and yi

  xi=((b1*c2 - b2*c1)*det_inv);
  yi=((a2*c1 - a1*c2)*det_inv);

}


void nuiDrawContext::DrawGradient(const nuiGradient& rGradient, const nuiRect& rEnclosingRect, nuiSize x1, nuiSize y1, nuiSize x2, nuiSize y2)
{
  nuiVector2 vec(x2 - x1, y2 - y1);
  nuiVector2 para(-vec[1], vec[0]);
  nuiVector2 vec1(vec);
  nuiVector2 para1(para);
  vec1.Normalize();
  para1.Normalize();

  // What Quadrant are we in?:
  //         |
  //     a   |   b
  //         |
  //  ----------------
  //         |
  //     c   |   d
  //         |
  float xa, xb, xc, xd;
  float ya, yb, yc, yd;
  float x, y;
  float xp, yp;
  float xx, yy;
  float xxp, yyp;

  xa = xc = rEnclosingRect.Left();
  xb = xd = rEnclosingRect.Right();
  ya = yb = rEnclosingRect.Top();
  yc = yd = rEnclosingRect.Bottom();

  if (x1 < x2)
  {
    // Go from a to d or c to b
    if (y1 == y2)
    {
      x  = xa; y  = ya;
      xp = xc; yp = yc;
      xx = xd; yy = yd;
      xxp= xb; yyp= yb;
    }
    else if (y1 < y2)
    {
      // a to d
      IntersectLines(xa,ya, para1[0], para1[1], xb, yb, vec1[0], vec1[1], x, y);
      IntersectLines(xa,ya, para1[0], para1[1], xc, yc, vec1[0], vec1[1], xp, yp);
      IntersectLines(xd,yd, para1[0], para1[1], xc, yc, vec1[0], vec1[1], xx, yy);
      IntersectLines(xd,yd, para1[0], para1[1], xb, yb, vec1[0], vec1[1], xxp, yyp);
    }
    else
    {
      // c to d
      IntersectLines(xc,yc, para1[0], para1[1], xa, ya, vec1[0], vec1[1], x, y);
      IntersectLines(xc,yc, para1[0], para1[1], xd, yd, vec1[0], vec1[1], xp, yp);
      IntersectLines(xb,yb, para1[0], para1[1], xd, yd, vec1[0], vec1[1], xx, yy);
      IntersectLines(xb,yb, para1[0], para1[1], xa, ya, vec1[0], vec1[1], xxp, yyp);
    }
  }
  else
  {
    if (y1 == y2)
    {
      x  = xd; y  = yd;
      xp = xb; yp = yb;
      xx = xa; yy = ya;
      xxp= xc; yyp= yc;
    }
    else if (y1 < y2)
    {
      // b to c
      IntersectLines(xb,yb, para1[0], para1[1], xd, yd, vec1[0], vec1[1], x, y);
      IntersectLines(xb,yb, para1[0], para1[1], xa, ya, vec1[0], vec1[1], xp, yp);
      IntersectLines(xc,yc, para1[0], para1[1], xa, ya, vec1[0], vec1[1], xx, yy);
      IntersectLines(xc,yc, para1[0], para1[1], xd, yd, vec1[0], vec1[1], xxp, yyp);
    }
    else
    {
      // d to a
      IntersectLines(xd,yd, para1[0], para1[1], xc, yc, vec1[0], vec1[1], x, y);
      IntersectLines(xd,yd, para1[0], para1[1], xb, yb, vec1[0], vec1[1], xp, yp);
      IntersectLines(xa,ya, para1[0], para1[1], xb, yb, vec1[0], vec1[1], xx, yy);
      IntersectLines(xa,ya, para1[0], para1[1], xc, yc, vec1[0], vec1[1], xxp, yyp);
    }
  }

  float startx,starty;
  float startxp,startyp;
  float stopx,stopy;
  float stopxp,stopyp;

  if (y1 != y2)
  {
    IntersectLines(x1, y1, para1[0], para1[1], x,  y,  vec1[0], vec1[1], startx,  starty);
    IntersectLines(x1, y1, para1[0], para1[1], xp, yp, vec1[0], vec1[1], startxp, startyp);
    IntersectLines(x2, y2, para1[0], para1[1], x,  y,  vec1[0], vec1[1], stopx,   stopy);
    IntersectLines(x2, y2, para1[0], para1[1], xp, yp, vec1[0], vec1[1], stopxp,  stopyp);
  }
  else
  {
    startx  = x1; starty  = y;
    startxp = x1; startyp = yp;
    stopx   = x2; stopy   = y;
    stopxp  = x2; stopyp  = yp;
  }

  nuiGradientStopList::const_iterator it = rGradient.GetStopList().begin();
  nuiGradientStopList::const_iterator end = rGradient.GetStopList().end();

  float px1, py1;
  float px2, py2;

  PushClipping();
  Clip(rEnclosingRect);
  EnableClipping(true);

  nuiRenderArray* pArray = new nuiRenderArray(GL_TRIANGLE_STRIP);
  pArray->EnableArray(nuiRenderArray::eVertex);
  pArray->EnableArray(nuiRenderArray::eColor);
  
  //  nuiRenderArray Array(GL_LINES);
//  pArray->SetVertexElements(3);
//  pArray->SetColorElements(4);

  nuiColor col = it->second;
  pArray->SetVertex(x, y);
  pArray->SetColor(col);
  pArray->PushVertex();
  pArray->SetVertex(xp, yp);
  pArray->PushVertex();

  for ( ; it != end; ++it)
  {
    float r = it->first;
    float rm = 1.0f - r;
    px1 = startx * rm + stopx * r;
    py1 = starty * rm + stopy * r;
    px2 = startxp * rm + stopxp * r;
    py2 = startyp * rm + stopyp * r;

    col = it->second;
    pArray->SetColor(col);
    pArray->SetVertex(px2, py2);
    pArray->PushVertex();
    pArray->SetVertex(px1, py1);
    pArray->PushVertex();
  }

  pArray->SetVertex(xx, yy);
  pArray->PushVertex();
  pArray->SetVertex(xxp, yyp);
  pArray->PushVertex();

  DrawArray(pArray);

  PopClipping();
}

static void nuiDrawRect(const nuiRect& out, nuiRenderArray& rArray, float strokesize)
{
  rArray.SetMode(GL_TRIANGLE_STRIP);
  rArray.Reserve(8);
  nuiRect in(out);
  in.Grow(-strokesize, -strokesize);
  
  rArray.SetVertex(out.Left(), out.Top()); rArray.PushVertex();
  rArray.SetVertex(in.Left(), in.Top()); rArray.PushVertex();

  rArray.SetVertex(out.Right(), out.Top()); rArray.PushVertex();
  rArray.SetVertex(in.Right(), in.Top()); rArray.PushVertex();
  
  rArray.SetVertex(out.Right(), out.Bottom()); rArray.PushVertex();
  rArray.SetVertex(in.Right(), in.Bottom()); rArray.PushVertex();
  
  rArray.SetVertex(out.Left(), out.Bottom()); rArray.PushVertex();
  rArray.SetVertex(in.Left(), in.Bottom()); rArray.PushVertex();

  rArray.SetVertex(out.Left(), out.Top()); rArray.PushVertex();
  rArray.SetVertex(in.Left(), in.Top()); rArray.PushVertex();
}

void nuiDrawContext::DrawRect(const nuiRect& rRect, nuiShapeMode Mode)
{

  if (Mode == eStrokeAndFillShape || Mode == eStrokeShape)
  {
    nuiRect rect(rRect);

    GLenum mode = GL_LINE_LOOP;

    if (rect.mRight - rect.mLeft <= 1.0f)
    {
      mode = GL_TRIANGLE_STRIP;
    }
    else
    {
      rect.mRight -= 1.0f;
    }
    
    if (rect.mBottom - rect.mTop <= 1.0f)
    {
      mode = GL_TRIANGLE_STRIP;
    }
    else
    {
      rect.mBottom -= 1.0f;
    }

    // Draw the stroke in all cases:
    if (mode == GL_TRIANGLE_STRIP)
    {
      nuiColor back = mCurrentState.mFillColor;
      mCurrentState.mFillColor = mCurrentState.mStrokeColor;
      DrawRect(rRect, eFillShape);
      mCurrentState.mFillColor = back;
      return;
    }
    else
    {
      nuiRenderArray* pStrokeArray = new nuiRenderArray(mode);
      pStrokeArray->EnableArray(nuiRenderArray::eColor, true);
      pStrokeArray->SetColor(mCurrentState.mStrokeColor);
      nuiDrawRect(rRect, *pStrokeArray, mCurrentState.mLineWidth);

      DrawArray(pStrokeArray);
    }
  }

  if (Mode == eStrokeAndFillShape)
  {
    if ((rRect.mRight - rRect.mLeft <= 2.0f) || (rRect.mBottom - rRect.mTop <= 2.0f))
      return;

    nuiRect rect(rRect);
    float v = mCurrentState.mLineWidth;
    rect.Grow(-v, -v);
    // Draw the filled part:
    nuiRenderArray* pFillArray = new nuiRenderArray(GL_TRIANGLE_STRIP);
    pFillArray->EnableArray(nuiRenderArray::eVertex, true);
    pFillArray->EnableArray(nuiRenderArray::eColor, true);
    pFillArray->Reserve(4);
    
    pFillArray->SetColor(mCurrentState.mFillColor);
    pFillArray->SetVertex(rect.mLeft, rect.mTop);
    pFillArray->PushVertex();

    pFillArray->SetVertex(rect.mRight, rect.mTop);
    pFillArray->PushVertex();

    pFillArray->SetVertex(rect.mLeft, rect.mBottom);
    pFillArray->PushVertex();
    
    pFillArray->SetVertex(rect.mRight, rect.mBottom);
    pFillArray->PushVertex();
    
    DrawArray(pFillArray);
  }
  else if (Mode == eFillShape)
  {
    nuiRect rect(rRect);
    // Draw the filled rectangle:
    nuiRenderArray* pFillArray = new nuiRenderArray(GL_TRIANGLE_STRIP);
    pFillArray->EnableArray(nuiRenderArray::eVertex, true);
    pFillArray->EnableArray(nuiRenderArray::eColor, true);
    pFillArray->Reserve(4);

    pFillArray->SetColor(mCurrentState.mFillColor);
    pFillArray->SetVertex(rect.mLeft, rect.mTop);
    pFillArray->PushVertex();

    pFillArray->SetVertex(rect.mRight, rect.mTop);
    pFillArray->PushVertex();

    pFillArray->SetVertex(rect.mLeft, rect.mBottom);
    pFillArray->PushVertex();
    
    pFillArray->SetVertex(rect.mRight, rect.mBottom);
    pFillArray->PushVertex();

    DrawArray(pFillArray);
  }
}

void nuiDrawContext::DrawLine(float x1, float y1, float x2, float y2)
{
  if (x1 == x2 && y1 == y2)
  {
    DrawPoint(x1, y1);
  }
  else
  {
    nuiShape* shp = new nuiShape();
    shp->LineTo(nuiPoint(x1, y1));
    shp->LineTo(nuiPoint(x2, y2));
    DrawShape(shp, eStrokeShape);
    shp->Release();
  }
}

void nuiDrawContext::DrawPoint(float x, float y)
{
  nuiShape* shp = new nuiShape();
  shp->AddCircle(x, y, mCurrentState.mLineWidth);
  nuiColor col(mCurrentState.mFillColor);
  SetFillColor(mCurrentState.mStrokeColor);
  DrawShape(shp, eFillShape);
  SetFillColor(col);
  shp->Release();
}

void nuiDrawContext::DrawPoint(const nuiVector2& rPoint)
{
  DrawPoint(rPoint[0], rPoint[1]);
}

void nuiDrawContext::DrawArray(nuiRenderArray* pArray)
{
  pArray->SetDebug(mDebug);

  uint32 size = pArray->GetSize();
  if (!size)
  {
    pArray->Release();
    return;
  }

#ifdef _DEBUG_
  {
    // Error checking:
    if (!pArray->GetIndexArrayCount())
    {
      switch (pArray->GetMode())
      {
        case GL_POINTS:
          NGL_ASSERT(size);
          break;
        case GL_LINES:
          NGL_ASSERT(!(size & 1));
          break;
        case GL_LINE_LOOP:
          NGL_ASSERT(size > 1);
          break;
        case GL_LINE_STRIP:
          NGL_ASSERT(size > 1);
          break;
        case GL_TRIANGLES:
          NGL_ASSERT(size > 2 && !(size % 3));
          break;
        case GL_TRIANGLE_STRIP:
          NGL_ASSERT(size > 2);
          break;
        case GL_TRIANGLE_FAN:
          NGL_ASSERT(size > 2);
          break;
#ifndef _OPENGL_ES_
        case GL_QUADS:
          NGL_ASSERT(size > 3 && !(size % 4));
          break;
        case GL_QUAD_STRIP:
          NGL_ASSERT(size > 3);
          break;
        case GL_POLYGON:
          NGL_ASSERT(size > 3);
          break;
#endif
      }
    }
  }
#endif

  if (mStateChanges)
    mpPainter->SetState(mCurrentState);
  mStateChanges = 0;

  mpPainter->DrawArray(pArray);
}

void nuiDrawContext::DrawObject(const nuiRenderObject& rObject)
{
  uint32 count = rObject.GetSize();
  for (uint32 i = 0; i < count; i++)
  {
    DrawArray(rObject.GetArray(i));
  }
}

void nuiDrawContext::Translate(const nuiVector& Vector)
{
  Translate(Vector[0],Vector[1]);
}

void nuiDrawContext::PushMatrix()
{
  mpPainter->PushMatrix();
}

void nuiDrawContext::PopMatrix()
{
  mpPainter->PopMatrix();
}

void nuiDrawContext::LoadMatrix(const nuiMatrix& Matrix)
{
  mpPainter->LoadMatrix(Matrix);
}

void nuiDrawContext::MultMatrix(const nuiMatrix& Matrix)
{
  if (Matrix.IsIdentity())
    return;
  mpPainter->MultMatrix(Matrix);
}

void nuiDrawContext::Translate(nuiSize X, nuiSize Y, nuiSize Z)
{
  if (X == 0 && Y == 0 && Z == 0)
    return;
  
  nuiMatrix m;
  m.SetTranslation(X, Y, Z);
  MultMatrix(m);
}

void nuiDrawContext::Scale(nuiSize X, nuiSize Y, nuiSize Z)
{
  if (X == 1.0 && Y == 1.0 && Z == 1.0)
    return;
  
  nuiMatrix m;
  m.SetScaling(X, Y, Z);
  MultMatrix(m);
}

void nuiDrawContext::LoadIdentity()
{
  nuiMatrix m;
  LoadMatrix(m);
}

void nuiDrawContext::GetMatrix(nuiMatrix& rMatrix) const
{
  rMatrix = mpPainter->GetMatrix();
}

const nuiMatrix& nuiDrawContext::GetMatrix() const
{
  return mpPainter->GetMatrix();
}

//////// Projection matrix:

void nuiDrawContext::PushProjectionMatrix()
{
  mpPainter->PushProjectionMatrix();
}

void nuiDrawContext::PopProjectionMatrix()
{
  mpPainter->PopProjectionMatrix();
}

void nuiDrawContext::LoadProjectionMatrix(const nuiRect& rViewport, const nuiMatrix& Matrix)
{
  mpPainter->LoadProjectionMatrix(rViewport, Matrix);
}

void nuiDrawContext::MultProjectionMatrix(const nuiMatrix& Matrix)
{
  if (Matrix.IsIdentity())
    return;
  mpPainter->MultProjectionMatrix(Matrix);
}

void nuiDrawContext::LoadProjectionIdentity()
{
  nuiMatrix m;
  LoadProjectionMatrix(nuiRect(0.0f, 0.0f, mWidth, mHeight), m);
}

void nuiDrawContext::GetProjectionMatrix(nuiMatrix& rMatrix) const
{
  rMatrix = mpPainter->GetProjectionMatrix();
}

const nuiMatrix& nuiDrawContext::GetProjectionMatrix() const
{
  return mpPainter->GetProjectionMatrix();
}

void nuiDrawContext::Set2DProjectionMatrix(const nuiRect& rRect)
{
//  printf("Set2DProjectionMatrix: %s\n", rRect.GetValue().GetChars());
  nuiMatrix m;
  m.Translate(-1.0f, 1.0f, 0.0f);
  m.Scale(2.0f/rRect.GetWidth(), -2.0f/rRect.GetHeight(), 1.0f);
  LoadProjectionMatrix(rRect, m);
}

void nuiDrawContext::SetPerspectiveProjectionMatrix(const nuiRect& rRect, float FovY, float Aspect, float Near, float Far)
{
  nuiMatrix m;
  m.SetPerspective(FovY, Aspect, Near, Far);
  LoadProjectionMatrix(rRect, m);
}

void nuiDrawContext::SetOrthoProjectionMatrix(const nuiRect& rRect, float Left, float Right, float Bottom, float Top, float Near, float Far)
{
  nuiMatrix m;
  m.SetOrtho(Left, Right, Bottom, Top, Near, Far);
  m.Translate(-1.0f, 1.0f, 0.0f);
  LoadProjectionMatrix(rRect, m);
}

void nuiDrawContext::SetFrustumProjectionMatrix(const nuiRect& rRect, float Left, float Right, float Bottom, float Top, float Near, float Far)
{
  nuiMatrix m;
  m.SetFrustum(Left, Right, Bottom, Top, Near, Far);
  LoadProjectionMatrix(rRect, m);
}

///////
void nuiDrawContext::EnableColorBuffer(bool set)
{
  if (mCurrentState.mColorBuffer != set)
  {
    mCurrentState.mColorBuffer = set;
    mStateChanges++;
  }
}

uint32 nuiDrawContext::GetClipStackSize() const
{
  return mpPainter->GetClipStackSize();
}

#define SHADE_ALPHA 0.3f

void nuiDrawContext::DrawShade(const nuiRect& rSourceRect, const nuiRect& rShadeRect, const nuiColor& rTint)
{
  bool texturing = mCurrentState.mTexturing;
  bool blending = mCurrentState.mBlending;
  nuiBlendFunc blendfunc;
  blendfunc = mCurrentState.mBlendFunc;

  if (!blending)
    EnableBlending(true);
  if (blendfunc != nuiBlendTransp)
    SetBlendFunc(nuiBlendTransp);

  nuiSize ShadeSize = rSourceRect.mLeft - rShadeRect.mLeft;

  nuiTexture* pShade = ::nuiTexture::GetTexture(nglString("NUI_Shade_LUT"));

  if (!pShade)
  {
    // Left shadow
    const uint32 size = 16;
    uint8 pLUT[size * 4];
    uint i;
    for (i = 0; i<size; i++)
    {
      pLUT[0+(i*4)] = 0;
      pLUT[1+(i*4)] = 0;
      pLUT[2+(i*4)] = 0;
      float p = (float)i * (255.0f / (float)size);
      pLUT[3+(i*4)] = ToBelow(p);
    }

    nglImageInfo info(false);
    info.mBitDepth = 32;
    info.mBufferFormat = eImageFormatRaw;
    info.mBytesPerLine = size * 4;
    info.mBytesPerPixel = 4;
    info.mHeight = 1;
    info.mWidth = size;
    info.mpBuffer = (char*)pLUT;
    info.mPixelFormat = eImagePixelRGBA;

    pShade = nuiTexture::GetTexture(info, true);
    pShade->SetSource("NUI_Shade_LUT");

    NGL_ASSERT(pShade);

    pShade->SetMinFilter(GL_LINEAR);
    pShade->SetMagFilter(GL_LINEAR);
#ifndef _OPENGL_ES_
    pShade->SetWrapS(GL_CLAMP);
    pShade->SetWrapT(GL_CLAMP);
#else
    pShade->SetWrapS(GL_CLAMP_TO_EDGE);
    pShade->SetWrapT(GL_CLAMP_TO_EDGE);  
#endif
    pShade->SetEnvMode(GL_MODULATE);
  }

  nuiColor transp(rTint);
  nuiColor opaque(rTint);
  transp.Multiply(0.0f);
  opaque.Multiply(SHADE_ALPHA);

  if (!texturing)
    EnableTexturing(true);
  
  SetTexture(pShade);

  nuiRenderArray* pArray = new nuiRenderArray(GL_TRIANGLES);
  pArray->EnableArray(nuiRenderArray::eVertex);
  pArray->EnableArray(nuiRenderArray::eColor);
  pArray->EnableArray(nuiRenderArray::eTexCoord);
  pArray->Reserve(42);

  // Top Left:
  pArray->SetColor(transp);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mTop);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mTop);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  ///
  pArray->SetColor(transp);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mTop);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  pArray->SetTexCoords(0, 0);
  pArray->SetColor(opaque);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  // Left
  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom);
  pArray->PushVertex();

  ///
  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mBottom);
  pArray->PushVertex();

  // Left Corner:
  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  ///
  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft-ShadeSize,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  // bottom shadow
  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  ///
  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mLeft,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  // Right Corner
  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  ///
  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom+ShadeSize);
  pArray->PushVertex();

  // Right
  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();
  
  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();
  
  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mBottom);
  pArray->PushVertex();

  ///
  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mBottom);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mBottom);
  pArray->PushVertex();

  // Top Right
  pArray->SetColor(transp);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mTop);
  pArray->PushVertex();

  pArray->SetColor(transp);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mTop);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  ///
  pArray->SetColor(transp);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mTop);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(0, 0);
  pArray->SetVertex(rSourceRect.mRight+ShadeSize,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();

  pArray->SetColor(opaque);
  pArray->SetTexCoords(1, 0);
  pArray->SetVertex(rSourceRect.mRight,rSourceRect.mTop+ShadeSize);
  pArray->PushVertex();


  DrawArray(pArray);

  if (!texturing)
    EnableTexturing(false);

  if (!blending)
    EnableBlending(blending);
  
  if (blendfunc != nuiBlendTransp)
    SetBlendFunc(blendfunc);
}

#define INACTIVE_SHADE_SIZE 8
#define ACTIVE_SHADE_SIZE 15

void nuiDrawContext::DrawWindowShade(const nuiRect& rRect, const nuiColor& rColor, bool Active)
{
  int ShadeSize = Active? ACTIVE_SHADE_SIZE : INACTIVE_SHADE_SIZE;
  nuiRect ShadeRect = rRect;
  ShadeRect.mLeft -= ShadeSize;
  ShadeRect.mRight += ShadeSize;
  ShadeRect.mTop += ShadeSize;
  ShadeRect.mBottom += ShadeSize;

  DrawShade(rRect,ShadeRect, rColor);
}

void nuiDrawContext::DrawMenuWindow(nuiWidget* pWidget, const nuiRect& rRect)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get("nuiDefaultDecorationPopupMenu");
  NGL_ASSERT(pFrame);

  nuiRect rect;
  rect.Set(rRect.Left() - pFrame->GetBorder(nuiLeft, pWidget), rRect.Top() - pFrame->GetBorder(nuiTop, pWidget),
           rRect.GetWidth() + pFrame->GetBorder(nuiLeft, pWidget) + pFrame->GetBorder(nuiRight, pWidget),
           rRect.GetHeight() + pFrame->GetBorder(nuiTop, pWidget) + pFrame->GetBorder(nuiBottom, pWidget));

  pFrame->Draw(this, pWidget, rect);
  pFrame->Release();
}


void nuiDrawContext::DrawTreeHandle(const nuiRect& rRect, bool IsOpened, nuiSize TREE_HANDLE_SIZE, const nuiColor& rColor)
{
  nuiFrame* pDeco = NULL;
  if (IsOpened)
    pDeco = (nuiFrame*)nuiDecoration::Get("nuiDefaultDecorationArrowOpen");
  else
    pDeco = (nuiFrame*)nuiDecoration::Get("nuiDefaultDecorationArrowClose");
  if (pDeco)
  {
    const nuiRect& rectSrc = pDeco->GetIdealClientRect(NULL);

    nuiSize x,y;
    x = rRect.Left() + (int)((rRect.GetWidth() - rectSrc.GetWidth()) / 2.f);
    y = rRect.Top() + (int)((rRect.GetHeight() - rectSrc.GetHeight()) / 2.f);
    nuiRect rectDest(x, y, rectSrc.GetWidth() , rectSrc.GetHeight());

    pDeco->SetColor(rColor);
    pDeco->Draw(this, NULL, rectDest);

    pDeco->Release();
  }
  else
  {
    SetFillColor(rColor);
    nuiRenderArray* array = new nuiRenderArray(GL_TRIANGLES);
    float l = MIN(rRect.GetWidth(), rRect.GetHeight());
    nuiRect r(l, l);
    r.SetPosition(nuiCenter, rRect);
    const float x = r.Left();
    const float y = r.Top();
    const float w = r.GetWidth();
    const float h = r.GetHeight();

    if (IsOpened)
    {
      array->SetVertex(x,       y);     array->PushVertex();
      array->SetVertex(x + w,   y);     array->PushVertex();
      array->SetVertex(x + w/2, y + h); array->PushVertex();
    }
    else
    {
      array->SetVertex(x,     y);       array->PushVertex();
      array->SetVertex(x + w, y + h/2); array->PushVertex();
      array->SetVertex(x,     y + h);   array->PushVertex();
    }

    DrawArray(array);
  }

}


void nuiDrawContext::DrawSelectionBackground(nuiWidget* pWidget, const nuiRect& rRect)
{
  nuiDecoration* pDeco = nuiDecoration::Get("nuiDefaultDecorationSelectionBackground");
  if (!pDeco)
    return;

  pDeco->Draw(this, pWidget, rRect);
  pDeco->Release();
}


void nuiDrawContext::DrawSelectionForeground(nuiWidget* pWidget, const nuiRect& rRect)
{
  nuiDecoration* pDeco = nuiDecoration::Get("nuiDefaultDecorationSelectionForeground");
  if (!pDeco)
    return;

  pDeco->Draw(this, pWidget, rRect);
  pDeco->Release();
}


void nuiDrawContext::DrawMarkee(const nuiRect& rRect, const nuiColor& rColor)
{
  EnableAntialiasing(false);
  EnableBlending(true);
  SetBlendFunc(nuiBlendTransp);
  EnableTexturing(false);
  nuiColor col = rColor;
  SetFillColor(col);
  col.Multiply(.3f);
  SetStrokeColor(col);
  DrawRect(rRect, eStrokeAndFillShape);
}

void nuiDrawContext::DrawScrollBarBackground(nuiScrollBar* pScroll)
{
  const nuiRect& rRect = pScroll->GetRangeRect();

  nuiDecoration* pDeco = NULL;
  if (pScroll->GetOrientation() == nuiVertical)
    pDeco = nuiDecoration::Get("nuiDefaultDecorationScrollBarVerticalBkg");
  else
    pDeco = nuiDecoration::Get("nuiDefaultDecorationScrollBarHorizontalBkg");
  NGL_ASSERT(pDeco);

  nuiRect rectDest(0.0f, 0.0f, rRect.GetWidth(), rRect.GetHeight());
  pDeco->Draw(this, pScroll, rectDest);
  pDeco->Release();
}


void nuiDrawContext::DrawScrollBarForeground(nuiScrollBar* pScroll)
{
  nuiDecoration* pDeco = NULL;
  if (pScroll->GetOrientation() == nuiVertical)
    pDeco = nuiDecoration::Get("nuiDefaultDecorationScrollBarVerticalHdl");
  else
    pDeco = nuiDecoration::Get("nuiDefaultDecorationScrollBarHorizontalHdl");
  NGL_ASSERT(pDeco);

  nuiRect rRect = pScroll->GetThumbRect();
  rRect.Grow(-1.f, -1.f);

  pDeco->Draw(this, pScroll, rRect);
  pDeco->Release();
}

void nuiDrawContext::DrawSliderBackground(nuiSlider* pSlider)
{
  nuiRect rect = pSlider->GetRect().Size();
  nuiDecoration* pDeco = NULL;

  nuiSize min = pSlider->GetHandlePosMin();
  nuiSize max = pSlider->GetHandlePosMax();

  if (pSlider->GetOrientation() == nuiVertical)
  {
    pDeco = nuiDecoration::Get("nuiDefaultDecorationSliderVerticalBkg");
    if (!pDeco)
      return;
    nuiSize x = (int)((rect.GetWidth() - pDeco->GetIdealClientRect(pSlider).GetWidth()) / 2);
    nuiSize w = pDeco->GetIdealClientRect(pSlider).GetWidth();
    rect.Set(x, min, w, max-min);
  }
  else
  {
    pDeco = nuiDecoration::Get("nuiDefaultDecorationSliderHorizontalBkg");
    if (!pDeco)
      return;
    nuiSize y = (int)((rect.GetHeight() - pDeco->GetIdealClientRect(pSlider).GetHeight()) / 2);
    nuiSize h = pDeco->GetIdealClientRect(pSlider).GetHeight();
    rect.Set(min, y, max - min, h);
  }

  if (pDeco)
  {
    pDeco->Draw(this, pSlider, rect);
    pDeco->Release();
  }
}

void nuiDrawContext::DrawSliderForeground(nuiSlider* pSlider)
{
  nuiRect rect = pSlider->GetRect().Size();
  float start;
  const nuiRange& Range = pSlider->GetRange();

  start  = Range.ConvertToUnit(Range.GetValue());
  nuiDecoration* pDeco = NULL;

  if (pSlider->GetOrientation() == nuiVertical)
  {
    pDeco = nuiDecoration::Get("nuiDefaultDecorationSliderVerticalHdl");

    if (pDeco)
    {
      const nuiRect& srcRect = pDeco->GetIdealClientRect(pSlider);

      nuiSize h = rect.GetHeight() - srcRect.GetHeight();
      rect.mTop = h - (start * h);
      rect.mBottom = rect.mTop + srcRect.GetHeight();

      rect.mLeft = (int)(rect.GetWidth() - srcRect.GetWidth())/2;
      rect.mRight = rect.mLeft + srcRect.GetWidth();
    }
  }
  else
  {
    pDeco = nuiDecoration::Get("nuiDefaultDecorationSliderHorizontalHdl");
    if (pDeco)
    {
      const nuiRect& srcRect = pDeco->GetIdealClientRect(pSlider);

      rect.mLeft = (start * (rect.GetWidth() - srcRect.GetWidth()));
      rect.mRight = rect.mLeft + srcRect.GetWidth();

      rect.mTop = (int)(rect.GetHeight() - srcRect.GetHeight())/2;
      rect.mBottom = rect.mTop + srcRect.GetHeight();
    }
  }

  if (pDeco)
  {
    pDeco->Draw(this, pSlider, rect);
    pDeco->Release();
  }
  else
  {
    if (pSlider->GetOrientation() == nuiVertical)
    {
      nuiRect srcRect = pSlider->GetRect().Size();
      srcRect.SetHeight(srcRect.GetWidth() / 2);

      nuiSize h = rect.GetHeight() - srcRect.GetHeight();
      rect.mTop = h - (start * h);
      rect.mBottom = rect.mTop + srcRect.GetHeight();

      rect.mLeft = (int)(rect.GetWidth() - srcRect.GetWidth())/2;
      rect.mRight = rect.mLeft + srcRect.GetWidth();
    }
    else
    {
      nuiRect srcRect = pSlider->GetRect().Size();
      srcRect.SetWidth(srcRect.GetHeight() / 2);

      rect.mLeft = (start * (rect.GetWidth() - srcRect.GetWidth()));
      rect.mRight = rect.mLeft + srcRect.GetWidth();

      rect.mTop = (int)(rect.GetHeight() - srcRect.GetHeight())/2;
      rect.mBottom = rect.mTop + srcRect.GetHeight();
    }

    DrawRect(rect, eFillShape);
  }
}














/////////////////////
#pragma mark -

nuiDrawContext *nuiDrawContext::CreateDrawContext(const nuiRect& rRect, nuiPainter* pPainter)
{
  nuiDrawContext* pC = new nuiDrawContext(rRect);
  pPainter->SetSize(ToNearest(rRect.GetWidth()), ToNearest(rRect.GetHeight()));
  pC->SetPainter(pPainter);
  pC->Set2DProjectionMatrix(rRect);
  return pC;
}



int nuiDrawContext::GetWidth() const
{
  return (int)mWidth;
}

int nuiDrawContext::GetHeight() const
{
  return (int)mHeight;
}

void nuiDrawContext::SetSurface(nuiSurface* pSurface)
{
  mpPainter->SetSurface(pSurface);
  mStateChanges++;
}

nuiSurface* nuiDrawContext::GetSurface() const
{
  return mpPainter->GetSurface();
}

