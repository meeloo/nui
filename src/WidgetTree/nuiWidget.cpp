/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/



#include "nui.h"

bool nuiWidget::mGlobalUseRenderCache = true;

void nuiWidget::SetGlobalUseRenderCache(bool set)
{
  mGlobalUseRenderCache = set;
}

bool nuiWidget::GetGlobalUseRenderCache()
{
  return mGlobalUseRenderCache;
}

//#define NUI_LOG_GETIDEALRECT

// Use like this:  nuiAnimation::RunOnAnimationTick(nuiMakeTask(nuiDelayedPlayAnim, eAnimFromStart, Time, count, loopmode));
void nuiDelayedPlayAnim(nuiAnimation* pAnim, nuiAnimWhence Whence, double Time, int32 count, nuiAnimLoop loopmode)
{
  pAnim->Play(count, loopmode);
  pAnim->SetTime(Time, Whence);
}


#ifdef NUI_WIDGET_STATS
static uint wcount = 0;
static uint maxwcount = 0;

class nuiWidgetCounter
{
public:
  nuiWidgetCounter()
  {
  }
  
  ~nuiWidgetCounter()
  {
    Dump();
  }
  
  void Dump()
  {
    std::map<nglString, uint>::iterator it = mCounters.begin(); 
    std::map<nglString, uint>::iterator end = mCounters.end(); 
    
    while (it != end)
    {
      NGL_OUT("%s: %d\n", it->first.GetChars(), it->second);
      ++it;
    }
  }

  void Add(const nglString& rString)
  {
    std::map<nglString, uint>::iterator it = mCounters.find(rString); 
    if (it == mCounters.end())
      mCounters.insert(std::pair<nglString, uint>(rString, 1));
    else
      it->second++;
  }
  
private:
  std::map<nglString, uint> mCounters;
};

static nuiWidgetCounter gWidgetCounter;
#endif

nuiWidget::nuiWidget()
: nuiObject(), mGenericWidgetSink(this)
{
  InitDefaultValues();
  
  if (SetObjectClass("nuiWidget"))
    InitAttributes();
    
#ifdef NUI_WIDGET_STATS
  wcount++;
  maxwcount = MAX(wcount, maxwcount);
  NGL_OUT("max widgets: %d (total %d)\n", maxwcount, wcount);
#endif
  
  Init();

  //InitDefaultDecorations();
}



nuiWidget::nuiWidget(const nglString& rObjectName)
: nuiObject(rObjectName), mGenericWidgetSink(this)
{
  InitDefaultValues();
  
  if (SetObjectClass("nuiWidget"))
    InitAttributes();
  
#ifdef NUI_WIDGET_STATS
  wcount++;
  maxwcount = MAX(wcount, maxwcount);
  NGL_OUT("max widgets: %d (total %d)\n", maxwcount, wcount);
#endif
  
  Init();
  
  //InitDefaultDecorations();
}


void nuiWidget::InitDefaultValues()
{
  mODLeft = 0;
  mODRight = 0;
  mODTop = 0;
  mODBottom = 0;
  mInteractiveOD = false;
  mInteractiveDecoration = false;
  mpDecoration = NULL;
  mpFocusDecoration = NULL;
  mShowFocus = true;
  mPosition = nuiFill;
  mFillRule = nuiFill;
  mCSSPasses = 0;
  mpMatrixNodes = NULL;
  mpParent = NULL;
  mpTheme = NULL;
  mDecorationEnabled = true;
  mNeedSurfaceRedraw = false;
  mSurfaceEnabled = false;
  mpSurface = NULL;
  mSurfaceColor = nuiColor(255, 255, 255, 255);
  mSurfaceBlendFunc = nuiBlendTransp;  
  mDecorationMode = eDecorationOverdraw;
  mHotKeyMask = -1;
  mClickThru = true;
  mInSetRect = false;
  mInTransition = 0;
  mpLayoutAnimation = NULL;
  mFixedAspectRatio = false;
  mAutoClip = true;
  mAutoDraw = false;
  mReverseRender = false;
  mOverrideVisibleRect = false;
  mAutoUpdateLayout = false;
  mAutoAcceptMouseCancel = true;
  mAutoAcceptMouseSteal = true;
}




bool nuiWidget::AttrIsEnabled()
{
	return IsEnabled();
}



bool nuiWidget::AttrIsVisible()
{
	return IsVisible();
}


void nuiWidget::InitAttributes()
{
  AddAttribute(new nuiAttribute<bool>
    (nglString("Enabled"), nuiUnitBoolean,
     nuiMakeDelegate(this, &nuiWidget::AttrIsEnabled),
     nuiMakeDelegate(this, &nuiWidget::SetEnabled)));
  
  AddAttribute(new nuiAttribute<bool>
    (nglString("Visible"), nuiUnitBoolean,
     nuiMakeDelegate(this, &nuiWidget::AttrIsVisible),
     nuiMakeDelegate(this, &nuiWidget::SetVisible)));

  AddAttribute(new nuiAttribute<bool>
    (nglString("Selected"), nuiUnitBoolean,
     nuiMakeDelegate(this, &nuiWidget::AttrIsSelected),
     nuiMakeDelegate(this, &nuiWidget::SetSelected)));

  
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("LayoutRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetRect),
                nuiMakeDelegate(this, &nuiWidget::SetLayout)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("LayoutRectUnsafe"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetRect),
                nuiAttribute<const nuiRect&>::SetterDelegate(this, &nuiWidget::InternalSetLayout)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("UserRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetUserRect),
                nuiMakeDelegate(this, &nuiWidget::SetUserRect)));
  
  AddAttribute(new nuiAttribute<nuiPoint>
               (nglString("UserPos"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetUserPos),
                nuiAttribute<nuiPoint>::SetterDelegate(this, &nuiWidget::SetUserPos)));
  
  AddAttribute(new nuiAttribute<const nuiRect&>
               (nglString("VisibleRect"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetVisibleRect),
                nuiMakeDelegate(this, &nuiWidget::SetVisibleRect)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("UserWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetUserWidth),
                nuiMakeDelegate(this, &nuiWidget::SetUserWidth)));

  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("UserHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetUserHeight),
                nuiMakeDelegate(this, &nuiWidget::SetUserHeight)));
  
  
  
  
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinIdealWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMinIdealWidth),
                nuiMakeDelegate(this, &nuiWidget::SetMinIdealWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinIdealHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMinIdealHeight),
                nuiMakeDelegate(this, &nuiWidget::SetMinIdealHeight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxIdealWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMaxIdealWidth),
                nuiMakeDelegate(this, &nuiWidget::SetMaxIdealWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxIdealHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMaxIdealHeight),
                nuiMakeDelegate(this, &nuiWidget::SetMaxIdealHeight)));
  

  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMinWidth),
                nuiMakeDelegate(this, &nuiWidget::SetMinWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MinHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMinHeight),
                nuiMakeDelegate(this, &nuiWidget::SetMinHeight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxWidth"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMaxWidth),
                nuiMakeDelegate(this, &nuiWidget::SetMaxWidth)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("MaxHeight"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMaxHeight),
                nuiMakeDelegate(this, &nuiWidget::SetMaxHeight)));
  
  
  
  
  
  
  
  AddAttribute(new nuiAttribute<bool>
              (nglString("UseRenderCache"), nuiUnitBoolean,
               nuiMakeDelegate(this, &nuiWidget::IsRenderCacheEnabled),
               nuiMakeDelegate(this, &nuiWidget::EnableRenderCache)));


  AddAttribute(new nuiAttribute<bool>
               (nglString("RedrawOnHover"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::GetRedrawOnHover),
                nuiMakeDelegate(this, &nuiWidget::SetRedrawOnHover)));
	
  AddAttribute(new nuiAttribute<bool>
               (nglString("AutoClip"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::GetAutoClip),
                nuiMakeDelegate(this, &nuiWidget::SetAutoClip)));
	
  AddAttribute(new nuiAttribute<bool>
               (nglString("Hover"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::GetHover)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("Focus"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::HasFocus)));
  
	// Decoration:
  nuiAttribute<const nglString&>* pDecoAttrib = new nuiAttribute<const nglString&>
    (nglString("Decoration"), nuiUnitNone,
     nuiMakeDelegate(this, &nuiWidget::GetDecorationName),
     nuiAttribute<const nglString&>::SetterDelegate(this, &nuiWidget::SetDecoration));
  pDecoAttrib->SetEditor(nuiAttribute<const nglString&>::NewEditorDelegate(&nuiDecoration::GetAttributeEditor));
  AddAttribute(pDecoAttrib);
  
  AddAttribute(new nuiAttribute<nuiDecorationMode>
               (nglString("DecorationMode"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetDecorationMode),
                nuiMakeDelegate(this, &nuiWidget::SetDecorationMode)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("EnableDecoration"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::IsDecorationEnabled),
                nuiMakeDelegate(this, &nuiWidget::EnableDecoration)));


  // Focus Decoration:
  nuiAttribute<const nglString&>* pFocusDecoAttrib = new nuiAttribute<const nglString&>
  (nglString("FocusDecoration"), nuiUnitName,
   nuiMakeDelegate(this, &nuiWidget::GetFocusDecorationName),
   nuiAttribute<const nglString&>::SetterDelegate(this, &nuiWidget::SetFocusDecoration));
  pFocusDecoAttrib->SetEditor(nuiAttribute<const nglString&>::NewEditorDelegate(&nuiDecoration::GetAttributeEditor));
  AddAttribute(pFocusDecoAttrib);


  
  AddAttribute(new nuiAttribute<nuiDecorationMode>
               (nglString("FocusDecorationMode"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetFocusDecorationMode),
                nuiMakeDelegate(this, &nuiWidget::SetFocusDecorationMode)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("FocusVisible"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::IsFocusVisible),
                nuiMakeDelegate(this, &nuiWidget::SetFocusVisible)));
  
  
  // nuiAttribute<nuiSize> <=> nuiAttribute<double>
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("Borders"), nuiUnitSize,
                //nuiMakeDelegate(this, &nuiWidget::GetBorderLeft),
                nuiMakeDelegate(this, &nuiWidget::SetBorders)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderLeft"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetBorderLeft),
                nuiMakeDelegate(this, &nuiWidget::SetBorderLeft)));

  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderTop"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetBorderTop),
                nuiMakeDelegate(this, &nuiWidget::SetBorderTop)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderRight"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetBorderRight),
                nuiMakeDelegate(this, &nuiWidget::SetBorderRight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("BorderBottom"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetBorderBottom),
                nuiMakeDelegate(this, &nuiWidget::SetBorderBottom)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("OverDrawLeft"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetOverDrawLeft),
                nuiMakeDelegate(this, &nuiWidget::SetOverDrawLeft)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("OverDrawTop"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetOverDrawTop),
                nuiMakeDelegate(this, &nuiWidget::SetOverDrawTop)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("OverDrawRight"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetOverDrawRight),
                nuiMakeDelegate(this, &nuiWidget::SetOverDrawRight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("OverDrawBottom"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetOverDrawBottom),
                nuiMakeDelegate(this, &nuiWidget::SetOverDrawBottom)));
  
  // nuiAttribute<nuiSize> <=> nuiAttribute<double>
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderLeft"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetActualBorderLeft)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderTop"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetActualBorderTop)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderRight"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetActualBorderRight)));
  
  AddAttribute(new nuiAttribute<nuiSize>
               (nglString("ActualBorderBottom"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetActualBorderBottom)));
  
  nuiAttribute<nuiPosition>* AttributePosition = new nuiAttribute<nuiPosition>
              (nglString("Position"), nuiUnitPosition,
               nuiMakeDelegate(this, &nuiWidget::GetPosition), 
               nuiMakeDelegate(this, &nuiWidget::SetPosition));
  AddAttribute("Position", AttributePosition);
  
  nuiAttribute<nuiPosition>* AttributeFillRule = new nuiAttribute<nuiPosition>
              (nglString("FillRule"), nuiUnitPosition,
               nuiMakeDelegate(this, &nuiWidget::GetFillRule), 
               nuiMakeDelegate(this, &nuiWidget::SetFillRule));
  AddAttribute("FillRule", AttributeFillRule);

  AddAttribute(new nuiAttribute<nuiMouseCursor>
               (nglString("Cursor"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::GetMouseCursor),
                nuiMakeDelegate(this, &nuiWidget::SetAttrMouseCursor)));
  
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("WantKeyBoardFocus"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::GetWantKeyboardFocus),
                nuiMakeDelegate(this, &nuiWidget::SetWantKeyboardFocus)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("EnableSurface"), nuiUnitBoolean,
                nuiMakeDelegate(this, &nuiWidget::IsSurfaceEnabled),
                nuiMakeDelegate(this, &nuiWidget::EnableSurface)));
  
  AddAttribute(new nuiAttribute<const nuiColor&>
               (nglString("SurfaceColor"), nuiUnitColor,
                nuiMakeDelegate(this, &nuiWidget::GetSurfaceColor),
                nuiMakeDelegate(this, &nuiWidget::SetSurfaceColor)));

  AddAttribute(new nuiAttribute<const nuiMatrix&>
               (nglString("SurfaceMatrix"), nuiUnitMatrix,
                nuiMakeDelegate(this, &nuiWidget::GetSurfaceMatrix),
                nuiMakeDelegate(this, &nuiWidget::SetSurfaceMatrix)));
  
  AddAttribute(new nuiAttribute<nuiMatrix>
               (nglString("Matrix"), nuiUnitMatrix,
                nuiMakeDelegate(this, &nuiWidget::_GetMatrix),
                nuiMakeDelegate(this, &nuiWidget::_SetMatrix)));
  
  AddAttribute(new nuiAttribute<nuiBlendFunc>
               (nglString("SurfaceBlendFunc"), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiWidget::GetSurfaceBlendFunc),
                nuiMakeDelegate(this, &nuiWidget::SetSurfaceBlendFunc)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("InteractiveDecoration"), nuiUnitYesNo,
                nuiMakeDelegate(this, &nuiWidget::IsDecorationInteractive),
                nuiMakeDelegate(this, &nuiWidget::EnableInteractiveDecoration)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("InteractiveOverDraw"), nuiUnitYesNo,
                nuiMakeDelegate(this, &nuiWidget::IsOverDrawInteractive),
                nuiMakeDelegate(this, &nuiWidget::EnableInteractiveOverDraw)));
  
  AddAttribute(new nuiAttribute<float>
               (nglString("LayoutAnimationDuration"), nuiUnitSize,
                nuiMakeDelegate(this, &nuiWidget::GetLayoutAnimationDuration),
                nuiMakeDelegate(this, &nuiWidget::SetLayoutAnimationDuration)
               ));

  nuiAttribute<float>* pAlphaAttrib = new nuiAttribute<float>
  (nglString("Alpha"), nuiUnitNone,
   nuiMakeDelegate(this, &nuiWidget::GetAlpha),
   nuiMakeDelegate(this, &nuiWidget::SetAlpha),
   nuiRange(0, 0, 1, .1, .1, 0)
   );
  AddAttribute(pAlphaAttrib);
  
  AddAttribute(new nuiAttribute<int32>
               (nglString("Debug"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiWidget::_GetDebug),
                nuiMakeDelegate(this, &nuiWidget::SetDebug),
                nuiRange(0, 0, 100, 1, 10, 10, 0)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString("FixedAspectRatio"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiWidget::GetFixedAspectRatio), 
                nuiMakeDelegate(this, &nuiWidget::SetFixedAspectRatio)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("ReverseRender"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiWidget::GetReverseRender),
                nuiMakeDelegate(this, &nuiWidget::SetReverseRender)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("AutoUpdateLayout"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiWidget::GetAutoUpdateLayout),
                nuiMakeDelegate(this, &nuiWidget::SetAutoUpdateLayout)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("AutoAcceptMouseCancel"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiWidget::GetAutoAcceptMouseCancel),
                nuiMakeDelegate(this, &nuiWidget::SetAutoAcceptMouseCancel)));

  AddAttribute(new nuiAttribute<bool>
               (nglString("AutoAcceptMouseSteal"), nuiUnitOnOff,
                nuiMakeDelegate(this, &nuiWidget::GetAutoAcceptMouseSteal),
                nuiMakeDelegate(this, &nuiWidget::SetAutoAcceptMouseSteal)));

}

 
void nuiWidget::Init()
{
  mDebugLevel = 0; // No debug by default.
  mCanRespectConstraint = false; ///< By default the widgets don't care about the constraints imposed by their parents. Only few ones care about this.
  mNeedInvalidateOnSetRect = true;
  mDrawingInCache = false;
  mpRenderCache = NULL;
	mUseRenderCache = false;

  mTrashed = false;
  mDoneTrashed = false;
  mRedrawOnHover = false;

  mMixAlpha = true;
  mInheritAlpha = true;

  mAnimateLayout = false;
  mObjCursor = eCursorDoNotSet;
  mAlpha = 1.0f;
  mHasFocus = false;
  mNeedRender = true;
  mNeedSelfLayout = true;
  mNeedLayout = true;
  mNeedIdealRect = true;
  mNeedSelfRedraw = true;
  mStateLocked = false;
  mHover = false;
  mHasUserPos = false;
  mHasUserSize = false;
  mHasUserWidth = false;
  mHasUserHeight = false;
  mForceIdealSize = false;
  mSelectionExclusive = false;

  mMouseEventEnabled = true;

  mBorderRight = mBorderLeft = 0.f;
  mBorderTop = mBorderBottom = 0.f;

  mMinIdealWidth = mMaxIdealWidth = mMinIdealHeight = mMaxIdealHeight = -1.0f;
  mMinWidth = mMaxWidth = mMinHeight = mMaxHeight = -1.0f;

  mpSavedPainter = NULL;

	mEnabled = true;
  mSelected = false;
  mVisible = true;
  
  mWantKeyboardFocus = false;
  mMuteKeyboardFocusDispatch = false;

  LoadIdentityMatrix();
  
  EnableRenderCache(true);
  
  // Events:
  NUI_ADD_EVENT(ChildAdded);
  NUI_ADD_EVENT(ChildDeleted);

  NUI_ADD_EVENT(Trashed);
  NUI_ADD_EVENT(Destroyed);
  NUI_ADD_EVENT(ParentChanged);

  NUI_ADD_EVENT(Selected);
  NUI_ADD_EVENT(Deselected);
  NUI_ADD_EVENT(Disabled);
  NUI_ADD_EVENT(StateChanged);

  NUI_ADD_EVENT(Shown);
  NUI_ADD_EVENT(Hiden);
  NUI_ADD_EVENT(VisibilityChanged);

  NUI_ADD_EVENT(HoverOn);
  NUI_ADD_EVENT(HoverOff);
  NUI_ADD_EVENT(HoverChanged);

  NUI_ADD_EVENT(UserRectChanged);
  NUI_ADD_EVENT(HotRectChanged);

  NUI_ADD_EVENT(Clicked);
  NUI_ADD_EVENT(Unclicked);
  NUI_ADD_EVENT(ClickCanceled);
  NUI_ADD_EVENT(MovedMouse);
  NUI_ADD_EVENT(WheelMovedMouse);

  NUI_ADD_EVENT(FocusChanged);
  
  NUI_ADD_EVENT(PreClicked);
  NUI_ADD_EVENT(PreUnclicked);
  NUI_ADD_EVENT(PreMouseMoved);
  NUI_ADD_EVENT(PreMouseWheelMoved);
}

bool nuiWidget::SetObjectClass(const nglString& rName)
{
  CheckValid();
  
  bool res = nuiObject::SetObjectClass(rName);
  ResetCSSPass();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_ALL);
  return res;
}

