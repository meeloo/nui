/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "MainWindow.h"
#include "Application.h"
#include "nuiLabel.h"
#include "nuiRadioButtonGroup.h"
#include "nuiHBox.h"
#include "nuiVBox.h"
#include "nuiPane.h"

#include "nuiColorDecoration.h"
#include "nuiGradientDecoration.h"
#include "nuiFrame.h"
#include "nuiCSS.h"

////////////////////////////////////////////////////////////////////////////////
#include "nuiTableView.h"

class ButtonCellSource : public nuiCellSource
{
public:
  ButtonCellSource() : nuiCellSource() {}
  
  nuiWidget* CreateCell()
  {
//    nuiButton* pButton = new nuiButton("Button 0");
//    return pButton;
    nuiHBox* pHBox = new nuiHBox();
    pHBox->SetExpand(nuiExpandShrinkAndGrow);
    pHBox->SetEqualizeCells(true);

    for (int i = 0; i < 1; ++i)
    {
      nuiButton* pButton = new nuiButton("Button ");
      pHBox->AddCell(pButton);
    }

    return pHBox;
  }
  
  void UpdateCell(int32 index, nuiWidget* pCell)
  {
    nuiHBox* pHBox = (nuiHBox*)pCell;
    for (int i = 0; i < 1; ++i)
    {
      nuiButton* pButton = (nuiButton*)pHBox->GetCell(i);
      nglString label;
      label.CFormat("Button %d-%d", index, i);
      ((nuiLabel*)pButton->GetChild(0))->SetText(label);
    }
  }
  
  uint32 GetNumberOfCells()
  {
    return 4096;
  }
};


class ComplexCellSource : public nuiCellSource
{
public:
  ComplexCellSource() : nuiCellSource() {}
  
  nuiWidget* CreateCell()
  {
    nuiHBox* pBox = new nuiHBox(0);
    pBox->SetExpand(nuiExpandShrinkAndGrow);

    // a simple togglebutton
    nuiToggleButton* pBtn = new nuiToggleButton(_T("toggleButton"));
    pBtn->SetObjectName(_T("MyButton"));
    pBox->AddCell(pBtn);
    
    pBtn = new nuiToggleButton(_T("toggleButton"));
    pBtn->SetObjectName(_T("MyButton"));
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton(_T("toggleButton"));
    pBtn->SetObjectName(_T("MyButton"));
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton(_T("toggleButton"));
    pBtn->SetObjectName(_T("MyButton"));
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton(_T("toggleButton"));
    pBtn->SetObjectName(_T("MyButton"));
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton();
    pBtn->SetObjectName(_T("MyButton"));
    pBox->AddCell(pBtn, nuiCenter);
    return pBox;
  }
  
  void UpdateCell(int32 index, nuiWidget* pCell)
  {
    nuiHBox* pBox = (nuiHBox*)pCell;
    nglString label;
    label.CFormat("Button %d", index+1);
    ((nuiLabel*)pBox->GetChild(0)->GetChild(0))->SetText(label);
  }
  
  uint32 GetNumberOfCells()
  {
    return 21;
  }
};

////////////////////////////////////////////////////////////////////////////////



/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
  : nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), mEventSink(this)
{
  SetDebugMode(true);
  //App->GetLog().SetLevel("nuiTexture", NGL_LOG_ALWAYS);
  LoadCSS(_T("rsrc:/css/main.css"));  
}

MainWindow::~MainWindow()
{
}


void MainWindow::OnClose()
{
  if (GetNGLWindow()->IsInModalState())
    return;

  App->Quit();
}

