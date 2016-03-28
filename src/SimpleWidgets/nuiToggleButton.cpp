/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

nuiSize nuiToggleButton::mDefaultCheckSize = 14;

nuiToggleButton::nuiToggleButton()
: nuiButton()
{
  if (SetObjectClass("nuiToggleButton"))
  {
    InitAttributes();
  }
  mDisplayAsCheckBox = false;
  mDisplayAsFrameBox = false;
  mCheckSize = mDefaultCheckSize;
  mHasContents = false;
}

nuiToggleButton::nuiToggleButton(const nglString& rText)
: nuiButton(rText)
{
  if (SetObjectClass("nuiToggleButton"))
  {
    InitAttributes();
  }
  mDisplayAsCheckBox = false;
  mDisplayAsFrameBox = false;
  mCheckSize = mDefaultCheckSize;
  mHasContents = true;
}

nuiToggleButton::nuiToggleButton(nuiStateDecoration* pDecoration)
: nuiButton(pDecoration)
{
  if (SetObjectClass("nuiToggleButton"))
  {
    InitAttributes();
  }
  mDisplayAsCheckBox = false;
  mDisplayAsFrameBox = false;
  mCheckSize = mDefaultCheckSize;
  mHasContents = false;
}

nuiToggleButton::nuiToggleButton(const nglImage& rImage)
: nuiButton(rImage)
{
  if (SetObjectClass("nuiToggleButton"))
  {
    InitAttributes();
  }
  mDisplayAsCheckBox = false;
  mDisplayAsFrameBox = false;
  mCheckSize = mDefaultCheckSize;
  mHasContents = true;
}

nuiToggleButton::~nuiToggleButton()
{
}

void nuiToggleButton::InitAttributes()
{
  AddAttribute(new nuiAttribute<bool>
               (nglString("CheckBox"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiToggleButton::GetDisplayAsCheckBox),
                nuiMakeDelegate(this, &nuiToggleButton::SetDisplayAsCheckBox)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("FrameBox"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiToggleButton::GetDisplayAsFrameBox),
                nuiMakeDelegate(this, &nuiToggleButton::SetDisplayAsFrameBox)));
}


nuiSize nuiToggleButton::GetDefaultCheckSize()
{
  return mDefaultCheckSize;
}

void nuiToggleButton::SetDefaultCheckSize(nuiSize CheckSize)
{
  mDefaultCheckSize = CheckSize;
}




// Rendering:
bool nuiToggleButton::Draw(nuiDrawContext* pContext)
{
  //NGL_OUT("0x%x nuitoggleButton::Draw(%d)\n", this, GetState(false));

  nuiDecoration* pDeco = GetDecoration();

  if (mDisplayAsFrameBox)
  {
    pContext->ResetState();

    if (!mPressed)
    {
      if (HasProperty("NormalBg"))
        pContext->SetStrokeColor(nuiColor(GetProperty("NormalBg")));
      else
        pContext->SetStrokeColor("nuiNormalButtonBg");
    }
    else
    {
      if (HasProperty("SelectedBg"))
        pContext->SetStrokeColor(nuiColor(GetProperty("SelectedBg")));
      else
        pContext->SetStrokeColor("nuiSelectedButtonBg");
    }

    nuiRect r(mRect.Size());
    pContext->DrawRect(r, eStrokeShape);
        
    DrawChildren(pContext);
  }
  else
  {
    nuiButton::Draw(pContext);
  }
  return true;
}


// Keyboard events:
bool nuiToggleButton::KeyDown(const nglKeyEvent& rEvent)
{
  if (rEvent.mKey == NK_ENTER || rEvent.mKey == NK_PAD_ENTER || rEvent.mKey == NK_SPACE)
  {
    mWasPressed = IsPressed();
    SetPressed(!mWasPressed);
    if (IsPressed())
      Activated();
    else
      Deactivated();
    return true;
  }
  
  return false;
}

