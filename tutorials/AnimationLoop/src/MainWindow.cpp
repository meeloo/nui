/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "MainWindow.h"
#include "Application.h"
#include "nuiCSS.h"
#include "nuiVBox.h"
#include "nuiComboBox.h"
#include "nuiAttributeAnimation.h"

#include "nuiAudioDevice.h"

/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
: nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), 
  mEventSink(this)
{
#ifdef _DEBUG_
  SetDebugMode(true);
#endif
  
#ifdef NUI_IPHONE
  LoadCSS("rsrc:/css/style-iPhone.css");
#else
  LoadCSS("rsrc:/css/style.css");
#endif

}

MainWindow::~MainWindow()
{
}

void MainWindow::OnCreation()
{
  mProgress = 0;
  
  AddAttribute(new nuiAttribute<float>
               (nglString("AnimProgress"), nuiUnitNone,
                nuiMakeDelegate(this, &MainWindow::GetProgress), 
                nuiMakeDelegate(this, &MainWindow::SetProgress)));
  
  
  nuiWidget* pContainer = (nuiWidget*)nuiBuilder::Get().CreateWidget("MainContainer");
  AddChild(pContainer);
  
  mpCountSlider = (nuiSlider*)pContainer->GetChild("countSlider");
  nuiRange& rRange = mpCountSlider->GetRange();
  rRange.SetValue(1);
  rRange.SetRange(1, 10);
  rRange.SetIncrement(1);
  rRange.SetPageIncrement(2);
  mpCountValLabel = (nuiLabel*)pContainer->GetChild("countValueLabel");
  mEventSink.Connect(mpCountSlider->InteractiveValueChanged, &MainWindow::OnCountSliderChanged);
  OnCountSliderChanged(NULL);
  
  
  mpLoopCombo = (nuiComboBox*)pContainer->GetChild("loopCombo");
  
  nuiTreeNode* pTree = new nuiTreeNode("loop");
  nuiTreeNode* pNode = new nuiTreeNode("forward");
  pTree->AddChild(pNode);
  pNode = new nuiTreeNode("reverse");
  pTree->AddChild(pNode);
  pNode = new nuiTreeNode("ping pong");
  pTree->AddChild(pNode);
  
  mpLoopCombo->SetTree(pTree);
  mpLoopCombo->SetSelected((uint32)0);
  
  mpGoButton = (nuiButton*)pContainer->GetChild("goButton");
  mEventSink.Connect(mpGoButton->Activated, &MainWindow::OnGoButtonClick);
  
  mEventSink.Connect(nuiAnimation::GetTimer()->Tick, &MainWindow::OnTimerTick);
}



void MainWindow::OnCountSliderChanged(const nuiEvent& rEvent)
{
  uint32 count = mpCountSlider->GetRange().GetValue();
  nglString str;
  str.Add(count);
  mpCountValLabel->SetText(str);
}



void MainWindow::OnGoButtonClick(const nuiEvent& rEvent)
{
  uint32 count = mpCountSlider->GetRange().GetValue();
  
  nuiAnimLoop loop = eAnimLoopForward;
  uint32 selected = mpLoopCombo->GetValue();
  
  if (selected == 0)
    loop = eAnimLoopForward;
  else if (selected == 1)
    loop = eAnimLoopReverse;
  else if (selected == 2)
    loop = eAnimLoopPingPong;
  
  
  nuiAttributeAnimation* pAnim = new nuiAttributeAnimation();
  pAnim->SetTargetObject(this);
  pAnim->SetTargetAttribute("AnimProgress");
  pAnim->SetStartValue(0);
  pAnim->SetEndValue(1);
  pAnim->SetEasing(nuiEasingCubic);
  pAnim->SetDuration(1);
  pAnim->SetDeleteOnStop(true);
  mEventSink.Connect(pAnim->AnimStop, &MainWindow::OnAnimStop);
  
  pAnim->Play(count, loop);
}

void MainWindow::OnAnimStop(const nuiEvent& rEvent)
{
  mProgress = 0;
}

void MainWindow::OnTimerTick(const nuiEvent& rEvent)
{
  nuiSize w = 80;
  nuiSize h = 60;
  w += 3 * w * mProgress;
  h += 3 * h * mProgress;
  mpGoButton->SetUserSize(w, h);
}

float MainWindow::GetProgress()
{
  return mProgress;
}

void MainWindow::SetProgress(float progress)
{
  mProgress = progress;
}


void MainWindow::OnClose()
{
  if (GetNGLWindow()->IsInModalState())
    return;
  
  
  App->Quit();
}


bool MainWindow::LoadCSS(const nglPath& rPath)
{
  nglIStream* pF = rPath.OpenRead();
  if (!pF)
  {
    NGL_OUT("Unable to open CSS source file '%ls'\n", rPath.GetChars());
    return false;
  }
  
  nuiCSS* pCSS = new nuiCSS();
  bool res = pCSS->Load(*pF, rPath);
  delete pF;
  
  if (res)
  {
    nuiMainWindow::SetCSS(pCSS);
    return true;
  }
  
  NGL_OUT("%ls\n", pCSS->GetErrorString().GetChars());
  
  delete pCSS;
  return false;
}
