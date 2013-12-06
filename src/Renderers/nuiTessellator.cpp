/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

#if (defined _UIKIT_) || (defined _ANDROID_)
#    include "glu//GL/glu.h"
#    include "glu/libtess/tess.h"
#endif

class nuiPoint;

float squared_distance(const nglVectorf& p1, const nglVectorf& p2, const nglVectorf& p3)
{
  nglVectorf v = p3 - p2;
  nglVectorf w = p1 - p2;

  float c1 = w * v;
  float c2 = v * v;
  float b = c1 / c2;

  nglVectorf Pb = p2 + b * v;
  return nglVectorf(Pb - p1).SquaredLength();
}

float distance(const nglVectorf& p1, const nglVectorf& p2, const nglVectorf& p3)
{
  return sqrtf(squared_distance(p1, p2, p3));
}



GLUtesselator* nuiTessellator::mpTess = NULL;
uint32 nuiTessellator::mRefs = 0;

nuiTessellator::nuiTessellator(nuiPathGenerator* pPathGenerator)
{
  mAntialiasGradients = false;
  mpPath = pPathGenerator;
  mpShape = NULL;
  mpObject = NULL;
  mEdgeFlag = true;
  mOutline = false;

  if (!mpTess)
    mpTess = gluNewTess();

  mRefs++;
}

nuiTessellator::nuiTessellator(nuiShape* pShape)
{
  mAntialiasGradients = false;
  mpPath = NULL;
  mpShape = pShape;
  mpObject = NULL;
  mEdgeFlag = true;
  mOutline = false;

  if (!mpTess)
    mpTess = gluNewTess();

  mRefs++;
}

nuiTessellator::~nuiTessellator()
{
  if (!--mRefs)
  {
    if (mpTess)
    {
			gluDeleteTess(mpTess);
			mpTess = NULL;
    }
  }
}

