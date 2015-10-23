/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */


#include "nui.h"

nuiLayersInspector::nuiLayersInspector()
: mSink(this)
{
  SetObjectClass("nuiLayersInspector");
  
  // decoration
  nuiDecoration* pDeco = nuiDecoration::Get(INTROSPECTOR_DECO_CLIENT_BKG);
  mpImage = NULL;
  mpAttributeGrid = NULL;
  
  if (pDeco)
  {
    SetDecoration(pDeco, eDecorationBorder);
  }
  
  UpdateLayers();
  
//  mSink.Connect(nuiLayer::LayersChanged, &nuiLayersInspector::OnLayersChanged);
}

nuiLayersInspector::~nuiLayersInspector()
{  
  
}

void nuiLayersInspector::OnLayersChanged(const nuiEvent& rEvent)
{
  UpdateLayers();
}

void nuiLayersInspector::UpdateLayers()
{
  Clear();
  
  nuiSplitter* pSplitter = new nuiSplitter(nuiVertical);
  AddChild(pSplitter);
  pSplitter->SetMasterChild(true);
  
  nuiScrollView* pScrollView1 = new nuiScrollView(false, true);
  nuiScrollView* pScrollView2 = new nuiScrollView(false, true);
  pSplitter->AddChild(pScrollView1);
  pSplitter->AddChild(pScrollView2);
  
  nuiList* pList = new nuiList();
  pScrollView1->AddChild(pList);
  
  std::map<nglString, nuiRef<nuiLayer>> layers;
  nuiLayer::GetLayers(layers);
  
  uint32 i = 0;
  for (const auto& it : layers)
  {
    nuiLayer* pLayer = it.second;
    nglString name(pLayer->GetObjectName());
    nglString index;
    index.SetCInt(i);
    nuiLabel* pLabel = new nuiLabel(name);
    pLabel->SetProperty("Layers", name);
    pLabel->SetProperty("Index", index);
    pLabel->SetToken(new nuiToken<nuiRef<nuiLayer>>(pLayer));
    pList->AddChild(pLabel);
    
    i++;
  }
  
  /// Attribute list
  mpAttributeGrid = new nuiGrid(2, 0);
  mpAttributeGrid->DisplayGridBorder(true, 1.0f);
  mpAttributeGrid->SetColumnExpand(1, nuiExpandShrinkAndGrow);
  
  pScrollView2->AddChild(mpAttributeGrid);
  
  mSink.Connect(pList->SelectionChanged, &nuiLayersInspector::OnLayerSelection, (void*)pList);
}

void nuiLayersInspector::OnLayerSelection(const nuiEvent& rEvent)
{
  nuiList* pList = (nuiList*)rEvent.mpUser;
  nuiWidget* pW = pList->GetSelected();
  nglString info("no info");
  nglString name;
  int32 index = -1;
  nuiLayer* pLayer = nullptr;
  if (pW)
  {
    name = pW->GetProperty("Layer");
    index = pW->GetProperty("Index").GetCUInt();
    nuiRef<nuiLayer> layerref;
    if (nuiGetTokenValue<nuiRef<nuiLayer>>(pW->GetToken(), layerref));
    {
      pLayer = layerref.Ptr();
    }
  }
  
  mSlot.DisconnectAll();
  //  mSlot.Connect(pLayers->Changed, nuiMakeDelegate(mpLabel, &nuiWidget::Invalidate));
  //  mSlot.Connect(pLayers->Changed, nuiMakeDelegate(mpLabel->GetParent(), &nuiWidget::Invalidate));

  if (!pLayer)
    return;
  
//  // build attributes list
//  std::map<nglString, nuiAttributeBase*> attributes;
//  nuiLayer::GetAttributesOfClass(index, attributes);
//  uint32 i = 0;
//  std::map<nglString, nuiAttributeBase*>::const_iterator it_a = attributes.begin();
//  std::map<nglString, nuiAttributeBase*>::const_iterator end_a = attributes.end();
//  
//  uint32 rows = mpAttributeGrid->GetNbRows();
//  if (rows)
//    mpAttributeGrid->RemoveRows(0, rows);
//  
//  rows = attributes.size();
//  if (rows)
//  {
//    mpAttributeGrid->AddRows(0, rows);
//  }
//  
//  while (it_a != end_a)
//  {
//    nglString pname(it_a->first);
//    //printf("\tattr: %s\n", pname.GetChars());
//    //nuiAttribBase Base = it_a->second;
//    //nuiAttributeEditor* pEditor = Base.GetEditor();
//    mpAttributeGrid->SetCell(0, i, new nuiLabel(pname + nglString(":")));
//        
//    ++it_a;
//    i++;
//  }
}