// virtual from nuiObject
void nuiWidget::SetObjectName(const nglString& rName)
{
  CheckValid();
  
  nuiObject::SetObjectName(rName);
  ResetCSSPass();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_ALL);
}




nglString nuiWidget::Dump()
{
  return nglString::Empty;
//  CheckValid();
//  nuiXML* xml = new nuiXML(GetObjectName());
//  
//  Serialize(xml, true);
//  
//  nglString dump = xml->Dump();
//  
//  delete xml;
//  return dump;
}


nuiWidget::~nuiWidget()
{
  CheckValid();  
  Destroyed();
  
#ifdef NUI_WIDGET_STATS
  wcount--;
  gWidgetCounter.Add(GetProperty("Class"));
#endif
	
#ifdef _DEBUG_
  if (GetDebug())
  {
    NGL_OUT("nuiWidget::~nuiWidget() [0x%x '%s':'%s']\n", this, GetObjectClass().GetChars(), GetObjectName().GetChars());
  }
#endif
  
  StopAutoDraw();
  ClearAnimations();

  nuiTopLevel* pRoot = GetTopLevel();
  if (pRoot)
    pRoot->AdviseObjectDeath(this);

  //NGL_ASSERT(!pRoot || IsTrashed());

  if (mpTheme)
    mpTheme->Release();
    
  if (mpDecoration)
  {
    mpDecoration->Release();
  }

  if (mpSurface)
  {
    mpSurface->Release();
  }
  delete mpRenderCache;
  if (mpMatrixNodes)
  {
    for (uint32 i = 0; i < mpMatrixNodes->size(); i++)
      mpMatrixNodes->at(i)->Release();
    delete mpMatrixNodes;
    mpMatrixNodes = NULL;
  }

  // Event Actions:
  for (auto pAction : mEventActions)
    pAction->Disconnect(this);


  // Delete all children:
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem)
    {
      //      if (!pItem->IsTrashed(false) && pItem->Release())
      //        pItem->SetParent(NULL);
      if (!pItem->IsTrashed(false))
        pItem->SetParent(NULL);
      pItem->Release();

    }
  }
  delete pIt;
}

void nuiWidget::Built()
{
  // ...
}

bool nuiWidget::IsTrashed(bool combined) const 
{ 
  CheckValid();
  if (!combined || !mpParent)
    return mTrashed;
  if (!mTrashed)
    return mpParent->IsTrashed(true);
  return mTrashed;
}

nuiWidgetPtr nuiWidget::GetParent() const
{
  return mpParent;
}

nuiTopLevel* nuiWidget::GetTopLevel() const
{
  CheckValid();
  NGL_ASSERT(mpParent != this);
  if (mpParent)
    return mpParent->GetTopLevel();
  else
    return NULL;
}


bool nuiWidget::SetParent(nuiWidgetPtr pParent)
{
  CheckValid();
  NGL_ASSERT(!IsTrashed(false));
  bool res = true;

  nuiTopLevel* pRoot = GetTopLevel();
  if (pRoot)
  {
    pRoot->ReleaseToolTip(this);
    if (!pParent) // If we used to be connected to a root but the current parenting changes that: tell the trunk of the tree about it!
    {
      pRoot->AdviseSubTreeDeath(this);
      CallDisconnectTopLevel(pRoot);
    }
  }

  mpParent = pParent;
  ParentChanged();

  SetLayoutConstraint(LayoutConstraint()); ///< Reset the constraints when the widget is reparented
  InvalidateLayout();
  
  nuiTopLevel* pTopLevel = GetTopLevel();
  if (pTopLevel)
    CallConnectTopLevel(pTopLevel);
  
  DebugRefreshInfo();
  return res;
}

void nuiWidget::LocalToGlobal(int& x, int& y) const
{
  CheckValid();
  if (!IsMatrixIdentity())
  {
    nuiVector vec((double)x,(double)y, 0);
    vec = GetMatrix() * vec;
    x = ToBelow(vec[0]);
    y = ToBelow(vec[1]);
  }

  x += (int)mRect.mLeft;
  y += (int)mRect.mTop;

  if (mpParent)
    mpParent->LocalToGlobal(x,y);
}

void nuiWidget::LocalToGlobal(nuiSize& x, nuiSize& y) const
{
  CheckValid();
  if (!IsMatrixIdentity())
  {
    nuiVector vec(x, y, 0);
    vec = GetMatrix() * vec;
    x = vec[0];
    y = vec[1];
  }

  x += mRect.mLeft;
  y += mRect.mTop;

  if (mpParent)
    mpParent->LocalToGlobal(x,y);
}

void nuiWidget::LocalToGlobal(nuiRect& rRect) const
{
  CheckValid();
  if (!IsMatrixIdentity())
  {
    nuiVector vec1(rRect.mLeft,rRect.mTop,0);
    nuiVector vec2(rRect.mRight,rRect.mBottom,0);
    vec1 = GetMatrix() * vec1;
    vec2 = GetMatrix() * vec2;
    rRect.mLeft   = vec1[0];
    rRect.mTop    = vec1[1];
    rRect.mRight  = vec2[0];
    rRect.mBottom = vec2[1];
  }

  rRect.Move(mRect.mLeft, mRect.mTop);

  if (mpParent)
  {
    mpParent->LocalToGlobal(rRect);
  }
}

void nuiWidget::GlobalToLocal(int& x, int& y) const
{
  CheckValid();
  if (mpParent)
    mpParent->GlobalToLocal(x,y);
  x -= (int)mRect.mLeft;
  y -= (int)mRect.mTop;

  if (!IsMatrixIdentity())
  {
    nuiVector vec((double)x,(double)y,0);
    nuiMatrix mat;
    GetMatrix(mat);
    mat.InvertHomogenous();
    vec = mat * vec;
    x = ToBelow(vec[0]);
    y = ToBelow(vec[1]);
  }
}

void nuiWidget::GlobalToLocal(nuiSize& x, nuiSize& y) const
{
  CheckValid();
  if (mpParent)
    mpParent->GlobalToLocal(x,y);
  x -= mRect.mLeft;
  y -= mRect.mTop;

  if (!IsMatrixIdentity())
  {
    nuiVector vec(x,y,0);
    nuiMatrix mat;
    GetMatrix(mat);
    mat.InvertHomogenous();
    vec = mat * vec;
    x = vec[0];
    y = vec[1];
  }
}

void nuiWidget::GlobalToLocal(nuiRect& rRect) const
{
  CheckValid();
  if (mpParent)
  {
    mpParent->GlobalToLocal(rRect);
  }
  rRect.Move(-mRect.mLeft, -mRect.mTop);

  if (!IsMatrixIdentity())
  {
    nuiMatrix mat;
    GetMatrix(mat);
    mat.InvertHomogenous();
    nuiVector vec1(rRect.mLeft,rRect.mTop,0);
    nuiVector vec2(rRect.mRight,rRect.mBottom,0);
    vec1 = mat * vec1;
    vec2 = mat * vec2;
    rRect.mLeft   = vec1[0];
    rRect.mTop    = vec1[1];
    rRect.mRight  = vec2[0];
    rRect.mBottom = vec2[1];
  }
}

void nuiWidget::LocalToLocal(nuiWidgetPtr pWidget,int& x, int& y) const
{                           
  CheckValid();
  if (GetRoot() != pWidget->GetRoot())
    return;

  LocalToGlobal(x,y);
  pWidget->GlobalToLocal(x,y);
}

void nuiWidget::LocalToLocal(nuiWidgetPtr pWidget, nuiSize& x, nuiSize& y) const
{                           
  CheckValid();
  if (GetRoot() != pWidget->GetRoot())
    return;

  LocalToGlobal(x,y);
  pWidget->GlobalToLocal(x,y);
}

void nuiWidget::LocalToLocal(nuiWidgetPtr pWidget,nuiRect& rRect) const
{                           
  CheckValid();
  if (GetRoot() != pWidget->GetRoot())
    return;

  LocalToGlobal(rRect);
  pWidget->GlobalToLocal(rRect);
}

const nuiRect& nuiWidget::GetRect() const
{
  CheckValid();
  return mRect;
}

nuiRect nuiWidget::GetBorderedRect() const
{
  CheckValid();
  return GetBorderedRect(GetRect());
}

nuiRect nuiWidget::GetBorderedRect(const nuiRect& rRect) const
{
  CheckValid();
  nuiRect rect = rRect;
  rect.Bottom() += GetActualBorderBottom();
  rect.Top() -= GetActualBorderTop();
  rect.Left() -= GetActualBorderLeft();
  rect.Right() += GetActualBorderRight();
  return rect;
}

nuiRect nuiWidget::GetBorderLessRect(const nuiRect& rRect) const
{
  CheckValid();
  nuiRect rect = rRect;
  rect.Bottom() -= GetActualBorderBottom();
  rect.Top() += GetActualBorderTop();
  rect.Left() += GetActualBorderLeft();
  rect.Right() -= GetActualBorderRight();
  return rect;
}



void nuiWidget::InvalidateRect(const nuiRect& rRect)
{
  CheckValid();
  #ifdef _DEBUG_LAYOUT
  if (GetDebug())
    NGL_OUT("  nuiWidget::InvalidateRect '%s' [%s] %s (thread: %d)\n", GetObjectClass().GetChars(), GetObjectName().GetChars(), rRect.GetValue().GetChars(), nglThread::GetCurThreadID());
  #endif
  
  if (IsVisible(true))
  {
    nuiRect tmp(rRect);
    tmp.RoundToBiggest();
    BroadcastInvalidateRect(this, tmp);
  }
  
  mNeedSelfRedraw = true;
  InvalidateSurface();
  DebugRefreshInfo();
}

void nuiWidget::BroadcastInvalidateRect(nuiWidgetPtr pSender, const nuiRect& rRect)
{
  CheckValid();
  nuiRect r = rRect;
  nuiRect rect = GetRect();
  nuiRect size = GetOverDrawRect(true, true);
  r.Intersect(r, size);

  if (!IsMatrixIdentity())
  {
    nuiMatrix m(GetMatrix());
    //m.InvertHomogenous();

    nuiVector vec1(r.mLeft,r.mTop,0);
    nuiVector vec2(r.mRight,r.mTop,0);
    nuiVector vec3(r.mRight,r.mBottom,0);
    nuiVector vec4(r.mLeft,r.mBottom,0);
    vec1 = m * vec1;
    vec2 = m * vec2;
    vec3 = m * vec3;
    vec4 = m * vec4;
    r.mLeft   = MIN(vec1[0], MIN(vec2[0], MIN(vec3[0], vec4[0]) ) );
    r.mTop    = MIN(vec1[1], MIN(vec2[1], MIN(vec3[1], vec4[1]) ) );
    r.mRight  = MAX(vec1[0], MAX(vec2[0], MAX(vec3[0], vec4[0]) ) );
    r.mBottom = MAX(vec1[1], MAX(vec2[1], MAX(vec3[1], vec4[1]) ) );
  }

  mNeedRender = true;
  if (mSurfaceEnabled)
  {
    mNeedSelfRedraw = true;
    
    AddInvalidRect(r);
  }

  r.Move(rect.Left(), rect.Top());

  if (mpParent)
  {
    mpParent->CheckValid();
    mpParent->BroadcastInvalidateRect(pSender, r);
  }
  DebugRefreshInfo();
}

void nuiWidget::Invalidate()
{
//  if (CFRunLoopGetCurrent() != CFRunLoopGetMain())
//  {
//    printf("!!! WARNING !!! [ INVALIDATE CALLED FROM A BAD RUN LOOP ] !!! WARNING !!!\n");
//  }

  CheckValid();
  if ((mNeedRender && mNeedSelfRedraw))
    return;

  NGL_ASSERT(nglThread::GetCurThreadID() == App->GetMainThreadID());
  if (GetDebug())
  {
    NGL_OUT("  nuiWidget::Invalidate '%s' [%s] (thread: %d)\n",  GetObjectClass().GetChars(), GetObjectName().GetChars(), nglThread::GetCurThreadID());
  }

  if (!IsVisible(true))
  {
    mNeedSelfRedraw = true;
    InvalidateSurface();
    return;
  }

  nuiRect r(GetOverDrawRect(true, true));
  r.Intersect(r, GetVisibleRect());
  nuiWidget::InvalidateRect(r);
  SilentInvalidate();
  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
}

void nuiWidget::InvalidateSurface()
{
  CheckValid();
  if (mNeedSurfaceRedraw || !mpSurface)
    return;
  
  mNeedSurfaceRedraw = true;
  mNeedRender = true;

  if (mpParent)
    mpParent->InvalidateRect(GetRect());
  DebugRefreshInfo();
}

nuiSurface* nuiWidget::GetSurface() const
{
  CheckValid();
  return mpSurface;
}

const nuiMatrix& nuiWidget::GetSurfaceMatrix() const
{
  CheckValid();
  return mSurfaceMatrix;
}

void nuiWidget::SetSurfaceMatrix(const nuiMatrix& rMatrix)
{
  CheckValid();
  mSurfaceMatrix = rMatrix;
  InvalidateSurface();
}

const nuiColor& nuiWidget::GetSurfaceColor() const
{
  CheckValid();
  return mSurfaceColor;
}

void nuiWidget::SetSurfaceColor(const nuiColor& rColor)
{
  CheckValid();
  mSurfaceColor = rColor;
  InvalidateSurface();
}

nuiBlendFunc nuiWidget::GetSurfaceBlendFunc() const
{
  CheckValid();
  return mSurfaceBlendFunc;
}

void nuiWidget::SetSurfaceBlendFunc(nuiBlendFunc BlendFunc)
{
  CheckValid();
  mSurfaceBlendFunc = BlendFunc;
  InvalidateSurface();
}


void nuiWidget::SilentInvalidate()
{
  CheckValid();
  mNeedRender = true;
  #ifdef _DEBUG_LAYOUT
  if (GetDebug())
  {
    NGL_OUT("  nuiWidget::SilentInvalidate '%s' [%s]\n", GetObjectClass().GetChars(), GetObjectName().GetChars());
  }
  #endif
  
  mNeedSelfRedraw = true;
  InvalidateSurface();
  if (mpRenderCache)
    mpRenderCache->Reset(NULL);
  DebugRefreshInfo();
}

void nuiWidget::BroadcastInvalidate(nuiWidgetPtr pSender)
{
  CheckValid();
  if (mpParent && !mNeedRender)
  {
    mpParent->BroadcastInvalidate(pSender);
  }

  mNeedRender = true;

  if (mpSurface)
    mNeedSelfRedraw = true;

  DebugRefreshInfo();
}

void nuiWidget::SilentInvalidateLayout()
{
  CheckValid();
  #ifdef _DEBUG_LAYOUT
  if (GetDebug())
  {
    NGL_OUT("  nuiWidget::SilentInvalidateLayout '%s' [%s]\n", GetObjectClass().GetChars(), GetObjectName().GetChars());
  }
  #endif
  mNeedSelfLayout = true;
  mNeedLayout = true;
  mNeedIdealRect = true;
  mNeedRender = true;
  mNeedSelfRedraw = true;
  DebugRefreshInfo();
}

void nuiWidget::InvalidateLayout()
{
  CheckValid();

  if (mAutoUpdateLayout)
  {
    UpdateLayout();
    Invalidate();
    return;
  }

  if ((!mNeedSelfLayout && HasUserRect()))
  {
    UpdateLayout();
    return;
  }
  
  bool broadcast = !mNeedLayout;
  SilentInvalidateLayout();

  if (mpParent && broadcast)
  {
    //NGL_OUT("InvalidateLayout + Broadcast from %s\n", GetObjectClass().GetChars());
    mpParent->BroadcastInvalidateLayout(this, false);
  }
  DebugRefreshInfo();
}

void nuiWidget::ForcedInvalidateLayout()
{
  CheckValid();
  bool broadcast = !mNeedLayout;
  SilentInvalidateLayout();
  
  if (mpParent && broadcast)
  {
    //NGL_OUT("InvalidateLayout + Broadcast from %s\n", GetObjectClass().GetChars());
    mpParent->BroadcastInvalidateLayout(this, false);
  }
  DebugRefreshInfo();
}

void nuiWidget::BroadcastInvalidateLayout(nuiWidgetPtr pSender, bool BroadCastOnly)
{
  CheckValid();

  if ((!mNeedSelfLayout && HasUserSize())) // A child can't change the ideal position of its parent so we can stop broadcasting if the parent has a fixed ideal size.
  {
    UpdateLayout();
    return;
  }
  
  mNeedRender = true;
  if (mpSurface)
    mNeedSelfRedraw = true;

  if (!BroadCastOnly)
  {
    mNeedSelfLayout = true;
    mNeedIdealRect = true;
  }

  if (mpParent)
  {
    mpParent->BroadcastInvalidateLayout(pSender, BroadCastOnly);
//    NGL_OUT("nuiWidget::BroadcastInvalidateLayout %s / %s / 0x%x\n", pSender->GetObjectClass().GetChars(), pSender->GetObjectName().GetChars(), pSender);
  }

#ifdef DEBUG
  if (GetDebug() || pSender->GetDebug())
  {
    //NGL_OUT("nuiWidget::BroadcastInvalidateLayout SKIP %s / %s / 0x%x\n", pSender->GetObjectClass().GetChars(), pSender->GetObjectName().GetChars(), pSender);
  }
#endif

  mNeedLayout = true;

  DebugRefreshInfo();
}

bool nuiWidget::InternalDrawWidget(nuiDrawContext* pContext, const nuiRect& _self, const nuiRect& _self_and_decorations, bool ApplyMatrix)
{
  CheckValid();
  pContext->PushState();
  pContext->ResetState();
  if (ApplyMatrix && !IsMatrixIdentity())
    pContext->MultMatrix(GetMatrix());
  
  if (mAutoClip)
  {
    pContext->PushClipping();
    if (mpDecoration)
      pContext->Clip(_self_and_decorations);
    pContext->EnableClipping(true);
  }
  
  uint32 clipdepth = pContext->GetClipStackSize();
  
  
  ////////////////////// Draw the Underlay
  if (mpDecoration && mDecorationEnabled)
  {      
    pContext->PushState();
    nuiRect sizerect(GetRect().Size());
    mpDecoration->ClientToGlobalRect(sizerect, this);
    mpDecoration->DrawBack(pContext, this, sizerect);
    pContext->PopState();
  }
  
  if (HasFocus())
  {
    pContext->PushState();
    DrawFocus(pContext, false);
    pContext->PopState();
  }
  
  ////////////////////// Draw the widget
  if (mAutoClip)
  {
    pContext->PushClipping();
    pContext->Clip(_self);
  }
  
  pContext->PushState();
  Draw(pContext);
  pContext->PopState();
  
  if (mAutoClip)
    pContext->PopClipping();
  
  ////////////////////// Draw the Overlay
  if (mpDecoration && mDecorationEnabled)
  {
    pContext->PushState();
    nuiRect sizerect(GetRect().Size());
    mpDecoration->ClientToGlobalRect(sizerect, this);
    mpDecoration->DrawFront(pContext, this, sizerect);
    pContext->PopState();
  }
  
  if (HasFocus())
  {
    pContext->PushState();
    DrawFocus(pContext, true);
    pContext->PopState();
  }
  
  mDrawingInCache = false;
  
  uint32 newclipdepth = pContext->GetClipStackSize();
  NGL_ASSERT(clipdepth == newclipdepth);
  
  if (mAutoClip)
    pContext->PopClipping();
  
  pContext->PopState();
  
  return true;
}

#if 0

2 options alter the rendering:
- RenderCache on/off
- Surface on/off

