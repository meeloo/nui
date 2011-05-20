/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "nuiDefaultDecoration.h"
#include "nuiColorDecoration.h"
#include "nuiGradientDecoration.h"
#include "nuiBorderDecoration.h"
#include "nuiMetaDecoration.h"
#include "nuiImageDecoration.h"
#include "../graphics/DefaultDecoration.h"
#include "nuiFrame.h"
#include "nuiMessageBox.h"
#include "nuiTabView.h"
#include "nuiBackgroundPane.h"
#include "nuiKnob.h"
#include "nuiNavigationBar.h"
#include "nuiNavigationButton.h"

nuiDefaultDecoration::nuiDefaultDecoration()
{
  NGL_ASSERT(0);
}


nuiDefaultDecoration::~nuiDefaultDecoration()
{
  NGL_ASSERT(0);
} 


//**************************************************************************************************************
//
// Init
//
// connect the object class to the decoration method
//
void nuiDefaultDecoration::Init()
{
  InitColors();
  InitSelection();
  InitMaps();
  InitIcons();
  InitImages();
    
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiWindow")), &nuiDefaultDecoration::Window);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiBackgroundPane")), &nuiDefaultDecoration::BackgroundPane);

  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiLabel")), &nuiDefaultDecoration::Label);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiEditText")), &nuiDefaultDecoration::EditText);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiEditLine")), &nuiDefaultDecoration::EditLine);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiSplitter")), &nuiDefaultDecoration::Splitter);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiTitledPane")), &nuiDefaultDecoration::TitledPane);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFolderPane")), &nuiDefaultDecoration::FolderPane);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiButton")), &nuiDefaultDecoration::Button);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiRadioButton")), &nuiDefaultDecoration::RadioButton);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiCloseButton")), &nuiDefaultDecoration::CloseButton);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiToggleButton")), &nuiDefaultDecoration::ToggleButton);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiKnob")), &nuiDefaultDecoration::KnobSequence);

  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiComboBox")), &nuiDefaultDecoration::ComboBox);

  // dialogs
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiDialog::Title")), &nuiDefaultDecoration::Dialog_Title);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiDialog::EditLine")), &nuiDefaultDecoration::Dialog_EditLine);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiMessageBox::Title")), &nuiDefaultDecoration::MessageBox_Title);
  
  
  // FileSelector
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::FolderView")), &nuiDefaultDecoration::FileSelector_FolderView);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::InfoView")), &nuiDefaultDecoration::FileSelector_InfoView);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::FolderLabel")), &nuiDefaultDecoration::FileSelector_FolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::VolumeIcon")), &nuiDefaultDecoration::FileSelector_VolumeIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::FolderIcon")), &nuiDefaultDecoration::FileSelector_FolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::TreeView")), &nuiDefaultDecoration::FileSelector_TreeView);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::TreeFolderLabel")), &nuiDefaultDecoration::FileSelector_TreeFolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::TreeFileLabel")), &nuiDefaultDecoration::FileSelector_TreeFileLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::TreeFolderIcon")), &nuiDefaultDecoration::FileSelector_TreeFolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileSelector::TreeFileIcon")), &nuiDefaultDecoration::FileSelector_TreeFileIcon);

  // FileTree
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree")), &nuiDefaultDecoration::FileTree_View);
//  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::Node")), &nuiDefaultDecoration::FileTree_Node);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::ColumnLabel")), &nuiDefaultDecoration::FileTree_ColumnLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::VolumeLabel")), &nuiDefaultDecoration::FileTree_FolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::FolderLabel")), &nuiDefaultDecoration::FileTree_FolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::FileLabel")), &nuiDefaultDecoration::FileTree_FileLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::FileInfo")), &nuiDefaultDecoration::FileTree_FileInfo);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::VolumeIcon")), &nuiDefaultDecoration::FileTree_VolumeIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::FolderIcon")), &nuiDefaultDecoration::FileTree_FolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::OpenFolderIcon")), &nuiDefaultDecoration::FileTree_OpenFolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::ParentFolderIcon")), &nuiDefaultDecoration::FileTree_ParentFolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiFileTree::FileIcon")), &nuiDefaultDecoration::FileTree_FileIcon);

  // NavigationViews
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex(_T("nuiNavigationBar")), &nuiDefaultDecoration::NavigationBar);
  
  
  App->AddExit(nuiDefaultDecoration::Exit);
}


void nuiDefaultDecoration::Exit()
{
  if (mpKnobSequence)
    mpKnobSequence->Release();
  mpKnobSequence = NULL;

  mIcons.clear();
  mImages.clear();
  
  nuiTexture::ClearAll();
}



//**************************************************************************************************************
//
// InitColors
//
void nuiDefaultDecoration::InitColors()
{
  nuiColor::SetColor(_T("nuiDefaultClrWindowBkg1"), nuiColor(225,230,235));
  nuiColor::SetColor(_T("nuiDefaultClrWindowBkg2"), nuiColor(255,255,255));

  nuiColor::SetColor(_T("nuiDefaultClrCaptionBkg1"), nuiColor(197,197,197));
  nuiColor::SetColor(_T("nuiDefaultClrCaptionBkg2"), nuiColor(150,150,150));
  
  nuiColor::SetColor(_T("nuiDefaultClrInactiveCaptionBkg1"), nuiColor(157,157,157));
  nuiColor::SetColor(_T("nuiDefaultClrInactiveCaptionBkg2"), nuiColor(110,110,110));
  

  nuiColor::SetColor(_T("nuiDefaultClrCaptionBorder"), nuiColor(150,150,150));
  nuiColor::SetColor(_T("nuiDefaultClrCaptionBorderLight"), nuiColor(238,238,238));
  nuiColor::SetColor(_T("nuiDefaultClrCaptionBorderDark"), nuiColor(64,64,64));
  nuiColor::SetColor(_T("nuiDefaultClrInactiveCaptionBorderLight"), nuiColor(198,198,198));
  
  nuiColor::SetColor(_T("nuiDefaultClrCaptionText"), nuiColor(16,16,16));
  nuiColor::SetColor(_T("nuiDefaultClrCaptionTextLight"), nuiColor(215,215,215));
  nuiColor::SetColor(_T("nuiDefaultClrInactiveCaptionTextLight"), nuiColor(175,175,175));
  
  nuiColor::SetColor(_T("nuiDefaultClrSelection"), nuiColor(56,117,215));

  nuiColor::SetColor(_T("nuiDefaultClrNormalTab"), nuiColor(64,64,64));
  nuiColor::SetColor(_T("nuiDefaultClrSelectedTab"), nuiColor(32,32,32));

  nuiColor::SetColor(_T("nuiDefaultClrProgressFg"), nuiColor(76,145,230));
  nuiColor::SetColor(_T("nuiDefaultClrProgressBg"), nuiColor(200,200,200));
  
  nuiColor::SetColor(_T("nuiDefaultClrPaneBkg"), nuiColor(200,200,200));

  nuiColor::SetColor(_T("nuiDefaultClrBorder"), nuiColor(190,190,190));
  

}




//**************************************************************************************************************
//
// InitSelection
//
void nuiDefaultDecoration::InitSelection()
{
  nuiColor color;
  nuiColor::GetColor(_T("nuiDefaultClrSelection"), color);
  
  nuiColorDecoration* pDeco = new nuiColorDecoration(_T("nuiDefaultDecorationSelectionBackground"), 
                                                     nuiRect(3,3,0,0), color);
}




