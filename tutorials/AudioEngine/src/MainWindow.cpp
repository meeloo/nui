/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "MainWindow.h"
#include "Application.h"
#include "nuiCSS.h"
#include "nuiGrid.h"

#include "nuiAudioDevice.h"
#include "nuiSynthSound.h"

#define GRID_NB_ROWS 4
#define GRID_NB_COLUMNS 4

/*
 * MainWindow
 */

MainWindow::MainWindow(const nglContextInfo& rContextInfo, const nglWindowInfo& rInfo, bool ShowFPS, const nglContext* pShared )
: nuiMainWindow(rContextInfo, rInfo, pShared, nglPath(ePathCurrent)), mEventSink(this)
{
#ifdef _DEBUG_
  SetDebugMode(true);
#endif
  
#ifdef NUI_IPHONE
  LoadCSS("rsrc:/css/style-iPhone.css");
#else
  LoadCSS("rsrc:/css/style.css");
#endif

  mpLoopVoice = NULL;
}

MainWindow::~MainWindow()
{
  if (mpLoopVoice)
    mpLoopVoice->Release();
  
  for (uint32 i = 0; i < mSounds.size(); i++)
    mSounds[i]->Release();
  
  for (uint32 i = 0; i < mSynthSounds.size(); i++)
    mSynthSounds[i]->Release();
}

