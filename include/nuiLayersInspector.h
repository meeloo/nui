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
  nuiLayersInspector(nuiRenderThread* pRenderThread);

protected:
  virtual ~nuiLayersInspector();

  nuiEventSink<nuiLayersInspector> mSink;
  
  void OnLayersChanged(const nuiEvent& rEvent);
  void OnLayerSelection(const nuiEvent& rEvent);
  void Setup();
  void UpdateLayers();
  void OnUpdateLayers(const nuiEvent& rEvent);

  
  nuiImage* mpImage = nullptr;
  nuiGrid* mpAttributeGrid = nullptr;
  nuiSlotsSink mSlot;
  nuiList* mpLayerList = nullptr;
  nuiComboBox* mpSortCombo = nullptr;
  nuiRenderThread* mpRenderThread = nullptr;
};