void MainWindow::OnCreation()
{
  int test = 0;
  switch (test)
  {
  case 0:
    {
      nuiPosition pos[] = {
        nuiTopLeft, nuiTop, nuiTopRight,
        nuiLeft, nuiCenter, nuiRight,
        nuiBottomLeft, nuiBottom, nuiBottomRight,
        nuiNoPosition
      };
      const char* name[] = {
        "TopLeft", "Top", "TopRight",
        "Left", "Center", "Right",
        "BottomLeft", "Bottom", "BottomRight",
        NULL
      };
      
      for (int i = 0; pos[i] != nuiNoPosition; i++)
      {
        nuiLabel* prout = new nuiLabel(name[i]);
        nuiButton* button = new nuiButton();
        button->AddChild(prout);
        prout->SetBackground(true);
        prout->SetBackgroundColor(nuiColor(0, 255, 0, 128));
    //    prout->SetPosition(nuiFillVertical);
        button->SetPosition(pos[i]);
        nuiAttributeAnimation* anim = new nuiAttributeAnimation();
        anim->SetTargetObject(prout);
        anim->SetTargetAttribute("Alpha");
        anim->SetCaptureStartOnPlay(true);
        anim->SetEndValue(0.5);
        anim->SetEasing(nuiEasingSinus);
        anim->SetDuration(1.0f);
        prout->AddAnimation("Fade", anim);
//        anim->Play(100000, eAnimLoopPingPong);

        AddChild(button);
      }
      
      
      
      return;
    }break;
    
  case 1:
    {
      ButtonCellSource* pSource = new ButtonCellSource();
    //  ComplexCellSource* pSource = new ComplexCellSource();
      nuiTableView* pListView = new nuiTableView(pSource);
      AddChild(pListView);
    nuiWidget* pChild = CreateTestDelChildren();
    AddChild(pChild);

    nuiScrollView* pSView = new nuiScrollView(false, true);
    AddChild(pSView);
    pSView->SetPosition(nuiFill);
    pSView->SetUserWidth(256);
    
    nuiVBox* pVBox = new nuiVBox();
    pVBox->SetBorderRight(42);
    pVBox->SetExpand(nuiExpandShrinkAndGrow);
    pSView->AddChild(pVBox);
    nuiButton* pButton;
    
      for (int i = 0; i < 128; i++)
      {
        nglString str;
        str.CFormat("Button %d", i);
        pButton = new nuiButton(str);
        pButton->SetObjectName(str);
        pButton->SetUserHeight(42);
        pVBox->AddCell(pButton);
      }
      
      return;
    } break;

  case 2:
    {
    // create a vertical box for the layout
      nuiVBox* pMainBox = new nuiVBox(0);
      pMainBox->SetExpand(nuiExpandShrinkAndGrow);
      pMainBox->SetPosition(nuiFillVertical);
      AddChild(pMainBox);

      // create a label width background for information display
      nuiPane* pPane = new nuiPane();
      pPane->SetBorder(20,20);
      pMainBox->AddCell(pPane, nuiFillHorizontal);
      mpLabel = new nuiLabel("---- Click on a button ---");
      pPane->AddChild(mpLabel);
      pPane->SetUserHeight(40);

      nuiLabel* pLabel = new nuiLabel("nuiButton:", nuiFont::GetFont(16));
      pMainBox->AddCell(pLabel, nuiLeft);
      pMainBox->AddCell(Tutorial_Buttons());

      pLabel = new nuiLabel("nuiToggleButton:", nuiFont::GetFont(16));
      pMainBox->AddCell(pLabel, nuiLeft);
      pMainBox->AddCell(Tutorial_ToggleButtons());

      pLabel = new nuiLabel("nuiRadioButton:", nuiFont::GetFont(16));
      pMainBox->AddCell(pLabel, nuiLeft);
      pMainBox->AddCell(Tutorial_RadioButtons1());

      pLabel = new nuiLabel("nuiRadioButton:", nuiFont::GetFont(16));
      pMainBox->AddCell(pLabel, nuiLeft);
      pMainBox->AddCell(Tutorial_RadioButtons2());

      // make the mainbox's layout fill the entire user size (c.f. line #33)
      pMainBox->SetAllCellsExpand(nuiExpandShrinkAndGrow);

      nuiAttributeAnim<float>* pAnim = new nuiAttributeAnim<float>(0.5, 1.0);
      pAnim->SetTargetAttribute("Alpha");
      pAnim->SetTargetObject(pPane);
      pPane->AddAnimation("Fade", pAnim);
      pAnim->Play(100000000, nuiAnimLoop::eAnimLoopPingPong);

//      AddChild(new nuiIntrospector(pMainBox));
    } break;

  case 3:
      {
        nuiLabel* pLabel = new nuiLabel("My Label");
        pLabel->SetPosition(nuiCenter);
        AddChild(pLabel);

  //      nuiAttributeAnim<float>* pAnim = new nuiAttributeAnim<float>(0.5, 1.0);
  //      pAnim->SetTargetAttribute("Alpha");
  //      pAnim->SetTargetObject(pLabel);
  //      pLabel->AddAnimation("Fade", pAnim);
  //      pAnim->Play(100000000, nuiAnimLoop::eAnimLoopPingPong);
      }

      if (0)
      {
          nuiButton* pButton = new nuiButton("My Button");
          pButton->SetPosition(nuiCenter);
          AddChild(pButton);
          
          nuiAttributeAnim<float>* pAnim = new nuiAttributeAnim<float>(0.5, 1.0);
          pAnim->SetTargetAttribute("Alpha");
          pAnim->SetTargetObject(pButton);
          pButton->AddAnimation("Fade", pAnim);
          pAnim->Play(100000000, nuiAnimLoop::eAnimLoopPingPong);
      } break;
    }
}








