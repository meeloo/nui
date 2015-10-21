/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#pragma once

#include "nui.h"
#include "nuiWidget.h"

class nuiGrid;
class nuiText;

class nuiLayersInspector : public nuiWidget
{
public:
  nuiLayersInspector();

protected:
  virtual ~nuiLayersInspector();

  nuiEventSink<nuiLayersInspector> mSink;
  
  void OnLayersChanged(const nuiEvent& rEvent);
  void OnLayerSelection(const nuiEvent& rEvent);
  void UpdateLayers();
  
  nuiImage* mpImage;
  nuiGrid* mpAttributeGrid;
  nuiSlotsSink mSlot;
};