4 cases:

  None:
    InternalDrawWidget(pContext); // Draw!

  RenderCache:
    if (mNeedRender)
      InternalDrawWidget(pRenderCache); // Fill the cache
    DrawRenderCache(pContext); // Draw the cache to the screen

  Surface:
    if (mNeedRender) // need to update the surface contents
      InternalDrawWidget(pSurface);
    DrawSurface(pContext);

  Surface + RenderCache:
    if (mNeedRender)
    {
      InternalDrawWidget(pRenderCache);
      DrawRenderCache(pSurface);
    }
    DrawSurface(pContext);
    
 
#endif



bool nuiWidget::DrawWidget(nuiDrawContext* pContext)
{
  CheckValid();
  if (!IsVisible())
    return false;

  //NGL_ASSERT(!mNeedLayout);
  //if (mNeedLayout)
  // printf("need layout bug on 0x%X [%s - %s]\n", this, GetObjectClass().GetChars(), GetObjectName().GetChars());

  if (mSurfaceEnabled)
  {
    bool drawingincache = mpParent ? mpParent->IsDrawingInCache(true) : false;
    
    nuiRect clip;
    pContext->GetClipRect(clip, true);
    nuiRect _self = GetOverDrawRect(true, false);
    nuiRect _self_and_decorations = GetOverDrawRect(true, true);
    nuiRect inter;
    
    bool rendertest = mNeedRender;
    if (mNeedRender)
    {
      if (mNeedSelfRedraw)
      {
        UpdateSurface(_self_and_decorations.Size());

        NGL_ASSERT(mpSurface);
        
        mNeedRender = false;
        
        if (mDirtyRects.empty())
        {
          mDirtyRects.push_back(nuiRect(mpSurface->GetWidth(), mpSurface->GetHeight()));
        }

        int count = mDirtyRects.size();
        
        //printf("drawing %d partial rects\n", count);
        
        for (int i = 0; i < count; i++)
        {
          //printf("\t%d: %s\n", i, mDirtyRects[i].GetValue().GetChars());
//          mpSurface->ResetState();
//          mpSurface->ResetClipRect();
//          mpSurface->SetStrokeColor(nuiColor(0.0f, 0.0f, 0.0f, 0.0f));
//          mpSurface->SetFillColor(nuiColor(0.0f, 0.0f, 0.0f, 0.0f));
//          mpSurface->LoadMatrix(nglMatrixf());
//          mpSurface->Translate(_self_and_decorations.Left(), _self_and_decorations.Top());
//          mpSurface->Set2DProjectionMatrix(nuiRect(mpSurface->GetWidth(), mpSurface->GetHeight()));
//          mpSurface->Clip(mDirtyRects[i]);
//          mpSurface->EnableClipping(true);
//          
//          // clear the surface with transparent black:
//          mpSurface->PushState();
//          mpSurface->SetClearColor(nuiColor(0.0f, 0.0f, 0.0f, 0.0f));
//          mpSurface->Clear();  
//          mpSurface->PopState();
//          
//          InternalDrawWidget(mpSurface, _self, _self_and_decorations, true);

        }

        mDirtyRects.clear();
        mNeedSelfRedraw = false;
      }
    }
    
    //nuiMatrix m = pContext->GetMatrix();
    //nglString d;
    //m.GetValue(d);
    //    NGL_OUT("nglWidget(0x%p):\n%s\n", this, d.GetChars());
    //NGL_ASSERT(m.Array[12] > 0);
    
    mNeedSurfaceRedraw = false;
    if (!drawingincache)
      DrawSurface(pContext);
    
    DebugRefreshInfo();
  }
  else
  {
    bool drawingincache = mpParent ? mpParent->IsDrawingInCache(true) : false;
    
    nuiRect clip;
    pContext->GetClipRect(clip, true);
    nuiRect _self = GetOverDrawRect(true, false);
    nuiRect _self_and_decorations = GetOverDrawRect(true, true);
    nuiRect inter;
    
    _self.Intersect(_self, mVisibleRect);
    _self_and_decorations.Intersect(_self_and_decorations, mVisibleRect);
//    if (!inter.Intersect(_self_and_decorations, clip)) // Only render at the last needed moment. As we are currently offscreen or clipped entirely we will redraw another day.
//      return false;
    
    nuiDrawContext* pSavedCtx = pContext;
    
    bool rendertest = mNeedRender;
    if (mGlobalUseRenderCache && mUseRenderCache)
    {
      NGL_ASSERT(mpRenderCache);
      
      if (mNeedSelfRedraw)
      {
        mpSavedPainter = pContext->GetPainter();
        mpRenderCache->Reset(mpSavedPainter);
        pContext->SetPainter(mpRenderCache);
        
        mDrawingInCache = true;
        
        InternalDrawWidget(pContext, _self, _self_and_decorations, false);
        
        pContext->SetPainter(mpSavedPainter);
        mNeedSelfRedraw = false;
      }
      
      if (!drawingincache && !pContext->GetPainter()->GetDummyMode())
      {
        mNeedRender = false;
        if (!IsMatrixIdentity())
        {
          pContext->PushMatrix();
          pContext->MultMatrix(GetMatrix());
        }
        
        mpRenderCache->ReDraw(pContext);
        
        if (!IsMatrixIdentity())
          pContext->PopMatrix();
      }
      
    }
    else
    {
      if (!drawingincache && !pContext->GetPainter()->GetDummyMode())
      {
        mNeedRender = false;
        
        InternalDrawWidget(pContext, _self, _self_and_decorations, true);
        mNeedSelfRedraw = false;
      }
    }
    
    pContext = pSavedCtx;
    
    DebugRefreshInfo();
  }

  return true;
}

void nuiWidget::DrawSurface(nuiDrawContext* pContext)
{
  CheckValid();
//  NGL_OUT("nuiWidget::DrawSurface %d x %d\n", (uint32)mpSurface->GetWidth(), (uint32)mpSurface->GetHeight());
  pContext->PushProjectionMatrix();
  pContext->PushMatrix();
  pContext->PushState();
  pContext->ResetState();
  pContext->EnableTexturing(true);
  nuiTexture* pTexture = mpSurface->GetTexture();
  NGL_ASSERT(pTexture);
  pContext->SetTexture(pTexture);
  pContext->EnableBlending(true);
  pContext->SetFillColor(mSurfaceColor);

  pContext->SetBlendFunc(mSurfaceBlendFunc);
  
  pContext->MultMatrix(mSurfaceMatrix);

  nuiRect src, dst;
  src = GetOverDrawRect(true, true);
  dst = src;
  pContext->DrawImage(dst, src);
  pContext->PopState();
  pContext->PopMatrix();
  pContext->PopProjectionMatrix();
}

bool nuiWidget::IsKeyDown (nglKeyCode Key) const
{
  CheckValid();
  nuiTopLevelPtr pRoot = GetTopLevel();

  return pRoot ? pRoot->IsKeyDown(Key) : false;
}

bool nuiWidget::DispatchTextInput(const nglString& rUnicodeText)
{
  nuiAutoRef;
  CheckValid();
  if (TextInput(rUnicodeText))
  {
    return true;
  }
  
  if (mpParent)
  {
    return mpParent->DispatchTextInput(rUnicodeText);
  }
  
  return false;
}

void nuiWidget::DispatchTextInputCancelled()
{
  CheckValid();
  TextInputCancelled();
  if (mpParent)
    mpParent->DispatchTextInputCancelled();
}


bool nuiWidget::DispatchKeyDown(const nglKeyEvent& rEvent, nuiKeyModifier Mask)
{
  nuiAutoRef;
  CheckValid();
  Mask &= mHotKeyMask;
  if (TriggerHotKeys(rEvent, true, true, Mask))
  {
    return true;
  }
  
  if (KeyDown(rEvent))
  {
    return true;
  }
  
  if (TriggerHotKeys(rEvent, true, false, Mask))
  {
    return true;
  }
  
  if (mpParent)
  {
    return mpParent->DispatchKeyDown(rEvent, Mask);
  }
  
  return false;
}

bool nuiWidget::DispatchKeyUp(const nglKeyEvent& rEvent, nuiKeyModifier Mask)
{
  nuiAutoRef;
  CheckValid();
  Mask &= mHotKeyMask;
  if (TriggerHotKeys(rEvent, false, true, Mask))
  {
    return true;
  }
  
  if (KeyUp(rEvent))
  {
    return true;
  }
  
  if (TriggerHotKeys(rEvent, false, false, Mask))
  {
    return true;
  }
  
  if (mpParent)
  {
    return mpParent->DispatchKeyUp(rEvent, Mask);
  }
  
  return false;
}

// Event callbacks:
void nuiWidget::TextCompositionStarted()
{
}

void nuiWidget::TextCompositionConfirmed()
{
}

void nuiWidget::TextCompositionCanceled()
{
}

void nuiWidget::TextCompositionUpdated(const nglString& rString, int32 CursorPosition)
{
}

nglString nuiWidget::GetTextComposition() const
{
  return nglString::Null;
}

void nuiWidget::TextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const
{
  x = 0; 
  y = 0;
}


bool nuiWidget::TextInput(const nglString& rUnicodeText)
{
  CheckValid();
  return false;
}

void nuiWidget::TextInputCancelled()
{
  CheckValid();
  UnFocus();
}

bool nuiWidget::KeyDown(const nglKeyEvent& rEvent)
{
  CheckValid();
  return false;
}

bool nuiWidget::KeyUp(const nglKeyEvent& rEvent)
{
  CheckValid();
  return false;
}

bool nuiWidget::TriggerHotKeys(const nglKeyEvent& rEvent, bool KeyDown,  bool Priority, nuiKeyModifier Mask)
{
  CheckValid();
  nuiAutoRef;
  nuiKeyModifier Modifiers = 0;
  
  if (IsKeyDown(NK_LSHIFT) || IsKeyDown(NK_RSHIFT))
    Modifiers |= nuiShiftKey;

  if (IsKeyDown(NK_LALT) || IsKeyDown(NK_RALT))
    Modifiers |= nuiAltKey;

  if (IsKeyDown(NK_LCTRL) || IsKeyDown(NK_RCTRL))
    Modifiers |= nuiControlKey;

  if (IsKeyDown(NK_LMETA) || IsKeyDown(NK_RMETA))
    Modifiers |= nuiMetaKey;

  if (IsKeyDown(NK_MENU))
    Modifiers |= nuiMenuKey;

  if (!Modifiers)
    Modifiers = nuiNoKey;
  
  if (Modifiers != (Modifiers & Mask))
    return false;
  
  std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.begin();
  std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator end = mHotKeyEvents.end();

  for ( ; it != end; ++it)
  {
    nglString name = it->first;
    nuiHotKey* pHotKey = GetTopLevel()->GetHotKey(name);

    if (pHotKey->IsEnabled() && (pHotKey->HasPriority() == Priority))
    {
      bool res = false;
      if (KeyDown)
        res = pHotKey->OnKeyDown(rEvent, Modifiers, *(it->second));
      else
        res = pHotKey->OnKeyUp(rEvent, Modifiers, *(it->second));

      if (res)
        return res;
    }
  }
  return false;
}

void nuiWidget::SetHotKeyMask(nuiKeyModifier Mask)
{
  CheckValid();
  mHotKeyMask = Mask; 
}

nuiKeyModifier nuiWidget::GetHotKeyMask() const
{
  CheckValid();
  return mHotKeyMask;
}


void nuiWidget::EnableMouseEvent(bool enable)
{
  CheckValid();
  mMouseEventEnabled = enable;
}

bool nuiWidget::MouseEventsEnabled() const
{
  CheckValid();
  return mMouseEventEnabled;
}