nuiRenderObject* nuiTessellator::GenerateFromPath(float Quality)
{
  nuiPath Points;

  mpPath->Tessellate(Points, Quality);

  gluTessNormal(mpTess, 0,0,1);
  gluTessProperty(mpTess,GLU_TESS_TOLERANCE, 0);
  gluTessCallback(mpTess, GLU_TESS_BEGIN_DATA,    NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessBegin);
  gluTessCallback(mpTess, GLU_TESS_EDGE_FLAG_DATA,NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessEdgeFlag);
  gluTessCallback(mpTess, GLU_TESS_VERTEX_DATA,   NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessVertex);
  gluTessCallback(mpTess, GLU_TESS_END_DATA,      NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessEnd);
  gluTessCallback(mpTess, GLU_TESS_COMBINE_DATA,  NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessCombine);
  gluTessCallback(mpTess, GLU_TESS_ERROR_DATA,    NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessError);

  gluTessProperty(mpTess,GLU_TESS_BOUNDARY_ONLY, mOutline?GL_TRUE:GL_FALSE);
  gluTessProperty(mpTess,GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

  mpObject = new nuiRenderObject();
  mEdgeFlag = true;

  gluTessBeginPolygon(mpTess, this);

  gluTessBeginContour(mpTess);

  uint count = Points.GetCount();
  for (uint i = 0; i < count; i++)
  {
    nuiPoint& rPoint = Points[i];
    double vec[4] = { rPoint[0], rPoint[1], rPoint[2], 0 };
    gluTessVertex(mpTess, vec, (void*)mTempPoints.AddVertex(rPoint));
  }
  gluTessEndContour(mpTess);

  gluTessEndPolygon(mpTess); 

  nuiRenderObject* pObject = mpObject;
  mpObject = NULL;
  mTempPoints.Clear();
  return pObject;
}

nuiRenderObject* nuiTessellator::GenerateFromShape(float Quality)
{
  gluTessNormal(mpTess, 0,0,1);
  gluTessProperty(mpTess,GLU_TESS_TOLERANCE, 0);
  gluTessCallback(mpTess, GLU_TESS_BEGIN_DATA,    NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessBegin);
  gluTessCallback(mpTess, GLU_TESS_EDGE_FLAG_DATA,NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessEdgeFlag);
  gluTessCallback(mpTess, GLU_TESS_VERTEX_DATA,   NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessVertex);
  gluTessCallback(mpTess, GLU_TESS_END_DATA,      NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessEnd);
  gluTessCallback(mpTess, GLU_TESS_COMBINE_DATA,  NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessCombine);
  gluTessCallback(mpTess, GLU_TESS_ERROR_DATA,    NUI_GLU_CALLBACK &nuiTessellator::StaticInternalTessError);

  gluTessProperty(mpTess,GLU_TESS_BOUNDARY_ONLY, mOutline?GL_TRUE:GL_FALSE);
  nuiShape::Winding Winding = mpShape->GetWinding();
  if (Winding == nuiShape::eNone)
    Winding = nuiShape::eNonZero;
  gluTessProperty(mpTess,GLU_TESS_WINDING_RULE, Winding);

  mpObject = new nuiRenderObject();
  mEdgeFlag = true;

  uint32 countours = mpShape->GetContourCount();

  gluTessBeginPolygon(mpTess, this);
  
  for (uint32 contour = 0; contour < countours; contour++)
  {
    nuiPath Points;
    nuiContour* pContour = mpShape->GetContour(contour);
    NGL_ASSERT(pContour != NULL);

    pContour->Tessellate(Points, Quality);

    uint count = Points.GetCount();
    bool beginNext = true;
    if (count)
    {
      for (uint i = 0; i < count; i++)
      {
        nuiPoint& rPoint = Points[i];
        if (!beginNext && rPoint.GetType() == nuiPointTypeStop)
        {
          gluTessEndContour(mpTess);
          beginNext = true;
        }
        else
        {
          if (beginNext)
          {
            gluTessBeginContour(mpTess);
            beginNext = false;
          }

          GLdouble vec[3] = { rPoint[0], rPoint[1], rPoint[2] };
          gluTessVertex(mpTess, vec, (void*)mTempPoints.AddVertex(rPoint));
        }
      }
      gluTessEndContour(mpTess);
      beginNext = true;
    }
  }

  gluTessEndPolygon(mpTess); 

  nuiRenderObject* pObject = mpObject;
  mpObject = NULL;
  mTempPoints.Clear();
  return pObject;
}

nuiRenderObject* nuiTessellator::Generate(float Quality)
{
  if (mpPath)
    return GenerateFromPath(Quality);
  else if (mpShape)
    return GenerateFromShape(Quality);
  return NULL;
}




GLvoid nuiTessellator::StaticInternalTessBegin(GLenum type, void * polygon_data)
{
  ((nuiTessellator*)polygon_data)->InternalTessBegin(type);
}

GLvoid nuiTessellator::StaticInternalTessEdgeFlag(GLboolean flag, void * polygon_data)
{
  ((nuiTessellator*)polygon_data)->InternalTessEdgeFlag(flag);
}

GLvoid nuiTessellator::StaticInternalTessVertex(void * vertex_data, void * polygon_data)
{
  ((nuiTessellator*)polygon_data)->InternalTessVertex(vertex_data);
}

GLvoid nuiTessellator::StaticInternalTessEnd(void * polygon_data)
{
  ((nuiTessellator*)polygon_data)->InternalTessEnd();
}

GLvoid nuiTessellator::StaticInternalTessCombine(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData, void * polygon_data)
{
  ((nuiTessellator*)polygon_data)->InternalTessCombine(coords, vertex_data, weight, outData);
}

GLvoid nuiTessellator::StaticInternalTessError(GLenum ErrNo, void * polygon_data)
{
  ((nuiTessellator*)polygon_data)->InternalTessError(ErrNo);
}


void nuiTessellator::InternalTessBegin(GLenum type)
{
  nuiRenderArray* pArray = new nuiRenderArray(type, false, false, mOutline);
  pArray->EnableArray(nuiRenderArray::eVertex);
  if (mAntialiasGradients)
    pArray->EnableArray(nuiRenderArray::eNormal);

  //pArray->EnableArray(nuiRenderArray::eEdgeFlag);
  mpObject->AddArray(pArray);
}

void nuiTessellator::InternalTessEdgeFlag(GLboolean flag)
{
  mEdgeFlag = flag?true:false;
}

void nuiTessellator::InternalTessVertex(void* vertex_data)
{
  nuiRenderArray* pArray = mpObject->GetLastArray();
  pArray->SetVertex(mTempPoints[(unsigned long int)vertex_data]);
  //pArray->SetEdgeFlag(mEdgeFlag);
  pArray->PushVertex();

  if (mAntialiasGradients)
  {
    // Store the antialiasing gradients in the normal vectors:
    int index = pArray->GetSize() - 1;
    int offset = index %3;
    mEdge[offset] = mEdgeFlag;
    if (offset == 2)
    {
      // (i.e. we are at the last vertex for this triangle)
      // Then we can fix the normals to hold the distance from each vertex to the opposite edge line.
      nuiRenderArray::Vertex& rV1 = pArray->GetVertex(index - 2);
      nuiRenderArray::Vertex& rV2 = pArray->GetVertex(index - 1);
      nuiRenderArray::Vertex& rV3 = pArray->GetVertex(index - 0);
      nglVectorf p1(rV1.mX, rV1.mY, rV1.mZ);
      nglVectorf p2(rV2.mX, rV2.mY, rV2.mZ);
      nglVectorf p3(rV3.mX, rV3.mY, rV3.mZ);

      // The default is to have no edges at all:
      const float huge = 100;

      float d1 = distance(p1, p2, p3); // distance from p1 to [p2, p3]
      float d2 = distance(p2, p1, p3); // distance from p2 to [p1, p3]
      float d3 = distance(p3, p2, p1); // distance from p3 to [p2, p1]

      // p1 to p2 is an edge
      rV1.mNZ = 0;
      rV2.mNZ = 0;
      rV3.mNZ = d3;

      // p1 to p3 is an edge
      rV1.mNY = 0;
      rV2.mNY = d2;
      rV3.mNY = 0;

      // p2 to p3 is an edge
      rV1.mNX = d1;
      rV2.mNX = 0;
      rV3.mNX = 0;

      if (!mEdge[0]) // [1,2] not an edge
      {
        rV1.mNZ = huge;
        rV2.mNZ = huge;
      }
      if (!mEdge[1])  // [2,3] not an edge
      {
        rV2.mNX = huge;
        rV3.mNX = huge;
      }
      if (!mEdge[2])  // [3,1] not an edge
      {
        rV1.mNY = huge;
        rV3.mNY = huge;
      }
    }
  }
}

void nuiTessellator::InternalTessCombine(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData)
{
  *outData = (void*)mTempPoints.AddVertex(nuiPoint((float)(coords[0]), (float)(coords[1]), (float)(coords[2])));
}

void nuiTessellator::InternalTessEnd()
{
}

void nuiTessellator::InternalTessError(GLenum ErrNo)
{
#ifdef __NUI_NO_GL__
  NGL_OUT(_T("nui_glu tessellation error\n"));
#elsif !defined(_OPENGL_ES_)
  NGL_OUT(_T("nui_glu tessellation error: %s\n"), gluErrorString(ErrNo));
#endif
}



