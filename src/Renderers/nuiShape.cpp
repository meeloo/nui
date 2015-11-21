/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


// class nuiShape
nuiShape::nuiShape()
{
  mWinding = eNone;
}

nuiShape::nuiShape(const nuiShape& rShape)
{
  NGL_ASSERT(0);
}

nuiShape& nuiShape::operator=(const nuiShape& rShape)
{
  NGL_ASSERT(0);
  return *this;
}

nuiShape::~nuiShape()
{
  // BEWARE: it the debuger gets you here then you have a nuiShape leak (some object have forgotten to release their shapes)!
  Clear();
  //NUI_ADD_EVENT(Changed);
}

void nuiShape::Clear()
{
  std::vector<nuiContour*>::const_iterator it;
  std::vector<nuiContour*>::const_iterator end = mpContours.end();

  for (it = mpContours.begin(); it != end; ++it)
    (*it)->Release();

  EmptyCaches();

  mpContours.clear();
}

void nuiShape::AddContour(nuiContour* pContour)
{
  mpContours.push_back(pContour);
}

void nuiShape::AddContour()
{
  nuiContour* pContour = new nuiContour();
  mpContours.push_back(pContour);
}

void nuiShape::CloseContour()
{
  if (mpContours.empty())
    return;
  mpContours.back()->Close();
}

void nuiShape::ArcTo(float X, float Y, float XRadius, float YRadius, float Angle, bool LargeArc, bool Sweep)
{
  if (mpContours.empty())
    AddContour();

  mpContours.back()->ArcTo(nuiPoint(X, Y), XRadius, YRadius, Angle, LargeArc, Sweep);
}

void nuiShape::AddLines(const nuiPath& rVertices)
{
  if (mpContours.empty())
    AddContour();

  mpContours.back()->AddLines(rVertices);
}

void nuiShape::AddPath(const nuiPath& rVertices)
{
  if (mpContours.empty() || mpContours.back()->GetElementsCount())
    AddContour();

  const uint count = rVertices.GetCount();
  for (uint i = 0; i < count; i++)
  {
    const nuiPoint& rPoint = rVertices[i];
    switch (rPoint.GetType())
    {
    case nuiPointTypeStop:
      AddContour();
      break;
    case nuiPointTypeNormal:
      if (rPoint[0] == 0.0f && rPoint[1] == 0.0f)
      {
        NGL_ASSERT(false);
      }
      LineTo(rPoint);
      break;
    default:
      NGL_ASSERT(false);
      break;
    }
  }
}

void nuiShape::LineTo(const nuiPoint& rVertex)
{
  if (mpContours.empty())
    AddContour();

  mpContours.back()->LineTo(rVertex);
}

void nuiShape::AddSpline(const nuiSpline& rSpline)
{
  if (mpContours.empty())
    AddContour();

  mpContours.back()->AddSpline(rSpline);
}

void nuiShape::AddPathGenerator(nuiPathGenerator* pPath)
{
  if (mpContours.empty())
    AddContour();

  mpContours.back()->AddPathGenerator(pPath);
}

nuiContour* nuiShape::GetContour(uint Index) const
{
  NGL_ASSERT(Index < mpContours.size());
  return mpContours[Index];
}

uint nuiShape::GetContourCount() const
{
  return mpContours.size();
}


bool nuiShape::Tessellate(nuiPath& rVertices, float Quality) const
{
  bool res = true;
  std::vector<nuiContour*>::const_iterator it;
  std::vector<nuiContour*>::const_iterator end = mpContours.end();

  for (it = mpContours.begin(); it != end; ++it)
  {
    res = res && (*it)->Tessellate(rVertices,Quality);
    rVertices.StopPath();
  }
  return res;
}

nuiShape::Winding nuiShape::GetWinding() const
{
  return mWinding;
}

void nuiShape::SetWinding(nuiShape::Winding Rule)
{
  mWinding = Rule;
  //Changed();
}

void nuiShape::EmptyCaches()
{
}



#define CIRCLE_FACTOR (1.0/3.5)

void nuiShape::AddCircle(float X, float Y, float Radius, bool CCW, float Quality)
{
  if (Radius <= 0)
    return;

  nuiContour* pContour = new nuiContour();
  AddContour(pContour);

  uint count = ToAbove((double)(2.0 * Quality * M_PI * (double)Radius * CIRCLE_FACTOR));
  float step = 2.0f * (float)M_PI / (float)count;
  if (CCW)
    step = -step;
  for (uint i = 0; i <= count; i++)
  {
    float angle = step * (float) i;
    float x = X + sin(angle) * Radius;
    float y = Y + cos(angle) * Radius;

    pContour->LineTo(nuiPoint(x, y, 0));
  }
}