// Multitouch events:
bool nuiWidget::MouseClicked(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::MouseUnclicked(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::MouseCanceled(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::MouseMoved(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::MouseWheelMoved(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}


bool nuiWidget::MouseGrabbed(nglTouchId id)
{
  CheckValid();
  return true;
}

bool nuiWidget::MouseUngrabbed(nglTouchId id)
{
  CheckValid();
  return false;
}

const std::map<nglTouchId, nglMouseInfo>& nuiWidget::GetMouseStates() const
{
  nuiTopLevel* pTopLevel = GetTopLevel();
  NGL_ASSERT(pTopLevel);
  return pTopLevel->GetMouseStates();
}


////// Private event management:

bool nuiWidget::DispatchGrab(nuiWidgetPtr pWidget)
{
  CheckValid();
  nuiAutoRef;
  if (mpParent)
    return mpParent->DispatchGrab(pWidget);
  return false;
}

bool nuiWidget::DispatchUngrab(nuiWidgetPtr pWidget)
{
  CheckValid();
  if (mpParent)
    return mpParent->DispatchUngrab(pWidget);
  return false;
}

bool nuiWidget::DispatchHasGrab(nuiWidgetPtr pWidget)
{
  CheckValid();
  if (mpParent)
    return mpParent->DispatchHasGrab(pWidget);
  return false;  
}

bool nuiWidget::DispatchHasGrab(nuiWidgetPtr pWidget, nglTouchId TouchId)
{
  CheckValid();
  if (mpParent)
    return mpParent->DispatchHasGrab(pWidget, TouchId);
  return false;  
}

bool nuiWidget::HasGrab()
{
  CheckValid();
  return DispatchHasGrab(this);
}

bool nuiWidget::HasGrab(nglTouchId TouchId)
{
  CheckValid();
  return DispatchHasGrab(this, TouchId);
}

bool nuiWidget::StealMouseEvent(const nglMouseInfo& rInfo)
{
  NGL_DEBUG(if (GetDebug()) NGL_LOG("widget", NGL_LOG_INFO, "%s/%s (%p) Preempting mouse from existing grabber!\n", GetObjectClass().GetChars(), GetObjectName().GetChars(), this););
  nuiTopLevel* pTop = GetTopLevel();
  NGL_ASSERT(pTop);
  return pTop->StealMouseEvent(this, rInfo);
}

bool nuiWidget::RequestStolenMouse(const nglMouseInfo& rInfo)
{
  NGL_DEBUG(if (GetDebug()) NGL_LOG("widget", NGL_LOG_INFO, "%p RequestStolenMouse (%d,%d)\n", this, rInfo.X, rInfo.Y); );
  return mAutoAcceptMouseSteal;
}

bool nuiWidget::Grab()
{
  CheckValid();
  if (!DispatchGrab(this))
  {
    ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
    return false;
  }
  return true;
}

bool nuiWidget::Ungrab()
{
  CheckValid();
  if (!DispatchUngrab(this))
  {
    ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
    return false;
  }  
  return true;
}


bool nuiWidget::GetWantKeyboardFocus() const
{
  CheckValid();
  return mWantKeyboardFocus;
}

void nuiWidget::SetWantKeyboardFocus(bool Set)
{
  CheckValid();
  mWantKeyboardFocus = Set;
}

bool nuiWidget::GetMuteKeyboardFocusDispatch() const
{
  CheckValid();
  return mMuteKeyboardFocusDispatch;
}

void nuiWidget::SetMuteKeyboardFocusDispatch(bool Set)
{
  CheckValid();
  mMuteKeyboardFocusDispatch = Set;
}

bool nuiWidget::Focus()
{
  CheckValid();
  if (HasFocus())
    return true;

  mHasFocus = true;
  Invalidate();

  nuiTopLevelPtr pRoot = GetTopLevel();

  if (pRoot)
    return pRoot->SetFocus(this);

  return false;
}

bool nuiWidget::UnFocus()
{
  CheckValid();
  if (!HasFocus())
    return false;

  mHasFocus = false;
  nuiTopLevelPtr pRoot = GetTopLevel();

  Invalidate();
  if (pRoot)
    return pRoot->SetFocus(NULL);

  return false;
}


bool nuiWidget::IsParentActive() const
{
  CheckValid();
  if (mpParent)
    return mpParent->IsParentActive();
  return false;
}

float nuiWidget::GetMixedAlpha() const
{
  CheckValid();
  if (mpParent)
    return mpParent->GetMixedAlpha() * mAlpha; // No transparency by default
  return mAlpha; // No transparency by default
}

float nuiWidget::GetAlpha() const
{
  CheckValid();
  return mAlpha; // No transparency by default
}

void nuiWidget::OnTrash()
{
  CheckValid();
}

bool nuiWidget::Trash()
{
  CheckValid();
  nuiAutoRef;

  if (!mTrashed)
  {
    CallOnTrash();
    TrashRequested();
  }

  nuiAnimation* pAnim = GetAnimation("TRASH");
  if (pAnim && (pAnim->GetTime()==0 && pAnim->GetDuration()>0))
  {
    StartAnimation("TRASH");
  }
  else
  {
    NGL_ASSERT(mTrashed);
    NGL_ASSERT(!mDoneTrashed);
    if (mDoneTrashed)
      return false;

    mDoneTrashed = true;
    Trashed();

    DebugRefreshInfo();
    if (mpParent)
      mpParent->DelChild(this);
    return true;
  }

  return true;
}

bool nuiWidget::HasInheritedProperty (const nglString& rName)
{
  CheckValid();
  if (HasProperty(rName))
    return true;
  if (mpParent)
    return mpParent->HasInheritedProperty(rName);
  return false;
}

nglString nuiWidget::GetInheritedProperty (const nglString& rName)
{
  CheckValid();
  if (HasProperty(rName))
    return GetProperty(rName);
  if (mpParent)
    return mpParent->GetInheritedProperty(rName);
  return nglString::Null;
}


void nuiWidget::OnSetFocus(nuiWidgetPtr pWidget)
{
  CheckValid();
}

void nuiWidget::DispatchFocus(nuiWidgetPtr pWidget)
{
  CheckValid();
  mHasFocus = (pWidget == this);
  OnSetFocus(pWidget);
  FocusChanged();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
  DebugRefreshInfo();
}

bool nuiWidget::HasFocus() const
{
  CheckValid();
  return mHasFocus;
}

nuiDrawContext* nuiWidget::GetDrawContext()
{
  CheckValid();
  nuiWidgetPtr pRoot=GetRoot();
  if (pRoot && pRoot!=this)
    return pRoot->GetDrawContext();
  else
    return NULL;
}

bool nuiWidget::HasLayoutChanged() const
{
  CheckValid();
  return mNeedSelfLayout;
}

bool nuiWidget::GetHover() const
{
  CheckValid();
  return mHover;
}

void nuiWidget::SetHover(bool Hover)
{
  CheckValid();
  if (mHover == Hover)
    return;
  mHover = Hover;

  if (mHover)
  {
    HoverOn();
  }
  else
  {
    HoverOff();
  }
  HoverChanged();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
  if (mRedrawOnHover || (mpDecoration && mpDecoration->GetRedrawOnHover() && mDecorationEnabled))
    Invalidate();
  DebugRefreshInfo();
}

bool nuiWidget::IsEnabled(bool combined) const
{
  CheckValid();
  if (!combined || !mpParent)
    return mEnabled;
  if (mEnabled && !mStateLocked) 
    return mpParent->IsEnabled(true);
  return mEnabled;
}


bool nuiWidget::AttrIsSelected() const
{
  CheckValid();
  return IsSelected();
}


bool nuiWidget::IsSelected(bool combined) const
{
  CheckValid();
  if (!combined || !mpParent)
    return mSelected;
  if (!mSelected && !mStateLocked) 
    return mpParent->IsSelected(true);
  return mSelected;
}

void nuiWidget::SetPosition(nuiPosition mode)
{
  CheckValid();
  if (mPosition == mode)
    return;
  
  mPosition = mode;
  InvalidateLayout();
}

nuiPosition nuiWidget::GetPosition() const
{
  CheckValid();
  return mPosition;
}

void nuiWidget::SetFillRule(nuiPosition mode)
{
  CheckValid();
  if (mFillRule == mode)
    return;
  mFillRule = mode;

  if (mPosition == nuiFill)
    InvalidateLayout();
}

nuiPosition nuiWidget::GetFillRule() const
{
  CheckValid();
  return mFillRule;
}


void nuiWidget::LockState()
{
  CheckValid();
  mStateLocked = true;
  DebugRefreshInfo();
}

void nuiWidget::UnlockState()
{
  CheckValid();
  mStateLocked = false;
  DebugRefreshInfo();
}


void nuiWidget::SilentSetVisible(bool Visible)
{
  CheckValid();
  if (mVisible == Visible)
    return;
  
  mVisible = Visible;
}


bool nuiWidget::IsVisible(bool combined) const
{
  CheckValid();
  if (!combined || !mpParent)
    return mVisible;
  if (mVisible) 
    return mpParent->IsVisible(true);
  return mVisible;
}

void nuiWidget::StartTransition()
{
  CheckValid();
  mInTransition++;
}

void nuiWidget::StopTransition()
{
  CheckValid();
  mInTransition--;
}

bool nuiWidget::IsInTransition() const
{
  CheckValid();
  return mInTransition != 0;
}


// virtual
bool nuiWidget::SetMouseCursor(nuiMouseCursor Cursor)
{
  CheckValid();
  mObjCursor = Cursor;
  DebugRefreshInfo();
  return true;
}

void nuiWidget::SetAttrMouseCursor(nuiMouseCursor Cursor)
{
  CheckValid();
  mObjCursor = Cursor;
  DebugRefreshInfo();
}



nuiMouseCursor nuiWidget::GetMouseCursor() const
{
  CheckValid();
  return mObjCursor;
}



bool nuiWidget::IsInsideFromRoot(nuiSize X, nuiSize Y, nuiSize GrowOffset)
{
  CheckValid();
  if (!IsVisible(false))
    return false;

  GlobalToLocal(X, Y);
  return IsInsideFromSelf(X,Y, GrowOffset);
}

bool nuiWidget::IsInsideFromParent(nuiSize X, nuiSize Y, nuiSize GrowOffset)
{
  CheckValid();
  if (!IsVisible(false))
    return false;
  return IsInsideFromSelf(X - mRect.Left(), Y - mRect.Top(), GrowOffset);
}

bool nuiWidget::IsInsideFromSelf(nuiSize X, nuiSize Y, nuiSize GrowOffset)
{
  CheckValid();
  if (!IsVisible(false))
    return false;
  
  if (mInteractiveDecoration)
  {
    nuiRect r = mVisibleRect;
    r.Intersect(mVisibleRect, GetOverDrawRect(true, true));
    r.Grow(GrowOffset, GrowOffset);
    return r.IsInside(X, Y);
  }
  if (mInteractiveOD)
  {
    nuiRect r = mVisibleRect;
    r.Intersect(r, GetOverDrawRect(true, false));
    r.Grow(GrowOffset, GrowOffset);
    return r.IsInside(X, Y);
  }

  nuiRect r(GetRect().Size());
  r.Grow(GrowOffset, GrowOffset);
  return r.IsInside(X,Y);
}


bool nuiWidget::SetToolTip(const nglString& rToolTip)
{
  CheckValid();
//  if (GetDebug())
//  {
//    NGL_OUT("nuiWidget::SetToolTip for 0x%x %s / %s to %s\n", this, GetObjectClass().GetChars(), GetObjectName().GetChars(), rToolTip.GetChars());
//  }
  SetProperty("ToolTip", rToolTip);
  return true;
}

nglString nuiWidget::GetToolTip() const
{
  return GetProperty("ToolTip");
}

bool nuiWidget::ActivateToolTip(nuiWidgetPtr pWidget, bool Now)
{
  CheckValid();
//  if (GetDebug())
//  {
//    NGL_OUT("nuiWidget::ActivateToolTip for 0x%x %s / %s\n", this, GetObjectClass().GetChars(), GetObjectName().GetChars());
//  }
  
  nglString tt = GetToolTip();

  if (tt.IsEmpty())
  {
    return false;
  }

  nuiTopLevelPtr pRoot = GetTopLevel();
  NGL_ASSERT(pRoot);

  if (pRoot)
  {
    bool res = pRoot->ActivateToolTip(pWidget, Now);
    NGL_ASSERT(res);
    return res;
  }

  return false;
}

bool nuiWidget::ReleaseToolTip(nuiWidgetPtr pWidget)
{
  CheckValid();
  nuiTopLevelPtr pRoot = GetTopLevel();
  if (pRoot)
    return pRoot->ReleaseToolTip(pWidget);
  return false;
}


/// Drag
bool nuiWidget::Drag(nglDragAndDrop* pDragObj)
{
  CheckValid();
  nuiMainWindow* pWin = (nuiMainWindow*)GetTopLevel();
  return pWin->Drag(this, pDragObj);
}




void nuiWidget::OnDragRequestData(nglDragAndDrop* pDragObject, const nglString& rMimeType)
{
  CheckValid();
  // Do nothing, drag and drop is not supported by default.
}

void nuiWidget::OnDragStop(bool canceled)
{
  CheckValid();
  // Do nothing, drag and drop is not supported by default.
}


/// Drop

nglDropEffect nuiWidget::OnCanDrop(nglDragAndDrop* pDragObject,nuiSize X,nuiSize Y)
{
  CheckValid();
  return eDropEffectNone; // Or decline the DnD operation by default.
}

void nuiWidget::OnDropped(nglDragAndDrop* pDragObject,nuiSize X,nuiSize Y, nglMouseInfo::Flags Button)
{
  CheckValid();
  // Do nothing, drag and drop is not supported by default.
}

void nuiWidget::OnDropLeave()
{
  CheckValid();
  // Do nothing, drag and drop is not supported by default.
}

uint32 GetParentCount(nuiWidget* pWidget)
{
  nuiWidgetPtr pParent = pWidget->GetParent();
  if (pParent)
    return GetParentCount(pParent) + 1;

  return 0;
}

// Layout stuff
const nuiRect& nuiWidget::GetIdealRect()
{
  CheckValid();
  if (mNeedIdealRect)
  {
    mIdealRect = CalcIdealSize();

    if (mHasUserSize && mHasUserPos)
      mIdealRect = mUserRect;
    else
    {
      if (mHasUserWidth)
        mIdealRect.SetWidth(mUserRect.GetWidth());
      if (mHasUserHeight)
        mIdealRect.SetHeight(mUserRect.GetHeight());
    }

    if (mMaxIdealWidth > 0.0f)
      mIdealRect.Right() = MIN(mMaxIdealWidth, mIdealRect.GetWidth());
    if (mMaxIdealHeight > 0.0f)
      mIdealRect.Bottom() = MIN(mMaxIdealHeight, mIdealRect.GetHeight());
    if (mMinIdealWidth > 0.0f)
      mIdealRect.Right() = MAX(mMinIdealWidth, mIdealRect.GetWidth());
    if (mMinIdealHeight > 0.0f)
      mIdealRect.Bottom() = MAX(mMinIdealHeight, mIdealRect.GetHeight());

    if (mHasUserPos && !mHasUserSize)
      mIdealRect.MoveTo(mUserRect.mLeft, mUserRect.mTop);
    

    mIdealRect.Bottom() += GetActualBorderBottom();
    mIdealRect.Top() -= GetActualBorderTop();
    mIdealRect.Left() -= GetActualBorderLeft();
    mIdealRect.Right() += GetActualBorderRight();
    if (!HasUserPos())
      mIdealRect.MoveTo(0, 0);
    mIdealRect.RoundToBiggest();

  }

#ifdef NUI_LOG_GETIDEALRECT
  nglString _log;
  uint32 parentcount = GetParentCount(this);
  _log .Add("|  ", parentcount)
    .Add(mNeedIdealRect?">>> ":"")
    .Add(GetObjectClass())
    .Add(" / ")
    .Add(GetObjectName())
    .Add(" [GetIdealRect] => ")
    .Add(mIdealRect.GetValue())
    .AddNewLine();

    NGL_OUT(_log.GetChars());
#endif //NUI_LOG_GETIDEALRECT

  mNeedIdealRect = false;
  return mIdealRect;
}

bool nuiWidget::IsInSetRect() const
{
  if (mInSetRect)
    return true;
  return mpParent ? mpParent->IsInSetRect() : false;
}

void nuiWidget::SetBorders(nuiSize XY)
{
  CheckValid();
  SetBorder(XY, XY);
}

void nuiWidget::SetBorder(nuiSize X, nuiSize Y)
{
  CheckValid();
  if (mBorderLeft == X && mBorderRight == X && mBorderTop == Y && mBorderBottom == Y)
    return;
  
  mBorderLeft = mBorderRight = X;
  mBorderTop = mBorderBottom = Y;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetBorderLeft(nuiSize border)
{
  CheckValid();
  if (mBorderLeft == border)
    return;
  
  mBorderLeft = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetBorderTop(nuiSize border)
{
  CheckValid();
  if (mBorderTop == border)
    return;
  
  mBorderTop = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetBorderRight(nuiSize border)
{
  CheckValid();
  if (mBorderRight == border)
    return;
    
  mBorderRight = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetBorderBottom(nuiSize border)
{
  CheckValid();
  if (mBorderBottom == border)
    return;
  
  mBorderBottom = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiWidget::GetBorderLeft() const
{
  CheckValid();
  return mBorderLeft;
}

nuiSize nuiWidget::GetBorderTop() const
{
  CheckValid();
  return mBorderTop;
}

nuiSize nuiWidget::GetBorderRight() const
{
  CheckValid();
  return mBorderRight;
}

nuiSize nuiWidget::GetBorderBottom() const
{
  CheckValid();
  return mBorderBottom;
}

nuiSize nuiWidget::GetActualBorderLeft() const
{
  CheckValid();
  nuiSize Left = mBorderLeft;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Left = MAX(Left, mpDecoration->GetBorder(nuiLeft, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Left = MAX(Left, mpFocusDecoration->GetBorder(nuiLeft, this));
  return Left;
}

nuiSize nuiWidget::GetActualBorderTop() const
{
  CheckValid();
  nuiSize Top = mBorderTop;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Top = MAX(Top, mpDecoration->GetBorder(nuiTop, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Top = MAX(Top, mpFocusDecoration->GetBorder(nuiTop, this));
  return Top;
}

nuiSize nuiWidget::GetActualBorderRight() const
{
  CheckValid();
  nuiSize Right = mBorderRight;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Right = MAX(Right, mpDecoration->GetBorder(nuiRight, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Right = MAX(Right, mpFocusDecoration->GetBorder(nuiRight, this));
  return Right;
}

nuiSize nuiWidget::GetActualBorderBottom() const
{
  CheckValid();
  nuiSize Bottom = mBorderBottom;
  if (mDecorationEnabled)
  {
    if (mpDecoration && mDecorationMode == eDecorationBorder)
      Bottom = MAX(Bottom, mpDecoration->GetBorder(nuiBottom, this));
  }
  if (mpFocusDecoration && mFocusDecorationMode == eDecorationBorder)
    Bottom = MAX(Bottom, mpFocusDecoration->GetBorder(nuiBottom, this));
  return Bottom;
}


void nuiWidget::SetBorder(nuiSize Left, nuiSize Right, nuiSize Top, nuiSize Bottom)
{
  CheckValid();
  if (mBorderLeft == Left && mBorderRight == Right && mBorderTop == Top && mBorderBottom == Bottom)
    return;

  mBorderLeft = Left;
  mBorderRight = Right;
  mBorderTop = Top;
  mBorderBottom = Bottom;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::GetBorder(nuiSize& rLeft, nuiSize& rRight, nuiSize& rTop, nuiSize& rBottom)
{
  CheckValid();
  rLeft = mBorderLeft;
  rRight = mBorderRight;
  rTop = mBorderTop;
  rBottom = mBorderBottom;
}

void nuiWidget::SetVisibleRect(const nuiRect& rRect)
{
  CheckValid();
  nuiRect temp = mVisibleRect;
  if (mVisibleRect == rRect)
    return;
  
  mVisibleRect = rRect;
  mOverrideVisibleRect = true;
  Invalidate();
}

void nuiWidget::SilentSetVisibleRect(const nuiRect& rRect)
{
  CheckValid();
  if (mVisibleRect == rRect)
    return;
  
  mVisibleRect = rRect;
  mOverrideVisibleRect = true;
  SilentInvalidate();
}

void nuiWidget::ResetVisibleRect()
{
  if (!mOverrideVisibleRect)
    return;
  mOverrideVisibleRect = false;
  InvalidateLayout();
}
const nuiRect& nuiWidget::GetVisibleRect() const
{
  CheckValid();
  return mVisibleRect;
}

static nglString GetSurfaceName(nuiWidget* pWidget)
{
  static uint32 gSurfaceCount = 0;
  nglString str;
  str.CFormat("'%s'/'%s' %x %d", pWidget->GetObjectClass().GetChars(), pWidget->GetObjectName().GetChars(), pWidget, gSurfaceCount++);
  return str;
}

void nuiWidget::UpdateSurface(const nuiRect& rRect)
{
  CheckValid();
  if (mSurfaceEnabled)
  {
    if (!mpSurface || (mpSurface->GetWidth() != rRect.GetWidth() || mpSurface->GetHeight() != rRect.GetHeight()))
    {
      if (mpSurface)
      {
        mpSurface->Release();
      }
      mpSurface = NULL;

//      if (!(rRect.GetWidth() > 0 && rRect.GetHeight() > 0))
//        return;

      nglString str(GetSurfaceName(this));
      mpSurface = nuiSurface::CreateSurface(str, ToAbove(rRect.GetWidth()), ToAbove(rRect.GetHeight()), eImagePixelRGBA);
      mDirtyRects.clear();
      mDirtyRects.push_back(nuiRect(mpSurface->GetWidth(), mpSurface->GetHeight()));
    }
  }
  else
  {
    if (mpSurface)
    {
      mpSurface->Release();
    }
    mpSurface = NULL;
    mDirtyRects.clear();
  }
}

void nuiWidget::SetFixedAspectRatio(bool set)
{
  mFixedAspectRatio = set;
  InvalidateLayout();
}

bool nuiWidget::GetFixedAspectRatio() const
{
  return mFixedAspectRatio;
}

nuiRect nuiWidget::GetLayoutForRect(const nuiRect& rRect)
{
  CheckValid();
  nuiRect rect(GetIdealRect());
  nuiRect r(rRect);

  if (mMaxWidth >= 0)
    rect.SetWidth(MIN(r.GetWidth(), mMaxWidth));

  if (mMaxHeight >= 0)
    rect.SetHeight(MIN(r.GetHeight(), mMaxHeight));

  if (mMinWidth >= 0)
    rect.SetWidth(MAX(r.GetWidth(), mMinWidth));

  if (mMinHeight >= 0)
    rect.SetHeight(MAX(r.GetHeight(), mMinHeight));

  // Prevent the widget from being bigger than the size provided by its parent:
  float w = r.GetWidth();
  float ww = rect.GetWidth();
  float h = r.GetHeight();
  float hh = rect.GetHeight();
  
  if (ww > w)
  {
    ww = w;
  }
  if (hh > h)
  {
    hh = h;
  }

  if (GetFixedAspectRatio())
  {
    // Give good ratio to keep things in proportions
    float tw = mIdealRect.GetWidth();
    float th = mIdealRect.GetHeight();
    float r = 1.0f;
    
    if (hh < th)
    {
      r = hh / th;
      ww = tw * r;
    }
    
    if (ww < tw)
    {
      r = ww / tw;
      hh = th * r;
    }
  }
  
  rect.SetWidth(ww);
  rect.SetHeight(hh);
  
  if (mPosition == nuiNoPosition)
  {
    rect.Move(r.Left(), r.Top());
  }
  else if (mPosition != nuiFill)
  {
    rect.SetPosition(mPosition, r);
  }
  else
  {
    if (mFillRule == nuiFill)
    {
      rect.SetPosition(mPosition, r);
    }
    else if (mFillRule == nuiTile)
    {
      rect = r;
    }
    else
    {
      nuiRect rct = r;
      float ratio,rratio,rw,rh;
      ratio  = (float)rct.GetWidth()  / (float)rct.GetHeight();
      rratio = (float)rect.GetWidth() / (float)rect.GetHeight();

      if (ratio < rratio) 
      {
        rw = (float)rct.GetWidth();
        rh = rw / rratio;
      }

      else 
      {
        rh = (float)rct.GetHeight();
        rw = rratio * rh;
      }

      rect = nuiRect(0.0f, 0.0f, rw, rh);
      rect.SetPosition(mFillRule, r);
    }
  }

  rect.Left()   += GetActualBorderLeft();
  rect.Right()  -= GetActualBorderRight();
  rect.Top()    += GetActualBorderTop();
  rect.Bottom() -= GetActualBorderBottom();

  rect.RoundToNearest();

  return rect;
}

void nuiWidget::SetLayout(const nuiRect& rRect)
{
  if (IsInTransition())
    return;

  mLayoutRect = rRect;

  CheckValid();
  nuiRect rect(GetLayoutForRect(rRect));
  
  if (GetLayoutAnimationDuration() > 0)
  {
    nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(true);
    bool value_already_set = pAnim->GetEndValue() == rect;

    if (pAnim->IsPlaying() && !value_already_set)
    {
      pAnim->Stop();
    }

    if (!(pAnim->IsPlaying() && value_already_set))
    {
      pAnim->SetEndValue(rect);
      nuiDelayedPlayAnim(pAnim, eAnimFromStart, 0.0, 1, eAnimLoopForward);
    }
  }
  else
  {
    InternalSetLayout(rect);
  }
}

void nuiWidget::InternalSetLayout(const nuiRect& rect)
{
  CheckValid();
  bool PositionChanged = (rect.Left() != mRect.Left()) || (rect.Top() != mRect.Top());
  bool SizeChanged = !rect.Size().IsEqual(mRect.Size());
  mNeedSelfLayout = mNeedSelfLayout || SizeChanged;
  
  InternalSetLayout(rect, PositionChanged, SizeChanged);

  if (!mOverrideVisibleRect)
    mVisibleRect = GetOverDrawRect(true, true);
  
  if (PositionChanged && mpParent)
    mpParent->Invalidate();
  
  mNeedSelfLayout = false;
  mNeedLayout = false;
  DebugRefreshInfo();
}

void nuiWidget::SetUserWidth(nuiSize s)
{
  CheckValid();
  if (mHasUserWidth && GetUserWidth() == s)
    return;
  
  mUserRect.SetWidth(s);
  mHasUserWidth = true;
  UserRectChanged();
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiWidget::GetUserWidth()
{
  CheckValid();
  return mUserRect.GetWidth();
}

void nuiWidget::SetUserHeight(nuiSize s)
{
  CheckValid();
  if (mHasUserHeight && GetUserHeight() == s)
    return;
  
  mUserRect.SetHeight(s);
  mHasUserHeight = true;
  UserRectChanged();
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiWidget::GetUserHeight()
{
  CheckValid();
  return mUserRect.GetHeight();
}


// User size
void nuiWidget::SetUserRect(const nuiRect& rRect)
{
  CheckValid();
  if (!(mUserRect == rRect) || !mHasUserPos || !mHasUserSize)
  {
    mUserRect = rRect;
    bool SizeChanged = !mUserRect.Size().IsEqual(mRect.Size());
    bool optim = HasUserRect() && !SizeChanged;
    mHasUserSize = true;
    mHasUserPos = true;
    mHasUserWidth = true;
    mHasUserHeight = true;
    UserRectChanged();

    mIdealRect = mUserRect;

    if (optim)
    {
      if (!IsInSetRect())
      {
        mInSetRect = true;
        SetRect(rRect);
        mInSetRect = false;
      }
      mpParent->Invalidate();
      Invalidate();
    }
    else
    {
      ForcedInvalidateLayout();
    }
    DebugRefreshInfo();
  }
}

const nuiRect& nuiWidget::GetUserRect() const
{
  CheckValid();
  return mUserRect;
}

nuiPoint nuiWidget::GetUserPos() const
{
  CheckValid();
  return nuiPoint(mUserRect.Left(), mUserRect.Top());
}

void nuiWidget::UnsetUserRect()
{
  CheckValid();
  if (mHasUserPos || mHasUserSize)
  {
    mHasUserSize = false;
    mHasUserPos = false;
    mHasUserWidth = false;
    mHasUserHeight = false;
    InvalidateLayout();
    DebugRefreshInfo();
  }
}

void nuiWidget::SetUserSize(nuiSize X,nuiSize Y)
{
  CheckValid();
  if (mHasUserSize && GetUserWidth() == X && GetUserHeight() == Y)
    return;
  
  mUserRect.SetSize(X,Y);
  mHasUserSize = true;
  mHasUserWidth = true;
  mHasUserHeight = true;
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::UnsetUserSize()
{
  CheckValid();
  if (mHasUserSize)
  {
    mHasUserSize = false;
    mHasUserWidth = false;
    mHasUserHeight =false;
    InvalidateLayout();
    DebugRefreshInfo();
  }
}

void nuiWidget::SetUserPos(nuiSize X, nuiSize Y)
{
  CheckValid();
  if (mHasUserPos && mUserRect.Left() == X && mUserRect.Top() == Y)
    return;
  
  mUserRect.MoveTo(X,Y);
  mHasUserPos = true;
  ForcedInvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetUserPos(nuiPoint Pos)
{
  CheckValid();
  SetUserPos(Pos[0], Pos[1]);
}

void nuiWidget::UnsetUserPos()
{
  CheckValid();
  mHasUserPos = false;
  InvalidateLayout();
  DebugRefreshInfo();
}

bool nuiWidget::HasUserRect() const
{
  CheckValid();
  return mHasUserSize && mHasUserPos;
}

bool nuiWidget::HasUserSize() const
{
  CheckValid();
  return mHasUserSize;
}

bool nuiWidget::HasUserWidth() const
{
  CheckValid();
  return mHasUserWidth;
}

bool nuiWidget::HasUserHeight() const
{
  CheckValid();
  return mHasUserHeight;
}



bool nuiWidget::HasUserPos() const
{
  CheckValid();
  return mHasUserPos;
}

void nuiWidget::ForceIdealRect(bool Force)
{
  CheckValid();
  if (mForceIdealSize == Force)
    return;
  
  mForceIdealSize = Force;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::GetHotRect(nuiRect& rRect) const
{
  CheckValid();
  rRect = mHotRect;
}

const nuiRect& nuiWidget::GetHotRect() const
{
  CheckValid();
  return mHotRect;
}

void nuiWidget::SetHotRect(const nuiRect& rRect)
{
  CheckValid();
  mHotRect = rRect;
  HotRectChanged();
  if (mpParent)
  {
    nuiRect r(rRect);
    LocalToLocal(mpParent, r);
    mpParent->OnChildHotRectChanged(this, r);
  }
  DebugRefreshInfo();
}


void nuiWidget::SetSelectionExclusive(bool Exclusive)
{
  CheckValid();
  mSelectionExclusive = Exclusive;
  DebugRefreshInfo();
}

bool nuiWidget::IsSelectionExclusive() const
{
  CheckValid();
  return mSelectionExclusive;
}

void nuiWidget::SetRedrawOnHover(bool RedrawOnHover)
{
  CheckValid();
  mRedrawOnHover = RedrawOnHover;
  DebugRefreshInfo();
}

bool nuiWidget::GetRedrawOnHover() const
{
  CheckValid();
  return mRedrawOnHover;
}

/* Animation Support: */
void nuiWidget::StartAutoDraw()
{
  CheckValid();
  if (!mAutoDraw)
    mGenericWidgetSink.Connect(nuiAnimation::AcquireTimer()->Tick, &nuiWidget::Animate);
  mAutoDraw = true;
  DebugRefreshInfo();
}

void nuiWidget::SetAutoDrawAnimateLayout(bool RecalcLayout)
{
  CheckValid();
  mAnimateLayout = RecalcLayout;
  DebugRefreshInfo();
}

bool nuiWidget::GetAutoDrawAnimateLayout() const
{
  CheckValid();
  return mAnimateLayout;
}

void nuiWidget::StopAutoDraw()
{
  CheckValid();
  nuiTimer* pTimer = nuiAnimation::GetTimer();
  if (pTimer && mAutoDraw)
  {
    mGenericWidgetSink.DisconnectSource(pTimer->Tick);
    nuiAnimation::ReleaseTimer();
  }
  mAutoDraw = false;
  DebugRefreshInfo();
}

void nuiWidget::Animate(const nuiEvent& rEvent)
{
  CheckValid();
  if (mVisible)
  {
    if (mAnimateLayout)
      InvalidateLayout();
    else
      Invalidate();
  }
}

nuiAnimation* nuiWidget::GetAnimation(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.find(rName);

  if (it == mAnimations.end())
    return NULL;

  return (*it).second;
}

void nuiWidget::GetAnimations(std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>& rAnims)
{
  CheckValid();
  rAnims = mAnimations;
}

uint32 nuiWidget::GetAnimCount()
{
  CheckValid();
  return (uint32)mAnimations.size();
}

void nuiWidget::AddAnimation(const nglString& rName, nuiAnimation* pAnim, bool TransitionAnimation)
{
  CheckValid();
  nuiAnimation* pOldAnim = GetAnimation(rName);
  if (pOldAnim)
    delete pOldAnim;

  mAnimations[rName] = pAnim;
  pAnim->SetDeleteOnStop(false); /// Cancel anim delete on stop or we'll crash as soon as the widget is destroyed or the user starts to play the anim once more.
  if (rName == "TRASH")
    mGenericWidgetSink.Connect(pAnim->AnimStop, &nuiWidget::AutoDestroy);
  if (rName == "HIDE")
    mGenericWidgetSink.Connect(pAnim->AnimStop, &nuiWidget::AutoHide);
  
  if (TransitionAnimation)
  {
    mGenericWidgetSink.Connect(pAnim->AnimStart, &nuiWidget::AutoStartTransition);
    mGenericWidgetSink.Connect(pAnim->AnimStop, &nuiWidget::AutoStopTransition);
  }
  
  DebugRefreshInfo();
}

void nuiWidget::DelAnimation(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.find(rName);

  if (it == mAnimations.end())
    return;

  mAnimations.erase(it);
  DebugRefreshInfo();
}

void nuiWidget::ClearAnimations()
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.begin();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator end = mAnimations.end();

  for ( ; it != end; ++it)
    delete (*it).second;

  mAnimations.clear();
  DebugRefreshInfo();
}

void nuiWidget::AnimateAll(bool Set, bool Reset)
{
  CheckValid();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator it = mAnimations.begin();
  std::map<nglString, nuiAnimation*, nglString::NaturalLessFunctor>::iterator end = mAnimations.end();

  for ( ; it != end; ++it)
  {
    if (Reset)
      (*it).second->SetTime(0);

    if (Set)
      (*it).second->Play();
    else
      (*it).second->Stop();
  }
  DebugRefreshInfo();
}


void nuiWidget::ResetAnimation(const nglString& rName)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
    pAnim->SetTime(0);
  DebugRefreshInfo();
}

void nuiWidget::StartAnimation(const nglString& rName, double Time)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
  {
    nuiDelayedPlayAnim(pAnim, eAnimFromStart, Time, 1, eAnimLoopForward);
  }
  DebugRefreshInfo();
}

void nuiWidget::StartAnimation(const nglString& rName, int32 count, nuiAnimLoop loopmode , double Time)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
  {
    nuiDelayedPlayAnim(pAnim, eAnimFromStart, Time, count, loopmode);
  }
  DebugRefreshInfo();
}


void nuiWidget::StopAnimation(const nglString& rName)
{
  CheckValid();
  nuiAnimation* pAnim = GetAnimation(rName);
  if (pAnim)
    pAnim->Stop();
  DebugRefreshInfo();
}

#define LAYOUT_ANIM_NAME "LAYOUT_ANIM"

nuiRectAttributeAnimation* nuiWidget::GetLayoutAnimation(bool CreateIfNotAvailable)
{
  CheckValid();
  if (mpLayoutAnimation)
    return mpLayoutAnimation;

  nuiRectAttributeAnimation* pAnim = (nuiRectAttributeAnimation*)GetAnimation(LAYOUT_ANIM_NAME);
  if (!pAnim && CreateIfNotAvailable)
  {
    pAnim = new nuiRectAttributeAnimation();
    pAnim->SetDuration(0);
    pAnim->SetTargetObject(this);
    pAnim->SetTargetAttribute("LayoutRectUnsafe");
    pAnim->SetCaptureStartOnPlay(true);
    AddAnimation(LAYOUT_ANIM_NAME, pAnim);
  }
  mpLayoutAnimation = pAnim;
  return pAnim;
}

void nuiWidget::SetLayoutAnimationDuration(float duration)
{
  CheckValid();
  nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(duration > 0);
  if (pAnim)
    pAnim->SetDuration(duration);
}

float nuiWidget::GetLayoutAnimationDuration()
{
  CheckValid();
  nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(false);
  if (pAnim)
    return pAnim->GetDuration();
  return 0;
}

void nuiWidget::SetLayoutAnimationEasing(const nuiEasingMethod& rMethod)
{
  CheckValid();
  nuiRectAttributeAnimation* pAnim = GetLayoutAnimation(true);
  if (pAnim)
    pAnim->SetEasing(rMethod);
}


/// Matrix Operations:
nuiMatrix nuiWidget::mIdentityMatrix;

void nuiWidget::AddMatrixNode(nuiMatrixNode* pNode)
{
  CheckValid();
  nuiWidget::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  if (!mpMatrixNodes)
    mpMatrixNodes = new std::vector<nuiMatrixNode*>;

  pNode->Acquire();
  mpMatrixNodes->push_back(pNode);
  mGenericWidgetSink.Connect(pNode->Changed, &nuiWidget::AutoInvalidateLayout);
  
  // Usual clean up needed for the partial redraw to work correctly
  nuiWidget::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();
  
  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
}

void nuiWidget::DelMatrixNode(uint32 index)
{
  if (!mpMatrixNodes)
    return;
  
  CheckValid();
  nuiWidget::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();
  
  mGenericWidgetSink.Disconnect(mpMatrixNodes->at(index)->Changed, &nuiWidget::AutoInvalidateLayout);
  mpMatrixNodes->at(index)->Release();
  mpMatrixNodes->erase(mpMatrixNodes->begin() + index);
  
  // Usual clean up needed for the partial redraw to work correctly
  nuiWidget::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();
  
  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
}


int32 nuiWidget::GetMatrixNodeCount() const
{
  CheckValid();
  if (!mpMatrixNodes)
    return 0;
  return mpMatrixNodes->size();
}


nuiMatrixNode* nuiWidget::GetMatrixNode(uint32 index) const
{
  CheckValid();
  if (mpMatrixNodes)
    return mpMatrixNodes->at(index);
  return nullptr;
}


void nuiWidget::LoadIdentityMatrix()
{
  CheckValid();
  Invalidate();
  
  if (mpMatrixNodes)
  {
    for (uint32 i = 0; i < mpMatrixNodes->size(); i++)
      mpMatrixNodes->at(i)->Release();
    delete mpMatrixNodes;
    mpMatrixNodes = NULL;
  }
  
  Invalidate();
  DebugRefreshInfo();
}

bool nuiWidget::IsMatrixIdentity() const
{
  return !mpMatrixNodes;
}

void nuiWidget::GetMatrix(nuiMatrix& rMatrix) const
{
  CheckValid();
  rMatrix.SetIdentity();
  if (IsMatrixIdentity())
    return;

  for (uint32 i = 0; i < mpMatrixNodes->size(); i++)
    mpMatrixNodes->at(i)->Apply(rMatrix);
}

nuiMatrix nuiWidget::GetMatrix() const
{
  CheckValid();
  nuiMatrix m;
  GetMatrix(m);
  return m;
}

nuiMatrix nuiWidget::_GetMatrix() const
{
  CheckValid();
  return GetMatrix();
}

void nuiWidget::_SetMatrix(nuiMatrix Matrix)
{
  SetMatrix(Matrix);
}

void nuiWidget::SetMatrix(const nuiMatrix& rMatrix)
{
  CheckValid();
  nuiWidget::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();

  // Special case: we only need one simple static matrix node at max
  LoadIdentityMatrix(); // So we load the identity matrix (i.e. clear any existing node)
  if (!rMatrix.IsIdentity()) // If the user wasn't asking for the identity matrix
  {
    AddMatrixNode(new nuiMatrixNode(rMatrix));
  }

  // Usual clean up needed for the partial redraw to work correctly
  nuiWidget::InvalidateRect(GetOverDrawRect(true, true));
  SilentInvalidate();
  
  if (mpParent)
    mpParent->BroadcastInvalidate(this);
  DebugRefreshInfo();
}

void nuiWidget::EnableRenderCache(bool set)
{
  CheckValid();
  if (mUseRenderCache != set)
  {
    mUseRenderCache = set;
    if (mUseRenderCache)
    {
      if (!mpRenderCache)
      {
        mpRenderCache = new nuiMetaPainter();
#ifdef _DEBUG_
        mpRenderCache->DBGSetReferenceObject(this);
#endif
      }
      mpRenderCache->Reset(NULL);
    }
    else
    {
      delete mpRenderCache;
      mpRenderCache = NULL;
    }
    
    Invalidate();
    DebugRefreshInfo();
  }
}

bool nuiWidget::IsRenderCacheEnabled()
{
  CheckValid();
  return mUseRenderCache;
}

const nuiMetaPainter* nuiWidget::GetRenderCache() const
{
  CheckValid();
//  if (mSurfaceEnabled && mpSurface)
//    return mpSurface->GetSurfacePainter();
  return mpRenderCache;
}

void nuiWidget::EnableSurface(bool Set)
{
  CheckValid();
  if (mSurfaceEnabled == Set)
    return;
  mSurfaceEnabled = Set;

  nuiRect r(GetOverDrawRect(true, true).Size());
  UpdateSurface(r);

  Invalidate();
  DebugRefreshInfo();
}

bool nuiWidget::IsSurfaceEnabled() const
{
  CheckValid();
  return mSurfaceEnabled;
}

void nuiWidget::SetColor(nuiWidgetElement element, const nuiColor& rColor)
{
  CheckValid();
  mWidgetElementColors[element] = rColor;
  Invalidate();
  DebugRefreshInfo();
}

void nuiWidget::DelColor(nuiWidgetElement element)
{
  CheckValid();
  mWidgetElementColors.erase(element);
  Invalidate();
  DebugRefreshInfo();
}

nuiColor nuiWidget::GetColor(nuiWidgetElement element)
{
  CheckValid();
  nuiColor col;
  std::map<nuiWidgetElement, nuiColor>::iterator it = mWidgetElementColors.find(element);
  if (it != mWidgetElementColors.end())
    col = it->second;
  else
  {
    if (mpParent)
      col = mpParent->GetColor(element);
    else
    {
      nuiTheme* pTheme = GetTheme();
      NGL_ASSERT(pTheme);
      col = pTheme->GetElementColor(element); // Return the theme color
      pTheme->Release();
    }
  }

  if (mMixAlpha)
  {
    float alpha = GetAlpha();
    col.Multiply(alpha);
  }
  return col;
}

void nuiWidget::SetFillColor(nuiDrawContext* pContext, nuiWidgetElement Element)
{
  CheckValid();
  pContext->SetFillColor(GetColor(Element));
}

void nuiWidget::SetStrokeColor(nuiDrawContext* pContext, nuiWidgetElement Element)
{
  CheckValid();
  pContext->SetStrokeColor(GetColor(Element));
}

void nuiWidget::SetTheme(nuiTheme* pTheme)
{
  CheckValid();
  if (mpTheme)
    mpTheme->Release();
  mpTheme = pTheme;
  if (mpTheme)
    mpTheme->Acquire();
  DebugRefreshInfo();
}

nuiTheme* nuiWidget::GetTheme()
{
  CheckValid();
  // Do we have a local theme?
  if (mpTheme)
  {
    mpTheme->Acquire();
    return mpTheme;
  }
  // Do we have a parent that may have a local theme?
  if (mpParent)
    return mpParent->GetTheme();
  // Revert to the global theme:
  return nuiTheme::GetTheme();
}

void nuiWidget::AutoDestroy(const nuiEvent& rEvent)
{
  CheckValid();
  NGL_ASSERT(mpParent!= nullptr);
  mpParent->DelChild(this);
}

void nuiWidget::AutoHide(const nuiEvent& rEvent)
{
  CheckValid();
  SilentSetVisible(false);
}

void nuiWidget::AutoStartTransition(const nuiEvent& rEvent)
{
  CheckValid();
  StartTransition();
}
               
void nuiWidget::AutoStopTransition(const nuiEvent& rEvent)
{
  CheckValid();
  StopTransition();
}
               

bool nuiWidget::IsDrawingInCache(bool Recurse) 
{ 
  CheckValid();
  if (mSurfaceEnabled)
    return false;
  if (mDrawingInCache)
    return true;
  if (Recurse && mpParent)
    return mpParent->IsDrawingInCache(Recurse);
  return false;
}

#undef max
#undef min


//class nuiWidget::LayoutConstraint
nuiWidget::LayoutConstraint::LayoutConstraint()
{
  mMaxWidth = mMaxHeight = std::numeric_limits<float>::max();
  mMinWidth = mMinHeight = 0;
}

nuiWidget::LayoutConstraint::LayoutConstraint(const LayoutConstraint& rSource)
{
  *this = rSource;
}

nuiWidget::LayoutConstraint& nuiWidget::LayoutConstraint::operator=(const LayoutConstraint& rSource)
{
  mMaxWidth  = rSource.mMaxWidth;
  mMinWidth  = rSource.mMinWidth;
  mMaxHeight = rSource.mMaxHeight;
  mMinHeight = rSource.mMinHeight;

  return *this;
}

bool nuiWidget::LayoutConstraint::operator==(const LayoutConstraint& rSource) const
{
  return ( mMaxWidth  == rSource.mMaxWidth
        && mMinWidth  == rSource.mMinWidth
        && mMaxHeight == rSource.mMaxHeight
        && mMinHeight == rSource.mMinHeight );
}

bool nuiWidget::SetLayoutConstraint(const nuiWidget::LayoutConstraint& rConstraint)
{
  CheckValid();
  if (!(mConstraint == rConstraint))
  {
    mConstraint = rConstraint;
    mNeedIdealRect = mNeedIdealRect || mCanRespectConstraint;
    //InvalidateLayout();
    return true;
  }
  return false;
}


const nuiWidget::LayoutConstraint& nuiWidget::GetLayoutConstraint() const
{
  CheckValid();
  return mConstraint;
}


void nuiWidget::SetDebug(int32 DebugLevel)
{
  CheckValid();
  mDebugLevel = DebugLevel;
  InvalidateLayout();
}

int32 nuiWidget::GetDebug(bool recursive) const
{
  CheckValid();
  if (recursive)
  {
    if (mpParent)
    {
      return MAX(mDebugLevel, mpParent->GetDebug(recursive));
    }
  }
  return mDebugLevel;
}

int32 nuiWidget::_GetDebug() const
{
  CheckValid();
  return mDebugLevel;
}

nuiHotKey* nuiWidget::RegisterHotKeyKey(const nglString& rName, nglKeyCode Trigger, nuiKeyModifier Modifiers, bool Priority /*= false*/, bool FireOnKeyUp /*= false*/, const nglString& rDescription /*= nglString::Empty*/)
{
  CheckValid();
  NGL_ASSERT(GetTopLevel());
  std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
  if (it == mHotKeyEvents.end())
    mHotKeyEvents[rName] = new nuiSimpleEventSource<nuiWidgetActivated>();
  SetWantKeyboardFocus(true);
  return GetTopLevel()->RegisterHotKeyKey(rName, Trigger, Modifiers, Priority, FireOnKeyUp, rDescription);
}

nuiHotKey* nuiWidget::RegisterHotKeyChar(const nglString& rName, nglChar Trigger, nuiKeyModifier Modifiers, bool Priority /*= false*/, bool FireOnKeyUp /*= false*/, const nglString& rDescription /*= nglString::Empty*/)
{
  CheckValid();
  NGL_ASSERT(GetTopLevel());
  std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
  if (it == mHotKeyEvents.end())
    mHotKeyEvents[rName] = new nuiSimpleEventSource<nuiWidgetActivated>();
  SetWantKeyboardFocus(true);
  return GetTopLevel()->RegisterHotKeyChar(rName, Trigger, Modifiers, Priority, FireOnKeyUp, rDescription);
}

nuiHotKey* nuiWidget::GetHotKey(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::const_iterator it = mHotKeyEvents.find(rName);
  if (it != mHotKeyEvents.end())
  {
    return GetTopLevel()->GetHotKey(rName);
  }
  else
  {
    return NULL;
  }
}

nuiSimpleEventSource<nuiWidgetActivated>& nuiWidget::GetHotKeyEvent(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiSimpleEventSource<nuiWidgetActivated>*>::iterator it = mHotKeyEvents.find(rName);
  NGL_ASSERT(it != mHotKeyEvents.end());
  return *(it->second);
}

void nuiWidget::DelHotKey(const nglString& rName)
{
  CheckValid();
  NGL_ASSERT(GetTopLevel());
  GetTopLevel()->DelHotKey(rName);
  mHotKeyEvents.erase(rName);
  if (mHotKeyEvents.empty())
    SetWantKeyboardFocus(false);
}

void nuiWidget::SetMaxIdealWidth(float MaxWidth)
{
  CheckValid();
  if (mMaxIdealWidth == MaxWidth)
    return;
  mMaxIdealWidth = MaxWidth;
  InvalidateLayout();
}

void nuiWidget::SetMaxIdealHeight(float MaxHeight)
{
  CheckValid();
  if (mMaxIdealHeight == MaxHeight)
    return;
  mMaxIdealHeight = MaxHeight;
  InvalidateLayout();
}

void nuiWidget::SetMinIdealWidth(float MinWidth)
{
  CheckValid();
  if (mMinIdealWidth == MinWidth)
    return;
  mMinIdealWidth = MinWidth;
  InvalidateLayout();
}

void nuiWidget::SetMinIdealHeight(float MinHeight)
{
  CheckValid();
  if (mMinIdealHeight == MinHeight)
    return;
  mMinIdealHeight = MinHeight;
  InvalidateLayout();
}

float nuiWidget::GetMaxIdealWidth() const
{
  CheckValid();
  return mMaxIdealWidth;
}

float nuiWidget::GetMaxIdealHeight() const
{
  CheckValid();
  return mMaxIdealHeight;
}

float nuiWidget::GetMinIdealWidth() const
{
  CheckValid();
  return mMinIdealWidth;
}

float nuiWidget::GetMinIdealHeight() const
{
  CheckValid();
  return mMinIdealHeight;
}


void nuiWidget::SetMaxWidth(float MaxWidth)
{
  CheckValid();
  if (mMaxWidth == MaxWidth)
    return;
  mMaxWidth = MaxWidth;
  InvalidateLayout();
}

void nuiWidget::SetMaxHeight(float MaxHeight)
{
  CheckValid();
  if (mMaxHeight == MaxHeight)
    return;
  mMaxHeight = MaxHeight;
  InvalidateLayout();
}

void nuiWidget::SetMinWidth(float MinWidth)
{
  CheckValid();
  if (mMinWidth == MinWidth)
    return;
  mMinWidth = MinWidth;
  InvalidateLayout();
}

void nuiWidget::SetMinHeight(float MinHeight)
{
  CheckValid();
  if (mMinHeight == MinHeight)
    return;
  mMinHeight = MinHeight;
  InvalidateLayout();
}

float nuiWidget::GetMaxWidth() const
{
  CheckValid();
  return mMaxWidth;
}

float nuiWidget::GetMaxHeight() const
{
  CheckValid();
  return mMaxHeight;
}

float nuiWidget::GetMinWidth() const
{
  CheckValid();
  return mMinWidth;
}

float nuiWidget::GetMinHeight() const
{
  CheckValid();
  return mMinIdealHeight;
}


nuiEventSource* nuiWidget::GetEvent(const nglString& rName)
{
  CheckValid();
  std::map<nglString, nuiEventSource*, nglString::LessFunctor>::iterator it = mEventMap.find(rName);
  if (it == mEventMap.end())
    return NULL;
  return (*it).second;
}

void nuiWidget::GetEvents(std::vector<nglString>& rNames)
{
  CheckValid();
  std::map<nglString, nuiEventSource*, nglString::LessFunctor>::iterator it;    
  std::map<nglString, nuiEventSource*, nglString::LessFunctor>::iterator end = mEventMap.end();
  
  for (it = mEventMap.begin(); it != end; ++it)
    rNames.push_back((*it).first);
}

void nuiWidget::AddEvent(const nglString& rName, nuiEventSource& rEvent)
{
  CheckValid();
  mEventMap[rName] = &rEvent;
}

bool nuiWidget::AddEventAction(const nglString& rEventName, nuiEventActionHolder* pActions)
{
  nuiEventSource* pEvent = GetEvent(rEventName);
  if (!pEvent)
    return false;

  pActions->Connect(pEvent, this);
  mEventActions.push_back(pActions);
  return true;
}

void nuiWidget::UpdateLayout()
{
  CheckValid();
  if (IsInSetRect())
    return;

  mNeedSelfLayout = true;
  mNeedIdealRect = true;

  GetIdealRect();
//  nuiRect r(GetRect());
//  mInSetRect = true;
//  SetRect(r);
  SetLayout(mLayoutRect);
//  mInSetRect = false;
  Invalidate();
}

void nuiWidget::SetOverDraw(nuiSize Left, nuiSize Top, nuiSize Right, nuiSize Bottom)
{
  CheckValid();
  if ( ( (Left != mODLeft) || (Right != mODRight) || (Top != mODTop) || (Bottom != mODBottom) ) )
  {
    Invalidate();
    mODLeft = Left;
    mODRight = Right;
    mODTop = Top;
    mODBottom = Bottom;
    Invalidate();
  }
}

void nuiWidget::SetOverDrawLeft(nuiSize border)
{
  CheckValid();
  if (mODLeft == border)
    return;
  
  mODLeft = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetOverDrawTop(nuiSize border)
{
  CheckValid();
  if (mODTop == border)
    return;
  
  mODTop = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetOverDrawRight(nuiSize border)
{
  CheckValid();
  if (mODRight == border)
    return;
  
  mODRight = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

void nuiWidget::SetOverDrawBottom(nuiSize border)
{
  CheckValid();
  if (mODBottom == border)
    return;
  
  mODBottom = border;
  InvalidateLayout();
  DebugRefreshInfo();
}

nuiSize nuiWidget::GetOverDrawLeft() const
{
  CheckValid();
  return mODLeft;
}

nuiSize nuiWidget::GetOverDrawTop() const
{
  CheckValid();
  return mODTop;
}

nuiSize nuiWidget::GetOverDrawRight() const
{
  CheckValid();
  return mODRight;
}

nuiSize nuiWidget::GetOverDrawBottom() const
{
  CheckValid();
  return mODBottom;
}


void nuiWidget::ResetOverDraw()
{
  CheckValid();
  SetOverDraw(0, 0, 0, 0);
}

void nuiWidget::GetOverDraw(nuiSize& Left, nuiSize& Top, nuiSize& Right, nuiSize& Bottom, bool IncludeDecorations) const
{
  CheckValid();
  Left = mODLeft;
  Right = mODRight;
  Top = mODTop;
  Bottom = mODBottom;
  
  if (IncludeDecorations)
  {
    if (mpDecoration && mDecorationEnabled)
    {    
      if (mDecorationMode != eDecorationClientOnly)
      {
        float l , r, t, b, h, v;
        mpDecoration->GetBorders(this, l, r, t, b, h, v);
        Left   = MAX(Left  , l);
        Top    = MAX(Top   , t);
        Right  = MAX(Right , r);
        Bottom = MAX(Bottom, b);
      }
    }
    
    if (mpFocusDecoration)
    {    
      if (mFocusDecorationMode != eDecorationClientOnly)
      {
        float l , r, t, b, h, v;
        mpFocusDecoration->GetBorders(this, l, r, t, b, h, v);
        Left   = MAX(Left  , l);
        Top    = MAX(Top   , t);
        Right  = MAX(Right , r);
        Bottom = MAX(Bottom, b);
      }
    }
  }  
}

nuiRect nuiWidget::GetOverDrawRect(bool LocalRect, bool IncludeDecorations) const
{
  CheckValid();
  nuiRect r(GetRect());
  if (LocalRect)
    r = r.Size();
  
  nuiSize Left = 0;
  nuiSize Right = 0;
  nuiSize Top = 0;
  nuiSize Bottom = 0;

  GetOverDraw(Left, Top, Right, Bottom, IncludeDecorations);
  
  r.Set(r.Left() - Left,
        r.Top() - Top,
        r.Right() + Right,
        r.Bottom() + Bottom,
        false);
  return r;
}

void nuiWidget::EnableInteractiveOverDraw(bool set)
{
  CheckValid();
  if (mInteractiveOD != set)
  {
    mInteractiveOD = set;
    Invalidate();
  }
}

void nuiWidget::EnableInteractiveDecoration(bool set)
{
  CheckValid();
  if (mInteractiveDecoration != set)
  {
    mInteractiveDecoration = set;
    Invalidate();
  }
}

bool nuiWidget::IsOverDrawInteractive() const
{
  CheckValid();
  return mInteractiveOD;
}

bool nuiWidget::IsDecorationInteractive() const
{
  CheckValid();
  return mInteractiveDecoration;
}

// static 
void nuiWidget::SetDefaultDecoration(int32 objectClassIndex, nuiDecorationDelegate dlg)
{
  if (objectClassIndex >= mDefaultDecorations.size())
    mDefaultDecorations.resize(objectClassIndex+1);

  mDefaultDecorations[objectClassIndex] = dlg;
}


void nuiWidget::SetDecoration(const nglString& rName)
{
  CheckValid();
  if (rName.IsEmpty())
  {
    SetDecoration(NULL);
    return;
  }

  nuiDecoration* pDecoration = nuiDecoration::Get(rName);
  if (!pDecoration)
  {
    nuiColor col;
    if (col.SetValue(rName))
    {
      pDecoration = new nuiColorDecoration(rName, rName, 0, nuiColor(), eFillShape, nuiBlendTransp, nuiRect());
    }
    else // Try to load a CSS description for an object:
    {
      nuiObject* pObj = nuiCSS::CreateObject(rName);
      pDecoration = dynamic_cast<nuiDecoration*> (pObj);
      if (!pDecoration)
        delete pObj;
    }
  }
  SetDecoration(pDecoration, mDecorationMode, true);
}

void nuiWidget::SetDecorationMode(nuiDecorationMode Mode)
{
  CheckValid();
  if (mDecorationMode == Mode)
    return;
  
  mDecorationMode = Mode;
  InvalidateLayout();
}

const nglString& nuiWidget::GetDecorationName() const
{
  CheckValid();
  if (mpDecoration)
    return mpDecoration->GetObjectName();
  return nglString::Null;
}

nuiDecorationMode nuiWidget::GetDecorationMode() const
{
  CheckValid();
  return mDecorationMode;
}

void nuiWidget::SetDecoration(nuiDecoration* pDecoration, nuiDecorationMode Mode, bool AlreadyAcquired)
{
  if (mpDecoration == pDecoration && Mode == GetDecorationMode())
    return;

  if (pDecoration && !AlreadyAcquired)
    pDecoration->Acquire();
  if (mpDecoration)
    mpDecoration->Release();
  
  mpDecoration = pDecoration;

  SetDecorationMode(Mode);
  
  InvalidateLayout();
}

nuiDecoration* nuiWidget::GetDecoration() const
{
  CheckValid();
  return mpDecoration;
}


void nuiWidget::EnableDecoration(bool set)
{
  CheckValid();
  if (mDecorationEnabled != set)
  {
    mDecorationEnabled = set;
    InvalidateLayout();
  }
}

bool nuiWidget::IsDecorationEnabled() const
{
  CheckValid();
  return mDecorationEnabled;
}




/// Focus Decoration:
void nuiWidget::SetFocusDecoration(const nglString& rName)
{
  CheckValid();
  nuiDecoration* pDecoration = nuiDecoration::Get(rName);
  SetFocusDecoration(pDecoration, mFocusDecorationMode);
}

void nuiWidget::SetFocusDecorationMode(nuiDecorationMode Mode)
{
  CheckValid();
  mFocusDecorationMode = Mode;
  InvalidateLayout();
}

const nglString& nuiWidget::GetFocusDecorationName() const
{
  CheckValid();
  if (mpFocusDecoration)
    return mpFocusDecoration->GetObjectName();
  return nglString::Null;
}

nuiDecorationMode nuiWidget::GetFocusDecorationMode() const
{
  CheckValid();
  return mFocusDecorationMode;
}

void nuiWidget::SetFocusDecoration(nuiDecoration* pDecoration, nuiDecorationMode Mode)
{
  CheckValid();
  if (pDecoration)
    pDecoration->Acquire();
  if (mpFocusDecoration)
    mpFocusDecoration->Release();
  
  mpFocusDecoration = pDecoration;
  
  SetFocusDecorationMode(Mode);
  
  InvalidateLayout();
}

nuiDecoration* nuiWidget::GetFocusDecoration() const
{
  CheckValid();
  return mpFocusDecoration;
}


//////// TopLevel Management:



void nuiWidget::ConnectTopLevel()
{
  CheckValid();
}

void nuiWidget::DisconnectTopLevel()
{
  CheckValid();
}

void nuiWidget::ApplyCSSForStateChange(uint32 MatchersTag)
{
  CheckValid();
  // Apply CSS, do default stuff, etc...
  if (!IsTrashed(true))
  {
    nuiTopLevel* pTopLevel = GetTopLevel();
    if (pTopLevel)
      pTopLevel->PrepareWidgetCSS(this, true, NUI_WIDGET_MATCHTAG_ALL);
  }
}

void nuiWidget::ResetCSSPass()
{
  CheckValid();
  InternalResetCSSPass();
}

void nuiWidget::IncrementCSSPass()
{
  CheckValid();
  mCSSPasses++;
}

uint32 nuiWidget::GetCSSPass() const
{
  CheckValid();
  return mCSSPasses;
}

void nuiWidget::DrawFocus(nuiDrawContext* pContext, bool FrontOrBack)
{
  CheckValid();

  if (!IsFocusVisible())
    return;
  
  if (mpFocusDecoration)
  {
    nuiRect sizerect(GetVisibleRect().Size());
    mpFocusDecoration->ClientToGlobalRect(sizerect, this);
    if (FrontOrBack)
      mpFocusDecoration->DrawFront(pContext, this, sizerect);
    else
      mpFocusDecoration->DrawBack(pContext, this, sizerect);
  }
  else
  {
    if (FrontOrBack)
    {
      nuiRect rect(GetVisibleRect());
      rect.Bottom() -= 1;
      rect.Top() += 0;
      rect.Left() += 0;
      rect.Right() -= 1;
      
      pContext->ResetState();
      pContext->SetLineWidth(2);
      pContext->SetBlendFunc(nuiBlendTransp);
      pContext->EnableBlending(true);
      //pContext->EnableTexturing(false);
      pContext->SetStrokeColor(nuiColor(64, 64, 255, ToBelow(128 * GetMixedAlpha())));
      
      nuiShape shp;
      shp.AddRect(rect);
      
      pContext->DrawShape(&shp, eStrokeShape);
    }
  }
}

void nuiWidget::SetFocusVisible(bool set)
{
  CheckValid();
  mShowFocus = set;
  Invalidate();
}

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
bool nuiWidget::mShowFocusDefault = false;
#else
bool nuiWidget::mShowFocusDefault = true;
#endif

void nuiWidget::SetFocusVisibleDefault(bool set)
{
  mShowFocusDefault = set;
}


bool nuiWidget::IsFocusVisible() const
{
  CheckValid();
  return mShowFocus && mShowFocusDefault;
}


void nuiWidget::SetClickThru(bool set)
{
  CheckValid();
  mClickThru = set;  
}

bool nuiWidget::GetClickThru() const
{
  CheckValid();
  return mClickThru;
}


void nuiWidget::AddInvalidRect(const nuiRect& rRect)
{
  CheckValid();
//  NGL_OUT("+++ AddInvalidRect in %s %s %s\n", GetObjectClass().GetChars(), GetObjectName().GetChars(), rRect.GetValue().GetChars());
  int count = mDirtyRects.size();
  
  nuiRect intersect;
  nuiSize surface = rRect.GetSurface();
  if (surface == 0.0f)
    return;
  
  for (int i = 0; i<count; i++)
  {
    if (intersect.Intersect(rRect, mDirtyRects[i]))
    {
      // The rectangles intersect so we create one big out of them
      nuiRect u;
      u.Union(rRect, mDirtyRects[i]);
      // Let's remove the coalesced rect from the list
      mDirtyRects.erase(mDirtyRects.begin() + i);
      // Try to add the new big rect to the list:
      AddInvalidRect(u);
      return;
    }
  }
  
  // Found no rect to blend into, let's create a new one:
//  NGL_OUT("--- AddInvalidRect OK %s\n", rRect.GetValue().GetChars());
  mDirtyRects.push_back(rRect);
}

bool nuiWidget::GetAutoClip() const
{
  return mAutoClip;
}

void nuiWidget::SetAutoClip(bool set)
{
  if (mAutoClip == set)
    return;
  mAutoClip = set;
  Invalidate();
}


void nuiWidget::AutoInvalidate(const nuiEvent& rEvent)
{
  Invalidate();
}

void nuiWidget::AutoInvalidateLayout(const nuiEvent& rEvent)
{
  InvalidateLayout();
}


// ***************************************************************************

void nuiWidget::OnFinalize()
{
  if (!IsTrashed())
    CallOnTrash();
}

// static 
std::vector<nuiDecorationDelegate> nuiWidget::mDefaultDecorations;

void nuiWidget::ClearDefaultDecorations()
{
  mDefaultDecorations.clear();
}

float nuiWidget::GetScale() const
{
  nuiTopLevel* pTop = GetTopLevel();
  if (pTop)
    return pTop->GetScale();
  return nuiGetScaleFactor();
}

float nuiWidget::GetScaleInv() const
{
  nuiTopLevel* pTop = GetTopLevel();
  if (pTop)
    return pTop->GetScaleInv();
  return nuiGetInvScaleFactor();
}


class nuiWidgetIterator : public nuiWidget::Iterator
{
public:
  nuiWidgetIterator(nuiWidget* pSource, bool DoRefCounting);
  nuiWidgetIterator(const nuiWidgetIterator& rIterator);
  virtual ~nuiWidgetIterator();

  virtual nuiWidgetPtr GetWidget() const;

  nuiWidgetIterator& operator = (const nuiWidgetIterator& rIterator);

  void Increment()
  {
    mIndex++;
  }

  void Decrement()
  {
    mIndex--;
  }
protected:
  int32 mIndex;
private:
  friend class nuiWidget;
  bool SetIndex(int32 index);
  int32 GetIndex() const;
};

class nuiWidgetConstIterator : public nuiWidget::ConstIterator
{
public:
  nuiWidgetConstIterator(const nuiWidget* pSource, bool DoRefCounting);
  nuiWidgetConstIterator(const nuiWidgetConstIterator& rIterator);
  virtual ~nuiWidgetConstIterator();

  virtual nuiWidgetPtr GetWidget() const;

  nuiWidgetConstIterator& operator = (const nuiWidgetConstIterator& rIterator);

  void Increment()
  {
    mIndex++;
  }

  void Decrement()
  {
    mIndex--;
  }
protected:
  int32 mIndex;
private:
  friend class nuiWidget;
  bool SetIndex(int32 index);
  int32 GetIndex() const;
};

typedef nuiWidgetIterator* nuiWidgetIteratorPtr;
typedef nuiWidgetConstIterator* nuiWidgetConstIteratorPtr;


bool nuiWidget::AddChild(nuiWidgetPtr pChild)
{
  CheckValid();
  if (GetDebug())
  {
    NGL_OUT("[%s] Add Child %p <--- %p (%s / %s)\n", GetObjectClass().GetChars(), this, pChild, pChild->GetObjectClass().GetChars(), pChild->GetObjectName().GetChars());
  }
  pChild->Acquire();
  nuiWidget* pParent = pChild->GetParent();
  NGL_ASSERT(pParent != this);

  int32 capacity = mpChildren.capacity();
  int32 size = mpChildren.size();
  if (size == capacity)
  {
    if (size < 128)
    {
      mpChildren.reserve(size * 2);
    }
    else
    {
      mpChildren.reserve(size + 128);
    }
  }

  mpChildren.push_back(pChild);
  if (pParent)
    pParent->DelChild(pChild); // Remove from previous parent...

  pChild->SetParent(this);
  ChildAdded(this, pChild);
  Invalidate();
  InvalidateLayout();

  DebugRefreshInfo();
  return true;
}

bool nuiWidget::DelChild(nuiWidgetPtr pChild)
{
  CheckValid();
  NGL_ASSERT(pChild->GetParent() == this)


  if (GetDebug())
  {
    NGL_OUT("[%s] Del Child %p <--- %p (%s / %s)\n", GetObjectClass().GetChars(), this, pChild, pChild->GetObjectClass().GetChars(), pChild->GetObjectName().GetChars());
  }

  nuiWidgetList::iterator it  = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    if (*it == pChild)
    {
      mpChildren.erase(it);
      if (!pChild->IsTrashed())
      {
        nuiTopLevel* pRoot = GetTopLevel();
        pChild->Trashed();
        Invalidate();

        if (pRoot)
          pRoot->AdviseObjectDeath(pChild);
        pChild->SetParent(NULL);
      }
      ChildDeleted(this, pChild);
      InvalidateLayout();
      DebugRefreshInfo();
      pChild->Release();
      return true;
    }
  }
  DebugRefreshInfo();
  return false;
}

int nuiWidget::GetChildrenCount() const
{
  CheckValid();
  return mpChildren.size();
}

nuiWidgetPtr nuiWidget::GetChild(int index)
{
  CheckValid();
  NGL_ASSERT(index >= 0);
  NGL_ASSERT(index < mpChildren.size());
  return mpChildren[index];
}

bool nuiWidget::Clear()
{
  CheckValid();
  // start by trashing everybody
  nuiWidget::ChildrenCallOnTrash();

  // then, clear the container
  int childCount = GetChildrenCount();
  for (childCount--; childCount >= 0; childCount--)
  {
    nuiWidget* pWidget = GetChild(childCount);
    if (pWidget)
    {
      DelChild(pWidget);
    }
  }
  mpChildren.clear();
  InvalidateLayout();
  DebugRefreshInfo();
  return true;
}


nuiWidget::Iterator* nuiWidget::GetFirstChild(bool DoRefCounting)
{
  CheckValid();
  IteratorPtr pIt;
  pIt = new nuiWidgetIterator(this, DoRefCounting);
  bool valid = !mpChildren.empty();
  pIt->SetValid(valid);
  if (valid)
    ((nuiWidgetIterator*)pIt)->SetIndex(0);
  return pIt;
}

nuiWidget::ConstIterator* nuiWidget::GetFirstChild(bool DoRefCounting) const
{
  CheckValid();
  nuiWidgetConstIteratorPtr pIt;
  pIt = new nuiWidgetConstIterator(this, DoRefCounting);
  bool valid = !mpChildren.empty();
  pIt->SetValid(valid);
  if (valid)
    pIt->SetIndex(0);
  return pIt;
}

nuiWidget::Iterator* nuiWidget::GetLastChild(bool DoRefCounting)
{
  CheckValid();
  IteratorPtr pIt;
  pIt = new nuiWidgetIterator(this, DoRefCounting);
  if (!mpChildren.empty())
  {
    ((nuiWidgetIterator*)pIt)->SetIndex(mpChildren.size() - 1);
    pIt->SetValid(true);
  }
  else
  {
    pIt->SetValid(false);
  }
  return pIt;
}

nuiWidget::ConstIterator* nuiWidget::GetLastChild(bool DoRefCounting) const
{
  CheckValid();
  nuiWidgetConstIteratorPtr pIt;
  pIt = new nuiWidgetConstIterator(this, DoRefCounting);
  if (!mpChildren.empty())
  {
    pIt->SetIndex(mpChildren.size() - 1);
    pIt->SetValid(true);
  }
  else
  {
    pIt->SetValid(false);
  }
  return pIt;
}

bool nuiWidget::GetNextChild(nuiWidget::IteratorPtr pIterator)
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  ((nuiWidgetIterator*)pIterator)->Increment();
  if (((nuiWidgetIterator*)pIterator)->GetIndex() >= mpChildren.size())
  {
    pIterator->SetValid(false);
    return false;
  }
  pIterator->SetValid(true);
  return true;
}

bool nuiWidget::GetNextChild(nuiWidget::ConstIteratorPtr pIterator) const
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  ((nuiWidgetConstIterator*)pIterator)->Increment();
  if (((nuiWidgetConstIterator*)pIterator)->GetIndex() >= mpChildren.size())
  {
    pIterator->SetValid(false);
    return false;
  }
  pIterator->SetValid(true);
  return true;
}

bool nuiWidget::GetPreviousChild(nuiWidget::IteratorPtr pIterator)
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  if (((nuiWidgetIterator*)pIterator)->GetIndex() <= 0)
  {
    pIterator->SetValid(false);
    return false;
  }

  ((nuiWidgetIterator*)pIterator)->Decrement();

  pIterator->SetValid(true);
  return true;
}

bool nuiWidget::GetPreviousChild(nuiWidget::ConstIteratorPtr pIterator) const
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  if (((nuiWidgetConstIterator*)pIterator)->GetIndex() <= 0)
  {
    pIterator->SetValid(false);
    return false;
  }

  ((nuiWidgetConstIterator*)pIterator)->Decrement();

  pIterator->SetValid(true);
  return true;
}

void nuiWidget::RaiseChild(nuiWidgetPtr pChild)
{
  CheckValid();

  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      nuiWidgetList::iterator next = it;
      ++next;
      mpChildren.erase(it);
      mpChildren.insert(next, pItem);
      Invalidate();
      DebugRefreshInfo();
      return;
    }
  }
  DebugRefreshInfo();
}

void nuiWidget::LowerChild(nuiWidgetPtr pChild)
{
  CheckValid();
  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  nuiWidgetList::iterator previous = it;
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      if (previous != mpChildren.begin())
      {
        nuiWidgetPtr pPrevious = *previous;
        mpChildren.erase(previous);
        mpChildren.insert(it, pPrevious);
        Invalidate();
      }
      DebugRefreshInfo();
      return;
    }
    previous = it;
  }
  DebugRefreshInfo();
}

void nuiWidget::RaiseChildToFront(nuiWidgetPtr pChild)
{
  CheckValid();
  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      mpChildren.erase(it);
      mpChildren.push_back(pItem);
      Invalidate();
      DebugRefreshInfo();
      return;
    }
  }
  DebugRefreshInfo();
}

void nuiWidget::LowerChildToBack(nuiWidgetPtr pChild)
{
  CheckValid();
  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      mpChildren.erase(it);
      mpChildren.insert(mpChildren.begin(), pItem);
      Invalidate();
      DebugRefreshInfo();
      return;
    }
  }
  DebugRefreshInfo();
}

///////////////////////
////// nuiWidget::Iterator

nuiWidgetIterator::nuiWidgetIterator(nuiWidget* pSource, bool DoRefCounting)
: nuiWidget::Iterator(pSource, DoRefCounting), mIndex(-1)
{
  mValid = false;
}

nuiWidgetConstIterator::nuiWidgetConstIterator(const nuiWidget* pSource, bool DoRefCounting)
: nuiWidget::ConstIterator(pSource, DoRefCounting), mIndex(-1)
{
  mValid = false;
}

nuiWidgetIterator::nuiWidgetIterator(const nuiWidgetIterator& rIterator)
: nuiWidget::Iterator(rIterator)
{
  mIndex = rIterator.mIndex;
}

nuiWidgetConstIterator::nuiWidgetConstIterator(const nuiWidgetConstIterator& rIterator)
: nuiWidget::ConstIterator(rIterator)
{
  mIndex = rIterator.mIndex;
}

nuiWidgetIterator& nuiWidgetIterator::operator = (const nuiWidgetIterator& rIterator)
{
  *((nuiWidget::Iterator*)this) = rIterator;
  mIndex = rIterator.mIndex;
  return *this;
}

nuiWidgetConstIterator& nuiWidgetConstIterator::operator = (const nuiWidgetConstIterator& rIterator)
{
  *((nuiWidget::ConstIterator*)this) = rIterator;
  mIndex = rIterator.mIndex;
  return *this;
}

bool nuiWidgetIterator::SetIndex(int32 index)
{
  mIndex = index;
  return true;
}

bool nuiWidgetConstIterator::SetIndex(int32 index)
{
  mIndex = index;
  return true;
}

int32 nuiWidgetIterator::GetIndex() const
{
  return mIndex;
}

int32 nuiWidgetConstIterator::GetIndex() const
{
  return mIndex;
}

nuiWidgetIterator::~nuiWidgetIterator()
{
}

nuiWidgetConstIterator::~nuiWidgetConstIterator()
{
}

nuiWidgetPtr nuiWidgetIterator::GetWidget() const
{
  return IsValid() ? (nuiWidget*)mpSource->GetChild(mIndex) : NULL;
}

nuiWidgetPtr nuiWidgetConstIterator::GetWidget() const
{
  return IsValid() ? const_cast<nuiWidget*>((nuiWidget*)mpSource)->GetChild(mIndex) : NULL;
}








void nuiWidget::CallOnTrash()
{
  CheckValid();
  ChildrenCallOnTrash();
  mTrashed = true;

  while (!mHotKeyEvents.empty())
  {
    DelHotKey(mHotKeyEvents.begin()->first);
  }

  nuiTopLevel* pRoot = GetTopLevel();
  if (pRoot)
  {
    //NGL_OUT("nuiWidget OnTrash [0x%x '%s']\n", this, GetObjectClass().GetChars());
    pRoot->AdviseObjectDeath(this);
  }
  else
  {
    //NGL_OUT("nuiWidget OnTrash NO ROOT! [0x%x '%s']\n", this, GetObjectClass().GetChars());
  }

  OnTrash();
}


void nuiWidget::ChildrenCallOnTrash()
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem)
      pItem->CallOnTrash();
  }
  delete pIt;
}

nuiWidgetPtr nuiWidget::GetRoot() const
{
  CheckValid();
  if (mpParent)
    return mpParent->GetRoot();
  else
    return const_cast<nuiWidgetPtr>(this);
}

nuiWidgetPtr nuiWidget::GetChild(nuiSize X, nuiSize Y)
{
  CheckValid();
  X -= mRect.mLeft;
  Y -= mRect.mTop;

  IteratorPtr pIt;
  for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem && pItem->IsInsideFromSelf(X,Y))
    {
      delete pIt;
      nuiWidgetPtr pContainer = dynamic_cast<nuiWidgetPtr>(pItem);
      if (pContainer)
        return pContainer->GetChild(X,Y);
      else
        return pItem;
    }
  }
  delete pIt;

  return this;
}

void nuiWidget::GetChildren(nuiSize X, nuiSize Y, nuiWidgetList& rChildren, bool DeepSearch)
{
  CheckValid();
  X -= mRect.mLeft;
  Y -= mRect.mTop;

  IteratorPtr pIt;
  for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem && pItem->IsInsideFromParent(X,Y))
    {
      if (DeepSearch)
      {
        nuiWidgetPtr pContainer = dynamic_cast<nuiWidgetPtr>(pItem);
        if (pContainer)
          pContainer->GetChildren(X, Y, rChildren, DeepSearch);
      }
      rChildren.push_back(pItem);
    }
  }
  delete pIt;
}