//******************************************************************************************
//
// nuiButton
//
//

#define TAG_BUTTON1 1
#define TAG_BUTTON2 2
#define TAG_BUTTON3 3
#define TAG_BUTTON4 4

nuiWidget* MainWindow::Tutorial_Buttons()
{
  nuiHBox* pBox = new nuiHBox(0);
  //pBox->EnableSurface(true);
  
  // a simple button
  nuiButton* pBtn = new nuiButton(_T("button"));
  pBtn->SetObjectName(_T("MyButton"));
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON1);
  pBox->AddCell(pBtn, nuiCenter);

  // a simple button filling the box's cell
  pBtn = new nuiButton(_T("button"));
  pBtn->SetObjectName(_T("MyButton"));
  pBox->AddCell(pBtn, nuiFill);
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON2);


  // a button with an image
  nglImage pImg(_T("rsrc:/decorations/button1.png"));
  pBtn = new nuiButton(pImg);
  pBtn->SetObjectName(_T("MyButton"));
  pBox->AddCell(pBtn);
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON3);


  // a roll-over button using decorations
  nuiGradientDecoration* pDecoUp = new nuiGradientDecoration(_T("DecoUp"), nuiColor(192,192,192), nuiColor(128,128,128), 1, nuiColor(0,0,0), eStrokeAndFillShape);
  nuiColorDecoration* pDecoUpHover = new nuiColorDecoration(_T("DecoUpHover"), nuiColor(255,0,0,128), 1, nuiColor(0,0,0));
  nuiFrame* pFrame = new nuiFrame(_T("DecoDown"), _T("rsrc:/decorations/button1.png"), nuiRect(0,0,57,54));

  // create a nuiStateDecoration using the three previous decorations for the rollover's three states : up, hover and done
  nuiStateDecoration* pStateDeco = new nuiStateDecoration(_T("Deco"), _T("DecoUp"), _T("DecoDown"), _T("DecoUpHover"));

  pBtn = new nuiButton(pStateDeco);
  pBtn->SetObjectName(_T("MyButton"));
  pBtn->SetUserSize(40,40);
  pBox->AddCell(pBtn, nuiCenter);
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON4);


  return pBox;
}




//******************************************************************************************
//
// nuiToggleButton
//
//

#define TAG_TOGGLEBUTTON1 1
#define TAG_TOGGLEBUTTON2 2
#define TAG_TOGGLEBUTTON3 3
#define TAG_TOGGLEBUTTON4 4

nuiWidget* MainWindow::Tutorial_ToggleButtons()
{
  nuiHBox* pBox = new nuiHBox(0);

  // a simple togglebutton
  nuiToggleButton* pBtn = new nuiToggleButton(_T("toggleButton"));
  pBtn->SetObjectName(_T("MyButton"));
  pBox->AddCell(pBtn);
  mEventSink.Connect(pBtn->ButtonPressed, &MainWindow::OnTogglePressed, (void*)TAG_BUTTON1);
  mEventSink.Connect(pBtn->ButtonDePressed, &MainWindow::OnTogglePressed, (void*)TAG_BUTTON2);

  // a togglebutton, with a "checkbox" look : leave the button without any child
  pBtn = new nuiToggleButton();
  pBtn->SetObjectName(_T("MyButton"));
  pBox->AddCell(pBtn, nuiCenter);
  mEventSink.Connect(pBtn->ButtonPressed, &MainWindow::OnTogglePressed, (void*)TAG_BUTTON3);
  mEventSink.Connect(pBtn->ButtonDePressed, &MainWindow::OnTogglePressed, (void*)TAG_BUTTON4);

  return pBox;
}




//******************************************************************************************
//
// nuiRadioButton
//
//

// first set : radio button with text inside
nuiWidget* MainWindow::Tutorial_RadioButtons1()
{
  nuiHBox* pBox = new nuiHBox(0);

  for (int index = 0; index < 3; index++)                // will create 3 radiobuttons,
  {
    nglString tmp;
    tmp.Format(_T("Radio %d"), index);
    nuiRadioButton* pRadioBut = new nuiRadioButton(tmp);// with text inside
    pRadioBut->SetObjectName(_T("MyButton"));
    pBox->AddCell(pRadioBut);
    pRadioBut->SetGroup(_T("radios"));                  // set the radio group for group behavior

    // will send an event in the ::OnRadioPressed receiver when the radiobutton is 'activated'
    mEventSink.Connect(pRadioBut->Activated, &MainWindow::OnRadioPressed, (void*)index);  // index is given as a user parameter to recognise the button
  }


  return pBox;
}