void MainWindow::OnCreation()
{  
  mpLoopVoice = GetAudioEngine()->PlaySound(nglPath("rsrc:/audio/drums-loop.wav"), nuiSound::eStream);
  mpLoopVoice->Acquire();
  mpLoopVoice->SetLoop(true);
  
  
  nuiGrid* pMainGrid = new nuiGrid(2, 2);
  AddChild(pMainGrid);
  pMainGrid->SetObjectName("MainGrid");
  
  
  uint32 nbRows = GRID_NB_ROWS;
  uint32 nbCol = GRID_NB_COLUMNS;
  
  
  
  {
    nuiLabel* pLabel = new nuiLabel("Memory Sounds");
    pLabel->SetObjectName("MemoryLabel");
    pMainGrid->SetCell(0, 0, pLabel);
    
    for (uint32 i = 0; i < GRID_NB_ROWS * GRID_NB_COLUMNS; i++)
    {
      nglString str;
      str.Format("rsrc:/audio/sound%d.wav", i);
      nglPath path(str);
      
      nuiSound* pSound = nuiSoundManager::Instance.GetSound(path, nuiSound::eMemory);
      pSound->Acquire();
      mSounds.push_back(pSound);
    }
    
    
    std::vector<std::pair<nglKeyCode, nglString> > keys;
    keys.push_back(std::make_pair(NK_1, "1"));
    keys.push_back(std::make_pair(NK_2, "2"));
    keys.push_back(std::make_pair(NK_3, "3"));
    keys.push_back(std::make_pair(NK_4, "4"));
    
    keys.push_back(std::make_pair(NK_Q, "q"));
    keys.push_back(std::make_pair(NK_W, "w"));
    keys.push_back(std::make_pair(NK_E, "e"));
    keys.push_back(std::make_pair(NK_R, "r"));
    
    keys.push_back(std::make_pair(NK_A, "a"));
    keys.push_back(std::make_pair(NK_S, "s"));
    keys.push_back(std::make_pair(NK_D, "d"));
    keys.push_back(std::make_pair(NK_F, "f"));
    
    keys.push_back(std::make_pair(NK_Z, "z"));
    keys.push_back(std::make_pair(NK_X, "x"));
    keys.push_back(std::make_pair(NK_C, "c"));
    keys.push_back(std::make_pair(NK_V, "v"));
    
    
    nuiGrid* pGrid = new nuiGrid(nbCol, nbRows);
    pGrid->SetObjectName("MemorySoundsGrid");
    pMainGrid->SetCell(0, 1, pGrid);
    
    NGL_ASSERT(keys.size() == nbRows * nbCol);
    
    for (uint32 r = 0; r < nbRows; r++)
    {
      for (uint32 c = 0; c < nbCol; c++)
      {
        uint32 index = r * nbCol + c;
        nglString text = keys[index].second;
        nuiButton* pButton = new nuiButton(text);
        mSoundButtons.push_back(pButton);
        pButton->SetObjectName("GridButton");
        pGrid->SetCell(c, r, pButton);
        mEventSink.Connect(pButton->Activated, &MainWindow::OnButtonActivated, (void*)index);
        
        nglString hotkeyName;
        nglString hotkeyDesc;
        hotkeyName.Format("SoundButton%d", index);
        hotkeyDesc.Format("PlaySound%d", index);
        RegisterHotKeyKey(hotkeyName, keys[index].first, nuiNoKey, false, false, hotkeyDesc);
        mEventSink.Connect(GetHotKeyEvent(hotkeyName), &MainWindow::OnSoundHotKey, (void*)index);
      }
    }
  }
  
  {    
    nuiLabel* pLabel = new nuiLabel("Synth Sounds");
    pLabel->SetObjectName("SynthLabel");
    pMainGrid->SetCell(1, 0, pLabel);
    
    for (uint32 r = 0; r < nbRows; r++)
    {
      for (uint32 c = 0; c < nbCol; c++)
      {
        nuiSynthSound* pSynthSound = nuiSoundManager::Instance.GetSynthSound();
        pSynthSound->SetSampleRate(GetAudioEngine()->GetSampleRate());
        pSynthSound->SetReleaseTime(3);
        
        float freq = 100;
        if (c == 0)
          freq = 82.41;
        else if (c == 1)
          freq = 123.47;
        else if (c == 2)
          freq = 146.83;
        else if (c == 3)
          freq = 164.81;
        
        nuiSynthSound::SignalType type = nuiSynthSound::eSinus;
        float gain;
        if (r == 0)
        {
          gain = 0.8;
          type = nuiSynthSound::eSinus;
        }
        else if (r == 1)
        {
          gain = 0.4;
          type = nuiSynthSound::eTriangle;
        }
        else if (r == 2)
        {
          gain = 0.4;
          type = nuiSynthSound::eSaw;
        }
        else if (r == 3)
        {
          gain = 0.2;
          type = nuiSynthSound::eSquare;
        }

        
        pSynthSound->SetGain(gain);
        pSynthSound->SetFreq(freq);
        pSynthSound->SetType(type);
        pSynthSound->Acquire();
        mSynthSounds.push_back(pSynthSound);
      }
    }
    
    
    std::vector<std::pair<nglKeyCode, nglString> > keys;
    keys.push_back(std::make_pair(NK_7, "7"));
    keys.push_back(std::make_pair(NK_8, "8"));
    keys.push_back(std::make_pair(NK_9, "9"));
    keys.push_back(std::make_pair(NK_0, "0"));
    
    keys.push_back(std::make_pair(NK_U, "u"));
    keys.push_back(std::make_pair(NK_I, "i"));
    keys.push_back(std::make_pair(NK_O, "o"));
    keys.push_back(std::make_pair(NK_P, "p"));
    
    keys.push_back(std::make_pair(NK_J, "j"));
    keys.push_back(std::make_pair(NK_K, "k"));
    keys.push_back(std::make_pair(NK_L, "l"));
    keys.push_back(std::make_pair(NK_SEMICOLON, ";"));
    
    keys.push_back(std::make_pair(NK_M, "m"));
    keys.push_back(std::make_pair(NK_COMMA, ","));
    keys.push_back(std::make_pair(NK_PERIOD, "."));
    keys.push_back(std::make_pair(NK_SLASH, "/"));
    
    uint32 nbRows = GRID_NB_ROWS;
    uint32 nbCol = GRID_NB_COLUMNS;
    nuiGrid* pGrid = new nuiGrid(nbCol, nbRows);
    pGrid->SetObjectName("SynthSoundsGrid");
    pMainGrid->SetCell(1, 1, pGrid);
    
    NGL_ASSERT(keys.size() == nbRows * nbCol);
    
    for (uint32 r = 0; r < nbRows; r++)
    {
      for (uint32 c = 0; c < nbCol; c++)
      {
        uint32 index = r * nbCol + c;
        nglString text = keys[index].second;
        nuiButton* pButton = new nuiButton(text);
        mSynthSoundButtons.push_back(pButton);
        pButton->SetObjectName("GridButton");
        pGrid->SetCell(c, r, pButton);
        mEventSink.Connect(pButton->Activated, &MainWindow::OnSynthButtonActivated, (void*)index);
        
        nglString hotkeyName;
        nglString hotkeyDesc;
        hotkeyName.Format("SynthSoundButton%d", index);
        hotkeyDesc.Format("PlaySynthSound%d", index);
        RegisterHotKeyKey(hotkeyName, keys[index].first, nuiNoKey, false, false, hotkeyDesc);
        mEventSink.Connect(GetHotKeyEvent(hotkeyName), &MainWindow::OnSynthSoundHotKey, (void*)index);
      }
    }
  }
  
  
  
}

void MainWindow::OnButtonActivated(const nuiEvent& rEvent)
{
  uint64 index = (uint64)rEvent.mpUser;
  GetAudioEngine()->PlaySound(mSounds[index]);
}

void MainWindow::OnSoundHotKey(const nuiEvent& rEvent)
{
  uint64 i = (uint64)rEvent.mpUser;
  mSoundButtons[i]->Activate();
}

void MainWindow::OnSynthButtonActivated(const nuiEvent& rEvent)
{
  uint64 index = (uint64)rEvent.mpUser;
  GetAudioEngine()->PlaySound(mSynthSounds[index]);
}

void MainWindow::OnSynthSoundHotKey(const nuiEvent& rEvent)
{
  uint64 i = (uint64)rEvent.mpUser;
  mSynthSoundButtons[i]->Activate();
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