nuiWidgetPtr nuiWidget::GetChildIf(nuiSize X, nuiSize Y, TestWidgetFunctor* pFunctor)
{
  CheckValid();
  X -= mRect.mLeft;
  Y -= mRect.mTop;

  IteratorPtr pIt;
  for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem && pItem->IsInsideFromParent(X,Y))
    {
      nuiWidgetPtr pContainer = dynamic_cast<nuiWidgetPtr>(pItem);
      if (pContainer)
      {
        nuiWidget* pWidget = pContainer->GetChildIf(X,Y, pFunctor);
        if (pWidget)
        {
          delete pIt;
          return pWidget;
        }
      }
      else
      {
        if ((*pFunctor)(pItem))
        {
          delete pIt;
          return pItem;
        }
      }
    }
  }
  delete pIt;

  if ((*pFunctor)(this))
    return this;

  return NULL;
}


nuiWidgetPtr nuiWidget::GetChild(const nglString& rName, bool ResolveNameAsPath)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetObjectName() == rName)
    {
      delete pIt;
      return pItem;
    }
  }
  delete pIt;

  if (!ResolveNameAsPath) // Are we allowed to search the complete tree?
    return NULL;

  nuiWidgetPtr pNode = this;
  nglString name = rName;

  if (name[0] == '/')
  {
    // Get the root of the tree:
    pNode = GetRoot();

    name.DeleteLeft(1); // Remove the '/'
  }

  // Get all the nodes and remove the slashes:
  std::vector<nglString> tokens;
  name.Tokenize(tokens, '/');

  size_t i;
  size_t count = tokens.size();
  for (i = 0; i < count; i++)
  {
    nglString& rTok = tokens[i];
    //Node* pOld = pNode;
    if (rTok == "..")
      pNode = pNode->GetParent();
    else
      pNode = pNode->GetChild(rTok, false);

    if (!pNode)
    {
      //NUI_OUT("Tried to find %s on %s", rTok.GetChars(), pOld->GetParamCString(ParamIds::Name));
      return NULL;
    }
  }

  return pNode;
}

