/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "ProjectGenerator/ProjectGenerator.h"
#include "nuiVBox.h"
#include "nuiHBox.h"
#include "Main/MainWindow.h"

#include "nuiDialogSelectDirectory.h"
#include "nuiDialogSelectFile.h"
#include "nuiMessageBox.h"

#include "Main/Yapuka.h"


ProjectGenerator::ProjectGenerator()
: nuiWidget(), mEventSink(this)
{
  SetObjectName(_T("ProjectGenerator"));
  mpTimer = NULL;
  
  GetPreferences().GetString(PREFERENCES_PROJECTGENERATOR, _T("nuiSourcePath"), mNuiSourcePath);
  if (!GetPreferences().GetString(PREFERENCES_PROJECTGENERATOR, _T("nuiTargetPath"), mProjectTargetPath))
  {
    nglPath userPath(ePathUser);
    mProjectTargetPath = userPath.GetPathName();    
  }
  mProjectTargetPath.Append(_T("/newNuiProject"));
  
  
  
  nuiVBox* pVBox = new nuiVBox(0);
  pVBox->SetExpand(nuiExpandShrinkAndGrow);
  AddChild(pVBox);
  
  pVBox->AddCell(BuildBlocSourceDirectory());

//  pVBox->AddCell(new nuiSeparator(nuiHorizontal));
  
  pVBox->AddCell(BuildBlocProjectDirectory());

//  pVBox->AddCell(new nuiSeparator(nuiHorizontal));
  
  pVBox->AddCell(BuildBlocOptions());

//  pVBox->AddCell(new nuiSeparator(nuiHorizontal));
  
  pVBox->AddCell(BuildBlocButtons());
  
  
  
  
  mpTimer = new nuiTimer(0.5 /*0.5s*/);
  mEventSink.Connect(mpTimer->Tick, &ProjectGenerator::OnTimerTick);
  
  if (mNuiSourcePath != nglString::Null)
  {
    nuiEvent event;
    OnTimerTick(event);
  }
}






