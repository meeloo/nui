/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nui.h"
#include "nuiVBox.h"
#include "nuiHBox.h"

// decorations defines
#define INTROSPECTOR_DECO_BKG "INTROSPECTOR_DECO_BKG"
#define INTROSPECTOR_DECO_BUTTON "INTROSPECTOR_DECO_BUTTON"
#define INTROSPECTOR_DECO_CLIENT_BKG "INTROSPECTOR_DECO_CLIENT_BKG"

#define INTROSPECTOR_DECO_BORDER "INTROSPECTOR_DECO_BORDER"

#define INTROSPECTOR_DECO_FOLDERPANE_TITLE "INTROSPECTOR_DECO_FOLDERPANE_TITLE"

#define INTROSPECTOR_DECO_GRID_TITLE "INTROSPECTOR_DECO_GRID_TITLE"
//#define INTROSPECTOR_DECO_GRID_CELL "INTROSPECTOR_DECO_GRID_LINE"

#define INTROSPECTOR_COLOR_GRID_TITLE nuiColor(110,110,110)
#define INTROSPECTOR_COLOR_FOLDERPANE_TITLE nuiColor(140,140,140)
#define INTROSPECTOR_COLOR_GRID nuiColor(200,200,200)


class nuiIntrospector : public nuiVBox
{
public:
  nuiIntrospector(nuiWidget* pTarget);
  virtual ~nuiIntrospector();
  
  nuiWidget* GetToolbar();
  nuiWidget* GetStatusbar();

private:

  void  InitDecorations();
  void ShowWidgetInspector(const nuiEvent& rEvent);
  void ShowThreadInspector(const nuiEvent& rEvent);
  void ShowFontInspector(const nuiEvent& rEvent);
  void ShowDecorationInspector(const nuiEvent& rEvent);
  void ShowTextureInspector(const nuiEvent& rEvent);
  void ShowObjectInspector(const nuiEvent& rEvent);
  void ShowLayersInspector(const nuiEvent& rEvent);

  nuiEventSink<nuiIntrospector> mEventSink;
  nuiWidget* mpTarget;
};