// second set : classic radio button, using a radiobutton group
nuiWidget* MainWindow::Tutorial_RadioButtons2()
{
  nuiHBox* pBox = new nuiHBox(0);

  for (int index = 0; index < 3; index++)
  {
    nuiRadioButton* pRadioBut = new nuiRadioButton(); // leave it without any child : it'll get a class "radio" look
    pRadioBut->SetObjectName(_T("MyButton"));
    pRadioBut->SetPosition(nuiCenter);
    pBox->AddCell(pRadioBut);
    pRadioBut->SetGroup(_T("radios2"));

    mEventSink.Connect(pRadioBut->Activated, &MainWindow::OnRadioPressed, (void*)index);
  }

  return pBox;

}

nuiWidget* MainWindow::CreateTestDelChildren()
{
  nuiWidget* pWidget = new nuiWidget();
  
  nuiButton* pButton = new nuiButton("Recreate view");
  pButton->SetUserHeight(48);
  pButton->SetPosition(nuiFillTop);
  mEventSink.Connect(pButton->Activated, &MainWindow::OnCreateView, pWidget);

  pWidget->AddChild(pButton);
  return pWidget;
}


void MainWindow::OnCreateView(const nuiEvent& rEvent)
{
  nuiWidget* pWidget = (nuiWidget*)rEvent.mpUser;
  if (pWidget->GetChildrenCount()>1)
  {
    pWidget->DelChild(pWidget->GetChild(1));
  }
  else
  {
    nuiVBox* pVBox = new nuiVBox();
    pVBox->SetBorderTop(48);
    pVBox->SetPosition(nuiFill);
    pVBox->SetExpand(nuiExpandShrinkAndGrow);
    for (int i = 0; i < 256; ++i)
    {
      nuiButton* pButton = new nuiButton("1");
      nglString name;
      name.SetCInt(i);
      ((nuiLabel*)pButton->GetChild(0))->SetText(name);
      pVBox->AddCell(pButton);
    }
    pWidget->AddChild(pVBox);
  }
}







//******************************************************************************************
//
// events receivers
//
//


void MainWindow::OnButtonPressed(const nuiEvent& rEvent)
{
  int64 tag = (int64)rEvent.mpUser;

  nglString msg;

  switch (tag)
  {
    case TAG_BUTTON1:
      msg = _T("a simple button\nwith a 'nuiCenter' position");
      break;
    case TAG_BUTTON2:
      msg = _T("the same simple button\nbut with a 'nuiFill' position");
      break;
    case TAG_BUTTON3:
      msg = _T("a simple button\nwith an image inside");
      break;
    case TAG_BUTTON4:
      msg = _T("a rollover button\nusing three decorations");
      break;
  }

  mpLabel->SetText(msg);
  rEvent.Cancel();
}


void MainWindow::OnTogglePressed(const nuiEvent& rEvent)
{
  int64 tag = (int64)rEvent.mpUser;

  nglString msg;

  switch (tag)
  {
    case TAG_TOGGLEBUTTON1:
      msg = _T("a simple togglebutton, pressed");
      break;
    case TAG_TOGGLEBUTTON2:
      msg = _T("a simple togglebutton, released");
      break;
    case TAG_TOGGLEBUTTON3:
      msg = _T("a checkbox, pressed");
      break;
    case TAG_TOGGLEBUTTON4:
      msg = _T("a checkbox, released");
      break;
  }

  mpLabel->SetText(msg);

  rEvent.Cancel();
}



void MainWindow::OnRadioPressed(const nuiEvent& rEvent)
{
  int64 index = (int64)rEvent.mpUser;

  nglString msg;
  msg.Format(_T("radio button #%d"), index);
  mpLabel->SetText(msg);

  rEvent.Cancel();
}












bool MainWindow::LoadCSS(const nglPath& rPath)
{
  nglIStream* pF = rPath.OpenRead();
  if (!pF)
  {
    NGL_OUT(_T("Unable to open CSS source file '%ls'\n"), rPath.GetChars());
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
  
  NGL_OUT(_T("%ls\n"), pCSS->GetErrorString().GetChars());
  
  delete pCSS;
  return false;
}


