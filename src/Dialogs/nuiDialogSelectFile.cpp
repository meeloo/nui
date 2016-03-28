/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


nuiDialogSelectFile::nuiDialogSelectFile(nuiMainWindow* pParent, const nglString& rTitle, const nglPath& rPath, const nglPath& rRootPath, const nglString& defaultEntry, const nglString& rFilter, bool showHiddenFiles, nuiSize Left, nuiSize Top, nuiSize Width, nuiSize Height)
: nuiDialog(pParent), mpParent(pParent), mPath(rPath), mEventSink(this)
{
  std::list<nglString> filters;
  filters.push_back(rFilter);
  Init(pParent, rTitle, rPath, rRootPath, defaultEntry, filters, showHiddenFiles, Left, Top, Width, Height);
}

nuiDialogSelectFile::nuiDialogSelectFile(nuiMainWindow* pParent, const nglString& rTitle, const nglPath& rPath, const nglPath& rRootPath, const nglString& defaultEntry, const std::list<nglString>& rFilters, bool showHiddenFiles, nuiSize Left, nuiSize Top, nuiSize Width, nuiSize Height)
: nuiDialog(pParent), mpParent(pParent), mPath(rPath), mEventSink(this)
{
  Init(pParent, rTitle, rPath, rRootPath, defaultEntry, rFilters, showHiddenFiles, Left, Top, Width, Height);
}


void nuiDialogSelectFile::Init(nuiMainWindow* pParent, const nglString& rTitle, const nglPath& rPath, const nglPath& rRootPath, const nglString& defaultEntry, const std::list<nglString>& rFilters, bool showHiddenFiles, nuiSize Left, nuiSize Top, nuiSize Width, nuiSize Height)
{
  mPath = rPath;
  mRootPath = rRootPath;

  nuiWidget* pEditContainer = new nuiWidget();
  pEditContainer->SetBorder(16,16,10,0);
  
  mpEditLine = new nuiEditLine();
  mpEditLine->SetObjectName("nuiDialog::EditLine");
  mpEditLine->SetObjectClass("nuiDialog::EditLine");
  mpEditLine->SetNormalTextFg(nuiColor(64,64,64));
//  mpEditLine->SetSelectedTextFg(nuiColor(64,64,64));
  pEditContainer->AddChild(mpEditLine);
	
  mpContainer = new nuiWidget();
  
  
  mpSelector = new nuiFileSelector(rPath, rRootPath, rFilters, mpEditLine);
  mpSelector->SetBorder(6,0);
  
  mEventSink.Connect(mpSelector->OK, &nuiDialogSelectFile::OnSelectorOK);
  
  nuiVBox* pBox = new nuiVBox();
  mpContainer->AddChild(mpSelector);
  pBox->AddCell(mpContainer);
  

  
  pBox->AddCell(pEditContainer);
  
  nuiSize userWidth = (Width == 0.f) ? mpParent->GetWidth() * .8 : Width;
  nuiSize userHeight = (Height == 0.f) ? mpParent->GetHeight() * .8 : Height;
  mpContainer->SetUserSize(userWidth, userHeight);
  
  
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  pBox->SetCellExpand(1, nuiExpandFixed);
  

  
  nuiLabel* pTitle = new nuiLabel(rTitle);
//  pTitle->SetObjectName("nuiDialogSelectFile::Title");
//  pTitle->SetObjectClass("nuiDialogSelectFile::Title");
  
  nuiWidget* pLayoutPane = new nuiWidget();
  pLayoutPane->SetObjectClass("nuiDialog::Pane");
  pLayoutPane->SetObjectName("nuiDialog::Pane");
//  pLayoutPane->SetNormalTextFg(nuiColor(0,0,0));

  nuiDefaultDecoration::Dialog(pLayoutPane);
  
  InitDialog(pTitle, pLayoutPane, nuiDialog::eDialogButtonOk | nuiDialog::eDialogButtonCancel);
    

  
  nuiButton* pButton = new nuiButton("New Folder");
  pButton->SetObjectName("nuiDialog::Button");
  AddButton(pButton, nuiDialog::eDialogButtonCustom);
  mEventSink.Connect(pButton->Activated, &nuiDialogSelectFile::OnCreateNewFolder);
  
  nuiDialog::GetButtonsGrid()->SetBorder(0,10,0,10);
  
  
  SetContents(pBox, nuiCenter);
  if ((Top == 0.f) && (Left == 0.f) && (Width == 0.f) && (Height == 0.f))
    SetDefaultPos();
  else
    SetUserPos(Left, Top);
  mEventSink.Connect(DialogDone, &nuiDialogSelectFile::OnDialogDone);
  
  mpSelector->UpdateLayout();

  mpEditLine->SetText(defaultEntry);
}



nuiDialogSelectFile::~nuiDialogSelectFile()
{
  
}



void nuiDialogSelectFile::OnSelectorOK(const nuiEvent& rEvent)
{
//  nglString text = mpEditLine->GetText();
//  text.Trim();
//  if (text.IsNull())
//    return false;
  
  mPath = mpSelector->GetPath();
  mRootPath = mpSelector->GetRootPath();
  
  // send event and close the dialog if the user answered true
  if (!FileSelected())
    Trash();
}


void nuiDialogSelectFile::OnCreateNewFolder(const nuiEvent& rEvent)
{
  mpCreateDialog = new nuiDialog(mpParent);
  nuiWidget* pContainer = new nuiWidget();
  pContainer->SetUserSize(400, 80);
  
  mpCreateEditLine = new nuiEditLine();
  mpCreateEditLine->SetPosition(nuiFillHorizontal);
  pContainer->AddChild(mpCreateEditLine);
  
  mpCreateDialog->InitDialog("CREATE A NEW FOLDER", NULL, nuiDialog::eDialogButtonOk + nuiDialog::eDialogButtonCancel);
  
  mpCreateDialog->SetContents(pContainer);
  mpCreateDialog->SetDefaultPos();
  mEventSink.Connect(mpCreateDialog->DialogDone, &nuiDialogSelectFile::OnCreateNewFolderDone);
  
  mpCreateEditLine->Focus();
  
  NGL_OUT("new folder \n");
  
  rEvent.Cancel();
}


void nuiDialogSelectFile::OnCreateNewFolderDone(const nuiEvent& rEvent)
{
  
  nuiDialog::DialogResult result = mpCreateDialog->GetResult();
  
  if (result == nuiDialog::eDialogAccepted)
  {
    nglString text = mpCreateEditLine->GetText();
    text.Trim();
    if (text.IsNull())
    {
      rEvent.Cancel();
      return;
    }
    
    mPath = mpSelector->GetFolderPath();
    mPath += nglPath(mpCreateEditLine->GetText());
    mPath.Create();
    
    //mRootPath = mpSelector->GetRootPath();
    mpSelector->SetRootPath(mPath.GetParent());
  }  
}

void nuiDialogSelectFile::OnDialogDone(const nuiEvent& rEvent)
{
  nuiDialog::DialogResult result = GetResult();
  
  if (result == nuiDialog::eDialogAccepted)
  {
    OnSelectorOK(rEvent);
    rEvent.Cancel();
  }
}


const nglPath& nuiDialogSelectFile::GetSelectedFile()
{
  return mPath;
}

const nglPath& nuiDialogSelectFile::GetRootPath()
{
  return mRootPath;
}

