/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/


#include "nui.h"
#include "nuiInit.h"
#include "Application.h"
#include "MainWindow.h"

#include "nglConsole.h"

#define APPLICATION_TITLE "Buttons tutorial"


NGL_APP_CREATE(Application);

Application::Application()
{
  mpMainWindow = NULL;
}

Application::~Application()
{
}

void Application::OnExit (int Code)
{
  if (mpMainWindow)
  {
    mpMainWindow->Release();
    mpMainWindow = NULL;
  }

  nuiUninit();
}

void Application::OnInit()
{
  nuiInit(NULL);

  uint Width = 0, Height = 0;
  bool HasSize = false;
  bool IsFullScreen = false;
  bool DebugObject = false;
  bool DebugInfo = false;
  bool ShowFPS = false;
//  nuiRenderer Renderer = eDirect3D;

#if TARGET_OS_IPHONE
  nuiRenderer Renderer = eOpenGL2;
#else
  nuiRenderer Renderer = eOpenGL2;
#endif
//  nuiRenderer Renderer = eSoftware;

  // Accept NGL default options
  ParseDefaultArgs();

  GetLog().UseConsole(true);
//  GetLog().SetLevel("font", 100);

  // Manual
  if ( (GetArgCount() == 1) &&
       ((!GetArg(0).Compare("-h")) || (!GetArg(0).Compare("--help"))) )
  {
    NGL_OUT("no params\n");
    Quit (0);
    return;
  }

  // Parse args
  int i = 0;
  while (i < GetArgCount())
  {
    nglString arg = GetArg(i);
    if ((!arg.Compare("--size") || !arg.Compare("-s")) && ((i+1) < GetArgCount()))
    {
      int w, h;

      std::string str(GetArg(i+1).GetStdString());
      sscanf(str.c_str(), "%dx%d", &w, &h);
      if (w > 0) Width  = w;
      if (h > 0) Height = h;
      HasSize = true;
      i++;
    }
    else if (!arg.Compare("--showfps") || !arg.Compare("-fps")) ShowFPS = true;
    else if (!arg.Compare("--fullscreen") || !arg.Compare("-f")) IsFullScreen = true;
    else if (!arg.Compare("--debugobject") || !arg.Compare("-d")) DebugObject = true;
    else if (!arg.Compare("--debuginfo") || !arg.Compare("-i")) DebugInfo = true;
    else if (!arg.Compare("--renderer") || !arg.Compare("-r"))
    {
      arg = GetArg(i+1);
      if (!arg.Compare("opengl")) Renderer = eOpenGL2;
      else if (!arg.Compare("direct3d")) Renderer = eDirect3D;
      else if (!arg.Compare("software")) Renderer = eSoftware;
      i++;
    }
    i++;
  }

  nuiMainWindow::SetRenderer(Renderer);

  if (!HasSize)
  {
    nglVideoMode current_mode;

    if (IsFullScreen)
    {
      Width = current_mode.GetWidth();
      Height = current_mode.GetHeight();
    }
    else
    {
#ifdef _UIKIT_
//      Width = 320;
//      Height = 480;
      Width = current_mode.GetWidth();
      Height = current_mode.GetHeight();
#else
      Width = 320;
      Height = 240;
#endif
    }
  }

  nuiDebugServer* pServer = new nuiDebugServer();
  pServer->Start(31337);

  // Test parser:
//  nglString str("Bleh(prout, 12.4 - 5 * 2);\n   Test += \"ho ho ho\"\n");
//  nuiLexer lexer(str);
  nglPath path("rsrc:/css/main.css");
  nuiLexer lexer(path.OpenRead(), path);
  nuiLexer::Token token;
  do {
    token = lexer.NextNonBlankToken();
    printf("Token %s ( %s )\n", token.GetTypeName(), token.mString.GetChars());
  } while (token.mType != nuiLexer::EndOfFile);


  /* Create the nglWindow (and thus a GL context, don't even try to
   *   instantiate the gui (or nglFont) before the nuiWin !)
   */
  nuiContextInfo ContextInfo(nuiContextInfo::StandardContext3D);
  nglWindowInfo Info;

  Info.Flags = IsFullScreen ? nglWindow::FullScreen : 0;
  Info.Width = Width;
  Info.Height = Height;
  Info.Pos = nglWindowInfo::ePosCenter;
  Info.Title = APPLICATION_TITLE;
  Info.XPos = 0;
  Info.YPos = 0;

  mpMainWindow = new MainWindow(ContextInfo,Info, ShowFPS);
  if ((!mpMainWindow) || (mpMainWindow->GetError()))
  {
    if (mpMainWindow)
      NGL_OUT("Error: cannot create window (%s)\n", mpMainWindow->GetErrorStr());
    Quit (1);
    return;
  }
  mpMainWindow->Acquire();
  mpMainWindow->DBG_SetMouseOverInfo(DebugInfo);  mpMainWindow->DBG_SetMouseOverObject(DebugObject);
  mpMainWindow->SetState(nglWindow::eShow);

}


Application* GetApp()
{
  return ((Application*)App);
}
