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

class nuiDecorationInspector : public nuiWidget
{
public:
  nuiDecorationInspector();

protected:
  virtual ~nuiDecorationInspector();
  nuiEventSink<nuiDecorationInspector> mSink;
  
  void OnDecosChanged(const nuiEvent& rEvent);
  void OnDecoSelection(const nuiEvent& rEvent);
  void UpdateDecos();
  void DumpDecl();
  void OnTrash();
  
  nuiLabel* mpLabel;
  nuiLabel* mpInfoLabel;
  nuiText* mpCSSLabel;
  nuiGrid* mpAttributeGrid;
  nuiSlotsSink mSlot;
};
