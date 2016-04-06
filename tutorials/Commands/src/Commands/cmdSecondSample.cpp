/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */



#include "nui.h"
#include "Commands/cmdSecondSample.h"
#include "Application.h"
#include "MainWindow_.h"

 

cmdSecondSample::cmdSecondSample() : nuiCommand("SecondCommandRef", "second Sample command", false/*can undo*/, true/* use command manager*/, false/*can repeat*/)
{
}
  
    
cmdSecondSample::~cmdSecondSample()
{
    
}
    


//************************************************************
//
// ExecuteDo
//
//                    
bool cmdSecondSample::ExecuteDo()
{
  GetMainWindow()->AddMessage("cmdSecondSample ExecuteDo\ncmdSecondSample can not be undone\n=> the CommandManager stacks are reset.\n");
  return true;
}
    


    


//************************************************************
//
// SetArgs
//
//
bool cmdSecondSample::SetArgs(const std::vector<nglString>& args)
{
  return true;
}












//***********************************************************************************************************
//
// CommandDesc
//
//***********************************************************************************************************


//************************************************************
//
// Constr
//
//
cmdSecondSample::Desc::Desc() : nuiCommandDesc("SecondCommandRef")
{
}
        


//************************************************************
//
// CreateCommand
//
//
nuiCommand* cmdSecondSample::Desc::CreateCommand() const
{
  cmdSecondSample* newCmd = new cmdSecondSample ();
  return newCmd;
}
        

//************************************************************
//
// Comment
//
//
nglString cmdSecondSample::Desc::Comment() const
{
  return "this a sample command. It does nothing special:)";
}
        
        

        
        
cmdSecondSample::Desc cmdSecondSample::ThisDesc;
 

