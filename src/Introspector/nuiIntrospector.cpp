/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


#define CELL_TOOLBAR 0
#define CELL_CLIENT 1
#define CELL_STATUS 2


nuiIntrospector::nuiIntrospector(nuiWidget* pTarget)
  : nuiVBox(3), mEventSink(this), mpTarget(pTarget)
{
  SetObjectClass("nuiIntrospector");
  SetExpand(nuiExpandShrinkAndGrow);

  InitDecorations();
  SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BKG), eDecorationBorder);
  
  SetCell(CELL_TOOLBAR, GetToolbar());
  SetCellExpand(CELL_TOOLBAR, nuiExpandFixed);

  SetCellExpand(CELL_CLIENT, nuiExpandShrinkAndGrow);
  
  SetCell(CELL_STATUS, GetStatusbar());
  SetCellExpand(CELL_STATUS, nuiExpandFixed);
  SetCellMinPixels(CELL_STATUS, 10);
  SetCellMaxPixels(CELL_STATUS, 10);
  

}


nuiIntrospector::~nuiIntrospector()
{

}


nuiWidget* nuiIntrospector::GetToolbar()
{
  nuiHBox* pBox = new nuiHBox(0);
  pBox->SetExpand(nuiExpandShrinkAndGrow);
  pBox->SetBorder(0, 0, 0, 10);
  
  // for visual comfort :)
//  nuiSeparator* pSeparator = new nuiSeparator(nuiHorizontal);
//  pSeparator->SetPosition(nuiFillHorizontal);
//  pBox->AddCell(pSeparator);
//  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  pBox->AddCell(NULL);
  pBox->SetCellExpand(pBox->GetNbCells()-1, nuiExpandShrinkAndGrow);

  nuiColor textColor = nuiColor(32,32,32);

  // add buttons to inspector's tools
  nuiRadioButton* pWBtn = new nuiRadioButton("Widget Tree");
  pWBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pWBtn->SetBorders(0);

  mEventSink.Connect(pWBtn->ButtonPressed, &nuiIntrospector::ShowWidgetInspector);
  pBox->AddCell(pWBtn);

  nuiRadioButton* pTBtn = new nuiRadioButton("Threads");
  pTBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pTBtn->SetBorders(0);
  mEventSink.Connect(pTBtn->ButtonPressed, &nuiIntrospector::ShowThreadInspector);
  pBox->AddCell(pTBtn);
    
  nuiRadioButton* pFontBtn = new nuiRadioButton("Fonts");
  pFontBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pFontBtn->SetBorders(0);
  mEventSink.Connect(pFontBtn->ButtonPressed, &nuiIntrospector::ShowFontInspector);
  pBox->AddCell(pFontBtn);

  nuiRadioButton* pDecoBtn = new nuiRadioButton("Decorations");
  pDecoBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pDecoBtn->SetBorders(0);
  mEventSink.Connect(pDecoBtn->ButtonPressed, &nuiIntrospector::ShowDecorationInspector);
  pBox->AddCell(pDecoBtn);
  
  nuiRadioButton* pTextureBtn = new nuiRadioButton("Textures");
  pTextureBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pTextureBtn->SetBorders(0);
  mEventSink.Connect(pTextureBtn->ButtonPressed, &nuiIntrospector::ShowTextureInspector);
  pBox->AddCell(pTextureBtn);
  
  nuiRadioButton* pObjectsBtn = new nuiRadioButton("Objects");
  pObjectsBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pObjectsBtn->SetBorders(0);
  mEventSink.Connect(pObjectsBtn->ButtonPressed, &nuiIntrospector::ShowObjectInspector);
  pBox->AddCell(pObjectsBtn);
  
  nuiRadioButton* pLayersBtn = new nuiRadioButton("Layers");
  pLayersBtn->SetDecoration(nuiDecoration::Get(INTROSPECTOR_DECO_BUTTON), eDecorationBorder);
  pLayersBtn->SetBorders(0);
  mEventSink.Connect(pLayersBtn->ButtonPressed, &nuiIntrospector::ShowLayersInspector);
  pBox->AddCell(pLayersBtn);
  
  // for visual comfort :).... doesn't work..!!?!
//  nuiSeparator* pSeparator2 = new nuiSeparator(nuiHorizontal);
//  pSeparator2->SetPosition(nuiFill);
//  pBox->AddCell(pSeparator2);
//  pBox->SetCellExpand(pBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  pBox->AddCell(NULL);
  pBox->SetCellExpand(pBox->GetNbCells()-1, nuiExpandShrinkAndGrow);
  
  // launch the first tool by default
  pWBtn->SetPressed(true);

  return pBox;
}


nuiWidget* nuiIntrospector::GetStatusbar()
{
  nuiWidget* pCont = new nuiWidget();
  return pCont;
}


void nuiIntrospector::ShowWidgetInspector(const nuiEvent& rEvent)
{
  SetCell(CELL_CLIENT, new nuiWidgetInspector(mpTarget)); 
  rEvent.Cancel();
}

void nuiIntrospector::ShowThreadInspector(const nuiEvent& rEvent)
{
  SetCell(CELL_CLIENT, new nuiThreadInspector()); 
  rEvent.Cancel();
}