nuiWidgetPtr nuiWidget::SearchForChild(const nglString& rName, bool recurse )
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetObjectName() == rName)
    {
      delete pIt;
      return pItem;
    }
  }
  delete pIt;

  if (!recurse) // Are we allowed to search the complete tree?
    return NULL;

  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    nuiWidgetPtr pContainer = NULL;
    pContainer = dynamic_cast<nuiWidgetPtr>(pItem);
    if (pContainer)
    {
      nuiWidgetPtr pWidget = pContainer->SearchForChild(rName,recurse);
      if (pWidget)
      {
        delete pIt;
        return pWidget;
      }
    }
  }
  delete pIt;

  return NULL;
}

void nuiWidget::CallConnectTopLevel(nuiTopLevel* pTopLevel)
{
  CheckValid();

  // Apply CSS, do default stuff, etc...
  if (HasFocus())
    pTopLevel->SetFocus(this);
  pTopLevel->PrepareWidgetCSS(this, false, NUI_WIDGET_MATCHTAG_ALL);
  ConnectTopLevel();

  // cal delegate for default decoration, if the user has not set any decoration, and if there is a default decoration
  int32 index = GetObjectClassNameIndex();
  if (!GetDecoration() && mDecorationEnabled && (mDefaultDecorations.size() > index))
  {
    nuiDecorationDelegate dlg = mDefaultDecorations[index];
    if (dlg)
      dlg(this);
  }

  StartAnimation("SHOW");

  IteratorPtr pIt;
  for (pIt = GetFirstChild(true); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    pIt->GetWidget()->CallConnectTopLevel(pTopLevel);
  }
  delete pIt;
}