bool nuiToggleButton::KeyUp(const nglKeyEvent& rEvent)
{
  if (rEvent.mKey == NK_ENTER || rEvent.mKey == NK_PAD_ENTER || rEvent.mKey == NK_SPACE)
    return true;
  
  return false;
}


// Received Mouse events:
bool nuiToggleButton::MouseClicked  (const nglMouseInfo& rInfo)
{
  if (IsDisabled())
    return false;

  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mClicked = true;
    mWasPressed = IsPressed();
    SetPressed(!mWasPressed);
    Invalidate();
    return true;
  }
  return false;
}            

bool nuiToggleButton::MouseUnclicked  (const nglMouseInfo& rInfo)
{
  if ( (rInfo.Buttons & nglMouseInfo::ButtonLeft) && mClicked)
  {
    mClicked = false;

    if (IsInsideFromSelf(rInfo.X, rInfo.Y, GetActivationOffset()))
    {
      SetPressed(!mWasPressed);
      if (IsPressed())
				Activated();
			else
				Deactivated();
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

bool nuiToggleButton::MouseMoved(const nglMouseInfo& rInfo)
{
  if (IsDisabled())
    return false;
  
  if (mClicked)
  {
    if (IsInsideFromSelf(rInfo.X, rInfo.Y, GetActivationOffset()))
    {
      SetPressed(!mWasPressed);
    }
    else
    {
      SetPressed(mWasPressed);
    }
    return true;
  }
  return false;
}

bool nuiToggleButton::MouseUngrabbed(nglTouchId Id)
{
  nuiWidget::MouseUngrabbed(Id);
  if (mClicked)
  {
    mClicked = false;
    SetPressed(mWasPressed);
  }
  return false;
}

void nuiToggleButton::SetDisplayAsCheckBox(bool set) 
{ 
  if (set)
  {
    mDisplayAsFrameBox = false;
  }
  
  mDisplayAsCheckBox = set;
  IteratorPtr pIt = GetFirstChild();
  nuiLabel* pItem = dynamic_cast<nuiLabel*>(pIt->GetWidget());
  
  if (pItem)
  {
    if (set)
    {
      pItem->SetPosition(nuiLeft);
    }
    else
    {
      pItem->SetPosition(nuiCenter);
    }
  }
  
  delete pIt;

  InvalidateLayout(); 
}

void nuiToggleButton::SetDisplayAsFrameBox(bool set) 
{ 
  if (set)
  {
    mDisplayAsCheckBox = false;
  }

  mDisplayAsFrameBox = set;
  
  IteratorPtr pIt = GetFirstChild();
  nuiLabel* pItem = dynamic_cast<nuiLabel*>(pIt->GetWidget());
  
  if (pItem)
  {
    if (mDisplayAsCheckBox)
    {
      pItem->SetPosition(nuiLeft);
    }
    else
    {
      pItem->SetPosition(nuiCenter);
    }
  }
  
  delete pIt;
  
  InvalidateLayout(); 
}

nuiRect nuiToggleButton::CalcIdealSize()
{
  if (mDisplayAsFrameBox)
  {
    nuiRect r(nuiWidget::CalcIdealSize());
    //r.Grow(2,2);
    return r.Size();
  }
  return nuiButton::CalcIdealSize();
}

bool nuiToggleButton::SetRect(const nuiRect& rRect)
{
  if (mDisplayAsFrameBox)
  {
    nuiWidget::SetSelfRect(rRect);
    nuiRect rect(rRect.Size());
    //rect.Grow(-2,-2);

    IteratorPtr pIt;
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (mCanRespectConstraint)
        pItem->SetLayoutConstraint(mConstraint);
      pItem->GetIdealRect();
      pItem->SetLayout(rect);
    }
    delete pIt;
    return true;
  }
  return nuiButton::SetRect(rRect);
}


bool nuiToggleButton::HasContents() const
{
  return mHasContents;
}

bool nuiToggleButton::GetDisplayAsCheckBox() const
{
  return mDisplayAsCheckBox;
}

bool nuiToggleButton::GetDisplayAsFrameBox() const
{
  return mDisplayAsFrameBox;
}

