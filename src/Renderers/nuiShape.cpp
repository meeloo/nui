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

#define NUI_DEBUG_STROKER 0
#define NUI_LOG_STROKER(X) if (NUI_DEBUG_STROKER) { X }

static void nuiCreateRoundCap(const nuiVector& center, const nuiVector& _p0, const nuiVector& _p1, const nuiVector& pma, const nuiVector& pmb, const nuiVector& nextPointInLine, nuiRenderArray* pArray, float HLWR, float HLW, const nuiColor& left, const nuiColor& right, const nuiVector& _v0, const nuiVector& n1)
{
  nglString val0, val1;
  _p0.GetValue(val0);
  _p1.GetValue(val1);
  NUI_LOG_STROKER(NGL_OUT("nuiCreateRoundCap in between %s and %s\n", val0.GetChars(), val1.GetChars());)

  const float EPSILON = 0.0001f;

  nuiVector v0 = _p0 - center;
  nuiVector v1 = _p1 - center;

  float radius = HLWR;

  float angle0 = atan2f(v0[1], v0[0]);
  float angle1 = atan2f(v1[1], v1[0]);

  float orgAngle0 = angle0;

  if ( angle1 > angle0)
  {
    if ( angle1 - angle0 >= M_PI - EPSILON)
    {
   			angle1 = angle1 - 2.0f * (float)M_PI;
    }
  }
  else
  {
    if ( angle0 - angle1 >= M_PI - EPSILON)
    {
   			angle0 = angle0 - 2.0f * (float)M_PI;
    }
  }

  float angleDiff = angle1 - angle0;

  if ((fabs( angleDiff ) >= M_PI - EPSILON) && (fabs( angleDiff ) <= M_PI + EPSILON))
  {
    nuiVector r1 = center - nextPointInLine;
    if ( r1[0] == 0 )
    {
      if (r1[1] > 0)
      {
        angleDiff = -angleDiff;
      }
    }
    else if ( r1[0] >= -EPSILON )
    {
      angleDiff = -angleDiff;
    }
  }

  const float GAIN = 1.0f;
  int nsegments = MAX(1, ToBelow(fabs(angleDiff * radius) / GAIN));

  float angleInc = angleDiff / (float)nsegments;

  nuiVector interior_vertex = pmb;
  nuiColor interior_color = right;
  nuiColor exterior_color = left;

  bool revert = ( _v0 * n1 ) < 0;
  float valRef = -pArray->GetCurrentVertex().mNY;
  float val = pArray->GetCurrentVertex().mNZ;

  if (revert)
  {
    NUI_LOG_STROKER(NGL_OUT("       Initial Round vertices  [reverted]\n");)
    interior_vertex = pma;
    interior_color = left;
    exterior_color = right;

    pArray->SetVertex(pma);
    pArray->SetColor(interior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();

    NUI_LOG_STROKER(NGL_OUT("       First Round vertices [reverted]\n");)
    pArray->SetVertex(_p0);
    pArray->SetColor(exterior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    NUI_LOG_STROKER(NGL_OUT("       Center Round vertices [reverted]\n");)
    pArray->SetVertex(center);
    pArray->SetColor(exterior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, 0, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
  }
  else{
    NUI_LOG_STROKER(NGL_OUT("       First Round vertices\n");)
    pArray->SetVertex(_p0);
    pArray->SetColor(exterior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex(pmb);
    pArray->SetColor(interior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    NUI_LOG_STROKER(NGL_OUT("       Center Round vertices\n");)
    pArray->SetVertex(center);
    pArray->SetColor(exterior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, 0, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();

    valRef = -valRef;
  }


  
  nuiColor mix = left;
  mix.Mix(right, 0.5);
  NUI_LOG_STROKER(NGL_OUT("       Round vertices\n");)
  for (int i = 0; i < nsegments+1; i++)
  {
    pArray->SetVertex(center);
    pArray->SetColor(interior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, 0, val);
    pArray->PushVertex();

    nuiVector exterior_point(center[0] + radius * cosf(orgAngle0 + angleInc * i), center[1] + radius * sinf(orgAngle0 + angleInc * i), 0);
    pArray->SetVertex(exterior_point);
    pArray->SetColor(exterior_color);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -valRef, val);
    pArray->PushVertex();
  }

  pArray->SetVertex(interior_vertex);
  pArray->SetColor(interior_color);
  pArray->SetNormal(pArray->GetCurrentVertex().mNX, valRef, val);
  pArray->PushVertex();

  NUI_LOG_STROKER(NGL_OUT("       Round vertices DONE\n");)
}

static void nuiAddRound(nuiRenderArray* pArray, float length, const nuiVector& p0, const nuiVector& p0a, const nuiVector& p0b, const nuiVector& p1, const nuiVector& p1a, const nuiVector& p1b, const nuiVector& p2, const nuiVector& pma, const nuiVector& pmb, float HalfLineWidthRef, float HalfLineWidth, float l0, float l1, const nuiColor& left, const nuiColor& right, const nuiVector& v0, const nuiVector& n1)
{
  if ( ( v0 * n1 ) > 0 )
  {
    NUI_LOG_STROKER(NGL_OUT("   First case\n");)
    nuiCreateRoundCap(p1, p0a, p1a, pma, pmb, p2, pArray, HalfLineWidthRef, HalfLineWidth, left, right, v0, n1);
    
    NUI_LOG_STROKER(NGL_OUT("   Next Vertexes (first case)\n");)
//    pArray->SetVertex(pmb);
//    pArray->SetColor(right);
//    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
//    pArray->PushVertex();

    pArray->SetVertex( p1a );
    pArray->SetColor(left);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();

    pArray->SetVertex(pmb);
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
  }
  else
  {
    NUI_LOG_STROKER(NGL_OUT("   Second case\n");)
    nuiCreateRoundCap(p1, p0b, p1b, pma, pmb, p2, pArray, HalfLineWidthRef, HalfLineWidth ,left, right, v0, n1);
    
    NUI_LOG_STROKER(NGL_OUT("   Next Vertexes (second case)\n");)
    pArray->SetVertex( p1b );
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
  }
  NUI_LOG_STROKER(NGL_OUT("   Round Done\n");)
}



static void nuiAddBevel(nuiRenderArray* pArray, float length, const nuiVector& p0, const nuiVector& p0a, const nuiVector& p0b, const nuiVector& p1, const nuiVector& p1a, const nuiVector& p1b, const nuiVector& pma, const nuiVector& pmb, float HalfLineWidthRef, float HalfLineWidth, float l0, float l1, const nuiColor& left, const nuiColor& right, const nuiVector& v0, const nuiVector& n1)
{
  if (length > l0 || length > l1)
  {
    pArray->SetVertex( p0a );
    pArray->SetColor(left);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex( p0b );
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex( p1a );
    pArray->SetColor(left);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex( p1b );
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
  }
  else
  {
    // close the gap
    if ( ( v0 * n1 ) > 0 )
    {
      pArray->SetVertex( p0a ); // first bevel point
      pArray->SetColor(left);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
      
      pArray->SetVertex(pmb);
      pArray->SetColor(right);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
      
      pArray->SetVertex( p1a ); // second bevel point
      pArray->SetColor(left);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
      
      pArray->SetVertex(pmb);
      pArray->SetColor(right);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
    }
    else
    {
      pArray->SetVertex( pma );
      pArray->SetColor(left);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
      
      pArray->SetVertex( p0b ); // first bevel point
      pArray->SetColor(right);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
      
      pArray->SetVertex( pma );
      pArray->SetColor(left);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
      
      pArray->SetVertex( p1b ); // second bevel point
      pArray->SetColor(right);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
    }
  }
}

static void nuiAddMiter(nuiRenderArray* pArray, float length, const nuiVector& p0, const nuiVector& p0a, const nuiVector& p0b, const nuiVector& p1, const nuiVector& p1a, const nuiVector& p1b, const nuiVector& pma, const nuiVector& pmb, float HalfLineWidthRef, float HalfLineWidth, float l0, float l1, const nuiColor& left, const nuiColor& right, const nuiVector& v0, const nuiVector& n1)
{
  if (length > l0 || length > l1)
  {
    pArray->SetVertex( p0a );
    pArray->SetColor(left);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex( p0b );
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    if ( ( v0 * n1 ) > 0 )
    {
      pArray->SetVertex( pma );
      pArray->SetColor(left);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
    }
    else
    {
      pArray->SetVertex( pmb );
      pArray->SetColor(right);
      pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
      pArray->PushVertex();
    }
    
    pArray->SetVertex( p1a );
    pArray->SetColor(left);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex( p1b );
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
  }
  else
  {
    // generate the triangle strip
    pArray->SetVertex( pma );
    pArray->SetColor(left);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
    
    pArray->SetVertex( pmb );
    pArray->SetColor(right);
    pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
    pArray->PushVertex();
  }
}

// Adapted from https://github.com/paulhoux/Cinder-Samples/blob/master/GeometryShader/assets/shaders/lines1.geom
static nuiRenderArray* StrokeSubPath(const std::vector<nuiVector>& subpath, float LineWidth, nuiLineJoin LineJoin, nuiLineCap LineCap, float MiterLimit, bool closed)
{
  NUI_LOG_STROKER(NGL_OUT("StrokeSubPath\n");)
  const float HalfLineWidth = LineWidth / 2;
  const float HalfLineWidthRef = HalfLineWidth + .5f;
  
  nuiRenderArray* pArray = new nuiRenderArray(GL_TRIANGLE_STRIP);
//  nuiRenderArray* pArray = new nuiRenderArray(GL_LINE_STRIP);
//  pArray->EnableArray(nuiRenderArray::eTexCoord);
  pArray->EnableArray(nuiRenderArray::eNormal);
  pArray->EnableArray(nuiRenderArray::eColor);
  pArray->SetShape(true);
  NUI_LOG_STROKER(pArray->SetDebug(true);)

  size_t count = subpath.size() - 2;
  for (size_t i = 0; i < count; i++)
  {
    nuiVector p0 = subpath[i];
    nuiVector p1 = subpath[i+1];
    nuiVector p2 = subpath[i+2];

    nuiVector v0 = p1 - p0; // from p0 to p1 (normalized later)
    nuiVector v1 = p2 - p1; // from p1 to p2 (normalized later)
    float l0 = v0.Length(); // distance from p0 to p1
    float l1 = v1.Length(); // distance from p1 to p2

    v0.Normalize();
    v1.Normalize();

    // determine the normal of each of the 3 segments (previous, current, next)
    nuiVector n0 = nuiVector( -v0[1], v0[0], v0[2], 0 ); // perpendicular to [p0, p1]
    nuiVector n1 = nuiVector( -v1[1], v1[0], v1[2], 0 ); // perpendicular to [p1, p2]

    // determine miter lines by averaging the normals of the 2 segments
    nuiVector miter = n0 + n1;	// miter at start of current segment
    miter.Normalize();

    // determine the length of the miter by projecting it onto the normal vector (n1 or n2) and then inverse it
    float length = HalfLineWidthRef / ( miter * n1 );

    // p0a--------p1a
    // |           |
    // p0----------p1
    // |           |
    // p0b--------p1b

    nuiVector o0 = n0 * HalfLineWidthRef;
    nuiVector o1 = n1 * HalfLineWidthRef;
    
    nuiVector p0a = p1 + o0;
    nuiVector p0b = p1 - o0;
    nuiVector p1a = p1 + o1;
    nuiVector p1b = p1 - o1;

    nuiVector pma = p1 + length * miter; // one side
    nuiVector pmb = p1 - length * miter; // the other side

#if 1
    nuiColor left(.5f, .5f, .5f, 1.f);
    nuiColor right(.5f, .5f, .5f, 1.f);
#else
    nuiColor left(255, 0, 0, 128);
    nuiColor right(0, 0, 255, 128);
#endif


    if ((i == 0 || i == count - 1) && !closed)
    {
      // make ends
      if (LineCap == nuiLineCapBut || LineCap == nuiLineCapSquare)
      {
        // just a small rectangle out of three triangles at the begining:
        float offRef = HalfLineWidthRef;
        float off = HalfLineWidth;
        if (LineCap == nuiLineCapBut)
        {
          offRef = .5;
          off = 0.1;
        }
        
        nuiVector offset = v0 * offRef;
        nuiVector sa = p1a - offset;
        nuiVector sp = p1  - offset;
        nuiVector sb = p1b - offset;

        // first half
        pArray->SetVertex( sp );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, -offRef, off);
        pArray->PushVertex();
        
        pArray->SetVertex( sb );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, -offRef, off);
        pArray->PushVertex();
        
        pArray->SetVertex( p1 );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, 0, off);
        pArray->PushVertex();
        
        pArray->SetVertex( p1b );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, -offRef, off);
        pArray->PushVertex();

        pArray->PushVertex();

        
        // Second half
        pArray->SetVertex( sp );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, offRef, off);
        pArray->PushVertex();
        pArray->PushVertex();

        pArray->SetVertex( p1 );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, 0, off);
        pArray->PushVertex();

        pArray->SetVertex( sa );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, offRef, off);
        pArray->PushVertex();

        pArray->SetVertex( p1a );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, offRef, off);
        pArray->PushVertex();

        // The actual start
        pArray->SetVertex( p1a );
        pArray->SetColor(left);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
        pArray->PushVertex();

        pArray->SetVertex( p1b );
        pArray->SetColor(right);
        pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
        pArray->PushVertex();

        pArray->SetNormal(1, HalfLineWidthRef, HalfLineWidth);
      }
      else
      {
        {
          pArray->SetNormal(1, HalfLineWidthRef, HalfLineWidth);

          pArray->SetVertex( p1a );
          pArray->SetColor(left);
          pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
          pArray->PushVertex();
          
          pArray->SetVertex( p1b );
          pArray->SetColor(right);
          pArray->SetNormal(pArray->GetCurrentVertex().mNX, -pArray->GetCurrentVertex().mNY, pArray->GetCurrentVertex().mNZ);
          pArray->PushVertex();
        }
      }
    }
    else
    {
      if ( LineJoin == nuiLineJoinRound)
      {
        NUI_LOG_STROKER(NGL_OUT("Add Round Join\n");)
        nuiAddRound(pArray, length, p0, p0a, p0b, p1, p1a, p1b, p2, pma, pmb, HalfLineWidthRef, HalfLineWidth, l0, l1, left, right, v0, n1);
      }
      else if (LineJoin == nuiLineJoinBevel)
      {
        NUI_LOG_STROKER(NGL_OUT("Add Bevel Join\n");)
        nuiAddBevel(pArray, length, p0, p0a, p0b, p1, p1a, p1b, pma, pmb, HalfLineWidthRef, HalfLineWidth, l0, l1, left, right, v0, n1);
      }
      else
      {
        // prevent excessively long miters at sharp corners
        if (( v0 * v1 ) < -MiterLimit)
        {
          NUI_LOG_STROKER(NGL_OUT("Add Bevel Join (Miter limit)\n");)
          nuiAddBevel(pArray, length, p0, p0a, p0b, p1, p1a, p1b, pma, pmb, HalfLineWidthRef, HalfLineWidth, l0, l1, left, right, v0, n1);
        }
        else
        {
          NUI_LOG_STROKER(NGL_OUT("Add Miter Join\n");)
          nuiAddMiter(pArray, length, p0, p0a, p0b, p1, p1a, p1b, pma, pmb, HalfLineWidthRef, HalfLineWidth, l0, l1, left, right, v0, n1);
        }
      }
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

      if (subpath.size() > 2 && subpath[1] == subpath.back()) // If the path is closed then do what needs to be done
      {
        subpath[0] = subpath[c - 2];
        subpath.push_back(subpath[2]);
        pObject->AddArray(StrokeSubPath(subpath, LineWidth, LineJoin, LineCap, MiterLimit, true));
      }
      else
      {
        subpath.push_back(2.0 * subpath[c - 1] - subpath[c - 2]);
        pObject->AddArray(StrokeSubPath(subpath, LineWidth, LineJoin, LineCap, MiterLimit, false));
      }


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