void nuiWidget::CallDisconnectTopLevel(nuiTopLevel* pTopLevel)
{
  CheckValid();

  if (HasFocus())
    UnFocus();
  pTopLevel->DisconnectWidget(this);
  DisconnectTopLevel();

  IteratorPtr pIt;
  for (pIt = GetFirstChild(true); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    pIt->GetWidget()->CallDisconnectTopLevel(pTopLevel);
  }
  delete pIt;
}

void nuiWidget::InvalidateChildren(bool Recurse)
{
  CheckValid();
  IteratorPtr pIt;
  if (Recurse)
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      nuiWidgetPtr pCont = dynamic_cast<nuiWidgetPtr>(pItem);
      if (pCont)
        pCont->InvalidateChildren(Recurse);
      pItem->Invalidate();
    }
  }
  else
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      pItem->Invalidate();
    }
  }
  delete pIt;
}

void nuiWidget::SilentInvalidateChildren(bool Recurse)
{
  CheckValid();
  IteratorPtr pIt;
  if (Recurse)
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      nuiWidgetPtr pCont = dynamic_cast<nuiWidgetPtr>(pItem);
      if (pCont)
        pCont->SilentInvalidateChildren(Recurse);
      pItem->SilentInvalidate();
    }
  }
  else
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      pItem->SilentInvalidate();
    }
  }
  delete pIt;
}

bool nuiWidget::Draw(nuiDrawContext* pContext)
{
  CheckValid();
  return DrawChildren(pContext);
}

bool nuiWidget::DrawChildren(nuiDrawContext* pContext)
{
  CheckValid();
  IteratorPtr pIt;

  if (mReverseRender)
  {
    for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (pItem)
        DrawChild(pContext, pItem);
    }
    delete pIt;
  }
  else
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (pItem)
        DrawChild(pContext, pItem);
    }
    delete pIt;
  }
  return true;
}

void nuiWidget::DrawChild(nuiDrawContext* pContext, nuiWidget* pChild)
{
  CheckValid();
  float x,y;

  x = (float)pChild->GetRect().mLeft;
  y = (float)pChild->GetRect().mTop;

  bool matrixchanged = false;
  if (x != 0 || y != 0)
  {
    pContext->PushMatrix();
    pContext->Translate( x, y );
    matrixchanged = true;
  }

  nuiPainter* pPainter = pContext->GetPainter();
  if (mpSavedPainter)
    pContext->SetPainter(mpSavedPainter);

  pChild->DrawWidget(pContext);

  if (mpSavedPainter)
    pContext->SetPainter(pPainter);

  if (IsDrawingInCache(true))
  {
    nuiMetaPainter* pMetaPainter = dynamic_cast<nuiMetaPainter*>(pPainter);
    if (pMetaPainter)
      pMetaPainter->DrawChild(pContext, pChild);
  }

  if (matrixchanged)
  {
    pContext->PopMatrix();
  }
}