//*******************************************************************
// bloc for nui source directory
//
nuiWidget* ProjectGenerator::BuildBlocSourceDirectory()
{
  nuiHBox* pMainBox = new nuiHBox(2);
  
  // icon
  mpIconSourceDirectory = new nuiImage();
  mpIconSourceDirectory->SetObjectName(_T("Icon::SourceDirectory::Disabled"));
  mpIconSourceDirectory->SetPosition(nuiTopLeft);
  pMainBox->SetCell(0, mpIconSourceDirectory);
  
  // ui bloc
  nuiVBox* pBloc = new nuiVBox(0);
  pMainBox->SetCell(1, pBloc);
  pMainBox->SetCellExpand(1, nuiExpandShrinkAndGrow);
  
  pBloc->SetExpand(nuiExpandShrinkAndGrow);
  pBloc->SetBorder(20,40,40,40);
  
  pBloc->AddCell(new nuiLabel(_T("nui source directory:")));
  pBloc->SetCellExpand(pBloc->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  // edit line
  nuiHBox* pBox = new nuiHBox(2);
  pBloc->AddCell(pBox);
  mpNuiSource = new nuiEditLine(mNuiSourcePath);
  mpNuiSource->SetEnabled(true);
  mpNuiSource->SetPosition(nuiFillHorizontal);
  mEventSink.Connect(mpNuiSource->TextChanged, &ProjectGenerator::OnSourceTextChanged);
  
  pBox->SetCell(0, mpNuiSource);
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  nuiButton* pBtn = new nuiButton(_T("browse"));
  pBox->SetCell(1, pBtn);
  mEventSink.Connect(pBtn->Activated, &ProjectGenerator::OnBrowseSource);
  
  // separation line
  pBloc->AddCell(NULL);
  pBloc->SetCellPixels(pBloc->GetNbCells()-1, 10);
  
  
  // read-only information : nui3 selected directory checks
  nuiHBox* pHBox = new nuiHBox(0);
  pBloc->AddCell(pHBox);
  
  pHBox->AddCell(new nuiLabel(_T("nui3 project file: ")));
  mpNuiCheckProjectFile = new nuiLabel(_T(""));
  mNuiCheckProjectFile = false;
  pHBox->AddCell(mpNuiCheckProjectFile);
  pHBox->SetCellExpand(pHBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  pHBox->AddCell(NULL);
  pHBox->SetCellPixels(pHBox->GetNbCells()-1, 10);
  
  pHBox->AddCell(new nuiLabel(_T("nui3 tools: ")));
  mpNuiCheckTools = new nuiLabel(_T(""));
  mNuiCheckTools = false;
  pHBox->AddCell(mpNuiCheckTools);
  pHBox->SetCellExpand(pHBox->GetNbCells()-1, nuiExpandShrinkAndGrow);

  pHBox->AddCell(NULL);
  pHBox->SetCellPixels(pHBox->GetNbCells()-1, 10);
  
  pHBox->AddCell(new nuiLabel(_T("nui3 template app: ")));
  mpNuiCheckTemplate = new nuiLabel(_T(""));
  mNuiCheckTemplate = false;
  pHBox->AddCell(mpNuiCheckTemplate);
  pHBox->SetCellExpand(pHBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  
  
  
  
  // read-only information : nui3 directory relative path from new project
  pBloc->AddCell(new nuiLabel(_T("nui3 relative path to project:")));
  mpNuiRelativeSource = new nuiLabel(mNuiRelativeSource.GetPathName());
  mpNuiRelativeSource->SetEnabled(false);
  pBloc->AddCell(mpNuiRelativeSource);

  return pMainBox;
}




//***********************************************************************
// bloc for new project target
//
nuiWidget* ProjectGenerator::BuildBlocProjectDirectory()
{
  nuiHBox* pMainBox = new nuiHBox(2);
  
  // icon
  mpIconProjectDirectory = new nuiImage();
  mpIconProjectDirectory->SetObjectName(_T("Icon::ProjectDirectory"));
  mpIconProjectDirectory->SetPosition(nuiTopLeft);
  pMainBox->SetCell(0, mpIconProjectDirectory);
  
  nuiVBox* pBloc = new nuiVBox(0);
  pMainBox->SetCell(1, pBloc);
  pMainBox->SetCellExpand(1, nuiExpandShrinkAndGrow);
  pBloc->SetExpand(nuiExpandShrinkAndGrow);
  pBloc->SetBorder(20,40,40,40);
  
  pBloc->AddCell(new nuiLabel(_T("new project target:")));
  
  nuiHBox* pBox = new nuiHBox(2);
  pBloc->AddCell(pBox);
  mpProjectTarget = new nuiEditLine(mProjectTargetPath);
  mpProjectTarget->SetPosition(nuiFillHorizontal);
  mpProjectTarget->SetEnabled(true);
  mEventSink.Connect(mpProjectTarget->TextChanged, &ProjectGenerator::OnTargetTextChanged);
  
  pBox->SetCell(0, mpProjectTarget);
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  nuiButton* pBtn = new nuiButton(_T("browse"));
  pBox->SetCell(1, pBtn);
  mEventSink.Connect(pBtn->Activated, &ProjectGenerator::OnBrowseTarget);  
  
  
  // separation line
  pBloc->AddCell(NULL);
  pBloc->SetCellPixels(pBloc->GetNbCells()-1, 10);
  
  // read-only bloc for project name, extracted from project target
  nglString projectFilename = mProjectTargetPath;
  projectFilename += _T("/newNuiProject.xcodeproj");
  pBloc->AddCell(new nuiLabel(_T("project file:")));
  mpProjectFilename = new nuiLabel(projectFilename);
  pBloc->AddCell(mpProjectFilename);
  mpProjectFilename->SetEnabled(false);

  return pMainBox;
}



//***********************************************************************
// bloc for options
//
nuiWidget* ProjectGenerator::BuildBlocOptions()
{
  nuiVBox* pMainBox = new nuiVBox(0);
  pMainBox->SetBorder(0, 20);
  
  
  // MacOS X xcode
  nuiHBox* pBloc = new nuiHBox(3);
  pMainBox->AddCell(pBloc);
  // icon
  nuiImage* pIcon = new nuiImage();
  pIcon->SetObjectName(_T("Icon::Xcode"));
  pIcon->SetToken(new nuiToken<nglString>(_T("Icon::Xcode")));
  pIcon->SetPosition(nuiTopLeft);
  pBloc->SetCell(0, pIcon);
  pBloc->SetCellPixels(0, 48);
  // check box
  mpCheckXcode = new nuiToggleButton();
  mpCheckXcode->SetToken(new nuiToken<nuiRef<nuiImage> >(pIcon));
  mEventSink.Connect(mpCheckXcode->ButtonPressed, &ProjectGenerator::OnIconUpdate, (void*)mpCheckXcode);
  mEventSink.Connect(mpCheckXcode->ButtonDePressed, &ProjectGenerator::OnIconUpdate, (void*)mpCheckXcode);
  mpCheckXcode->SetPressed(true);
  mpCheckXcode->SetBorder(25,0,0, 0);
  pBloc->SetCell(1, mpCheckXcode, nuiCenter);
  // label
  pBloc->SetCell(2, new nuiLabel(nuiTR("generate Xcode project file for Mac OS X, iPhone/iPad")));
  

  
  // Visual Studio 2008
  pBloc = new nuiHBox(3);
  pMainBox->AddCell(pBloc);
  // icon
  pIcon = new nuiImage();
  pIcon->SetObjectName(_T("Icon::VisualStudio2008"));
  pIcon->SetToken(new nuiToken<nglString>(_T("Icon::VisualStudio2008")));
  pIcon->SetPosition(nuiTopLeft);
  pBloc->SetCell(0, pIcon);
  pBloc->SetCellPixels(0, 48);
  // check box
  mpCheckVisualStudio2008 = new nuiToggleButton();
  mpCheckVisualStudio2008->SetToken(new nuiToken<nuiRef<nuiImage> >(pIcon));
  mEventSink.Connect(mpCheckVisualStudio2008->ButtonPressed, &ProjectGenerator::OnIconUpdate, (void*)mpCheckVisualStudio2008);
  mEventSink.Connect(mpCheckVisualStudio2008->ButtonDePressed, &ProjectGenerator::OnIconUpdate, (void*)mpCheckVisualStudio2008);
  mpCheckVisualStudio2008->SetPressed(true);
  mpCheckVisualStudio2008->SetBorder(25,0,0, 0);
  pBloc->SetCell(1, mpCheckVisualStudio2008, nuiCenter);
  // label
  pBloc->SetCell(2, new nuiLabel(nuiTR("generate MS Visual Studio 2008 project file for Win32")));
  
  
  
  
  return pMainBox;
}



void ProjectGenerator::OnIconUpdate(const nuiEvent& rEvent)
{
  nuiToggleButton* pBtn = (nuiToggleButton*)rEvent.mpUser;
  nuiRef<nuiImage> pIcon;
  nglString objectName;
  
  nuiGetTokenValue<nuiRef<nuiImage> >(pBtn->GetToken(), pIcon);
  nuiGetTokenValue<nglString>(pIcon->GetToken(), objectName);
  
  if (!pBtn->IsPressed())
    objectName += nglString(_T("::Disabled"));

  pIcon->SetObjectName(objectName);
  
  rEvent.Cancel();
}



//***********************************************************************
// bloc for buttons
//
nuiWidget* ProjectGenerator::BuildBlocButtons()
{
  nuiHBox* pBox = new nuiHBox(2);
  pBox->SetExpand(nuiExpandShrinkAndGrow);
  pBox->SetBorder(20,40,100,40);
  
  nuiButton* pGeneratorButton = new nuiButton(_T("generate project"));
  pGeneratorButton->SetPosition(nuiRight);
  pBox->AddCell(pGeneratorButton);
  pBox->SetCellExpand(pBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  nuiButton* pQuitButton = new nuiButton(_T("quit"));
  pQuitButton->SetPosition(nuiRight);
  pBox->AddCell(pQuitButton);
  
  mEventSink.Connect(pGeneratorButton->Activated, &ProjectGenerator::OnGenerateButton);
  mEventSink.Connect(pQuitButton->Activated, &ProjectGenerator::OnQuitButton);
  
  return pBox;
}


void ProjectGenerator::OnSourceTextChanged(const nuiEvent& rEvent)
{
  mpTimer->Stop();
  mpTimer->Start(false);
    
  rEvent.Cancel();
}

void ProjectGenerator::OnTimerTick(const nuiEvent& rEvent)
{
  mpTimer->Stop();
  
  // check if nui3 project file has been found
  nglString text = mpNuiSource->GetText();
  text.Trim();
  
  nglPath path(text);
  
  nglPath proj = path;
  proj += nglPath(_T("nui3.xcodeproj"));
  
  bool allOK = true;
  
  if (proj.Exists())
  {
    mNuiCheckProjectFile = true;
    mpNuiCheckProjectFile->SetText(_T("found"));
//    mpNuiCheckProjectFile->SetColor(eNormalTextFg, nuiColor(_T("green")));

    allOK &= true;
  }
  else
  {
    mNuiCheckProjectFile = false;
    mpNuiCheckProjectFile->SetText(_T("not found!"));
//    mpNuiCheckProjectFile->SetColor(eNormalTextFg, nuiColor(_T("red")));

    allOK &= false;
  }
  
  // check if nui3 tool has been found (make_rc.py is taken as a reference)
  nglPath tool = path;
  tool += nglPath(_T("tools/make_rc.py"));
  
  if (tool.Exists())
  {
    mNuiCheckTools = true;
    mpNuiCheckTools->SetText(_T("found"));
//    mpNuiCheckTools->SetColor(eNormalTextFg, nuiColor(_T("green")));

    allOK &= true;
  }
  else
  {
    mNuiCheckTools = false;
    mpNuiCheckTools->SetText(_T("not found!"));
//    mpNuiCheckTools->SetColor(eNormalTextFg, nuiColor(_T("red")));
  
    allOK &= false;
  }
  
  nglPath templatePath = path;
  templatePath += nglPath(_T("tools/TemplateApp/TemplateApp.xcodeproj"));
  if (templatePath.Exists())
  {
    mNuiCheckTemplate = true;
    mpNuiCheckTemplate->SetText(_T("found"));
//    mpNuiCheckTemplate->SetColor(eNormalTextFg, nuiColor(_T("green")));
    allOK &= true;
  }
  else
  {
    mNuiCheckTemplate = false;
    mpNuiCheckTemplate->SetText(_T("not found!"));
//    mpNuiCheckTemplate->SetColor(eNormalTextFg, nuiColor(_T("red")));
    allOK &= false;
  }
  
  
  
  if (allOK)
    mpIconSourceDirectory->SetObjectName(_T("Icon::SourceDirectory"));
  else
    mpIconSourceDirectory->SetObjectName(_T("Icon::SourceDirectory::Disabled"));

  
  // compute nui relative path to project
  mNuiRelativeSource = nglPath(mpNuiSource->GetText());
  mNuiRelativeSource.MakeRelativeTo(nglPath(mpProjectTarget->GetText()));
  mpNuiRelativeSource->SetText(mNuiRelativeSource.GetPathName());
  
  mNuiTemplatePath = nglPath(mpNuiSource->GetText());
  mNuiTemplatePath += nglPath(_T("tools/TemplateApp"));
  
  rEvent.Cancel();
}


void ProjectGenerator::OnTargetTextChanged(const nuiEvent& rEvent)
{
  nglString text = mpProjectTarget->GetText();
  text.Trim();
  
  mpIconProjectDirectory->SetObjectName(_T("Icon::ProjectDirectory"));
  
  nglPath path(text);
  
  nglString newtext = text + _T("/") + path.GetNodeName() + _T(".xcodeproj");
  mpProjectFilename->SetText(newtext);
  
  mpTimer->Stop();
  mpTimer->Start(false);
  
  rEvent.Cancel();
}




ProjectGenerator::~ProjectGenerator()
{
  mpTimer->Stop();
  delete mpTimer;
}


void ProjectGenerator::OnGenerateButton(const nuiEvent& rEvent)
{
  nglString source = mpNuiSource->GetText();
  nglString target = mpProjectTarget->GetText();
  source.Trim();
  target.Trim();
  
  if ((source == nglString::Null) || (target == nglString::Null))
  {
    nuiMessageBox* pMessageBox = new nuiMessageBox(GetMainWindow(), _T("Project Creator"), _T("source and target information can't be empty!"), eMB_OK);
    pMessageBox->QueryUser();   
    rEvent.Cancel();
    return;
  }

  nglPath sourcePath(source);
  if (!sourcePath.Exists())
  {
    nglString msg;
    msg.Format(_T("the nui source directory '%ls' does not exist!"), sourcePath.GetChars());
    nuiMessageBox* pMessageBox = new nuiMessageBox(GetMainWindow(), nglString(_T("Project Creator")), msg, eMB_OK);
    pMessageBox->QueryUser();     
    rEvent.Cancel();
    return;
  }
  
  if (!mNuiCheckProjectFile || !mNuiCheckTools)
  {
    nglString msg;
    msg.Format(_T("Parts of nui could not be found.\nCheck the nui source directory or checkout the complete nui sources from libnui.net!"));
    nuiMessageBox* pMessageBox = new nuiMessageBox(GetMainWindow(), nglString(_T("Project Creator")), msg, eMB_OK);
    pMessageBox->QueryUser();
    rEvent.Cancel();
    return;        
  }

  mNuiSourcePath = source;
  mProjectTargetPath = target;
  nglPath path = nglPath(target);
  mProjectName = path.GetNodeName();

  nglPath targetPath(target);
  nglString xcodeproj = mProjectName + _T(".xcodeproj");
  targetPath += nglPath(xcodeproj);
  if (targetPath.Exists())
  {
    nglString msg;
    msg.Format(_T("the following project exists already!\n'%ls'"), targetPath.GetChars());
    nuiMessageBox* pMessageBox = new nuiMessageBox(GetMainWindow(), nglString(_T("Project Creator")), msg, eMB_OK);
    pMessageBox->QueryUser();     
    rEvent.Cancel();
    return;
  }
  
  
  GetPreferences().Save();
  
  Make();
  
  rEvent.Cancel();
}



void ProjectGenerator::OnBrowseSource(const nuiEvent& rEvent)
{
  mNuiSourcePath.Trim();
  if (mNuiSourcePath == nglString::Null)
  {
    nglPath path(ePathUser);
    mNuiSourcePath = path.GetPathName();
  }

  nuiDialogSelectDirectory* pDialog = new nuiDialogSelectDirectory(GetMainWindow(), _T("SELECT THE NUI SOURCE DIRECTORY"), mNuiSourcePath, nglPath(_T("/")));
  mEventSink.Connect(pDialog->DirectorySelected, &ProjectGenerator::OnSourceSelected, (void*)pDialog);
  
  rEvent.Cancel();
  return;
}


void ProjectGenerator::OnSourceSelected(const nuiEvent& rEvent)
{
  nuiDialogSelectDirectory* pDialog = (nuiDialogSelectDirectory*)rEvent.mpUser;
  mNuiSourcePath = pDialog->GetSelectedDirectory();
  mpNuiSource->SetText(mNuiSourcePath);
  
  OnTimerTick(rEvent);

  GetPreferences().SetString(PREFERENCES_PROJECTGENERATOR, _T("nuiSourcePath"), mNuiSourcePath);
}




void ProjectGenerator::OnBrowseTarget(const nuiEvent& rEvent)
{
  nglPath path = nglPath(mProjectTargetPath).GetParent();
  
  nuiDialogSelectDirectory* pDialog = new nuiDialogSelectDirectory(GetMainWindow(), _T("ENTER THE NEW PROJECT TARGET"), path, nglPath(_T("/")));
  mEventSink.Connect(pDialog->DirectorySelected, &ProjectGenerator::OnTargetSelected, (void*)pDialog);
  rEvent.Cancel();
}


void ProjectGenerator::OnTargetSelected(const nuiEvent& rEvent)
{
  nuiDialogSelectDirectory* pDialog = (nuiDialogSelectDirectory*)rEvent.mpUser;
  mProjectTargetPath = pDialog->GetSelectedDirectory();
  mpProjectTarget->SetText(mProjectTargetPath);
  
  nglPath path(mProjectTargetPath);
  
  OnTargetTextChanged(rEvent);
  if (!rEvent.IsCanceled())
  {
    rEvent.Cancel();
    return;
  }
  
  GetPreferences().SetString(PREFERENCES_PROJECTGENERATOR, _T("nuiTargetPath"), path.GetParent().GetPathName());  
}



bool ProjectGenerator::Make()
{
  NGL_OUT(_T("nui project generator\n"));
  

  // create target directory
  nglPath targetpath = nglPath(mProjectTargetPath);
  if (!targetpath.Create())
  {
    nglString msg;
    msg.Format(_T("creating target directory '%ls'"), targetpath.GetChars());
    return MsgError(msg);
  }

  NGL_OUT(_T("nui project generator : target directory created '%ls'\n"), targetpath.GetChars());

    
  //copy the src folder 
  if (!CopyDirectory(targetpath + nglPath(_T("src")), mNuiTemplatePath + nglPath(_T("src"))))
    return false;
  

  //copy the resources folders
  if (!CopyDirectory(targetpath + nglPath(_T("resources")), mNuiTemplatePath + nglPath(_T("resources"))))
    return false;
  if (!CopyDirectory(targetpath + nglPath(_T("resources/css")), mNuiTemplatePath + nglPath(_T("resources/css"))))
    return false;
  if (!CopyDirectory(targetpath + nglPath(_T("resources/decorations")), mNuiTemplatePath + nglPath(_T("resources/decorations"))))
    return false;

  
  nglPath projpath;
  nglPath projectfile;
  nglString filename;
  
  // create xcodeproj folder
  if (mpCheckXcode->IsPressed())
  {
    projpath = targetpath;
    nglString projfolder = mProjectName + nglString(_T(".xcodeproj"));
    projpath += nglPath(projfolder);
    if (!projpath.Create())
    {
      nglString msg;
      msg.Format(_T("creating xcodeproj folder '%ls'"), projpath.GetChars());
      return MsgError(msg);
    }
      
    NGL_OUT(_T("nui project generator : project folder created '%ls'\n"), projpath.GetChars());

  
    // generate xcode project file
    projectfile = targetpath;
    projectfile += nglPath(projfolder);
    projectfile += nglPath(_T("project.pbxproj"));
    if (!GenerateFile(mNuiTemplatePath + nglPath(_T("TemplateApp.xcodeproj/project.pbxproj")), projectfile))
      return false;
    
  }

  
  // generate visual studio 2008 project file
  if (mpCheckVisualStudio2008->IsPressed())
  {
    filename = mProjectName + nglString(_T(".2008.vcproj"));
    projectfile = targetpath;
    projectfile += nglPath(filename);
    if (!GenerateFile(mNuiTemplatePath + nglPath(_T("TemplateApp.2008.vcproj")), projectfile))
      return false;
  }
  
  // generate visual studio 2008 solution file
  if (mpCheckVisualStudio2008->IsPressed())
  {
    filename = mProjectName + nglString(_T(".2008.sln"));
    projectfile = targetpath;
    projectfile += nglPath(filename);
    if (!GenerateFile(mNuiTemplatePath + nglPath(_T("TemplateApp.2008.sln")), projectfile))
      return false;
  }
  
  
  
  // generate Info.plist
  if (mpCheckXcode->IsPressed())
  {
    filename = mProjectName + nglString(_T(".plist"));
    projectfile = targetpath;
    projectfile += nglPath(filename);
    if (!GenerateFile(mNuiTemplatePath + nglPath(_T("TemplateApp.plist")), projectfile))
      return false;
  }

  // generate iPhone Info.plist
  if (mpCheckXcode->IsPressed())
  {
    filename = mProjectName + nglString(_T("-iPhone.plist"));
    projectfile = targetpath;
    projectfile += nglPath(filename);
    if (!GenerateFile(mNuiTemplatePath + nglPath(_T("TemplateApp-iPhone.plist")), projectfile))
      return false;
  }
  
  
  
  // generate resource.rc
  filename = _T("resource.rc");
  projectfile = targetpath;
  projectfile += nglPath(filename);
  if (!GenerateFile(mNuiTemplatePath + nglPath(_T("resource.rc")), projectfile))
    return false;
   

  nglString msg;
  msg.Format(_T("nui project '%ls' successfully generated!"), mProjectName.GetChars());
  nuiMessageBox* pMessageBox = new nuiMessageBox(GetMainWindow(), nglString(_T("Project Creator")), msg, eMB_OK);
  pMessageBox->QueryUser();      
  
  return true;
  
  
}


bool ProjectGenerator::CopyDirectory(const nglPath& targetPath, const nglPath& srcpath)
{
  // create folder
  if (!targetPath.Create())
  {
    nglString msg;
    msg.Format(_T("creating target folder '%ls'"), targetPath.GetChars());
    return MsgError(msg);
  }
  
  
  std::vector<nglPath> children;
  srcpath.GetChildren(&children);
  for (auto it = children.begin(); it != children.end(); ++it)
  {
    const nglPath& srcpath = *it;
    
    if (!srcpath.IsLeaf())
      continue;
    
    nglPath dstpath = targetPath;
    dstpath += srcpath.GetNodeName();
    
    nglString contents;
    
    nglIStream* piFile = srcpath.OpenRead();
    if (!piFile)
    {
      nglString msg;
      msg.Format(_T("opening for reading input file '%ls'"), srcpath.GetChars());
      return MsgError(msg);
    }
    
    nglOStream* poFile = dstpath.OpenWrite(false);
    if (!poFile)
    {
      nglString msg;
      msg.Format(_T("opening for writing output file '%ls'"), dstpath.GetChars());
      return MsgError(msg);
    }
    
    piFile->PipeTo(*poFile);
    delete poFile;
    delete piFile;
    
    NGL_OUT(_T("nui project generator : created file '%ls'\n"), dstpath.GetChars());
  }
  
  return true;
}


bool ProjectGenerator::GenerateFile(const nglPath& src, const nglPath& dst)
{
  uint32 srcsize = (uint32)src.GetSize();
  
  nglIStream* pFile = src.OpenRead();
  if (!pFile)
  {
    nglString msg;
    msg.Format(_T("reading input file '%ls'"), src.GetChars());
    return MsgError(msg);
  }

  char* str = new char[srcsize + 1];
  pFile->Read(str, srcsize, 1);
  str[srcsize] = 0;
  delete pFile;
  
  nglString contents(str);
  contents.Replace(_T("TemplateApp"), mProjectName);
  contents.Replace(_T("../../../nui3"), mNuiRelativeSource.GetPathName());

  
  nglOStream* poFile = dst.OpenWrite(false);
  if (!poFile)
  {
    nglString msg;
    msg.Format(_T("writing output file '%ls'"), dst.GetChars());
    return MsgError(msg);
  }

  char* ptr = contents.Export();
  
  poFile->Write(ptr, contents.GetLength(), 1);
  delete poFile;
  if (ptr)
    free(ptr);

  NGL_OUT(_T("nui project generator : generated '%ls'\n"), dst.GetChars());
  return true;
}


bool ProjectGenerator::MsgError(const nglString& error)
{
  nglString msg;
  msg.Format(_T("error %ls"), error.GetChars());
  NGL_OUT(msg);
  nuiMessageBox* pMessageBox = new nuiMessageBox(GetMainWindow(), _T("Project Creator"), msg, eMB_OK);
  pMessageBox->QueryUser();  
  return false;
}


void ProjectGenerator::OnQuitButton(const nuiEvent& rEvent)
{
  GetApp()->Quit();
  rEvent.Cancel();
}
