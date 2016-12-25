/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nuiMainWindow.h"

#define DUMMY_DEBUGGER 0


class MainWindow : public nuiMainWindow
{
public:
  MainWindow(const nglContextInfo& rContext, const nglWindowInfo& rInfo, bool ShowFPS = false, const nglContext* pShared = NULL);
  ~MainWindow();

  void OnCreation();
  void OnClose();
  
protected:

  nuiWidget* Tutorial_Buttons();
  nuiWidget* Tutorial_ToggleButtons();
  nuiWidget* Tutorial_RadioButtons1();
  nuiWidget* Tutorial_RadioButtons2();
  
  nuiWidget* CreateTestDelChildren();
  void OnCreateView(const nuiEvent& rEvent);
  
  
  void OnButtonPressed(const nuiEvent& rEvent);
  void OnTogglePressed(const nuiEvent& rEvent);
  void OnRadioPressed(const nuiEvent& rEvent);
  
  bool LoadCSS(const nglPath& rPath);

  bool KeyDown(const nglKeyEvent& rEvent);
private:
  
  nuiLabel* mpLabel;
  nuiWidget* mpHiddenWidget;
  nuiEventSink<MainWindow> mEventSink;

#if DUMMY_DEBUGGER
  nuiTCPClient* mpClient = nullptr;
  nuiMessageClient* mpMessageClient = nullptr;
  nuiSocketPool mSocketPool;
#endif
};

