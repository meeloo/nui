/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nui.h"
#include "Main/MainWindow.h"
#include "Main/Preferences.h"
#include "Main/Debugger.h"

class nuiApp : public nuiApplication
{
public:
  nuiApp();
  ~nuiApp();

  void OnInit();
  void OnExit (int Code);
	MainWindow* GetMainWindow();
  
  
  Preferences& GetPreferences();
  nuiDebugger& GetDebugger() const;
  bool IsFullVersion();
  
    
protected:
   MainWindow *win;


private:
  Preferences mPreferences;
  bool mFullVersion;

  nuiDebugger* mpDebugger;

  void RegisterCustomWidgets();
};

nuiApp* GetApp();
MainWindow* GetMainWindow();
Preferences& GetPreferences();
nuiDebugger& GetDebugger();
