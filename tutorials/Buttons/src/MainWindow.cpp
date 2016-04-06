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

#include "nuiWidgetInspector.h"

#include "nuiDebugServer.h"

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
    nuiToggleButton* pBtn = new nuiToggleButton("toggleButton");
    pBtn->SetObjectName("MyButton");
    pBox->AddCell(pBtn);
    
    pBtn = new nuiToggleButton("toggleButton");
    pBtn->SetObjectName("MyButton");
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton("toggleButton");
    pBtn->SetObjectName("MyButton");
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton("toggleButton");
    pBtn->SetObjectName("MyButton");
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton("toggleButton");
    pBtn->SetObjectName("MyButton");
    pBox->AddCell(pBtn);

    pBtn = new nuiToggleButton();
    pBtn->SetObjectName("MyButton");
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

static const char* vertex_shader =
SHADER_STRING
(
 
attribute vec4 Position;
attribute vec3 Normal;
attribute vec4 Color;
uniform mat4 SurfaceMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 Offset;
varying vec4 ColorVar;
varying vec3 NormalVar;

void main()
{
  ColorVar = Color;
  NormalVar = Normal;
  gl_Position = (SurfaceMatrix * ProjectionMatrix * ModelViewMatrix * (Position  + Offset));
}
 
);

static const char* fragment_shader =
SHADER_STRING
(
varying vec4 ColorVar;
varying vec3 NormalVar;

void main()
{
  float feather = 1.;
  float ref = abs(NormalVar.y);
  float width = NormalVar.z;
  float smooth = smoothstep(width - feather, width + feather, ref);
  gl_FragColor = vec4(ColorVar.xyz, ColorVar.w * vec4(1.0 - sqrt(smooth)));
}
 
);

////////////////////////////////////////////////////////////////////////////////
class nuiStrokeTest : public nuiWidget
{
public:
  
  nuiStrokeTest()
  {
    if (SetObjectClass("nuiStrokeTest"))
    {
      AddAttribute(new nuiAttribute<float>
                   (nglString("StrokeWidth"), nuiUnitSize,
                    nuiMakeDelegate(this, &nuiStrokeTest::GetStrokeWidth),
                    nuiMakeDelegate(this, &nuiStrokeTest::SetStrokeWidth),
                    nuiRange(1, 0.125, 150, .1, .1, 0)
                    ));

    }

    mpShape = new nuiShape();
    mpShape->LineTo(nuiPoint(50, 50));
    mpShape->LineTo(nuiPoint(200, 50));
//      mpShape->LineTo(nuiPoint(180, 180));
//      mpShape->LineTo(nuiPoint(231, 200));
//    mpShape->LineTo(nuiPoint(70, 180));
//      mpShape->LineTo(nuiPoint(200, 50));
    mpShape->LineTo(nuiPoint(200, 225));
//    mpShape->LineTo(nuiPoint(50, 80));

    mpShader = new nuiShaderProgram("Stroker");
    mpShader->AddShader(eVertexShader, vertex_shader);
    mpShader->AddShader(eFragmentShader, fragment_shader);
    mpShader->Link();
    mpShaderState = mpShader->GetCurrentState();
    
    nuiAttribBase Attrib(GetAttribute("StrokeWidth"));
    if (Attrib.IsValid())
    {
      nuiAttributeEditor* pEditor = Attrib.GetEditor();
      pEditor->SetPosition(nuiTopLeft);
      AddChild(pEditor);
    }
  }
  
  nuiRect CalcIdealSize()
  {
    return nuiRect(200, 200);
  }
  
  bool Draw(nuiDrawContext* pContext)
  {
    pContext->EnableBlending(true);
    pContext->SetBlendFunc(nuiBlendTransp);
    pContext->SetFillColor(nuiColor("blue"));
    pContext->SetShader(mpShader, mpShaderState);
    pContext->DrawObject(*mpShape->Stroke(1.0, mStrokeWidth, nuiLineJoinRound, nuiLineCapBut, 1.));
    
    
    return true;
  }

  NUI_GETSETDO(float , StrokeWidth, Invalidate());
private:
  nuiShape* mpShape = nullptr;
  nuiShaderProgram *mpShader = nullptr;
  nuiShaderState *mpShaderState = nullptr;
  float mStrokeWidth = 10;
};