void nuiShape::AddEllipse(float X, float Y, float XRadius, float YRadius, bool CCW, float Quality)
{
  nuiContour* pContour = new nuiContour();
  AddContour(pContour);

  uint count = ToAbove((double)(M_PI * Quality * (XRadius+YRadius) * CIRCLE_FACTOR));
  float step = 2.0f * (float)M_PI / (float)count;
  if (CCW)
    step = -step;
  for (uint i = 0; i <= count; i++)
  {
    float angle = step * (float) i;
    float x = X + sin(angle) * XRadius;
    float y = Y + cos(angle) * YRadius;

    pContour->LineTo(nuiPoint(x, y, 0));
  }
}

void nuiShape::AddRect(const nuiRect& rRect, bool CCW)
{
  AddContour();
  mpContours.back()->AddRect(rRect, CCW);
}

void nuiShape::AddRoundRect(const nuiRect& rRect, float Radius, bool CCW, float Quality)
{
  nuiRect rect(rRect);
  rect.Right() = MAX(rect.Left(), rect.Right()-1);
  rect.Bottom() = MAX(rect.Top(), rect.Bottom()-1);

  Radius = MIN(Radius, MIN(rect.GetWidth()/2, rect.GetHeight()/2));

  nuiContour* pContour = new nuiContour();
  AddContour(pContour);

  if (!CCW)
  {
    // Top Left
    pContour->LineTo(nuiPoint(rect.Left(), rect.Top() + Radius));
    pContour->ArcTo(nuiPoint(rect.Left() + Radius, rect.Top()), Radius, Radius, 0, false, true);

    // Top
    pContour->LineTo(nuiPoint(rect.Right() - Radius, rect.Top(), 0));

    // Top Right
    pContour->ArcTo(nuiPoint(rect.Right(), rect.Top() + Radius), Radius, Radius, 0, false, true);

    // Right
    pContour->LineTo(nuiPoint(rect.Right(), rect.Bottom() - Radius, 0));

    // Bottom Right
    pContour->ArcTo(nuiPoint(rect.Right() - Radius, rect.Bottom()), Radius, Radius, 0, false, true);

    // Bottom
    pContour->LineTo(nuiPoint(rect.Left() + Radius, rect.Bottom(), 0));


    // Bottom Left
    pContour->ArcTo(nuiPoint(rect.Left(), rect.Bottom() - Radius), Radius, Radius, 0, false, true);

    // Left
    pContour->LineTo(nuiPoint(rect.Left(), rect.Top() + Radius));
  }
  else
  {
    // Top Left
    pContour->LineTo(nuiPoint(rect.Left() + Radius, rect.Top()));
    pContour->ArcTo(nuiPoint(rect.Left(), rect.Top() + Radius), Radius, Radius, 0, false, false);

    // Top
    pContour->LineTo(nuiPoint(rect.Left(), rect.Bottom() - Radius, 0));

    // Top Right
    pContour->ArcTo(nuiPoint(rect.Left() + Radius, rect.Bottom()), Radius, Radius, 0, false, false);

    // Right
    pContour->LineTo(nuiPoint(rect.Right() - Radius, rect.Bottom(), 0));

    // Bottom Right
    pContour->ArcTo(nuiPoint(rect.Right(), rect.Bottom() - Radius), Radius, Radius, 0, false, false);

    // Bottom
    pContour->LineTo(nuiPoint(rect.Right(), rect.Top() +  Radius, 0));


    // Bottom Left
    pContour->ArcTo(nuiPoint(rect.Right() - Radius, rect.Top()), Radius, Radius, 0, false, false);

    // Left
    pContour->LineTo(nuiPoint(rect.Left() + Radius, rect.Top()));
  }
  pContour->Close();
}

nuiRect nuiShape::GetRect()
{
  nuiRect rect;
  std::vector<nuiContour*>::const_iterator it;
  std::vector<nuiContour*>::const_iterator end = mpContours.end();

  for (it = mpContours.begin(); it != end; ++it)
    rect.Union(rect, (*it)->GetRect());
  return rect;
}

nuiPoint nuiShape::GetStartPoint() const
{
  if (mpContours.empty())
    return nuiPoint();
  return mpContours.front()->GetStartPoint();
}

nuiPoint nuiShape::GetEndPoint() const
{
  if (mpContours.empty())
    return nuiPoint();
  return mpContours.back()->GetEndPoint();
}

nuiRenderObject* nuiShape::Fill(float Quality)
{
  nuiTessellator* pTess = new nuiTessellator(this);
  pTess->SetFill(true);
  nuiRenderObject* pObj = pTess->Generate(Quality);
  delete pTess;

  return pObj;
}

