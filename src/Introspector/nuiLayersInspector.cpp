/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */


#include "nui.h"

nuiLayersInspector::nuiLayersInspector(nuiRenderThread* pRenderThread)
: mSink(this), mpRenderThread(pRenderThread)
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
  
  Setup();
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

static bool CompareLayerTimes(const std::pair<nuiLayer*, nuiRenderingStat>& first, const std::pair<nuiLayer*, nuiRenderingStat> & second)
{
  return first.second.mTime > second.second.mTime;
}

static bool CompareLayerCounts(const std::pair<nuiLayer*, nuiRenderingStat>& first, const std::pair<nuiLayer*, nuiRenderingStat> & second)
{
  return first.second.mCount > second.second.mCount;
}

static bool CompareLayerAverage(const std::pair<nuiLayer*, nuiRenderingStat>& first, const std::pair<nuiLayer*, nuiRenderingStat> & second)
{
  return (first.second.mTime / (float)first.second.mCount) > (second.second.mTime / (float)second.second.mCount);
}


void nuiLayersInspector::Setup()
{
  Clear();
  
  nuiVBox* pBox = new nuiVBox();
  AddChild(pBox);
  nuiHBox* pHeader = new nuiHBox();
  pHeader->AddCell(new nuiLabel("Sort"));
  nuiTreeNode* pSortOptions = new nuiTreeNode("Layers Sorting");
  
  auto pNodeNone = new nuiTreeNode("None");
  pNodeNone->SetProperty("sort_function", "none");
  pSortOptions->AddChild(pNodeNone);

  auto pNodeTime = new nuiTreeNode("Total Time");
  pNodeTime->SetProperty("sort_function", "time");
  pSortOptions->AddChild(pNodeTime);

  auto pNodeCount = new nuiTreeNode("Total Count");
  pNodeCount->SetProperty("sort_function", "count");
  pSortOptions->AddChild(pNodeCount);

  auto pNodeAverage = new nuiTreeNode("Average Time");
  pNodeAverage->SetProperty("sort_function", "average");
  pSortOptions->AddChild(pNodeAverage);

  mpSortCombo = new nuiComboBox(pSortOptions);
  pHeader->AddCell(mpSortCombo);
  pBox->AddCell(pHeader);

  pHeader->AddCell(nullptr);
  pHeader->SetCellExpand(pHeader->GetNbCells() - 1, nuiExpandShrinkAndGrow);

  nuiButton* pUpdateBtn = new nuiButton("Update");
  pHeader->AddCell(pUpdateBtn);
  mSink.Connect(pUpdateBtn->Activated, &nuiLayersInspector::OnUpdateLayers);
  
  nuiSplitter* pSplitter = new nuiSplitter(nuiVertical);
  pBox->AddCell(pSplitter);
  pBox->SetCellExpand(1, nuiExpandShrinkAndGrow);

//  pSplitter->SetMasterChild(true);
  pSplitter->SetPosition(nuiFill);
  
  nuiScrollView* pScrollView1 = new nuiScrollView(false, true);
  nuiScrollView* pScrollView2 = new nuiScrollView(false, true);
  pSplitter->AddChild(pScrollView1);
  pSplitter->AddChild(pScrollView2);
  
  mpLayerList = new nuiList();
  pScrollView1->AddChild(mpLayerList);
  
  std::map<nglString, nuiRef<nuiLayer>> layers;
  nuiLayer::GetLayers(layers);
  
  /// Attribute list
  mpAttributeGrid = new nuiGrid(2, 0);
  mpAttributeGrid->DisplayGridBorder(true, 1.0f);
  mpAttributeGrid->SetColumnExpand(1, nuiExpandShrinkAndGrow);
  
  pScrollView2->AddChild(mpAttributeGrid);
  
  mSink.Connect(mpSortCombo->SelectionChanged, &nuiLayersInspector::OnUpdateLayers, nullptr);
  mSink.Connect(mpLayerList->SelectionChanged, &nuiLayersInspector::OnLayerSelection, (void*)mpLayerList);
}