/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
  : nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), mEventSink(this)
{
  SetDebugMode(true);
  //App->GetLog().SetLevel("nuiTexture", NGL_LOG_ALWAYS);
  LoadCSS("rsrc:/css/main.css");

#if DUMMY_DEBUGGER
  mpClient = new nuiTCPClient();
  if (mpClient->Connect("127.0.0.1", 31337))
  {
    mSocketPool.Add(mpClient, nuiSocketPool::eStateChange);
    mpMessageClient = new nuiMessageClient(mpClient);
    mEventSink.Connect(nuiAnimation::GetTimer()->Tick, [=](const nuiEvent& rEvent)
    {
      mSocketPool.DispatchEvents(1);
    });
  }
#endif

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
  nuiMessage msg;
  msg.Add("TestMessage");
  msg.Add(10);
  msg.Add(10.0f);
  msg.Add(10.0);
  msg.Add(-10);
  NGL_OUT("Input Message: %s\n", msg.GetDescription().GetChars());

  nuiMessageParser parser;
  std::vector<uint8> datamsg = parser.Build(msg);
  parser.Parse(datamsg, [](nuiMessage* pMessage)
  {
    NGL_OUT("Output Message: %s\n", pMessage->GetDescription().GetChars());
    return true;
  });


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
        button->SetTrace(true);
        button->SetDebug(1000);
//        nuiAttributeAnimation* anim = new nuiAttributeAnimation();
//        anim->SetTargetObject(prout);
//        anim->SetTargetAttribute("Alpha");
//        anim->SetCaptureStartOnPlay(true);
//        anim->SetEndValue(0.5);
//        anim->SetEasing(nuiEasingSinus);
//        anim->SetDuration(1.0f);
//        prout->AddAnimation("Fade", anim);
//        anim->Play(100000, eAnimLoopPingPong);

        mEventSink.Connect(button->Activated, [=](const nuiEvent& event)
        {
          printf("pouet %p - %p\n", this, button);
#if DUMMY_DEBUGGER
          switch (i % 3)
          {
            case 0:
              mpMessageClient->Post(nuiMessage("HelloWorld"));
              mpMessageClient->Post(nuiMessage("HelloWorld2", i));
              break;
            case 1:
              mpMessageClient->Post(nuiMessage("HelloWorld"));
              mpMessageClient->Post(nuiMessage("HelloWorld3", i, 1.1f, 1.2, "pouet!"));
              break;
            default:
              mpMessageClient->Post(nuiMessage("HelloWorld"));
              break;
          }
#endif
        });

        AddChild(button);
      }
      
//      nuiVBox* pBox = new nuiVBox();
//      
//      nuiFolderPane* pPane1 = new nuiFolderPane();
//      pPane1->SetTitle("Prout");
//      pPane1->SetContents(new nuiLabel("Contents bleh..."));
//
//      nuiFolderPane* pPane2 = new nuiFolderPane();
//      pPane2->SetTitle("Prout 2");
//      pPane2->SetContents(new nuiLabel("Contents bleh...2"));
//      pBox->AddCell(pPane1);
//      pBox->AddCell(pPane2);
//      
//      pBox->SetExpand(nuiExpandFixed);
//      pBox->SetAllCellsExpand(nuiExpandFixed);
//
//      nuiScrollView* pScrollView = new nuiScrollView();
//      pScrollView->AddChild(pBox);
//
//      AddChild(pScrollView);

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

