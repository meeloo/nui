/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiPathGenerator_h__
#define __nuiPathGenerator_h__

//#include "nui.h"
#include "nglMath.h"
#include "nglVector.h"
#include "nuiEvent.h"
#include "nuiPoint.h"
#include "nuiPath.h"

class nuiXMLNode;

class NUI_API nuiPathGenerator
{
public:
  nuiPathGenerator();
  virtual ~nuiPathGenerator();

  /** @name Serialization */
  //@{
  virtual nuiXMLNode* Serialize(nuiXMLNode* pParentNode) const; ///< This method saves the state in the given XML Node.
  //@}

  virtual bool Tessellate(nuiPath& rVertices, float Quality = 0.5f) const = 0;

  virtual nuiPoint GetStartPoint() const;
  virtual nuiPoint GetEndPoint() const;

  virtual nuiRect GetRect();

  //nuiSimpleEventSource<nuiChanged> Changed; ///< This event is fired each time the ContourElement is changed.
};

#endif