//**************************************************************************************************************
//
// InitMaps
//
// those maps are loaded in decorations, but are used dynamically in nuiTheme drawing methods, 
// during the application running
//
void nuiDefaultDecoration::InitMaps()
{
  // vertical scrollbar background
  nglIMemory* pIMem = new nglIMemory(gpScrollbarVerticalBkg, gScrollbarVerticalBkgSize);
  nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame(_T("nuiDefaultDecorationScrollBarVerticalBkg"), pTex, nuiRect(6,8,0,0));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;

  // vertical scrollbar handle
  pIMem = new nglIMemory(gpScrollbarVerticalHdl, gScrollbarVerticalHdlSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationScrollBarVerticalHdl"), pTex, nuiRect(2,6,6,0));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;
  
  // Horizontal scrollbar background
  pIMem = new nglIMemory(gpScrollbarHorizontalBkg, gScrollbarHorizontalBkgSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationScrollBarHorizontalBkg"), pTex, nuiRect(8,6,0,0));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;
  
  // Horizontal scrollbar handle
  pIMem = new nglIMemory(gpScrollbarHorizontalHdl, gScrollbarHorizontalHdlSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationScrollBarHorizontalHdl"), pTex, nuiRect(6,2,0,6));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;  
  
  
  
  // mobile scrollbar handle
  pIMem = new nglIMemory(gpScrollbarMobile, gScrollbarMobileSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationMobileScrollbarHandle"), pTex, nuiRect(5,5,1,1));
  pFrame->UseWidgetAlpha(true);
  delete pIMem; 
  
  
  
  
  
  
  // vertical slider background
  pIMem = new nglIMemory(gpSliderVerticalBkg, gSliderVerticalBkgSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationSliderVerticalBkg"), pTex, nuiRect(0,6,4,0));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;
  
  // vertical slider handle
  pIMem = new nglIMemory(gpSliderVerticalHdl, gSliderVerticalHdlSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationSliderVerticalHdl"), pTex, nuiRect(0,0,20,20));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;
  
  // Horizontal slider background
  pIMem = new nglIMemory(gpSliderHorizontalBkg, gSliderHorizontalBkgSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationSliderHorizontalBkg"), pTex, nuiRect(6,0,0,4));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;
  
  // Horizontal slider handle
  pIMem = new nglIMemory(gpSliderHorizontalHdl, gSliderHorizontalHdlSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationSliderHorizontalHdl"), pTex, nuiRect(0,0,20,20));
  pFrame->UseWidgetAlpha(true);
  delete pIMem; 
  
  
  
  // popup Menu
  pIMem = new nglIMemory(gpPopupMenu, gPopupMenuSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationPopupMenu"), pTex, nuiRect(8,8,0,14));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;  



  // arrow handle close
  pIMem = new nglIMemory(gpArrowClose, gArrowCloseSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationArrowClose"), pTex, nuiRect(0,0,7,6), nuiColor(0,0,0));
  pFrame->UseWidgetAlpha(true);

  delete pIMem;  
  
  
  // arrow handle open
  pIMem = new nglIMemory(gpArrowOpen, gArrowOpenSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationArrowOpen"), pTex, nuiRect(0,0,6,7), nuiColor(0,0,0));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;  
  
  
  
  
  // Checkerboard for color alpha viewing
  pIMem = new nglIMemory(gpCheckerboardSmall, gCheckerboardSmallSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationCheckerboardSmall"), pTex, nuiRect(0,0,20,20));
  pFrame->UseWidgetAlpha(true);
  delete pIMem;  
  

}




//**************************************************************************************************************
//
// InitIcons
//

// static 
std::list<nuiTexture*> nuiDefaultDecoration::mIcons;

void nuiDefaultDecoration::InitIcons()
{
  // volume icon
  nglIMemory* pIMem = new nglIMemory(gpIconVolume, gIconVolumeSize);
  nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
  delete pIMem;
  NGL_ASSERT(pTex);
  pTex->SetSource(_T("nuiDefaultDecorationIconVolume"));
  mIcons.push_back(pTex);
  
  // folder icon
  pIMem = new nglIMemory(gpIconFolder, gIconFolderSize);
  pTex = nuiTexture::GetTexture(pIMem);
  delete pIMem;
  NGL_ASSERT(pTex);
  pTex->SetSource(_T("nuiDefaultDecorationIconFolder"));
  mIcons.push_back(pTex);

  // open folder icon
  pIMem = new nglIMemory(gpIconOpenFolder, gIconOpenFolderSize);
  pTex = nuiTexture::GetTexture(pIMem);
  delete pIMem;
  NGL_ASSERT(pTex);
  pTex->SetSource(_T("nuiDefaultDecorationIconOpenFolder"));
  mIcons.push_back(pTex);

  // parent folder icon
  pIMem = new nglIMemory(gpIconParentFolder, gIconParentFolderSize);
  pTex = nuiTexture::GetTexture(pIMem);
  delete pIMem;
  NGL_ASSERT(pTex);
  pTex->SetSource(_T("nuiDefaultDecorationIconParentFolder"));
  mIcons.push_back(pTex);
  
  // file icon
  pIMem = new nglIMemory(gpIconFile, gIconFileSize);
  pTex = nuiTexture::GetTexture(pIMem);
  delete pIMem;
  NGL_ASSERT(pTex);
  pTex->SetSource(_T("nuiDefaultDecorationIconFile"));
  mIcons.push_back(pTex);
}




//**************************************************************************************************************
//
// InitImages
//

// static 
std::map<nglString, std::pair<char*,long> > nuiDefaultDecoration::mImages;

void nuiDefaultDecoration::InitImages()
{
  mImages[_T("nuiFileTree::DraggedFileIcon")] = std::pair<char*,long>((char*)gpImageDraggedFile,(long)gImageDraggedFileSize);
}


//static 
nglImage* nuiDefaultDecoration::GetImage(const nglString& rRef)
{
  std::map<nglString, std::pair<char*,long> >::iterator it = mImages.find(rRef);
  if (it == mImages.end())
    return NULL;
  
  const char* pMem = it->second.first;
  const long size = it->second.second;
  nglIMemory* pIMem = new nglIMemory(pMem, size);
  nglImage* pImage = new nglImage(pIMem);
  return pImage;
}




//**************************************************************************************************************
//
// nuiWindow
//
void nuiDefaultDecoration::Window(nuiWidget* pWidget)
{
  nuiWindow* pWindow = (nuiWindow*)pWidget;
  if (pWindow->GetFlags() & nuiWindow::DecoratedBackground)
  {
    nuiGradientDecoration* pDeco = (nuiGradientDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationWindow"));
    if (!pDeco)
    {
      nuiColor color1, color2;
      nuiColor::GetColor(_T("nuiDefaultClrWindowBkg1"), color1);
      nuiColor::GetColor(_T("nuiDefaultClrWindowBkg2"), color2);
      
      pDeco = new nuiGradientDecoration(_T("nuiDefaultDecorationWindow"), 
                                        nuiRect(0,0, 0,0), color1, color2, nuiVertical, 1, nuiColor(175,175,175), eStrokeAndFillShape);
      pDeco->SetOffset1(0.f);
      pDeco->SetOffset2(0.5f);
    }
    pWindow->SetDecoration(pDeco);
  }
  else
  {
    pWindow->SetColor(eActiveWindowBg, nuiColor(255, 255, 255));
    pWindow->SetColor(eInactiveWindowBg, nuiColor(200, 200, 200));
  }

  // see nuiTheme::DrawActiveWindow for the rest
  
}



//**************************************************************************************************************
//
// nuiBackgroundPane
//
void nuiDefaultDecoration::BackgroundPane(nuiWidget* pWidget)
{
  nuiBackgroundPane* pPane = (nuiBackgroundPane*)pWidget;

  nglString decoName;
  const char* deco;
  uint32 decoSize;
  nuiRect rect;
  
  if (pPane->GetType() == eOutterBackground)
  {
    decoName = _T("nuiDefaultDecorationOutterPane");
    deco = gpPaneOutter;
    decoSize = gPaneOutterSize;
    rect = nuiRect(12,12,0,1);
  }
  else
  {
    decoName = _T("nuiDefaultDecorationInnerPane");
    deco = gpPaneInner;
    decoSize = gPaneInnerSize;
    rect = nuiRect(6,6,0,0);
  }
  
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(decoName);
  
  if (!pFrame)
  {
    nglIMemory* pIMem = new nglIMemory(deco, decoSize);
    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame(decoName, pTex, rect);
    pFrame->UseWidgetAlpha(true);
    delete pIMem; 
  }
  NGL_ASSERT(pFrame);
  pWidget->SetDecoration(pFrame, eDecorationBorder);
}




//**************************************************************************************************************
//
// nuiLabel
//
void nuiDefaultDecoration::Label(nuiWidget* pWidget)
{
//  nuiColor cText;
//  cText.SetValue(_T("nuiDefaultColorLabelText"));
//  pWidget->SetColor(eNormalTextFg, cText);
}


//**************************************************************************************************************
//
// nuiEditText
//
void nuiDefaultDecoration::EditText(nuiWidget* pWidget)
{
  
}


//**************************************************************************************************************
//
// nuiEditLine
//
void nuiDefaultDecoration::EditLine(nuiWidget* pWidget)
{
  nuiBackgroundPane* pPane = (nuiBackgroundPane*)pWidget;
  
  nglString decoName = _T("nuiDefaultDecorationInnerPane");
  const char* deco = gpPaneInner;
  uint32 decoSize = gPaneInnerSize;
  nuiRect rect = nuiRect(6,6,0,0);
  
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(decoName);
  
  if (!pFrame)
  {
    nglIMemory* pIMem = new nglIMemory(deco, decoSize);
    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame(decoName, pTex, rect);
    pFrame->UseWidgetAlpha(true);
    delete pIMem; 
  }
  NGL_ASSERT(pFrame);
  
  pWidget->SetDecoration(pFrame, eDecorationBorder);  
}


//**************************************************************************************************************
//
// nuiSplitter
//
void nuiDefaultDecoration::Splitter(nuiWidget* pWidget)
{
  nuiSplitter* pSplitter = (nuiSplitter*)pWidget;
  nuiSplitterHandle* pHandle = pSplitter->GetHandle();
  NGL_ASSERT(pHandle);
  
  if (pSplitter->GetOrientation() == nuiVertical)
  {
    pHandle->SetUserSize(7,0);
    
    nuiMetaDecoration* pDeco = (nuiMetaDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationVerticalSplitter"));
    if (!pDeco)
    {
      // handle bar 
      nglIMemory* pIMem = new nglIMemory(gpSplitterVertical, gSplitterVerticalSize);
      nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
      NGL_ASSERT(pTex);
      
      nuiFrame* pFrame = new nuiFrame(_T("nuiDefaultDecorationVerticalSplitterBar"), pTex, nuiRect(0,0,7,3));
      pFrame->SetInterpolated(false);
      
      // handle spot
      pIMem = new nglIMemory(gpSplitterVerticalHandle, gSplitterVerticalHandleSize);
      pTex = nuiTexture::GetTexture(pIMem);
      NGL_ASSERT(pTex);
      
      nuiImageDecoration* pDeco3 = new nuiImageDecoration(_T("nuiDefaultDecorationVerticalSplitterHandle"), 
                                                          pTex, nuiRect(0,0,7,6), nuiCenter);

      // build meta decoration
      pDeco = new nuiMetaDecoration(_T("nuiDefaultDecorationVerticalSplitter"));
      pDeco->AddDecoration(_T("nuiDefaultDecorationVerticalSplitterBar"));
      pDeco->AddDecoration(_T("nuiDefaultDecorationVerticalSplitterHandle"));

    }
    pHandle->SetDecoration(pDeco, eDecorationBorder);  
    
  }
  else
  {
    pHandle->SetUserSize(0,7);
    
    nuiMetaDecoration* pDeco = (nuiMetaDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationHorizontalSplitter"));
    if (!pDeco)
    {
      // handle bar 
      nglIMemory* pIMem = new nglIMemory(gpSplitterHorizontal, gSplitterHorizontalSize);
      nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
      NGL_ASSERT(pTex);
      
      nuiFrame* pFrame = new nuiFrame(_T("nuiDefaultDecorationHorizontalSplitterBar"), pTex, nuiRect(0,0,3,7));
      pFrame->SetInterpolated(false);
      
      // handle spot
      pIMem = new nglIMemory(gpSplitterHorizontalHandle, gSplitterHorizontalHandleSize);
      pTex = nuiTexture::GetTexture(pIMem);
      NGL_ASSERT(pTex);
      
      nuiImageDecoration* pDeco3 = new nuiImageDecoration(_T("nuiDefaultDecorationHorizontalSplitterHandle"), 
                                                          pTex, nuiRect(0,0,6,7), nuiCenter);
      
      // build meta decoration
      pDeco = new nuiMetaDecoration(_T("nuiDefaultDecorationHorizontalSplitter"));
      pDeco->AddDecoration(_T("nuiDefaultDecorationHorizontalSplitterBar"));
      pDeco->AddDecoration(_T("nuiDefaultDecorationHorizontalSplitterHandle"));
      
    }
   pHandle->SetDecoration(pDeco, eDecorationBorder);  
  }
  
  
}


//void nuiDefaultDecoration::Splitter(nuiWidget* pWidget)
//{
//  nuiSplitter* pSplitter = (nuiSplitter*)pWidget;
//  nuiSplitterHandle* pHandle = pSplitter->GetHandle();
//  NGL_ASSERT(pHandle);
//  
//  if (pSplitter->GetOrientation() == nuiVertical)
//  {
//    pHandle->SetUserSize(6,0);
//    
//    nuiMetaDecoration* pDeco = (nuiMetaDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationVerticalSplitter"));
//    if (!pDeco)
//    {
//      // handle bar gradient
//      nuiGradientDecoration* pDeco1 = new nuiGradientDecoration(_T("nuiDefaultDecorationVerticalSplitterGradient"), 
//                                                                nuiRect(0, 0, 0, 0), nuiColor(232,232,232), nuiColor(196,196,196), nuiHorizontal);
//      
//      // handle bar borders
//      nuiBorderDecoration* pDeco2 = new nuiBorderDecoration(_T("nuiDefaultDecorationVerticalSplitterBorders"));
//      pDeco2->SetBorderType(_T("All"));
//      pDeco2->SetStrokeSize(1);
//      pDeco2->SetStrokeLeftColor(nuiColor(190,190,190));
//      pDeco2->SetStrokeRightColor(nuiColor(170,170,170));
//      pDeco2->SetStrokeTopColor(nuiColor(180,180,180));
//      pDeco2->SetStrokeBottomColor(nuiColor(180,180,180));
//      
//      // handle control spot
//      nglIMemory* pIMem = new nglIMemory(gpSplitterHandleVertical, gSplitterHandleVerticalSize);
//      nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
//      NGL_ASSERT(pTex);
//      
//      nuiImageDecoration* pDeco3 = new nuiImageDecoration(_T("nuiDefaultDecorationVerticalSplitterHandle"), 
//                                                          pTex, nuiRect(0,0,5,5), nuiCenter);
//      
//      // build meta decoration
//      pDeco = new nuiMetaDecoration(_T("nuiDefaultDecorationVerticalSplitter"));
//      pDeco->AddDecoration(_T("nuiDefaultDecorationVerticalSplitterGradient"));
//      pDeco->AddDecoration(_T("nuiDefaultDecorationVerticalSplitterBorders"));
//      pDeco->AddDecoration(_T("nuiDefaultDecorationVerticalSplitterHandle"));
//      
//    }
//    pHandle->SetDecoration(pDeco, eDecorationBorder);  
//    
//  }
//  else
//  {
//    pHandle->SetUserSize(0,6);
//    
//    nuiMetaDecoration* pDeco = (nuiMetaDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationHorizontalSplitter"));
//    if (!pDeco)
//    {
//      nuiGradientDecoration* pDeco1 = new nuiGradientDecoration(_T("nuiDefaultDecorationHorizontalSplitterGradient"), 
//                                                                nuiRect(0, 0, 0, 0), nuiColor(232,232,232), nuiColor(196,196,196), nuiVertical);
//      nuiBorderDecoration* pDeco2 = new nuiBorderDecoration(_T("nuiDefaultDecorationHorizontalSplitterBorders"));
//      pDeco2->SetBorderType(_T("All"));
//      pDeco2->SetStrokeSize(1);
//      pDeco2->SetStrokeLeftColor(nuiColor(180,180,180));
//      pDeco2->SetStrokeRightColor(nuiColor(180,180,180));
//      pDeco2->SetStrokeTopColor(nuiColor(190,190,190));
//      pDeco2->SetStrokeBottomColor(nuiColor(170,170,170));
//      
//      // handle control spot
//      nglIMemory* pIMem = new nglIMemory(gpSplitterHandleHorizontal, gSplitterHandleHorizontalSize);
//      nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
//      NGL_ASSERT(pTex);
//      
//      nuiImageDecoration* pDeco3 = new nuiImageDecoration(_T("nuiDefaultDecorationHorizontalSplitterHandle"), 
//                                                          pTex, nuiRect(0,0,5,5), nuiCenter);
//      
//      
//      pDeco = new nuiMetaDecoration(_T("nuiDefaultDecorationHorizontalSplitter"));
//      pDeco->AddDecoration(_T("nuiDefaultDecorationHorizontalSplitterGradient"));
//      pDeco->AddDecoration(_T("nuiDefaultDecorationHorizontalSplitterBorders"));
//      pDeco->AddDecoration(_T("nuiDefaultDecorationHorizontalSplitterHandle"));
//      
//    }
//    pHandle->SetDecoration(pDeco, eDecorationBorder);  
//  }
//  
//  
//}


//**************************************************************************************************************
//
// nuiTitledPane
//
void nuiDefaultDecoration::TitledPane(nuiWidget* pWidget)
{
  
}


//**************************************************************************************************************
//
// nuiFolderPane
//
void nuiDefaultDecoration::FolderPane(nuiWidget* pWidget)
{
  
}


extern float NUI_SCALE_FACTOR;


//**************************************************************************************************************
//
// nuiButton
//
void nuiDefaultDecoration::Button(nuiWidget* pWidget)
{
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationButton"));
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nglIMemory* pIMemUp = new nglIMemory(gpButtonUp, gButtonUpSize);
  nuiTexture* pTexUp = nuiTexture::GetTexture(pIMemUp);
  NGL_ASSERT(pTexUp);
  nuiFrame* pFrameUp = new nuiFrame(_T("nuiDefaultDecorationButtonUp"), pTexUp, nuiRect(4,4,2,6));
  delete pIMemUp;

  
  nglIMemory* pIMemHover = new nglIMemory(gpButtonUp, gButtonUpSize);
  nuiTexture* pTexHover = nuiTexture::GetTexture(pIMemHover);
  NGL_ASSERT(pTexHover);
  nuiFrame* pFrameHover = new nuiFrame(_T("nuiDefaultDecorationButtonHover"), pTexHover, nuiRect(4,4,2,6));
  delete pIMemHover;
  
  
  nglIMemory* pIMemDown = new nglIMemory(gpButtonDown, gButtonDownSize);
  nuiTexture* pTexDown = nuiTexture::GetTexture(pIMemDown);
  NGL_ASSERT(pTexDown);
  nuiFrame* pFrameDown = new nuiFrame(_T("nuiDefaultDecorationButtonDown"), pTexDown, nuiRect(4,4,2,6));
  delete pIMemDown;
  
  
  
  nuiStateDecoration* pState = new nuiStateDecoration(_T("nuiDefaultDecorationButton"), 
                                                      _T("nuiDefaultDecorationButtonUp"),
                                                      _T("nuiDefaultDecorationButtonDown"),
                                                      _T("nuiDefaultDecorationButtonHover"));
  pState->SetSourceClientRect(nuiRect(4,4,2,6));

  pWidget->SetDecoration(pState, eDecorationBorder);

}





//**************************************************************************************************************
//
// nuiRadioButton
//
void nuiDefaultDecoration::RadioButton(nuiWidget* pWidget)
{
  nuiRadioButton* pBtn = (nuiRadioButton*)pWidget;
  if (pBtn->HasContents())
  {
    nuiDefaultDecoration::Button(pWidget);
    return;
  }
  
  
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationRadioButton"));
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nglIMemory* pIMem = new nglIMemory(gpRadioButtonUp, gRadioButtonUpSize);
  nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame(_T("nuiDefaultDecorationRadioButtonUp"), pTex, nuiRect(0,0,13,13));
  delete pIMem;
  
  
  pIMem = new nglIMemory(gpRadioButtonDown, gRadioButtonDownSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationRadioButtonDown"), pTex, nuiRect(0,0,13,13));
  delete pIMem;

  pIMem = new nglIMemory(gpRadioButtonUpDisabled, gRadioButtonUpDisabledSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationRadioButtonUpDisabled"), pTex, nuiRect(0,0,13,13));
  delete pIMem;

  pIMem = new nglIMemory(gpRadioButtonDownDisabled, gRadioButtonDownDisabledSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationRadioButtonDownDisabled"), pTex, nuiRect(0,0,13,13));
  delete pIMem;
  
  
  nuiStateDecoration* pState = new nuiStateDecoration(_T("nuiDefaultDecorationRadioButton"), 
                                                      _T("nuiDefaultDecorationRadioButtonUp"),
                                                      _T("nuiDefaultDecorationRadioButtonDown"),
                                                      _T("nuiDefaultDecorationRadioButtonUp"),/* hover up */
                                                      _T("nuiDefaultDecorationRadioButtonDown"), /* hover down*/
                                                      _T("nuiDefaultDecorationRadioButtonUpDisabled"),
                                                      _T("nuiDefaultDecorationRadioButtonDownDisabled"));
  pState->SetSourceClientRect(nuiRect(0,0,13,13));
  
  pWidget->SetDecoration(pState, eDecorationBorder);
  
}







//**************************************************************************************************************
//
// nuiCloseButton (for windows)
//
void nuiDefaultDecoration::CloseButton(nuiWidget* pWidget)
{
  
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationCloseButton"));
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nglIMemory* pIMemUp = new nglIMemory(gpCloseButtonUp, gCloseButtonUpSize);
  nuiTexture* pTexUp = nuiTexture::GetTexture(pIMemUp);
  NGL_ASSERT(pTexUp);
  nuiFrame* pFrameUp = new nuiFrame(_T("nuiDefaultDecorationCloseButtonUp"), pTexUp, nuiRect(0,0,12,15));
  delete pIMemUp;
  
  
  nglIMemory* pIMemHover = new nglIMemory(gpCloseButtonHover, gCloseButtonHoverSize);
  nuiTexture* pTexHover = nuiTexture::GetTexture(pIMemHover);
  NGL_ASSERT(pTexHover);
  nuiFrame* pFrameHover = new nuiFrame(_T("nuiDefaultDecorationCloseButtonHover"), pTexHover, nuiRect(0,0,12,15));
  delete pIMemHover;
  
  
  nglIMemory* pIMemDown = new nglIMemory(gpCloseButtonDown, gCloseButtonDownSize);
  nuiTexture* pTexDown = nuiTexture::GetTexture(pIMemDown);
  NGL_ASSERT(pTexDown);
  nuiFrame* pFrameDown = new nuiFrame(_T("nuiDefaultDecorationCloseButtonDown"), pTexDown, nuiRect(0,0,12,15));
  delete pIMemDown;
  
  
  
  nuiStateDecoration* pState = new nuiStateDecoration(_T("nuiDefaultDecorationCloseButton"), 
                                                      _T("nuiDefaultDecorationCloseButtonUp"),
                                                      _T("nuiDefaultDecorationCloseButtonDown"),
                                                      _T("nuiDefaultDecorationCloseButtonHover"));
  pState->SetSourceClientRect(nuiRect(0,0,12,15));
  
  pWidget->SetDecoration(pState, eDecorationBorder);
  
}




//**************************************************************************************************************
//
// nuiToggleButton
//
void nuiDefaultDecoration::ToggleButton(nuiWidget* pWidget)
{
  nuiToggleButton* pBtn = (nuiToggleButton*)pWidget;
  if (!pBtn->GetDisplayAsCheckBox() && !pBtn->GetDisplayAsFrameBox())
  {
    nuiDefaultDecoration::Button(pWidget);
    return;
  }
  
  if (pBtn->GetDisplayAsCheckBox())
  {
    nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationToggleButtonCheckBox"));
    if (pDeco)
    {
      pWidget->SetDecoration(pDeco, eDecorationBorder);
      return;
    }
    
    nglIMemory* pIMem = new nglIMemory(gpToggleButtonUp, gToggleButtonUpSize);
    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    nuiImageDecoration* pFrame = new nuiImageDecoration(_T("nuiDefaultDecorationToggleButtonCheckBoxUp"), pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
    delete pIMem;
    
    
    pIMem = new nglIMemory(gpToggleButtonDown, gToggleButtonDownSize);
    pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiImageDecoration(_T("nuiDefaultDecorationToggleButtonCheckBoxDown"), pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
    delete pIMem;
    
    pIMem = new nglIMemory(gpToggleButtonUpDisabled, gToggleButtonUpDisabledSize);
    pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiImageDecoration(_T("nuiDefaultDecorationToggleButtonCheckBoxUpDisabled"), pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
    delete pIMem;
    
    pIMem = new nglIMemory(gpToggleButtonDownDisabled, gToggleButtonDownDisabledSize);
    pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiImageDecoration(_T("nuiDefaultDecorationToggleButtonCheckBoxDownDisabled"), pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
    delete pIMem;
    
    
    nuiStateDecoration* pState = new nuiStateDecoration(_T("nuiDefaultDecorationToggleButtonCheckBox"), 
                                                        _T("nuiDefaultDecorationToggleButtonCheckBoxUp"),
                                                        _T("nuiDefaultDecorationToggleButtonCheckBoxDown"),
                                                        _T("nuiDefaultDecorationToggleButtonCheckBoxUp"),/* hover up */
                                                        _T("nuiDefaultDecorationToggleButtonCheckBoxDown"), /* hover down*/
                                                        _T("nuiDefaultDecorationToggleButtonCheckBoxUpDisabled"),
                                                        _T("nuiDefaultDecorationToggleButtonCheckBoxDownDisabled"));
    pState->SetSourceClientRect(nuiRect(0,0,13,13));
    pWidget->SetDecoration(pState, eDecorationBorder);
  }
  
}



//**************************************************************************************************************
//
// nuiComboBox
//
void nuiDefaultDecoration::ComboBox(nuiWidget* pWidget)
{
 
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationComboBox"));
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nglIMemory* pIMem = new nglIMemory(gpComboUp, gComboUpSize);
  nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame(_T("nuiDefaultDecorationComboBoxUp"), pTex, nuiRect(12,4,2,10));
  pFrame->EnableBorder(false);
  delete pIMem;
    
  pIMem = new nglIMemory(gpComboDown, gComboDownSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(_T("nuiDefaultDecorationComboBoxDown"), pTex, nuiRect(12,4,2,10));
  pFrame->EnableBorder(false);
  delete pIMem;
  
  
  
  nuiStateDecoration* pState = new nuiStateDecoration(_T("nuiDefaultDecorationComboBox"));
  pState->SetState(nuiStateEnabled  | nuiStateReleased, nuiDecoration::Get(_T("nuiDefaultDecorationComboBoxUp")));
  pState->SetState(nuiStateEnabled  | nuiStateSelected, nuiDecoration::Get(_T("nuiDefaultDecorationComboBoxDown")));
  pWidget->SetDecoration(pState, eDecorationBorder);
  pWidget->SetUserHeight(18);
  
  pWidget->SetColor(eSelectedTextFg, nuiColor(32,32,32));
  
}



//**************************************************************************************************************
//
// nuiKnob
//

nuiImageSequence* nuiDefaultDecoration::mpKnobSequence;

void nuiDefaultDecoration::KnobSequence(nuiWidget* pWidget)
{
  nuiKnob* pKnob = (nuiKnob*)pWidget;

  if (!pKnob->GetImageSequence() && !mpKnobSequence)
  {
    nglIMemory* pIMem = new nglIMemory(gpKnobSequenceHdl, gKnobSequenceHdlSize);
    nglImage* pImage = new nglImage(pIMem);
    NGL_ASSERT(pImage);
    mpKnobSequence = new nuiImageSequence(31, pImage, nuiVertical);
    mpKnobSequence->Acquire();
    delete pIMem;     
    delete pImage;
  }

  if (!pKnob->GetImageSequence())
    pKnob->SetImageSequence(mpKnobSequence);
}




//***********************************************************************************************************************************
//***********************************************************************************************************************************
//
// FileSelector
//
//***********************************************************************************************************************************
//***********************************************************************************************************************************

void nuiDefaultDecoration::FileSelector_FolderView(nuiWidget* pWidget)
{
  nuiColorDecoration* pDeco = (nuiColorDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationFileSelector_FolderView"));
  if (!pDeco)
  {
    pDeco = new nuiColorDecoration(_T("nuiDefaultDecorationFileSelector_FolderView"), nuiRect(5,5,0,0), nuiColor(214,221,229), 1, nuiColor(139,139,139), eStrokeAndFillShape);
  }
  pWidget->SetDecoration(pDeco, eDecorationBorder);
}

void nuiDefaultDecoration::FileSelector_InfoView(nuiWidget* pWidget)
{

}


void nuiDefaultDecoration::FileSelector_FolderLabel(nuiWidget* pWidget)
{
  
}


void nuiDefaultDecoration::FileSelector_VolumeIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconVolume")));
  NGL_ASSERT(pTex);

  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
  
}


void nuiDefaultDecoration::FileSelector_FolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconFolder")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}



void nuiDefaultDecoration::FileSelector_TreeView(nuiWidget* pWidget)
{
//  nuiGradientDecoration* pDeco = (nuiGradientDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationFileSelectorWindow"));
//  if (!pDeco)
//  {
////  nuiGradientDecoration* pDeco;
//    nuiColor color1, color2;
//    nuiColor::GetColor(_T("nuiDefaultClrWindowBkg1"), color1);
//    nuiColor::GetColor(_T("nuiDefaultClrWindowBkg2"), color2);
//    
//    pDeco = new nuiGradientDecoration(_T("nuiDefaultDecorationFileSelectorWindow"), 
//                                      nuiRect(0,0, 0,0), color1, color2, nuiVertical, 1, nuiColor(175,175,175), eStrokeAndFillShape);
//    pDeco->SetOffset1(0.f);
//    pDeco->SetOffset2(0.5f);
//  }
//  pWidget->SetDecoration(pDeco, eDecorationBorder);  

  nuiColorDecoration* pDeco = (nuiColorDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationFileSelectorWindow"));
  if (!pDeco)
  {
    pDeco = new nuiColorDecoration(_T("nuiDefaultDecorationFileSelectorWindow"), nuiColor(250,250,250), 1, nuiColor(175,175,175), eStrokeAndFillShape);
  }
  pWidget->SetDecoration(pDeco, eDecorationBorder);  
  
}


void nuiDefaultDecoration::FileSelector_TreeFolderLabel(nuiWidget* pWidget)
{
  
}


void nuiDefaultDecoration::FileSelector_TreeFileLabel(nuiWidget* pWidget)
{
  
}


void nuiDefaultDecoration::FileSelector_TreeFolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconFolder")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileSelector_TreeFileIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconFile")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}
















//***********************************************************************************************************************************
//***********************************************************************************************************************************
//
// FileTree
//
//***********************************************************************************************************************************
//***********************************************************************************************************************************

void nuiDefaultDecoration::FileTree_View(nuiWidget* pWidget)
{
  nuiGradientDecoration* pDeco = (nuiGradientDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationWindow"));
  if (!pDeco)
  {
    nuiColor color1, color2;
    nuiColor::GetColor(_T("nuiDefaultClrWindowBkg1"), color1);
    nuiColor::GetColor(_T("nuiDefaultClrWindowBkg2"), color2);
    
    pDeco = new nuiGradientDecoration(_T("nuiDefaultDecorationWindow"), 
                                      nuiRect(0,0, 0,0), color1, color2, nuiVertical, 1, nuiColor(175,175,175), eStrokeAndFillShape);
    pDeco->SetOffset1(0.f);
    pDeco->SetOffset2(0.5f);
  }
  pWidget->SetDecoration(pDeco, eDecorationBorder);  
}


//void nuiDefaultDecoration::FileTree_Node(nuiWidget* pWidget)
//{
//  nuiBorderDecoration* pDeco = (nuiBorderDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationFileTreeNode"));
//  if (!pDeco)
//  {
//    nuiColor color;
//    nuiColor::GetColor(_T("nuiDefaultClrBorder"), color);
//    
//    pDeco = new nuiBorderDecoration(_T("nuiDefaultDecorationFileTreeNode"));
//    pDeco->SetSourceClientRect(nuiRect(0,0,0,0));
//    pDeco->SetStrokeColor(color);
//    pDeco->SetStrokeSize(1);
//    pDeco->SetBorderType(_T("Bottom"));
//  }
//  
//  pWidget->SetDecoration(pDeco, eDecorationBorder);  
//}
//

void nuiDefaultDecoration::FileTree_ColumnLabel(nuiWidget* pWidget)
{  
  pWidget->SetPosition(nuiFillHorizontal);
  
  nuiColorDecoration* pDeco = (nuiColorDecoration*)nuiDecoration::Get(_T("nuiDefaultDecorationFileTreeColumnLabel"));
  if (!pDeco)
  {
    nuiColor fillColor;
    nuiColor::GetColor(_T("nuiDefaultClrPaneBkg"), fillColor);
    pDeco = new nuiColorDecoration(_T("nuiDefaultDecorationFileTreeColumnLabel"), nuiRect(3,3,0,0), fillColor);
  }
  
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(10), true);
  
  pWidget->SetDecoration(pDeco, eDecorationBorder);
}


void nuiDefaultDecoration::FileTree_FolderLabel(nuiWidget* pWidget)
{  
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(10), true);
}

void nuiDefaultDecoration::FileTree_FileLabel(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(10), true);  
}

void nuiDefaultDecoration::FileTree_FileInfo(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(8), true);
}


void nuiDefaultDecoration::FileTree_VolumeIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconVolume")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileTree_FolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconFolder")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileTree_OpenFolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconOpenFolder")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileTree_ParentFolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconParentFolder")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}



void nuiDefaultDecoration::FileTree_FileIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString(_T("nuiDefaultDecorationIconFile")));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}





//***********************************************************************************************************************************
//***********************************************************************************************************************************
//
// special cases. Those are not called through the automatic default decoration system.
// they are called from their own source code.
//
//***********************************************************************************************************************************
//***********************************************************************************************************************************


//**************************************************************************************************************
//
// nuiMainWindow
//
void nuiDefaultDecoration::MainWindow(nuiMainWindow* pWindow)
{
  nuiWidget* pCont = new nuiWidget();
  pWindow->AddChild(pCont);
  nuiColor color1, color2;
  nuiColor::GetColor(_T("nuiDefaultClrWindowBkg1"), color1);
  nuiColor::GetColor(_T("nuiDefaultClrWindowBkg2"), color2);
  nuiGradientDecoration* pDeco = new nuiGradientDecoration(_T("nuiDefaultDecorationMainWindow"), 
                                                           nuiRect(0,0, 0,0), color1, color2, nuiVertical, 0, nuiColor(0,0,0), eFillShape);
  pDeco->SetOffset1(0.f);
  pDeco->SetOffset2(0.5f);                                                           
//  pWindow->SetDecoration(pDeco);  
}


//**************************************************************************************************************
//
// nuiDialog
//
void nuiDefaultDecoration::Dialog(nuiSimpleContainer* pCont)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(_T("nuiDefaultDecorationOutterPane"));
  if (!pFrame)
  {
    nglIMemory* pIMem = new nglIMemory(gpPaneOutter, gPaneOutterSize);
    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame(_T("nuiDefaultDecorationOutterPane"), pTex, nuiRect(12,12,0,1));
    pFrame->UseWidgetAlpha(true);
    delete pIMem; 
  }
  NGL_ASSERT(pFrame);
  pCont->SetDecoration(pFrame);  
}


//**************************************************************************************************************
//
// nuiDialog
//
//void nuiDefaultDecoration::DialogSelectFile(nuiSimpleContainer* pCont)
//{
//  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(_T("nuiDefaultDecorationOutterPane"));
//  if (!pFrame)
//  {
//    nglIMemory* pIMem = new nglIMemory(gpPaneOutter, gPaneOutterSize);
//    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
//    NGL_ASSERT(pTex);
//    pFrame = new nuiFrame(_T("nuiDefaultDecorationOutterPane"), pTex, nuiRect(12,12,0,1));
//    pFrame->UseWidgetAlpha(true);
//    pFrame->EnableBorder(false);
//    delete pIMem; 
//  }
//  NGL_ASSERT(pFrame);
//  pCont->SetDecoration(pFrame, eDecorationBorder);  
//}


//**************************************************************************************************************
//
// nuiDialog::Title
//
void nuiDefaultDecoration::Dialog_Title(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  
  pLabel->SetFont(nuiFont::GetFont(13), true);
  pLabel->SetBorder(0,0,10,15);
}

//**************************************************************************************************************
//
// DialogSelectFile::Title
//
//void nuiDefaultDecoration::DialogSelectFile_Title(nuiWidget* pWidget)
//{
//  nuiLabel* pLabel = (nuiLabel*)pWidget;
//  
//  pLabel->SetFont(nuiFont::GetFont(13), true);
//  pLabel->SetBorder(16,0,16,15);
//}
//





//**************************************************************************************************************
//
// nuiDialog::EditLine
//
void nuiDefaultDecoration::Dialog_EditLine(nuiWidget* pWidget)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(_T("nuiDefaultDecorationInnerPane"));
  if (!pFrame)
  {
    nglIMemory* pIMem = new nglIMemory(gpPaneInner, gPaneInnerSize);
    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame(_T("nuiDefaultDecorationInnerPane"), pTex, nuiRect(6,6,0,0));
    pFrame->UseWidgetAlpha(true);
    delete pIMem; 
  }
  NGL_ASSERT(pFrame);
  pWidget->SetDecoration(pFrame, eDecorationBorder);    
}



//**************************************************************************************************************
//
// nuiMessageBox
//
void nuiDefaultDecoration::MessageBox(nuiMessageBox* pBox)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(_T("nuiDefaultDecorationOutterPane"));
  if (!pFrame)
  {
    nglIMemory* pIMem = new nglIMemory(gpPaneOutter, gPaneOutterSize);
    nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame(_T("nuiDefaultDecorationOutterPane"), pTex, nuiRect(12,12,0,1));
    pFrame->UseWidgetAlpha(true);
    delete pIMem; 
  }
  NGL_ASSERT(pFrame);
  pBox->SetDecoration(pFrame);
}



//**************************************************************************************************************
//
// nuiMessageBox::Title
//
void nuiDefaultDecoration::MessageBox_Title(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  
  pLabel->SetFont(nuiFont::GetFont(14), true);
  pLabel->SetBorder(0,0,0,15);
}



//**************************************************************************************************************
//
// nuiTabView::Tab
//
void nuiDefaultDecoration::TabView_Tab(nuiTabView* pView, nuiWidget* pTab)
{
  nglString decoName = _T("nuiDefaultDecorationTabTop");
  nglString decoUpName;
  nglString decoDownName;
  
  nuiRect frameRect;

  const char* decoUp = gpTabTopUp;
  const char* decoDown = gpTabTopDown;
  long decoUpSize = gTabTopUpSize;
  long decoDownSize = gTabTopDownSize;
  
  switch (pView->GetTabPosition())
  {
    case nuiTop:
      decoName = _T("nuiDefaultDecorationTabTop");
      decoUp = gpTabTopUp;
      decoDown = gpTabTopDown;
      decoUpSize = gTabTopUpSize;
      decoDownSize = gTabTopDownSize;
      frameRect = nuiRect(4,5,2,4);
      break;
      
    case nuiLeft:
      decoName = _T("nuiDefaultDecorationTabLeft");
      decoUp = gpTabLeftUp;
      decoDown = gpTabLeftDown;
      decoUpSize = gTabLeftUpSize;
      decoDownSize = gTabLeftDownSize;
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiRight:
      decoName = _T("nuiDefaultDecorationTabRight");
      decoUp = gpTabRightUp;
      decoDown = gpTabRightDown;
      decoUpSize = gTabRightUpSize;
      decoDownSize = gTabRightDownSize;
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiBottom:
      decoName = _T("nuiDefaultDecorationTabBottom");
      decoUp = gpTabBottomUp;
      decoDown = gpTabBottomDown;
      decoUpSize = gTabBottomUpSize;
      decoDownSize = gTabBottomDownSize;
      frameRect = nuiRect(4,5,2,4);
      break;
      
    default:
      NGL_OUT(_T("nui3 error : the nuiTabView object doesn't have a nuiPosition valid parameter!\n"));
      NGL_ASSERT(0);
      break;
  }
  
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(decoName);
  if (pDeco)
  {
    pTab->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  decoUpName = decoName + _T("Up");
  decoDownName = decoName + _T("Down");
  
  nglIMemory* pIMem = new nglIMemory(decoUp, decoUpSize);
  nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame(decoUpName, pTex, frameRect);
  delete pIMem;
  
  pIMem = new nglIMemory(decoDown, decoDownSize);
  pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(decoDownName, pTex, frameRect);
  delete pIMem;
  
  
  
  nuiStateDecoration* pState = new nuiStateDecoration(decoName);
  pState->SetState(nuiStateEnabled  | nuiStateReleased, nuiDecoration::Get(decoUpName));
  pState->SetState(nuiStateEnabled  | nuiStateSelected, nuiDecoration::Get(decoDownName));
  
  pState->SetSourceClientRect(frameRect);
  
  pTab->SetDecoration(pState, eDecorationBorder);
  
}





//**************************************************************************************************************
//
// nuiTabView::Contents
//

void nuiDefaultDecoration::TabView_Contents(nuiTabView* pView, nuiWidget* pContents)
{
  nglString decoName = _T("nuiDefaultDecorationTabTopContents");
  
  nuiRect frameRect;
  
  const char* deco = gpTabTopContents;
  long decoSize = gTabTopContentsSize;
  
  switch (pView->GetTabPosition())
  {
    case nuiTop:
      decoName = _T("nuiDefaultDecorationTabTopContents");
      deco = gpTabTopContents;
      decoSize = gTabTopContentsSize;
      frameRect = nuiRect(4,5,2,4);
      break;
      
    case nuiLeft:
      decoName = _T("nuiDefaultDecorationTabLeftContents");
      deco = gpTabLeftContents;
      decoSize = gTabLeftContentsSize;
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiRight:
      decoName = _T("nuiDefaultDecorationTabRightContents");
      deco = gpTabRightContents;
      decoSize = gTabRightContentsSize;
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiBottom:
      decoName = _T("nuiDefaultDecorationTabBottomContents");
      deco = gpTabBottomContents;
      decoSize = gTabBottomContentsSize;
      frameRect = nuiRect(4,5,2,4);
      break;
  }
  
  nuiFrame* pDeco = (nuiFrame*)nuiDecoration::Get(decoName);
  if (pDeco)
  {
    pContents->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nglIMemory* pIMem = new nglIMemory(deco, decoSize);
  nuiTexture* pTex = nuiTexture::GetTexture(pIMem);
  NGL_ASSERT(pTex);
  pDeco = new nuiFrame(decoName, pTex, frameRect);
  delete pIMem;
  
  pContents->SetDecoration(pDeco, eDecorationBorder);
  
}




//**************************************************************************************************************
//
// NavigationViews
//

//static 
void nuiDefaultDecoration::NavigationBar(nuiWidget* pWidget)
{
  nuiNavigationBar* pBar = (nuiNavigationBar*)pWidget;
  if (!pBar->IsVisible())
    return;
  
  pBar->SetPosition(nuiTopLeft);

  // TODO : plateform? orientation?
  pBar->SetUserWidth(320.f);
  pBar->SetUserHeight(44.f);
  
  
  nuiMetaDecoration* pMeta = (nuiMetaDecoration*)nuiDecoration::Get(_T("nuiDefaultNavigationBarDecoration"));
  if (pMeta)
  {
    pBar->SetDecoration(pMeta);
    return;
  }
  
  pMeta = new nuiMetaDecoration(_T("nuiDefaultNavigationBarDecoration"));
  
  nuiGradientDecoration* pGradient = new nuiGradientDecoration(_T("nuiDefaultNavigationBarDecoration_Gradient"));
  pGradient->SetShapeMode(eFillShape);
  
  // TODO : plateform? 
  if (pBar->GetBarStyle() == eBarStyleDefault)
  {
    pGradient->SetColors(nuiColor(145,165,191), nuiColor(109,131,161));

  }
  else if (pBar->GetBarStyle() == eBarStyleBlack)
  {
    pGradient->SetColors(nuiColor(0,0,0), nuiColor(0,0,0));    
  }
//  else
//  {
//    nuiColor tint = pBar->GetTintColor();
//    nuiColor tint2 = nuiColor(tint.Red()+(18.f/255.f), tint.Green()+(17.f/255.f), tint.Blue()+(15.f/255.f), tint.Alpha());
//    pGradient->SetColors(tint2, tint);  
//  }

  
  pMeta->AddDecoration(pGradient);
  
  nuiBorderDecoration* pBorder = new nuiBorderDecoration(_T("nuiDefaultNavigationBarDecoration_BorderTop"));
  pBorder->SetBorderType(_T("Top"));
  pBorder->SetStrokeTopColor(nuiColor(255,255,255,140));
  pMeta->AddDecoration(pBorder);
  
  pBorder = new nuiBorderDecoration(_T("nuiDefaultNavigationBarDecoration_BorderBottom"));
  pBorder->SetBorderType(_T("Bottom"));
  pBorder->SetStrokeBottomColor(nuiColor(0,0,0,164));
  pMeta->AddDecoration(pBorder);
  
  pBar->SetDecoration(pMeta);  
}


//static 
void nuiDefaultDecoration::NavigationButton(nuiNavigationButton* pWidget, nuiNavigationBarStyle style, bool leftyButton)
{
  nglString decoName;
  nglString barStyle = NavigationBarStyleToString(style);
  
  decoName.Format(_T("nuiDefaultNavigation%lsButtonDecoration"), barStyle.GetChars());
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(decoName);
  
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco);
    return;
  }
  
  // choose the right map, depending on the style of the navigation bar, and on the button kind
  const void* pButtonUp = NULL;
  const void* pButtonDown = NULL;
  long buttonUpSize = 0;
  long buttonDownSize = 0;
  if (leftyButton)
    switch (style)
    {
      case eBarStyleDefault: pButtonUp = gpNavigationBarDefaultLeftButtonUp; buttonUpSize = gNavigationBarDefaultLeftButtonUpSize; pButtonDown = gpNavigationBarDefaultLeftButtonDown; buttonDownSize = gNavigationBarDefaultLeftButtonDownSize; break;
      case eBarStyleBlack: pButtonUp = gpNavigationBarBlackLeftButtonUp; buttonUpSize = gNavigationBarBlackLeftButtonUpSize; pButtonDown = gpNavigationBarBlackLeftButtonDown; buttonDownSize = gNavigationBarBlackLeftButtonDownSize; break;
      //case eBarStyleTint: return _T("Tint");
      case eBarStyleNone: default: break;
    }
  else
    switch (style)
    {
      case eBarStyleDefault: pButtonUp = gpNavigationBarDefaultButtonUp; buttonUpSize = gNavigationBarDefaultButtonUpSize; pButtonDown = gpNavigationBarDefaultButtonDown; buttonDownSize = gNavigationBarDefaultButtonDownSize; break;
      case eBarStyleBlack: pButtonUp = gpNavigationBarBlackButtonUp; buttonUpSize = gNavigationBarBlackButtonUpSize; pButtonDown = gpNavigationBarBlackButtonDown; buttonDownSize = gNavigationBarBlackButtonDownSize; break;
        //case eBarStyleTint: return _T("Tint");
      case eBarStyleNone: default: break;
    }


  // assign the texture for the "Up" state
  nglString decoUpName;
  if (leftyButton)
    decoUpName.Format(_T("nuiDefaultNavigation%lsLeftButtonUp"), barStyle.GetChars());
  else
    decoUpName.Format(_T("nuiDefaultNavigation%lsButtonUp"), barStyle.GetChars());

  nglIMemory* pIMemUp = new nglIMemory(pButtonUp, buttonUpSize);
  nuiTexture* pTexUp = nuiTexture::GetTexture(pIMemUp);
  NGL_ASSERT(pTexUp);
  nuiFrame* pFrameUp = new nuiFrame(decoUpName, pTexUp, nuiRect(13,0,1,30));
  delete pIMemUp;
  

  // assign the texture for the "Down" state
  nglString decoDownName;
  if (leftyButton)
    decoDownName.Format(_T("nuiDefaultNavigation%lsLeftButtonDown"), barStyle.GetChars());
  else
    decoDownName.Format(_T("nuiDefaultNavigation%lsButtonDown"), barStyle.GetChars());
  
  nglIMemory* pIMemDown = new nglIMemory(pButtonDown, buttonDownSize);
  nuiTexture* pTexDown = nuiTexture::GetTexture(pIMemDown);
  NGL_ASSERT(pTexDown);
  nuiFrame* pFrameDown = new nuiFrame(decoDownName, pTexDown, nuiRect(13,0,1,30));
  delete pIMemDown;
  
  
  // create "state" decoration for 2-states button
  nuiStateDecoration* pState = new nuiStateDecoration(decoName, decoUpName, decoDownName);
  pState->SetSourceClientRect(nuiRect(13,0,1,30));
  
  // set decoration
  pWidget->SetDecoration(pState, eDecorationBorder);
  
  // set style
  pWidget->SetPosition(nuiLeft);
  pWidget->SetBorder(20, 0, 7, 0);
  
}




//static
nglString nuiDefaultDecoration::NavigationBarStyleToString(nuiNavigationBarStyle barStyle)
{
  switch (barStyle)
  {
    case eBarStyleDefault: return _T("Default");
    case eBarStyleBlack: return _T("Black");
      //case eBarStyleTint: return _T("Tint");
    case eBarStyleNone: return _T("None");
  }
  return _T("UnknownStyle");
}





