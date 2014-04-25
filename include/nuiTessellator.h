/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

// nuiTessellator.h

#ifndef __nuiTessellator_h__
#define __nuiTessellator_h__

class nuiRenderObject;
class nuiShape;
class nuiPathGenerator;

class nuiTessellator
{
public:
  explicit nuiTessellator(nuiPathGenerator* pPathGenerator);
  explicit nuiTessellator(nuiShape* pShape);
  virtual ~nuiTessellator();

  nuiRenderObject* Generate(float Quality = 0.5f);

  void SetFill(bool Set) { mOutline = !Set; }
  void SetOutline(bool Set) { mOutline = Set; }
  bool GetFill() const { return !mOutline; }
  bool GetOutline() const { return mOutline; }

private:
  nuiPathGenerator* mpPath;
  nuiShape* mpShape;
  bool mOutline;

  nuiRenderObject* GenerateFromPath(float Quality);
  nuiRenderObject* GenerateFromShape(float Quality);

  static class GLUtesselator* mpTess;
  static uint32 mRefs;

#ifndef CALLBACK
#define CALLBACK
#endif


  static GLvoid CALLBACK StaticInternalTessBegin(GLenum type, void * polygon_data);
  static GLvoid CALLBACK StaticInternalTessEdgeFlag(GLboolean flag, void * polygon_data);
  static GLvoid CALLBACK StaticInternalTessVertex(void * vertex_data, void * polygon_data);
  static GLvoid CALLBACK StaticInternalTessEnd(void * polygon_data);
  static GLvoid CALLBACK StaticInternalTessCombine(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData, void * polygon_data);
  static GLvoid CALLBACK StaticInternalTessError(GLenum ErrNo, void * polygon_data);
  void InternalTessBegin(GLenum type);
  void InternalTessEdgeFlag(GLboolean flag);
  void InternalTessVertex(void* vertex_data);
  void InternalTessCombine(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData);
  void InternalTessEnd();
  void InternalTessError(GLenum ErrNo);

  nuiRenderObject* mpObject;
  nuiPath mTempPoints;
  bool mEdgeFlag;
};

#endif // nuiTessellator
