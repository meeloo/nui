
#include "nui.h"
#include "nuiDrawerView.h"

static const float SPRING_TOLERANCE = 0.0000000005f;
static const float STIFFNESS = .3f;
static const float PULL_COEF = 4;
static const float MOVE_TOLERANCE = 8;


nuiDrawerView::nuiDrawerView() :
mpLeft(nullptr), mpMain(nullptr), mpRight(nullptr), mOffset(0), mTouched(false), mMoving(false), mOriginalOffset(0), mTargetOffset(0), mEventSink(this), mInteractive(true)
{
  if (SetObjectClass("nuiDrawerView"))
  {
    // Init attributes:
    AddAttribute(new nuiAttribute<bool>
                 ("Interactive", nuiUnitBoolean,
                  nuiMakeDelegate(this, &nuiDrawerView::GetInteractive),
                  nuiMakeDelegate(this, &nuiDrawerView::SetInteractive)));

    AddAttribute(new nuiAttribute<float>
                 ("AnimRatio", nuiUnitBoolean,
                  nuiMakeDelegate(this, &nuiDrawerView::GetAnimRatio),
                  nuiMakeDelegate(this, &nuiDrawerView::SetAnimRatio)));
  }

  NUI_ADD_EVENT(LeftOpened);
  NUI_ADD_EVENT(RightOpened);
  NUI_ADD_EVENT(Opened);
  NUI_ADD_EVENT(Closed);

  nuiAnimation::AcquireTimer();
}

nuiDrawerView::~nuiDrawerView()
{
  nuiAnimation::ReleaseTimer();
}

void nuiDrawerView::OpenLeft()
{
  if (!mpLeft)
    return;

  mMoving = false;
  mTouched = false;
  mStealRefused = false;

  float width = mpLeft->GetIdealRect().GetWidth();
  mTargetOffset = width;

  if (mOffset != mTargetOffset)
  {
    mEventSink.Connect(nuiAnimation::GetTimer()->Tick, &nuiDrawerView::OnAnimateDrawer);
  }

  UpdateLayout();
}

void nuiDrawerView::OpenRight()
{
  if (!mpRight)
    return;

  mMoving = false;
  mTouched = false;
  mStealRefused = false;

  float width = mpRight->GetIdealRect().GetWidth();
    mTargetOffset = -width;

  if (mOffset != mTargetOffset)
  {
    mEventSink.Connect(nuiAnimation::GetTimer()->Tick, &nuiDrawerView::OnAnimateDrawer);
  }

  UpdateLayout();
}

void nuiDrawerView::Close()
{
  mMoving = false;
  mTouched = false;
  mStealRefused = false;

  mTargetOffset = 0;

  if (mOffset != mTargetOffset)
  {
    mEventSink.Connect(nuiAnimation::GetTimer()->Tick, &nuiDrawerView::OnAnimateDrawer);
  }

  UpdateLayout();
}

bool nuiDrawerView::IsOpen() const
{
  return mTargetOffset != 0;
}

bool nuiDrawerView::IsLeftOpen() const
{
  return mTargetOffset > 0;
}

bool nuiDrawerView::IsRightOpen() const
{
  return mTargetOffset < 0;
}

void nuiDrawerView::ToggleLeft()
{
  if (!IsLeftOpen())
    OpenLeft();
  else
    Close();
}

void nuiDrawerView::ToggleRight()
{
  if (!IsRightOpen())
    OpenRight();
  else
    Close();
}


bool nuiDrawerView::SetRect(const nuiRect& rRect)
{
  nuiWidget::SetRect(rRect);
  //NGL_OUT("Set Rect with mOffset %f\n", mOffset);
  if (mpLeft)
  {
    nuiRect r(MIN(mpLeft->GetIdealRect().GetWidth(), rRect.GetWidth()), rRect.GetHeight());
    r.Move(MIN(0, mOffset - r.GetWidth()), 0);
    mpLeft->SetLayout(r);
  }

  if (mpMain)
  {
    nuiRect r(rRect.Size());
    r.Move(mOffset, 0);
    mpMain->SetLayout(r);
  }

  if (mpRight)
  {
    nuiRect r(MIN(mpRight->GetIdealRect().GetWidth(), rRect.GetWidth()), rRect.GetHeight());
    r.Move(MAX(mOffset + rRect.GetWidth(), rRect.GetWidth() - r.GetWidth()), 0);
    mpRight->SetLayout(r);
  }

  return true;
}

