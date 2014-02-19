
#include "nui.h"
#include "nuiDrawerView.h"

static const float SPRING_TOLERANCE = 0.0000000005f;
static const float STIFFNESS = .3f;
static const float PULL_COEF = 4;
static const float MOVE_TOLERANCE = 8;

nuiDrawerView::nuiDrawerView() :
mpLeft(nullptr), mpMain(nullptr), mpRight(nullptr), mOffset(0), mTouched(false), mMoving(false), mOriginalOffset(0), mTargetOffset(0), mEventSink(this)
{
  if (SetObjectClass("nuiDrawerView"))
  {
    // Init attributes:
  }

  nuiAnimation::AcquireTimer();
}

nuiDrawerView::~nuiDrawerView()
{
  nuiAnimation::ReleaseTimer();
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
  nuiSimpleContainer::AddChild(pWidget);
}

bool nuiDrawerView::DelChild(nuiWidgetPtr pWidget)
{
  if (pWidget == mpLeft)
    mpLeft = NULL;
  if (pWidget == mpRight)
    mpRight = NULL;
  if (pWidget == mpMain)
    mpMain = NULL;
  nuiSimpleContainer::DelChild(pWidget);
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
  if (rInfo.Buttons & nglMouseInfo::ButtonLeft)
  {
    if (mTouched && !mMoving && mOffset != 0)
    {
      if (mpMain)
      {
        mpMain->DispatchMouseCanceled(rInfo);
      }
      mTouched = false;

      mTargetOffset = 0;
      mEventSink.Connect(nuiAnimation::GetTimer()->Tick, &nuiDrawerView::OnAnimateDrawer);

      return true;
    }
    mTouched = false;
  }
  return false;
}

bool nuiDrawerView::PreMouseMoved(const nglMouseInfo& rInfo)
{
  if (mTouched && !mMoving)
  {
    float x = 0;
    x = mTouch.X - rInfo.X;
    float dist = fabs(x);

    if (dist > MOVE_TOLERANCE)
    {
      //NGL_OUT("nuiDrawerView Preempting mouse from existing grabber!\n");
      NGL_ASSERT(GetTopLevel());

      if (StealMouseEvent(rInfo))
      {
        mTouched = false;
        mMoving = true;
        return true;
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
      mTargetOffset = 0;
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
      mTargetOffset = 0;
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
  mOffset += diff / 8;

  //NGL_OUT(".");
  diff = mTargetOffset - mOffset;
  if (fabs(diff) < 1.0)
  {
    //NGL_OUT("nuiDrawerView Offset Realease Done\n");
    mOffset = mTargetOffset;
    //NGL_OUT("!!!!!!!! Anim end mOffset = %f\n", mOffset);
    mEventSink.DisconnectAll();
  }

  mOriginalOffset = mOffset;
  UpdateLayout();
}