////// Private event management:
bool nuiWidget::DispatchMouseClick(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return false;

  bool hasgrab = HasGrab(rInfo.TouchId);
  if (IsDisabled() && !hasgrab)
    return false;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  if (CallPreMouseClicked(info))
  {
    Grab();
    return true;
  }

  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    if (!hasgrab)
    {
      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (IsEnabled() && !HasGrab(rInfo.TouchId))
          {
            if (pItem->DispatchMouseClick(rInfo))
            {
              delete pIt;
              return true;
            }
          }
        }
      }
      delete pIt;
    }

    nglMouseInfo info(rInfo);
    GlobalToLocal(info.X, info.Y);
    if (PreClicked(info))
    {
      Grab();
      return true;
    }
    bool ret = MouseClicked(info);
    ret |= Clicked(info);
    ret = ret | (!mClickThru);
    if (ret)
      Grab();

    return ret;
  }
  return false;
}

bool nuiWidget::DispatchMouseCanceled(nuiWidgetPtr pThief, const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (mTrashed)
    return false;

  bool hasgrab = HasGrab(rInfo.TouchId);

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  PreClickCanceled(info);

  IteratorPtr pIt;
  for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem)
    {
      pItem->DispatchMouseCanceled(pThief, rInfo);
    }
  }
  delete pIt;

  if (pThief != this)
  {
    GlobalToLocal(info.X, info.Y);
    PreClickCanceled(info);
    bool ret = MouseCanceled(info);
    ret |= ClickCanceled(info);
    ret = ret | (!mClickThru);
    return ret;
  }

  return false;
}

bool nuiWidget::DispatchMouseUnclick(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return false;

  bool hasgrab = HasGrab(rInfo.TouchId);
  if (IsDisabled() && !hasgrab)
    return false;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);
  // Get a chance to preempt the mouse event before the children get it:
  if (CallPreMouseUnclicked(info))
  {
    Ungrab();
    return true;
  }

  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    if (!hasgrab)
    {
      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (IsEnabled())
          {
            if ((pItem)->DispatchMouseUnclick(rInfo))
            {
              delete pIt;
              return true;
            }
          }
        }
      }
      delete pIt;
    }

    bool res = PreUnclicked(info);
    if (!res)
    {
      res = MouseUnclicked(info);
      res |= Unclicked(info);
    }

    res = res | (!mClickThru);
    if (res)
      Ungrab();
    return res;
  }
  return false;
}

nuiWidgetPtr nuiWidget::DispatchMouseMove(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return NULL;

  nuiWidgetPtr pHandled=NULL;
  bool inside=false,res=false;
  bool hasgrab = HasGrab(rInfo.TouchId);

  if (IsDisabled() && !hasgrab)
    return NULL;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  if (CallPreMouseMoved(info))
    return this;

  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    inside = true;

    // If the object has the grab we should not try to notify its children of mouse events!
    if (!hasgrab)
    {

      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (pItem->IsVisible())
          {
            pHandled = pItem->DispatchMouseMove(rInfo);
          }
        }
        if (pHandled)
        {
          // stop as soon as someone caught the event
          delete pIt;
          return pHandled;
        }
      }
      delete pIt;
    }

    res = MouseMoved(info);
    res |= MovedMouse(info);
  }
  else
  {
    if (GetHover())
    {
      res = MouseMoved(info);
      res |= MovedMouse(info);
    }
  }

  if (!pHandled && (res | (!mClickThru)) && inside)
  {
    nuiTopLevelPtr pRoot = GetTopLevel();
    if (pRoot)
      return this;
  }

  if (pHandled)
    return pHandled;

  return (res && inside) ? this : NULL;
}

nuiWidgetPtr nuiWidget::DispatchMouseWheelMove(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return NULL;

  nuiWidgetPtr pHandled=NULL;
  bool inside=false,res=false;
  bool hasgrab = HasGrab(rInfo.TouchId);

  if (IsDisabled() && !hasgrab)
    return NULL;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  if (CallPreMouseWheelMoved(info))
    return this;

  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    inside = true;

    // If the object has the grab we should not try to notify its children of mouse events!
    if (!hasgrab)
    {

      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (pItem->IsVisible())
          {
            pHandled = pItem->DispatchMouseWheelMove(rInfo);
          }
        }
        if (pHandled)
        {
          // stop as soon as someone caught the event
          delete pIt;
          return pHandled;
        }
      }
      delete pIt;
    }

    res = MouseWheelMoved(info);
    res |= WheelMovedMouse(info);
  }
  else
  {
    if (GetHover())
    {
      res = MouseWheelMoved(info);
      res |= WheelMovedMouse(info);
    }
  }

  if (!pHandled && (res | (!mClickThru)) && inside)
  {
    nuiTopLevelPtr pRoot = GetTopLevel();
    if (pRoot)
      return this;
  }

  if (pHandled)
    return pHandled;

  return (res && inside) ? this : NULL;
}


void nuiWidget::SetAlpha(float Alpha)
{
  CheckValid();
  if (Alpha == mAlpha)
    return;
  const float a = nuiClamp(Alpha, 0.0f, 1.0f);
  if (mAlpha == a)
    return;
  mAlpha = a;
  Invalidate();
  SilentInvalidateChildren(true);
  DebugRefreshInfo();
}

void nuiWidget::SetEnabled(bool set)
{
  CheckValid();
  if (set == mEnabled)
    return;
  if (mEnabled == set)
    return;

  mEnabled = set;

  if (mEnabled)
  {
    Enabled();
    StartAnimation("ENABLED");
  }
  else
  {
    Disabled();
    StartAnimation("DISABLED");
  }

  StateChanged();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
  SilentInvalidateChildren(true);
  Invalidate();
  DebugRefreshInfo();
}

void nuiWidget::SetSelected(bool set)
{
  CheckValid();
  if (mSelected == set)
    return;
  if (mSelected == set)
    return;

  mSelected = set;

  if (mSelected)
  {
    if (mSelectionExclusive && mpParent)
    {
      nuiWidget::IteratorPtr pIt = mpParent->GetFirstChild(true);
      do
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem && pItem != this)
        {
          pItem->SetSelected(false);
        }
      } while (mpParent->GetNextChild(pIt));
      delete pIt;
    }

    Selected();
    StartAnimation("SELECT");
  }
  else
  {
    Deselected();
    StartAnimation("DESELECT");
  }

  StateChanged();
  ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
  SilentInvalidateChildren(true);
  Invalidate();
  DebugRefreshInfo();
}

void nuiWidget::SetVisible(bool Visible)
{
  CheckValid();
  if (IsVisible(false) == Visible)
    return;

  if (IsVisible(false) == Visible)
    return;

  nuiAnimation* pHideAnim = GetAnimation("HIDE");
  nuiAnimation* pShowAnim = GetAnimation("SHOW");

  if (Visible)
  {
    // Show
    if (pHideAnim && pHideAnim->IsPlaying())
    {
      // Stop hiding anim
      pHideAnim->Stop();
      // Start Show Anim if there is one
      if (pShowAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //pShowAnim->SetTime(0, eAnimFromStart);
        StartAnimation("SHOW");
        //        pShowAnim->SilentSetTime(0, eAnimFromStart);
        //        pShowAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // otherwise set visible = true
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
    else if (pShowAnim && pShowAnim->IsPlaying())
    {
      //  Do nothing (return)
      NGL_ASSERT(mVisible == true);
    }
    else if (mVisible)
    {
      // Do nothing (return)
      NGL_ASSERT(mVisible == true);
    }
    else // !mVisible
    {
      // Start Show Anim if there is one
      if (pShowAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //pShowAnim->SetTime(0, eAnimFromStart);
        StartAnimation("SHOW");
        //        pShowAnim->SilentSetTime(0, eAnimFromStart);
        //        pShowAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // otherwise set visible = true
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
  }
  else // !Visible
  {
    if (pHideAnim && pHideAnim->IsPlaying())
    {
      // Do nothing
    }
    else if (pShowAnim && pShowAnim->IsPlaying())
    {
      // Stop Showing
      pShowAnim->Stop();
      // Start Hiding anim if there is one
      if (pHideAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //        pHideAnim->SetTime(0, eAnimFromStart);
        StartAnimation("HIDE");
        //        pHideAnim->SilentSetTime(0, eAnimFromStart);
        //        pHideAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // Otherwise set visible = false
      {
        Invalidate();
        mVisible = false;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
    else if (mVisible)
    {
      // Start Hiding anim if there is one
      if (pHideAnim)
      {
        Invalidate();
        mVisible = true;
        InvalidateLayout();
        VisibilityChanged();
        //        pHideAnim->SetTime(0, eAnimFromStart);
        StartAnimation("HIDE");
        //pHideAnim->SilentSetTime(0, eAnimFromStart);
        //       pHideAnim->Play();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
      else // Otherwise set visible = false
      {
        Invalidate();
        mVisible = false;
        InvalidateLayout();
        VisibilityChanged();
        DebugRefreshInfo();
        ApplyCSSForStateChange(NUI_WIDGET_MATCHTAG_STATE);
      }
    }
    else // !mVisible
    {
      // Do nothing
      NGL_ASSERT(mVisible == false);
    }
  }

  if (mVisible)
    BroadcastVisible();
}

void nuiWidget::BroadcastVisible()
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetNeedSelfRedraw())
    {
      pItem->InvalidateRect(pItem->GetRect().Size());
    }
    nuiWidget* pContainer = dynamic_cast<nuiWidget*>(pItem);
    if (pContainer)
    {
      pContainer->BroadcastVisible();
    }
  }
  delete pIt;
}

nuiRect nuiWidget::CalcIdealSize()
{
  CheckValid();
  nuiRect temp;

  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    nuiRect r(pItem->GetIdealRect()); // Dummy call. Only the side effect is important: the object recalculates its layout.
    temp.Union(temp, r.Size());
  }
  delete pIt;

  DebugRefreshInfo();
  return temp.Size();
}

bool nuiWidget::SetSelfRect(const nuiRect& rRect)
{
  CheckValid();
#ifdef _DEBUG_LAYOUT
  if (GetDebug())
    NGL_OUT("nuiWidget::SetRect on '%s' (%f, %f - %f, %f)\n", GetObjectClass().GetChars(), rRect.mLeft, rRect.mTop, rRect.GetWidth(), rRect.GetHeight());
#endif

  bool inval = mNeedInvalidateOnSetRect;
  if (!(mRect == rRect))
    inval = true;

  if (inval)
    Invalidate();
  if (mForceIdealSize)
    mRect.Set(rRect.Left(), rRect.Top(), mIdealRect.GetWidth(), mIdealRect.GetHeight());
  else
    mRect = rRect;

  if (!mOverrideVisibleRect)
    mVisibleRect = GetOverDrawRect(true, true);

  if (inval)
    Invalidate();

  DebugRefreshInfo();
}

bool nuiWidget::SetRect(const nuiRect& rRect)
{
  SetSelfRect(rRect);
  nuiRect rect(rRect.Size());
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    pItem->GetIdealRect();
    pItem->SetLayout(rect);
  }
  delete pIt;

  DebugRefreshInfo();
  return true;
}

void nuiWidget::InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged)
{
  CheckValid();

  if (mNeedSelfLayout || SizeChanged)
  {
    mInSetRect = true;
    SetRect(rect);
    mInSetRect = false;
    Invalidate();
  }
  else
  {
    // Is this case the widget have just been moved inside its parent. No need to re layout it, only change the rect...
    mRect = rect;

    if (mNeedLayout)
    {
      // The children need to be re layed out (at least one of them!).
      nuiWidget::IteratorPtr pIt = GetFirstChild(false);
      do
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          // The rect of each child doesn't change BUT we still ask for its ideal rect.
          nuiRect rect(pItem->GetBorderedRect());
          nuiRect ideal(pItem->GetIdealRect());

          if (pItem->HasUserPos())
          {
            rect = ideal;
          }
          else if (pItem->HasUserSize())
          {
            rect.SetSize(ideal.GetWidth(), ideal.GetHeight());
          }
          else
          {
            // Set the widget to the size of the parent
          }

          pItem->SetLayout(rect);
        }
      } while (GetNextChild(pIt));
      delete pIt;

    }
  }

  //#TEST:
#ifdef NUI_CHECK_LAYOUTS
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->IsVisible())
    {
      NGL_ASSERT(!pItem->GetNeedLayout());
    }
  }
  delete pIt;
  //#TEST end
#endif
}


////// nuiWidget::Iterator

nuiWidget::Iterator::Iterator(nuiWidget* pSource, bool DoRefCounting)
: mpSource(pSource), mRefCounting(DoRefCounting)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();

  }
  mValid = false;
}

nuiWidget::ConstIterator::ConstIterator(const nuiWidget* pSource, bool DoRefCounting)
: mpSource(pSource), mRefCounting(DoRefCounting)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
  }
  mValid = false;
}

nuiWidget::Iterator::Iterator(const nuiWidget::Iterator& rIterator)
: mpSource(rIterator.mpSource), mRefCounting(rIterator.mRefCounting), mValid(rIterator.mValid)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
  }
}

nuiWidget::ConstIterator::ConstIterator(const nuiWidget::ConstIterator& rIterator)
: mpSource(rIterator.mpSource), mRefCounting(rIterator.mRefCounting), mValid(rIterator.mValid)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
  }
}

nuiWidget::Iterator& nuiWidget::Iterator::operator = (const nuiWidget::Iterator& rIterator)
{
  mRefCounting = rIterator.mRefCounting;
  const nuiWidget* pOld = mpSource;
  mpSource = rIterator.mpSource;
  if (mRefCounting)
  {
    mpSource->Acquire();
    pOld->Release();
  }
  mValid = rIterator.mValid;
  return *this;
}

nuiWidget::ConstIterator& nuiWidget::ConstIterator::operator = (const nuiWidget::ConstIterator& rIterator)
{
  mRefCounting = rIterator.mRefCounting;
  const nuiWidget* pOld = mpSource;
  mpSource = rIterator.mpSource;
  if (mRefCounting)
  {
    mpSource->Acquire();
    pOld->Release();
  }
  mValid = rIterator.mValid;
  return *this;
}

nuiWidget::Iterator::~Iterator()
{
  if (mRefCounting)
    mpSource->Release();
}

nuiWidget::ConstIterator::~ConstIterator()
{
  if (mRefCounting)
    mpSource->Release();
}

bool nuiWidget::Iterator::IsValid() const
{
  return mValid;
}

bool nuiWidget::ConstIterator::IsValid() const
{
  return mValid;
}

void nuiWidget::Iterator::SetValid(bool Valid)
{
  mValid = Valid;
}

void nuiWidget::ConstIterator::SetValid(bool Valid)
{
  mValid = Valid;
}


/////////////////////////////////////////////////////////////
nuiWidgetPtr nuiWidget::Find(const nglString& rName)
{
  CheckValid();
  int slash = rName.Find('/');

  if (slash >= 0)
  {
    nglString path = rName.GetLeft(slash);
    nglString rest = rName.Extract(slash + 1);
    nuiWidgetPtr node = SearchForChild(path, false);

    return node ? node->Find(rest) : NULL;
  }
  else
    return SearchForChild(rName,false);
}

void nuiWidget::OnChildHotRectChanged(nuiWidget* pChild, const nuiRect& rChildHotRect)
{
  CheckValid();
  SetHotRect(rChildHotRect);
}

void nuiWidget::InternalResetCSSPass()
{
  CheckValid();
  mCSSPasses = 0;

  IteratorPtr pIt = GetFirstChild(false);
  for (; pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->ResetCSSPass();
  }
  delete pIt;
}

nuiWidget::IteratorPtr nuiWidget::GetChildIterator(nuiWidgetPtr pChild, bool DoRefCounting)
{
  CheckValid();
  IteratorPtr pIt = GetFirstChild(DoRefCounting);
  while (pIt->IsValid() && pIt->GetWidget() != pChild)
    GetNextChild(pIt);
  return pIt;
}

nuiWidget::ConstIteratorPtr nuiWidget::GetChildIterator(nuiWidgetPtr pChild, bool DoRefCounting) const
{
  CheckValid();
  ConstIteratorPtr pIt = GetFirstChild(DoRefCounting);
  while (pIt->IsValid() && pIt->GetWidget() != pChild)
    GetNextChild(pIt);
  return pIt;
}

nuiWidgetPtr nuiWidget::GetNextFocussableChild(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetFirstChild();
  if (!pIt->IsValid())
    return NULL;

  if (pChild)
    GetNextChild(pIt);

  while (pIt->IsValid() && !pIt->GetWidget()->GetWantKeyboardFocus() && pIt->GetWidget())
    GetNextChild(pIt);

  if (pIt->IsValid())
  {
    nuiWidgetPtr pW = pIt->GetWidget();
    delete pIt;
    return pW;
  }

  delete pIt;
  return NULL;
}

nuiWidgetPtr nuiWidget::GetPreviousFocussableChild(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetLastChild();
  if (!pIt->IsValid())
    return NULL;

  if (pChild)
    GetPreviousChild(pIt);

  while (pIt->IsValid() && !pIt->GetWidget()->GetWantKeyboardFocus() && pIt->GetWidget())
    GetPreviousChild(pIt);

  if (pIt->IsValid())
  {
    nuiWidgetPtr pW = pIt->GetWidget();
    delete pIt;
    return pW;
  }

  delete pIt;
  return NULL;
}


nuiWidgetPtr nuiWidget::GetNextSibling(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetFirstChild();
  if (!pIt->IsValid())
    return NULL;

  if (pChild)
    GetNextChild(pIt);

  nuiWidgetPtr pW = NULL;
  if (pIt->IsValid())
    pW = pIt->GetWidget();

  delete pIt;
  return pW;
}

nuiWidgetPtr nuiWidget::GetPreviousSibling(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetFirstChild();
  if (!pIt->IsValid())
    return NULL;

  if (pChild)
    GetPreviousChild(pIt);

  nuiWidgetPtr pW = NULL;
  if (pIt->IsValid())
    pW = pIt->GetWidget();

  delete pIt;
  return pW;
}

void nuiWidget::SetChildrenLayoutAnimationDuration(float duration)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationDuration(duration);
  }
  delete pIt;
}

void nuiWidget::SetChildrenLayoutAnimationEasing(const nuiEasingMethod& rMethod)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationEasing(rMethod);
  }
  delete pIt;
}

bool nuiWidget::CallPreMouseClicked(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::CallPreMouseUnclicked(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::CallPreMouseMoved(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiWidget::CallPreMouseWheelMoved(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}


void nuiWidget::GetHoverList(nuiSize X, nuiSize Y, std::set<nuiWidget*>& rHoverSet, std::list<nuiWidget*>& rHoverList) const
{
  CheckValid();
  nuiWidget::ConstIteratorPtr pIt = NULL;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->IsInsideFromRoot(X, Y))
    {
      rHoverList.push_back(pItem);
      rHoverSet.insert(pItem);
      nuiWidget* pChild = dynamic_cast<nuiWidget*>(pItem);
      if (pChild)
        pChild->GetHoverList(X, Y, rHoverSet, rHoverList);
    }
  }
  delete pIt;
}