nuiRect nuiDrawerView::CalcIdealSize()
{
  return mpMain ? mpMain->GetIdealRect() : nuiRect();
}


bool nuiDrawerView::AddChild(nuiWidgetPtr pWidget)
{
  if (pWidget->GetProperty("Drawer").Compare("left", false) == 0)
  {
    if (mpLeft)
      DelChild(mpLeft);
    mpLeft = pWidget;
  }
  else if (pWidget->GetProperty("Drawer").Compare("right", false) == 0)
  {
    if (mpRight)
      DelChild(mpRight);
    mpRight = pWidget;
  }
  else
  {
    if (mpMain)
      DelChild(mpMain);
    mpMain = pWidget;
  }
  return nuiSimpleContainer::AddChild(pWidget);
}

bool nuiDrawerView::DelChild(nuiWidgetPtr pWidget)
{
  if (pWidget == mpLeft)
    mpLeft = NULL;
  if (pWidget == mpRight)
    mpRight = NULL;
  if (pWidget == mpMain)
    mpMain = NULL;
  return nuiSimpleContainer::DelChild(pWidget);
}

void nuiDrawerView::SetLeft(nuiWidgetPtr pWidget)
{
  if (mpLeft)
    DelChild(mpLeft);
  if (pWidget)
    AddChild(pWidget);
  mpLeft = pWidget;
}

void nuiDrawerView::SetMain(nuiWidgetPtr pWidget)
{
  if (mpMain)
    DelChild(mpMain);
  if (pWidget)
    AddChild(pWidget);
  mpMain = pWidget;
}

void nuiDrawerView::SetRight(nuiWidgetPtr pWidget)
{
  if (mpRight)
    DelChild(mpRight);
  if (pWidget)
    AddChild(pWidget);
  mpRight = pWidget;
}


nuiWidgetPtr nuiDrawerView::GetLeft()
{
  return mpLeft;
}

nuiWidgetPtr nuiDrawerView::GetMain()
{
  return mpMain;
}

nuiWidgetPtr nuiDrawerView::GetRight()
{
  return mpRight;
}

bool nuiDrawerView::PreMouseClicked(const nglMouseInfo& rInfo)
{
  if (!mInteractive)
    return false;

  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mTouch = rInfo;
    mTouched = true;
    mOriginalOffset = mOffset;
   //NGL_OUT("PreMouseClicked mOriginalOffset = %f\n", mOriginalOffset);
  }
  return false;
}

bool nuiDrawerView::PreMouseUnclicked(const nglMouseInfo& rInfo)
{
  if (!mInteractive)
    return false;

  if (mStealRefused)
  {
    mTouched = false;
    mMoving = false;
    mStealRefused = false;
    return false;
  }

  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    if (mTouched && !mMoving && mOffset != 0)
    {
      if ((mOffset < 0 && rInfo.X < mRect.GetWidth() + mOffset) || (mOffset > 0 && rInfo.X > mOffset))
      {
        if (mpMain)
        {
          mpMain->DispatchMouseCanceled(this, rInfo);
        }
        mTouched = false;

        Close();

        return true;
      }
    }
    mTouched = false;
  }
  return false;
}

bool nuiDrawerView::PreMouseMoved(const nglMouseInfo& rInfo)
{
  if (!mInteractive)
    return false;

  if (mTouched && !mMoving)
  {
    float x = 0;
    x = mTouch.X - rInfo.X;
    float dist = fabs(x);

    if (dist > MOVE_TOLERANCE)
    {
      NGL_ASSERT(GetTopLevel());

      if (StealMouseEvent(rInfo))
      {
        mTouched = false;
        mMoving = true;
        mStealRefused = false;
        return true;
      }
      else
      {
        NGL_LOG("nuiDrawer", NGL_LOG_DEBUG, "PreMouseMoved: StealMouseEvent refused\n");
        mStealRefused = true;
      }
    }
  }
  return false;
}

