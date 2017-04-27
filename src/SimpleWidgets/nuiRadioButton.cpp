/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

nuiRadioButton::nuiRadioButton()
  : nuiToggleButton(), mRadioEventSink(this)
{
  SetObjectClass("nuiRadioButton");
  SetGroup("Default");
  mpGroupManager = NULL;
  mCanToggle = false;
}

nuiRadioButton::nuiRadioButton(const nglString& rText)
  : nuiToggleButton(rText), mRadioEventSink(this)
{
  SetObjectClass("nuiRadioButton");
  SetGroup("Default");
  mpGroupManager = NULL;
  mCanToggle = false;
}

nuiRadioButton::nuiRadioButton(nuiStateDecoration* pDecoration)
  : nuiToggleButton(pDecoration), mRadioEventSink(this)
{
  SetObjectClass("nuiRadioButton");
  SetGroup("Default");
  mpGroupManager = NULL;
  mCanToggle = false;
}

nuiRadioButton::nuiRadioButton(const nglImage& rImage)
  : nuiToggleButton(rImage), mRadioEventSink(this)
{
  SetObjectClass("nuiRadioButton");
  SetGroup("Default");
  mpGroupManager = NULL;
  mCanToggle = false;
}

nuiRadioButton::~nuiRadioButton()
{
  if (mpGroupManager)
    mpGroupManager->RemoveRadioButton(this);
}

// Keyboard events:
bool nuiRadioButton::KeyDown(const nglKeyEvent& rEvent)
{
  if (rEvent.mKey == NK_ENTER || rEvent.mKey == NK_PAD_ENTER || rEvent.mKey == NK_SPACE)
  {
    mWasPressed = IsPressed(); 
    if (mCanToggle)
      SetPressed(!mWasPressed);
    else
      SetPressed(true);

    if (IsPressed() && !mWasPressed)
    {
      if (mpParent && !mpGroupManager)
      {
        nuiWidget::IteratorPtr pIt = mpParent->GetFirstChild();
        while (pIt->IsValid())
        {
          nuiRadioButton* pRadio = dynamic_cast<nuiRadioButton*>(pIt->GetWidget());
          if (pRadio && (pRadio != this) && pRadio->GetGroup() == GetGroup())
          {
            bool wasPressed = pRadio->IsPressed();
            pRadio->SetPressed(false);
            if (wasPressed)
            {
              pRadio->Deactivated();
            }
          }
          mpParent->GetNextChild(pIt);
        }
        delete pIt;
      }
      Activated();
    }
    else if (!IsPressed() && mWasPressed)
    {
      Deactivated();
    }
    
    return true;
  }
  
  return false;
}

bool nuiRadioButton::KeyUp(const nglKeyEvent& rEvent)
{
  if (rEvent.mKey == NK_ENTER || rEvent.mKey == NK_PAD_ENTER || rEvent.mKey == NK_SPACE)
    return true;
  
  return false;
}


bool nuiRadioButton::MouseClicked  (const nglMouseInfo& rInfo)
{
  if (IsDisabled())
    return false;

  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mClicked = true;
    mWasPressed = IsPressed();
    if (mCanToggle)
      SetPressed(!mWasPressed);
    else
      SetPressed(true);
    Invalidate();

    if (mDelayedActivation > 0)
    {
      mLastTime = nglTime();
      mUntilRepeat = mDelayedActivation;

      if (!mpAutoRepeatTimer)
      {
        mpAutoRepeatTimer = nuiAnimation::AcquireTimer();
        mRadioEventSink.Connect(mpAutoRepeatTimer->Tick, &nuiRadioButton::OnDelayedActivation);
      }
    }

    return true;
  }
  return false;
}            

