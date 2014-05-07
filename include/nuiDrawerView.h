
#pragma once

#include "nui.h"

class nuiDrawerView : public nuiSimpleContainer
{
public:
  nuiDrawerView();
  virtual ~nuiDrawerView();

  bool SetRect(const nuiRect& rRect);
  nuiRect CalcIdealSize();

  bool AddChild(nuiWidgetPtr pWidget);
  bool DelChild(nuiWidgetPtr pWidget);

  void SetLeft(nuiWidgetPtr pWidget);
  void SetMain(nuiWidgetPtr pWidget);
  void SetRight(nuiWidgetPtr pWidget);

  nuiWidgetPtr GetLeft();
  nuiWidgetPtr GetMain();
  nuiWidgetPtr GetRight();

  bool MouseClicked(const nglMouseInfo& rInfo);
  bool MouseUnclicked(const nglMouseInfo& rInfo);
  bool MouseMoved(const nglMouseInfo& rInfo);
  bool MouseCanceled(const nglMouseInfo& rInfo);

  bool PreMouseClicked(const nglMouseInfo& rInfo);
  bool PreMouseUnclicked(const nglMouseInfo& rInfo);
  bool PreMouseMoved(const nglMouseInfo& rInfo);

  NUI_GETSET(bool, Interactive);
  NUI_GETSET(float, AnimRatio);

  void OpenLeft();
  void OpenRight();
  void Close();
  void ToggleLeft();
  void ToggleRight();
  bool IsOpen() const;
  bool IsLeftOpen() const;
  bool IsRightOpen() const;

  nuiSimpleEventSource<0> LeftOpened;
  nuiSimpleEventSource<0> RightOpened;
  nuiSimpleEventSource<0> Opened;
  nuiSimpleEventSource<0> Closed;
protected:
  nuiWidgetPtr mpLeft;
  nuiWidgetPtr mpMain;
  nuiWidgetPtr mpRight;
  float mOffset;
  bool mTouched;
  bool mMoving;
  float mOriginalOffset;
  float mTargetOffset;
  bool mInteractive;
  float mAnimRatio = 1.0 / 8.0;

  nglMouseInfo mTouch;
  nuiEventSink<nuiDrawerView> mEventSink;
  void OnAnimateDrawer(const nuiEvent& rEvent);

  void ReleaseTouch();
};