//      nuiScrollView* pSView = new nuiScrollView(false, true);
//      AddChild(pSView);
//      pSView->SetPosition(nuiFill);
//      pSView->SetUserWidth(256);
//      
//      nuiVBox* pVBox = new nuiVBox();
//      pVBox->SetBorderRight(42);
//      pVBox->SetExpand(nuiExpandShrinkAndGrow);
//      pSView->AddChild(pVBox);
//      nuiButton* pButton;
//    
//      for (int i = 0; i < 128; i++)
//      {
//        nglString str;
//        str.CFormat("Button %d", i);
//        pButton = new nuiButton(str);
//        pButton->SetObjectName(str);
//        pButton->SetUserHeight(42);
//        pVBox->AddCell(pButton);
//      }
      
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

        nuiAttributeAnim<float>* pAnim = new nuiAttributeAnim<float>(0.5, 1.0);
        pAnim->SetTargetAttribute("Alpha");
        pAnim->SetTargetObject(pLabel);
        pLabel->AddAnimation("Fade", pAnim);
        pAnim->Play(100000000, nuiAnimLoop::eAnimLoopPingPong);
      } break;

    case 4:
      {
        nuiSplitter* pSplitter = new nuiSplitter(nuiVertical);
        AddChild(pSplitter);
        nuiButton* pButton = new nuiButton("My Button");
        pButton->SetPosition(nuiCenter);

        if (0)
        {
          nuiAttributeAnim<float>* pAnim = new nuiAttributeAnim<float>(0.5, 1.0);
          pAnim->SetTargetAttribute("Alpha");
          pAnim->SetTargetObject(pButton);
          pButton->AddAnimation("Fade", pAnim);
          pAnim->Play(100000000, nuiAnimLoop::eAnimLoopPingPong);
        }
        
        nuiWidgetInfo* pInspector = new nuiWidgetInfo(pButton);
        pSplitter->AddChild(pButton);
        pSplitter->AddChild(pInspector);
      } break;
    case 5:
    {
      nuiScrollView* pScroll = new nuiScrollView();
      nuiList* pList = new nuiList();
      
      for (int i = 0; i < 100; i++)
      {
        nglString str;
        str.CFormat("Item %d", i);
        pList->AddChild(new nuiLabel(str));
      }
      
      pScroll->AddChild(pList);
      AddChild(pScroll);
    } break;
    case 6:
    {
      AddChild(new nuiStrokeTest());
    }
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
  nuiButton* pBtn = new nuiButton("button");
  pBtn->SetObjectName("MyButton");
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON1);
  pBox->AddCell(pBtn, nuiCenter);

  // a simple button filling the box's cell
  pBtn = new nuiButton("button");
  pBtn->SetObjectName("MyButton");
  pBox->AddCell(pBtn, nuiFill);
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON2);


  // a button with an image
  nglImage pImg("rsrc:/decorations/button1.png");
  pBtn = new nuiButton(pImg);
  pBtn->SetObjectName("MyButton");
  pBox->AddCell(pBtn);
  mEventSink.Connect(pBtn->Activated, &MainWindow::OnButtonPressed, (void*)TAG_BUTTON3);


  // a roll-over button using decorations
  nuiGradientDecoration* pDecoUp = new nuiGradientDecoration("DecoUp", nuiColor(192,192,192), nuiColor(128,128,128), 1, nuiColor(0,0,0), eStrokeAndFillShape);
  nuiColorDecoration* pDecoUpHover = new nuiColorDecoration("DecoUpHover", nuiColor(255,0,0,128), 1, nuiColor(0,0,0));
  nuiFrame* pFrame = new nuiFrame("DecoDown", "rsrc:/decorations/button1.png", nuiRect(0,0,57,54));

  // create a nuiStateDecoration using the three previous decorations for the rollover's three states : up, hover and done
  nuiStateDecoration* pStateDeco = new nuiStateDecoration("Deco", "DecoUp", "DecoDown", "DecoUpHover");

  pBtn = new nuiButton(pStateDeco);
  pBtn->SetObjectName("MyButton");
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
  nuiToggleButton* pBtn = new nuiToggleButton("toggleButton");
  pBtn->SetObjectName("MyButton");
  pBox->AddCell(pBtn);
  mEventSink.Connect(pBtn->ButtonPressed, &MainWindow::OnTogglePressed, (void*)TAG_BUTTON1);
  mEventSink.Connect(pBtn->ButtonDePressed, &MainWindow::OnTogglePressed, (void*)TAG_BUTTON2);

  // a togglebutton, with a "checkbox" look : leave the button without any child
  pBtn = new nuiToggleButton();
  pBtn->SetObjectName("MyButton");
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
    tmp.Format("Radio %d", index);
    nuiRadioButton* pRadioBut = new nuiRadioButton(tmp);// with text inside
    pRadioBut->SetObjectName("MyButton");
    pBox->AddCell(pRadioBut);
    pRadioBut->SetGroup("radios");                  // set the radio group for group behavior

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
    pRadioBut->SetObjectName("MyButton");
    pRadioBut->SetPosition(nuiCenter);
    pBox->AddCell(pRadioBut);
    pRadioBut->SetGroup("radios2");

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
      msg = "a simple button\nwith a 'nuiCenter' position";
      break;
    case TAG_BUTTON2:
      msg = "the same simple button\nbut with a 'nuiFill' position";
      break;
    case TAG_BUTTON3:
      msg = "a simple button\nwith an image inside";
      break;
    case TAG_BUTTON4:
      msg = "a rollover button\nusing three decorations";
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
      msg = "a simple togglebutton, pressed";
      break;
    case TAG_TOGGLEBUTTON2:
      msg = "a simple togglebutton, released";
      break;
    case TAG_TOGGLEBUTTON3:
      msg = "a checkbox, pressed";
      break;
    case TAG_TOGGLEBUTTON4:
      msg = "a checkbox, released";
      break;
  }

  mpLabel->SetText(msg);

  rEvent.Cancel();
}



void MainWindow::OnRadioPressed(const nuiEvent& rEvent)
{
  int64 index = (int64)rEvent.mpUser;

  nglString msg;
  msg.Format("radio button #%d", index);
  mpLabel->SetText(msg);

  rEvent.Cancel();
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


