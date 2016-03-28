/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "MainMenu.h"
#include "MainWindow_.h"
#include "Application.h"

#include "nuiCommandManager.h"
#include "nuiCommand.h"


enum mxMenuCommand
{
  eMenu1Cmd1 = 0,
  eMenu1Cmd2,
  
  eMenu2Undo,
  eMenu2Redo,
  eMenu2Repeat
};



MainMenu::MainMenu()
: nuiMainMenu(NULL),mEventSink(this)
{
  Init();
  
  
  // root item for the menu bar. it's not displayed
  nuiMainMenuItem* pRoot = new nuiMainMenuItem("MainMenuRoot"); 
  
  // create the main menu object, you'll have to register it from the main window
  SetRoot(pRoot);
  
  
  nuiMainMenuItem* pMenu1 = new nuiMainMenuItem("Menu1");
  pRoot->AddChild(pMenu1);
  nuiMainMenuItem* pMenu2 = new nuiMainMenuItem("Menu2");
  pRoot->AddChild(pMenu2);
  
  
  //***********************************************************
  // menu 1
  //
  nuiMainMenuItem* pItem = new nuiMainMenuItem("First Command");
  pMenu1->AddChild(pItem);
  mEventSink.Connect(pItem->Activated, &MainMenu::OnMenuCommand, (void*)eMenu1Cmd1);

  pItem = new nuiMainMenuItem(_T(""), eItemSeparator);
  pMenu1->AddChild(pItem);
  
  pItem = new nuiMainMenuItem("Second Command");
  pMenu1->AddChild(pItem);
  mEventSink.Connect(pItem->Activated, &MainMenu::OnMenuCommand, (void*)eMenu1Cmd2);
  
  
  //***********************************************************
  // menu 2
  //

  mpUndo = new nuiMainMenuItem("Undo");
  pMenu2->AddChild(mpUndo);
  mEventSink.Connect(mpUndo->Activated, &MainMenu::OnMenuCommand, (void*)eMenu2Undo);
  
  mpRedo = new nuiMainMenuItem("Redo");
  pMenu2->AddChild(mpRedo);
  mEventSink.Connect(mpRedo->Activated, &MainMenu::OnMenuCommand, (void*)eMenu2Redo);
  
  mpRepeat = new nuiMainMenuItem("Repeat");
  pMenu2->AddChild(mpRepeat);
  mEventSink.Connect(mpRepeat->Activated, &MainMenu::OnMenuCommand, (void*)eMenu2Repeat);
  
  mpUndo->SetEnabled(nuiCommandManager::CanUndo());
  mpRedo->SetEnabled(nuiCommandManager::CanRedo());
  mpRepeat->SetEnabled(nuiCommandManager::CanRepeat());  
    
  
  //*******************************************************************
  // undo, redo and repeat must update after each Command
  mEventSink.Connect(nuiCommandManager::CommandChanged, &MainMenu::OnCommandChanged);
  
}


MainMenu::~MainMenu()
{
}



void MainMenu::Init()
{
  mCommands[eMenu1Cmd1] = "FirstCommandRef";
  mCommands[eMenu1Cmd2] = "SecondCommandRef";

  mCommands[eMenu2Undo] = "Undo";
  mCommands[eMenu2Redo] = "Redo";
  mCommands[eMenu2Repeat] = "Repeat";  
}



void MainMenu::OnMenuCommand(const nuiEvent& rEvent)
{
  uint64 menuCmd = (uint64)rEvent.mpUser;
  
  // undo, redo and repeat are special cases
  switch (menuCmd)
  {
    case eMenu2Undo:
    {
      if (nuiCommandManager::Undo())
        rEvent.Cancel();
      return;
    }
  
    case eMenu2Redo:
    {
      if (nuiCommandManager::Redo())
        rEvent.Cancel();
      return;
    }
  
    case eMenu2Repeat:
    {
      if (nuiCommandManager::Repeat())
        rEvent.Cancel();
      return;
    }
      
    // in our sample application, we automatically launch any command from the menu,
    // withouth any arguments.
    // Obviously, you're free to make your own system depending on your application purpose.
    //
    default:
    {
      std::map<uint32,nglString>::iterator it = mCommands.find(menuCmd);
      NGL_ASSERT(it != mCommands.end());
      const nglString cmd = it->second;
      
      nuiCommandDesc* cmdDesc = nuiCommandManager::GetCommandDesc(cmd);
      NGL_ASSERT(cmdDesc != NULL);
      nuiCommand* pCommand = cmdDesc->CreateCommand();
      nuiCommandManager::Do(pCommand);      
    }
  }
  
  rEvent.Cancel();
}



void MainMenu::OnCommandChanged(const nuiEvent& rEvent)
{
  // update undo / redo menu
  bool canundo = nuiCommandManager::CanUndo();
  bool canredo =  nuiCommandManager::CanRedo();
  bool canrepeat = nuiCommandManager::CanRepeat();
  
  nuiCommand* pLastDoneCmd = nuiCommandManager::GetLastDoneCmd();
  nuiCommand* pLastUndoneCmd = nuiCommandManager::GetLastUndoneCmd();
  
  // update Undo MenuItem text
  mpUndo->SetEnabled(canundo);
  nglString undotext = "Undo";
  if (canundo && pLastDoneCmd)
  {
    undotext.Add(" '").Add(pLastDoneCmd->GetDescription()).Add("'");
  }
  SetItemText(mpUndo, undotext);
  
  
  // update Redo MenuItem text
  mpRedo->SetEnabled(canredo);
  nglString redotext = "Redo";
  if (canredo && pLastUndoneCmd)
  {
    redotext.Add(" '").Add(pLastUndoneCmd->GetDescription()).Add("'");
  }
  SetItemText(mpRedo, redotext);
  
  
  // update Repeat MenuItem text
  mpRepeat->SetEnabled(canrepeat);
  nglString repeattext = "Repeat";
  if (canrepeat && pLastDoneCmd)
  {
    repeattext.Add(" '").Add(pLastDoneCmd->GetDescription()).Add("'");
  }
  SetItemText(mpRepeat, repeattext);
  
  
  // update monitor info
  GetMainWindow()->UpdateCommandManagerInfo();
  
  rEvent.Cancel();
}