void nuiIntrospector::ShowFontInspector(const nuiEvent& rEvent)
{
  SetCell(CELL_CLIENT, new nuiFontInspector()); 
  rEvent.Cancel();
}

void nuiIntrospector::ShowDecorationInspector(const nuiEvent& rEvent)
{
  SetCell(CELL_CLIENT, new nuiDecorationInspector()); 
  rEvent.Cancel();
}

void nuiIntrospector::ShowTextureInspector(const nuiEvent& rEvent)
{
  SetCell(CELL_CLIENT, new nuiTextureInspector()); 
  rEvent.Cancel();
}

void nuiIntrospector::ShowObjectInspector(const nuiEvent& rEvent)
{
  SetCell(CELL_CLIENT, new nuiObjectInspector()); 
  rEvent.Cancel();
}

void nuiIntrospector::ShowLayersInspector(const nuiEvent& rEvent)
{
  nuiTopLevel* pTop = mpTarget->GetTopLevel();
  nuiRenderThread* pRenderThread = pTop->GetRenderThread();
  SetCell(CELL_CLIENT, new nuiLayersInspector(pRenderThread));
  rEvent.Cancel();
}

void nuiIntrospector::InitDecorations()
{
  //nuiDefaultDecoration::Init();
  // window background
  new nuiColorDecoration(INTROSPECTOR_DECO_BKG, nuiRect(10,10,0,0), nuiColor(224,224,224));
  
  // toolbar's buttons
  nuiStateDecoration* pBtn = new nuiStateDecoration(INTROSPECTOR_DECO_BUTTON);
  nuiGradientDecoration* pGradOff
    = new nuiGradientDecoration
            (
              "INTROSPECTOR_DECO_GRAD_OFF", 
              nuiRect(3, 3, 0, 0),
              nuiColor(246,246,246),
              nuiColor(220,220,220),
              nuiColor(210,210,210),
              nuiColor(255,255,255), 
              nuiVertical,
              1,
              nuiColor(100,100,100),
              eStrokeAndFillShape
            );
  nuiGradientDecoration* pGradOn
    = new nuiGradientDecoration
            (
              "INTROSPECTOR_DECO_GRAD_ON", 
              nuiRect(3, 3, 0, 0),
              nuiColor(227,237,250),
              nuiColor(161,202,243),
              nuiColor(126,182,239),
              nuiColor(215,255,255),
              nuiVertical,
              1,
              nuiColor(100,100,100),
              eStrokeAndFillShape
            );
  pBtn->SetState(nuiStateReleased, pGradOff);
  
  
  pBtn->SetState(nuiStatePressed, pGradOn);
  
  // client background
  new nuiColorDecoration(INTROSPECTOR_DECO_CLIENT_BKG, nuiRect(0,0,0,0), nuiColor(250,250,250), 1, nuiColor(190,190,190), eStrokeAndFillShape);

  nuiBorderDecoration* pTex = new nuiBorderDecoration("INTROSPECTOR_TEXTURE_BORDER");
  pTex->SetStrokeColor(nuiColor(0,0,0));
  pTex->SetStrokeSize(1);

  // folderpane's title
  nuiGradientDecoration* pFTitle
    = new nuiGradientDecoration
            (
              INTROSPECTOR_DECO_FOLDERPANE_TITLE,
              nuiRect(2, 2, 0, 0),
              nuiColor(255,255,255),
              nuiColor(232,232,232),
              nuiColor(232,232,232),
              nuiColor(208,208,208),
              nuiVertical,
              1,
              nuiColor(180,180,180),
              eStrokeAndFillShape
            );
  
  pFTitle->SetOffset1(0);
  pFTitle->SetOffset2(0.2);
  pFTitle->SetOffset3(0.2);
  pFTitle->SetOffset4(1);
    

  // grids' title
  new nuiGradientDecoration(INTROSPECTOR_DECO_GRID_TITLE, 
    nuiRect(3, 3, 0, 0), nuiColor(246,246,246), nuiColor(220,220,220), nuiColor(210,210,210), nuiColor(255,255,255));
    
  // grids' cell
  //  new nuiColorDecoration(INTROSPECTOR_DECO_GRID_CELL, nuiRect(2,2,0,0), nuiColor(250,250,250), 1, nuiColor(180,180,180), eStrokeAndFillShape);
  
  nuiFontRequest* pFontRequest = new nuiFontRequest();
  nuiFontRequest& rFontRequest(*pFontRequest);
  rFontRequest.SetGenericName("sans-serif", 1.0f);
  rFontRequest.SetBold(false, 1.0); 
  rFontRequest.SetItalic(false, 1.0);
  rFontRequest.MustHaveSize(10, 1.0);
  nuiFontManager::GetManager().GetFont(rFontRequest, "INTROSPECTOR_FONT_NORMAL");
  rFontRequest.SetGenericName("sans-serif", 1.0f);
  rFontRequest.SetBold(true, 1.0); 
  rFontRequest.SetItalic(false, 1.0);
  rFontRequest.MustHaveSize(10, 1.0);
  nuiFontManager::GetManager().GetFont(rFontRequest, "INTROSPECTOR_FONT_BOLD");
  pFontRequest->Release();
}