void nuiLayersInspector::UpdateLayers()
{
  mpLayerList->Clear();
  
  std::map<nglString, nuiRef<nuiLayer>> layers;
  nuiLayer::GetLayers(layers);
  
  std::map<nuiLayer*, nuiRenderingStat> stats = mpRenderThread->GetStats();
  
  std::vector<nuiLayer*> sortedlayers;
  for (const auto& it : layers)
  {
    sortedlayers.push_back(it.second.Ptr());
  }
  
  nuiTreeNode* pSelection = mpSortCombo->GetSelected();

  nglString sort_function_name;
  int sort_function = 0;
  if (pSelection)
  {
    sort_function_name = pSelection->GetProperty("sort_function");
    if (sort_function_name == "time")
    {
      sort_function = 1;
      std::sort(sortedlayers.begin(), sortedlayers.end(), [&](nuiLayer* first, nuiLayer* second) {
        return stats[first].mTime > stats[second].mTime;
      });
    }
    else if (sort_function_name == "count")
    {
      sort_function = 2;
      std::sort(sortedlayers.begin(), sortedlayers.end(), [&](nuiLayer* first, nuiLayer* second) {
        return stats[first].mCount > stats[second].mCount;
      });
    }
    else if (sort_function_name == "average")
    {
      sort_function = 3;
      std::sort(sortedlayers.begin(), sortedlayers.end(), [&](nuiLayer* first, nuiLayer* second) {
        int64 v0 = MAX(1, stats[first].mCount);
        int64 v1 = MAX(1, stats[second].mCount);
        return (stats[first].mTime / (double)v0) > (stats[second].mTime / (double)v1);
      });
    }
  }
  
  uint32 i = 0;
  for (auto pLayer : sortedlayers)
  {
    nuiWidget* pHolder = new nuiWidget();
    nglString name(pLayer->GetObjectName());
    nglString index;
    index.SetCInt(i);
    nuiLabel* pLabel = new nuiLabel(name);
    pHolder->SetProperty("Layers", name);
    pHolder->SetProperty("Index", index);
    pHolder->SetToken(new nuiToken<nuiRef<nuiLayer>>(pLayer));
    pLabel->SetTextPosition(nuiLeft);
    pHolder->AddChild(pLabel);

    nglString sortlabel;
    if (sort_function == 1)
    {
      sortlabel.SetCDouble(stats[pLayer].mTime);
    }
    else if (sort_function == 2)
    {
      sortlabel.SetCUInt(stats[pLayer].mCount);
    }
    else if (sort_function == 3)
    {
      int64 v = MAX(1, stats[pLayer].mCount);
      sortlabel.SetCDouble(stats[pLayer].mTime / (double)v);
    }
    
    nuiLabel* pSortLabel = new nuiLabel(sortlabel);
    pSortLabel->SetPosition(nuiRight);
    pSortLabel->SetFont(nuiFont::GetFont(9));
    pHolder->AddChild(pSortLabel);
    
    pHolder->SetPosition(nuiFill);
    pHolder->SetFillRule(nuiFill);
    
    mpLayerList->AddChild(pHolder);
    
    i++;
  }
  
}

void nuiLayersInspector::OnUpdateLayers(const nuiEvent& rEvent)
{
  UpdateLayers();
}

void nuiLayersInspector::OnLayerSelection(const nuiEvent& rEvent)
{
  nuiWidget* pW = mpLayerList->GetSelected();
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

  if (!pLayer)
    return;
  
  // build attributes list
  std::map<nglString, nuiAttribBase> attributes;
  pLayer->GetAttributes(attributes);
  uint32 i = 0;
  std::map<nglString, nuiAttribBase>::const_iterator it_a = attributes.begin();
  std::map<nglString, nuiAttribBase>::const_iterator end_a = attributes.end();
  
  uint32 rows = mpAttributeGrid->GetNbRows();
  if (rows)
    mpAttributeGrid->RemoveRows(0, rows);
  
  rows = attributes.size() + 1;
  if (rows)
  {
    mpAttributeGrid->AddRows(0, rows);
  }
  
  {
    nuiImage* pImage = new nuiImage();
    pImage->SetPosition(nuiCenter);
    mpAttributeGrid->SetCell(0, i, new nuiLabel("Contents"));
    mpAttributeGrid->SetCell(1, i, pImage);
    pImage->SetTexture(pLayer->GetTexture());
    i++;
  }
  
  
  while (it_a != end_a)
  {
    nglString pname(it_a->first);
    //printf("\tattr: %s\n", pname.GetChars());
    nuiAttribBase Base = it_a->second;
    nuiAttributeEditor* pEditor = Base.GetEditor();
    mpAttributeGrid->SetCell(0, i, new nuiLabel(pname));
    mpAttributeGrid->SetCell(1, i, pEditor);

    ++it_a;
    i++;
  }
  

}