bool nuiRadioButton::MouseUnclicked  (const nglMouseInfo& rInfo)
{
  if ( (rInfo.Buttons & nglMouseInfo::ButtonLeft) && mClicked)
  {
    mClicked = false;
    if (IsInsideFromSelf(rInfo.X, rInfo.Y, GetActivationOffset()))
    {
      if (mCanToggle)
        SetPressed(!mWasPressed);
      else
        SetPressed(true);

      if (IsPressed() && !mWasPressed)
      {
        if (mpParent && !mpGroupManager)
        {
          nuiWidget::IteratorPtr pIt = mpParent->GetFirstChild();
          while (pIt->IsValid())
          {
            nuiRadioButton* pRadio = dynamic_cast<nuiRadioButton*>(pIt->GetWidget());
            if (pRadio && (pRadio != this) && pRadio->GetGroup() == GetGroup())
            {
              bool wasPressed = pRadio->IsPressed();
              pRadio->SetPressed(false);
              if (wasPressed) {
                pRadio->Deactivated();
              }
            }
            mpParent->GetNextChild(pIt);
          }
          delete pIt;
        }
        Activated();
      }
      else if (!IsPressed() && mWasPressed)
      {
        Deactivated();
      }
    }
    else
    {
      SetPressed(mWasPressed);
    }

    Invalidate();
    return true;
  }
  return false;
}

bool nuiRadioButton::MouseMoved(const nglMouseInfo& rInfo)
{
  if (IsDisabled())
    return false;
  
  if (mClicked)
  {
    if (IsInsideFromSelf(rInfo.X, rInfo.Y, GetActivationOffset()))
    {
      if (mCanToggle)
        SetPressed(!mWasPressed);
      else
        SetPressed(true);
    }
    else
    {
      SetPressed(mWasPressed);
    }
    return true;
  }
  return false;
}

bool nuiRadioButton::MouseUngrabbed(nglTouchId Id)
{
  nuiWidget::MouseUngrabbed(Id);
  if (mClicked)
  {
    mClicked = false;
    SetPressed(mWasPressed);
  }
  if (mpAutoRepeatTimer)
  {
    if (mDelayedActivation > 0)
      mRadioEventSink.Disconnect(mpAutoRepeatTimer->Tick, &nuiRadioButton::OnDelayedActivation);
    mpAutoRepeatTimer = NULL;
    nuiAnimation::ReleaseTimer();
  }
  return false;
}

void nuiRadioButton::OnDelayedActivation(const nuiEvent& rEvent)
{
  NGL_ASSERT(mDelayedActivation > 0);
  if (mUntilRepeat >= 0)
  {
    nglTime now;
    mUntilRepeat -= (now.GetValue() - mLastTime.GetValue());
    mLastTime = now;
    if (mUntilRepeat < 0)
    {
      GetTopLevel()->CancelGrab();
      DelayedActivation();
    }
  }
}

void nuiRadioButton::SetGroup(const nglString& rGroupName)
{
  SetProperty("Group", rGroupName);
}

void nuiRadioButton::SetGroupManager(nuiRadioButtonGroup* pGroup)
{
  mpGroupManager = pGroup;
}

nuiRadioButtonGroup* nuiRadioButton::GetGroupManager()
{
  return mpGroupManager;
}

nglString nuiRadioButton::GetGroup() const
{
  return GetProperty("Group");
}


bool nuiRadioButton::SetParent(nuiWidgetPtr pParent)
{
  bool res = nuiToggleButton::SetParent(pParent);

  if (mpParent && mPressed)
  {
    nuiWidget::IteratorPtr pIt = mpParent->GetFirstChild();
    while (pIt->IsValid())
    {
      nuiRadioButton* pRadio = dynamic_cast<nuiRadioButton*>(pIt->GetWidget());
      if (pRadio && (pRadio != this) && pRadio->GetGroup() == GetGroup())
        pRadio->SetPressed(false);

      mpParent->GetNextChild(pIt);
    }
    delete pIt;
  }

  return res;
}

void nuiRadioButton::EnableToggleable(bool set)
{
  mCanToggle = set;
}

bool nuiRadioButton::IsToggleable() const
{
  return mCanToggle;
}