nuiRenderObject* nuiShape::Outline(float Quality, float LineWidth, nuiLineJoin LineJoin, nuiLineCap LineCap, float MiterLimit)
{
  nuiOutliner* Outliner = new nuiOutliner(this, LineWidth);
  Outliner->SetLineJoin(LineJoin);
  Outliner->SetLineCap(LineCap);
  Outliner->SetMiterLimit(MiterLimit);

  nuiPath outline;
  Outliner->Tessellate(outline, 1.0);
  Outliner->Release();

  nuiPolyLine* polyline = new nuiPolyLine(outline);
  nuiTessellator tesselator(polyline);
  nuiRenderObject* pObject = tesselator.Generate();
  polyline->Release();
  return pObject;

}

// Adapted from https://github.com/paulhoux/Cinder-Samples/blob/master/GeometryShader/assets/shaders/lines1.geom
static nuiRenderArray* StrokeSubPath(const std::vector<nuiVector>& subpath, float LineWidth, nuiLineJoin LineJoin, nuiLineCap LineCap, float MiterLimit)
{
  const float HalfLineWidth = LineWidth / 2;
  nuiRenderArray* pArray = new nuiRenderArray(GL_TRIANGLE_STRIP);
//  pArray->EnableArray(nuiRenderArray::eTexCoord);
  pArray->EnableArray(nuiRenderArray::eNormal);
  pArray->EnableArray(nuiRenderArray::eColor);

  size_t count = subpath.size() - 2;
  for (size_t i = 0; i < count; i++)
  {
    nuiVector p0 = subpath[i];
    nuiVector p1 = subpath[i+1];
    nuiVector p2 = subpath[i+2];

    nuiVector v0 = p1 - p0;
    nuiVector v1 = p2 - p1;

    v0.Normalize();
    v1.Normalize();

    // determine the normal of each of the 3 segments (previous, current, next)
    nuiVector n0 = nuiVector( -v0[1], v0[0], v0[2], 0 );
    nuiVector n1 = nuiVector( -v1[1], v1[0], v1[2], 0 );

    // determine miter lines by averaging the normals of the 2 segments
    nuiVector miter = n0 + n1;	// miter at start of current segment
    miter.Normalize();

    // determine the length of the miter by projecting it onto normal and then inverse it
    float length = HalfLineWidth / ( miter* n1 );

    // p0a--------p1b
    // |           |
    // p0----------p1
    // |           |
    // p0b--------p1b

    nuiVector p0a = p0 + n0 * HalfLineWidth;
    nuiVector p0b = p0 - n0 * HalfLineWidth;
    nuiVector p1a = p1 + n1 * HalfLineWidth;
    nuiVector p1b = p1 - n1 * HalfLineWidth;

    bool skipfirst = false;
    
    // prevent excessively long miters at sharp corners
    if ( ( v0 * v1 ) < -MiterLimit )
    {
      miter = n1;
      length = HalfLineWidth;
      
      skipfirst = true;
      
      // close the gap
      if ( ( v0 * n1 ) > 0 )
      {
        pArray->SetTexCoords(0, 0);
        pArray->SetVertex(p1 - HalfLineWidth * n0);
        pArray->SetNormal(1, 0, 0);
        pArray->SetColor(1.0f, 1.0f, 0.0f, 1.0f);
        pArray->PushVertex();
        
        pArray->SetTexCoords( 0, 1 );
        pArray->SetVertex( p1 - length * miter );
        pArray->SetNormal(-1, 0, 0);
        pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        pArray->PushVertex();

        pArray->SetTexCoords( 0, 0 );
        pArray->SetVertex( p1 - HalfLineWidth * n1 );
        pArray->SetNormal(1, 0, 0);
        pArray->SetColor(0.0f, 1.0f, 1.0f, 1.0f);
        pArray->PushVertex();
//
//        pArray->SetTexCoords( 0, 0.5 );
//        pArray->SetVertex(p1);
//        pArray->SetNormal(1, 0, 0);
//        pArray->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
//        pArray->PushVertex();

//        pArray->SetTexCoords( 0, 0 );
//        pArray->SetVertex( p1 + length * miter );
//        pArray->SetNormal(1, 0, 0);
//        pArray->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
//        pArray->PushVertex();
//
//        pArray->SetTexCoords( 0, 1 );
//        pArray->SetVertex( p1 - length * miter );
//        pArray->SetNormal(-1, 0, 0);
//        pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
//        pArray->PushVertex();
      }
      else
      {
        pArray->SetTexCoords( 0, 1 );
        pArray->SetVertex( p1 + HalfLineWidth * n0 );
        pArray->SetNormal(-1, 0, 0);
        pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        pArray->PushVertex();

          pArray->SetTexCoords( 0, 1 );
          pArray->SetVertex( p1 + length * miter );
          pArray->SetNormal(-1, 0, 0);
          pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
          pArray->PushVertex();

        pArray->SetTexCoords( 0, 1 );
        pArray->SetVertex( p1 + HalfLineWidth * n1 );
        pArray->SetNormal(-1, 0, 0);
        pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        pArray->PushVertex();
        
//        pArray->SetTexCoords( 0, 0.5 );
//        pArray->SetVertex(p1);
//        pArray->SetNormal(0, 0, 0);
//        pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
//        pArray->PushVertex();

//        pArray->SetTexCoords( 0, 0 );
//        pArray->SetVertex( p1 + length * miter );
//        pArray->SetNormal(1, 0, 0);
//        pArray->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
//        pArray->PushVertex();
//
//        pArray->SetTexCoords( 0, 1 );
//        pArray->SetVertex( p1 - length * miter );
//        pArray->SetNormal(-1, 0, 0);
//        pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
//        pArray->PushVertex();
      }
    }
    else
    {
      // generate the triangle strip
      pArray->SetTexCoords( 0, 0 );
      pArray->SetVertex( p1 + length * miter );
      pArray->SetNormal(1, 0, 0);
      pArray->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
      pArray->PushVertex();
      
      pArray->SetTexCoords( 0, 1 );
      pArray->SetVertex( p1 - length * miter );
      pArray->SetNormal(-1, 0, 0);
      pArray->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
      pArray->PushVertex();
    }
  }
  
  return pArray;
}