bool nuiDrawerView::MouseClicked(const nglMouseInfo& rInfo)
{
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    mMoving = false;
    mTouched = false;

    mTouch = rInfo;
    mOriginalOffset = mOffset;

    SetSelected(true);
    // Stop events, just in case...
    mEventSink.DisconnectAll();
  }
  return false;
}

void nuiDrawerView::ReleaseTouch()
{
  mMoving = false;
  mTouched = false;

  if (mOffset < 0)
  {
    // We should we go?
    float width = mpRight->GetIdealRect().GetWidth();
    if (-mOffset < width * 0.5)
    {
      // We're less than half way though, so we go back to our original position:
      Close();
      return;
    }
    else
    {
      mTargetOffset = -width;
    }
  }
  else if (mOffset > 0)
  {
    // We should we go?
    float width = mpLeft->GetIdealRect().GetWidth();
    if (mOffset < width * 0.5)
    {
      // We're less than half way though, so we go back to our original position:
      Close();
      return;
    }
    else
    {
      mTargetOffset = width;
    }
  }
  else
  {
    mTargetOffset = 0;
  }

  if (mOffset != mTargetOffset)
  {
    mEventSink.Connect(nuiAnimation::GetTimer()->Tick, &nuiDrawerView::OnAnimateDrawer);
  }

  UpdateLayout();
}

bool nuiDrawerView::MouseUnclicked(const nglMouseInfo& rInfo)
{
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    ReleaseTouch();
    SetSelected(false);
  }
  return false;
}

bool nuiDrawerView::MouseMoved(const nglMouseInfo& rInfo)
{
  if (mMoving)
  {
    float dx = mTouch.X - rInfo.X;
    mOffset = mOriginalOffset -dx;
    if (mOffset < 0)
    {
      if (!mpRight)
      {
        mOffset = 0;
      }
      else
      {
        float width = mpRight->GetIdealRect().GetWidth();
        if (mOffset < -width)
        {
          // Compute the position with the
          float diff = -mOffset - width ;
          mOffset = -width - PULL_COEF * sqrt(diff);
          //NGL_OUT("Diff : %f\n", diff);
        }
      }
    }
    else
    {
      if (!mpLeft)
      {
        mOffset = 0;
      }
      else
      {
        float width = mpLeft->GetIdealRect().GetWidth();
        if (mOffset > width)
        {
          // Compute the position with the
          float diff = mOffset - width ;
          mOffset = width + PULL_COEF * sqrt(diff);
          //NGL_OUT("Diff : %f\n", diff);
        }
      }
    }

    //NGL_OUT("Mouse moved, new offset = %f\n", mOffset);
    UpdateLayout();
    return true;
  }

  if (mTouched)
  {
    float x = 0;
    x = mTouch.X - rInfo.X;
    float dist = fabs(x);

    if (dist > MOVE_TOLERANCE)
    {
      //NGL_OUT("nuiDrawerView Preempting mouse from existing grabber!\n");
      NGL_ASSERT(GetTopLevel());

      mTouched = false;
      mMoving = true;
    }
  }
  return false;
}

bool nuiDrawerView::MouseCanceled(const nglMouseInfo& rInfo)
{
  if (mMoving)
  {
    ReleaseTouch();
  }
  SetSelected(false);
  mTouched = false;
  mMoving = false;
  return false;
}

void nuiDrawerView::OnAnimateDrawer(const nuiEvent &rEvent)
{
  float diff = mTargetOffset - mOffset;
  mOffset += diff * mAnimRatio;

  //NGL_OUT(".");
  diff = mTargetOffset - mOffset;
  if (fabs(diff) < 1.0)
  {
    //NGL_OUT("nuiDrawerView Offset Realease Done\n");
    mOffset = mTargetOffset;
    //NGL_OUT("!!!!!!!! Anim end mOffset = %f\n", mOffset);
    mEventSink.DisconnectAll();


    if (IsOpen())
    {
      Opened();
      if (IsLeftOpen())
      {
        LeftOpened();
      }
      else
      {
        RightOpened();
      }
    }
    else
    {
      Closed();
    }
  }

  mOriginalOffset = mOffset;
  UpdateLayout();
}