nuiRenderObject* nuiShape::Stroke(float Quality, float LineWidth, nuiLineJoin LineJoin, nuiLineCap LineCap, float MiterLimit)
{
  nuiPath Vertices;
  bool res = Tessellate(Vertices, Quality);

  if (!res)
    return nullptr;

  nuiRenderObject* pObject = new nuiRenderObject();
  uint total = Vertices.GetCount();

  uint offset = 0;
  uint count;

  std::vector<nuiVector> subpath;
  //Find sub path:
  for (uint i = offset; i < total; i++)
  {
    uint ii = i+1;
    if (Vertices[i].GetType() == nuiPointTypeStop || ii == total)
    {
      size_t c = subpath.size();
      subpath.push_back(2.0 * subpath[c - 1] - subpath[c - 2]);
      pObject->AddArray(StrokeSubPath(subpath, LineWidth, LineJoin, LineCap, MiterLimit));
      
      // restart the process:
      subpath.clear();
      offset = ii;
    }
    else
    {
      if (i == offset)
      {
        subpath.push_back(2.0 * Vertices[i] - Vertices[i+1]);
        subpath.push_back(Vertices[i]);
      }
      else
      {
        subpath.push_back(Vertices[i]);
      }
    }

  }

  return pObject;
}


/*
nuiRenderObject* nuiShape::Outline(float Quality, float LineWidth, nuiLineJoin LineJoin, nuiLineCap LineCap, float MiterLimit)
{
  nuiShape* pTmpShape = new nuiShape();

  nuiOutliner* pOutliner = new nuiOutliner(NULL, LineWidth);
  pOutliner->SetLineJoin(LineJoin);
  pOutliner->SetLineCap(LineCap);
  pOutliner->SetMiterLimit(MiterLimit);

  uint32 contours = GetContourCount();
  for (uint32 i = 0; i < contours; i++)
  {
    nuiContour* pContour = GetContour(i);

    pOutliner->SetPath(pContour);

    nuiPath Vertices;
    pOutliner->Tessellate(Vertices, Quality);
    pTmpShape->AddPath(Vertices);
    pTmpShape->CloseContour();
  }

  delete pOutliner;

  nuiRenderObject* pObj = pTmpShape->Fill(Quality);
  delete pTmpShape;
  return pObj;
}
*/

void nuiShape::AddArc(float cX, float cY, float rX, float rY, float Theta1InDegree, float Theta2InDegree, float Phi, float Quality)
{
  AddContour();
  mpContours.back()->AddArc(cX, cY, rX, rY, Theta1InDegree, Theta2InDegree, Phi);
}


float nuiShape::GetDistanceFromPoint(float X, float Y, float Quality) const
{
  nuiPath path;
  if (!Tessellate(path, Quality))
    return std::numeric_limits<float>::infinity();
  
  return path.GetDistanceFromPoint(X, Y);
}

