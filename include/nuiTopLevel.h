/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiTopLevel_h__
#define __nuiTopLevel_h__

//#include "nui.h"
#include "nuiWidget.h"
#include "nuiTimer.h"
#include "nuiDrawContext.h"
#include "nuiRenderThread.h"
#include "nuiNotification.h"

#if defined _ANDROID_ || defined _UIKIT_
#define DISABLE_TOOLTIP
#else
#define DISABLE_TOOLTIP
#endif

class nuiLabel;
class nuiCSS;
class nuiToolTip;

class nuiTopLevel : public nuiWidget
{
public:
  /** @name Life */
  //@{
  nuiTopLevel(const nglPath& rPath); ///< Create an nuiTopLevel.
  //@}

  /* @name Trash management */
  //@{
  virtual void AdviseObjectDeath(nuiWidgetPtr pWidget); ///< Any nuiWidget will call this method when dying. Don't forget to call this version in any overriden AdviseObjectDeath you make.
  void AdviseSubTreeDeath(nuiWidgetPtr pWidget);
  void DisconnectWidget(nuiWidget* pWidget);

  //@}

  /** @name Rendering: */
  //@{
  bool DrawTree(class nuiDrawContext *pContext);
//  bool DrawWidget(nuiDrawContext* pContext);
  bool Draw(class nuiDrawContext *pContext);
  bool SetRect(const nuiRect& rRect);
  //@}

  /** @name Renderer choice: */
  //@{
  nuiDrawContext* GetDrawContext();
  nuiRenderThread* GetRenderThread();
  static void SetRenderer(nuiRenderer Renderer = eOpenGL);
  static nuiRenderer GetRenderer();
  virtual nglContext* GetNGLContext() const = 0;
  void OnRenderingDone(nuiRenderThread* pThread, bool Result);
  //@}

  nuiTopLevel* GetTopLevel() const;

  /* @name Mouse & keyboard focus management */
  //@{
  virtual bool DispatchGrab(nuiWidgetPtr pWidget);
  virtual bool DispatchUngrab(nuiWidgetPtr pWidget);
  virtual bool DispatchHasGrab(nuiWidgetPtr pWidget);
  virtual bool DispatchHasGrab(nuiWidgetPtr pWidget, nglTouchId TouchId);

  virtual bool Grab(nuiWidgetPtr pWidget); ///< Redirect all mouse event to this object.
  virtual bool Grab(nuiWidgetPtr pWidget, const nglMouseInfo& rInfo);
  virtual bool Ungrab(nuiWidgetPtr pWidget); ///< Stop redirecting all mouse event to the given grab object.
  virtual bool Ungrab(nuiWidgetPtr pWidget, const nglMouseInfo& rInfo);
  virtual nuiWidgetPtr GetGrab() const; ///< Returns the object that currently has the mouse focus.
  virtual bool CancelGrab(); ///< Cancel any current ongoing grab action and empty the grab stack.
  bool StealMouseEvent(nuiWidgetPtr pWidget, const nglMouseInfo& rInfo); ///< Try to assign the mouse/touch grab for the given mouse event to the given widget, canceling the event if necessary. If a widget already has acquired the grab it is refused and the function returns false.

  virtual bool SetFocus(nuiWidgetPtr pWidget); ///< Redirect all keyboard events to this object.
  virtual nuiWidgetPtr GetFocus() const; ///< Returns the object that currently has the keyboard focus.
  
  void SetTabForward(nuiWidget* pSource, nuiWidget* pDestination, bool AutoReverse = true);
  void SetTabBackward(nuiWidget* pSource, nuiWidget* pDestination, bool AutoReverse = false);
  nuiWidget* GetTabForward(nuiWidget* pFrom) const;
  nuiWidget* GetTabBackward(nuiWidget* pFrom) const;
  void GetTabForwardSources(nuiWidget* pDestination, std::set<nuiWidgetPtr>& rSources) const;
  void GetTabBackwardSources(nuiWidget* pDestination, std::set<nuiWidgetPtr>& rSources) const;
  //@}

  /** @name Generic tooltips */
  //@{
#ifndef DISABLE_TOOLTIP
  virtual bool ActivateToolTip(nuiWidgetPtr pWidget, bool Now = false); ///< nuiMainWindow override the default tool-tip mechanism to actually display them.
  virtual bool ReleaseToolTip(nuiWidgetPtr pWidget); ///< Remove the current tool-tip for the given widget. 
  virtual void SetToolTipOn(bool AutoStop);
  virtual void ToolTipOn(const nuiEvent& rEvent);
  virtual void ToolTipOff(const nuiEvent& rEvent);
#else
  virtual bool ActivateToolTip(nuiWidgetPtr pWidget, bool Now = false) { return false; } ///< nuiMainWindow override the default tool-tip mechanism to actually display them.
  virtual bool ReleaseToolTip(nuiWidgetPtr pWidget) { return false; } ///< Remove the current tool-tip for the given widget.
#endif
  //@}

  virtual bool IsKeyDown (nglKeyCode Key) const;

  // Events entry points:
  void CallTextCompositionStarted(); ///< Tells the widget that a complex text input session is starting (mostly used to enter diacritics with dead keys and complex scripts like east asian glyphs)
  void CallTextCompositionConfirmed(); ///< Tells the widget to confirm the composed text as final and end the composition session start with TextCompositionStart.
  void CallTextCompositionCanceled(); ///< Tells the widget to cancel the composed text input and end the composition session started with TextCompositionStart.
  void CallTextCompositionUpdated(const nglString& rString, int32 CursorPosition);
  nglString CallGetTextComposition() const;
  void CallTextCompositionIndexToPoint(int32 CursorPosition, float& x, float& y) const;
  bool CallTextInput (const nglString& rUnicodeText);
  void CallTextInputCancelled ();
  bool CallKeyDown (const nglKeyEvent& rEvent);
  bool CallKeyUp (const nglKeyEvent& rEvent);
  bool CallMouseClick (nglMouseInfo& rInfo);
  bool CallMouseUnclick(nglMouseInfo& rInfo);
  bool CallMouseWheel (nglMouseInfo& rInfo);
  bool CallMouseMove (nglMouseInfo& rInfo);
  bool CallMouseCancel(nuiWidgetPtr pThief, nglMouseInfo& rInfo);
  void GetMouseInfo(nglMouseInfo& rMouseInfo) const;
  nuiWidget* GetWidgetUnderMouse() { return mpUnderMouse; }

  nglPath GetResourcePath() const; ///< Return the resource path used by this TopLevel object.

  void EnablePartialRedraw(bool Set = true) { mPartialRedraw = Set; }
  bool IsPartialRedrawEnabled() { return mPartialRedraw; }
#ifndef DISABLE_TOOLTIP
  void DisplayToolTips(nuiDrawContext* pContext);
#endif
  void EnableClearBackground(bool set = true) { mClearBackground = set; Invalidate(); }
  bool IsClearBackgroundEnabled() const { return mClearBackground; }

  /** @name Hot Keys */
  //@{
  bool InitHotKeys(nglIStream* pHotKeys); // initialize hotkeys in toplevel so that they are registered once and for all (with default shortcuts or user preferences) and not just when widgets using a particular hotkey are created
  bool InitHotKeys(nuiXMLNode* pHotKeys); // initialize hotkeys in toplevel so that they are registered once and for all (with default shortcuts or user preferences) and not just when widgets using a particular hotkey are created
  nuiHotKey* RegisterHotKeyKey(const nglString& rName, nglKeyCode Trigger, nuiKeyModifier Modifiers, bool Priority = false, bool FireOnKeyUp = false, const nglString& rDescription = nglString::Empty); ///< Register hotkey. If rName is not registered, create new hotkey and return it. Otherwise, increment reference count on hotkey and return the existing.
  nuiHotKey* RegisterHotKeyChar(const nglString& rName, nglChar Trigger, nuiKeyModifier Modifiers, bool Priority = false, bool FireOnKeyUp = false, const nglString& rDescription = nglString::Empty); ///< Register hotkey. If rName is not registered, create new hotkey and return it. Otherwise, increment reference count on hotkey and return the existing.
  void SetHotKey(const nglString& rName, nuiHotKey* pHotKey); // Replaces the rName hotkey (or add a new one if necessary) with the nuiHotKey* given as argument.
  nuiHotKey* GetHotKey(const nglString& rName); ///< Return the hotkey associated to a name, NULL if none exists
  const std::map<nglString, nuiHotKey*>& GetHotKeys() const; // return the whole map of name and hotkeys
  const nglString& FindHotKeyKey(nglKeyCode Trigger, nuiKeyModifier Modifiers); // searches the map for a matching hotkey, return its name, or nglString::Empty if not found
  const nglString& FindHotKeyChar(nglChar Trigger, nuiKeyModifier Modifiers); // searches the map for a matching hotkey, return its name, or nglString::Empty if not found
  void DelHotKey(const nglString& rName); ///< Decrement reference count for hotkey. If reference count reaches 0, delete hotkey.
  void PrintHotKeyMap(const nglString& rText);
  //@}

  nuiSimpleEventSource <nuiWidgetHoverChanged> GlobalHoverChanged; ///< Send an event when the hovering state changes.

  void SetWatchedWidget(nuiWidget* pWatchedWidget);
  
  // CSS Stuff:
  void PrepareWidgetCSS(nuiWidget* pWidget, bool Recursive, uint32 MatchersTag);
  void ApplyWidgetCSS(nuiWidget* pWidget, bool Recursive, uint32 MatchersTag);
  void SetCSS(nuiCSS* pCSS);
  nuiCSS* GetCSS() const;
  bool LoadCSS(const nglPath& rPath);
  
  virtual void EnterModalState();
  virtual void ExitModalState();
  
  virtual void StartTextInput(int32 X, int32 Y, int32 W, int32 H);
  virtual void EndTextInput();
  virtual bool IsEnteringText() const;

  // Notification manager proxy:
  void PostNotification(nuiNotification* pNotification); ///< Put this notification in a queue in order to broadcast when the system feels like it.
  void BroadcastNotification(const nuiNotification& rNotification); ///< Send this notification now to all registered observers.
  void RegisterObserver(const nglString& rNotificationName, nuiNotificationObserver* pObserver); ///< Register an observer for the given notification type. If the type is nglString::Empty, all the notifications will be sent to the observer.
  void UnregisterObserver(nuiNotificationObserver* pObserver, const nglString& rNotificationName = nglString::Null); ///< Unregister pObserver so that it doesn't receive the given notification. By default it is removed from all notification types (nglString::Null).
  
  virtual const std::map<nglTouchId, nglMouseInfo>& GetMouseStates() const;
  virtual nuiSize GetStatusBarSize() const;

  virtual float GetScale() const;
  virtual float GetScaleInv() const;

protected:
  virtual ~nuiTopLevel();

  void Exit();
  void SetDrawContext(nuiDrawContext* pDrawContext);
  virtual void BroadcastInvalidateRect(nuiWidgetPtr pSender, const nuiRect& rRect);
//  void OnMessageQueueTick(const nuiEvent& rEvent);
  void UpdateHoverList(nglMouseInfo& rInfo);
  
  nuiWidgetPtr mpFocus;
  nuiWidgetPtr mpUnderMouse;

#ifndef DISABLE_TOOLTIP
  // ToolTips:
  nuiTimer mToolTipTimerOn;
  nuiTimer mToolTipTimerOff;
  nglTime mToolTipDelayOn;
  nglTime mToolTipDelayOff;
  bool mDisplayToolTip;
  nuiWidgetPtr mpToolTipSource;
  nuiToolTip* mpToolTipLabel;
#endif
  nuiLabel* mpInfoLabel;

  nuiWidget* mpDragFeedback;

  typedef std::map<nglTouchId, nuiWidgetPtr> nuiGrabMap;
  nuiGrabMap mpGrab;
  void DumpGrabMap(int line) const;
  std::map<nglTouchId, bool> mpGrabAcquired;
  bool HasGrab(nuiWidgetPtr pWidget); ///< Returns true if this \p pWidget has been grabbed by any touch
  nuiWidgetPtr GetGrab(nglTouchId touchId) const;  ///< Returns the Widget that has been grabbed by this \p touchId, NULL otherwise
  std::map<nglTouchId, nglMouseInfo> mMouseClickedEvents;
  std::map<nglTouchId, nglMouseInfo> mMouseStates;

  nglPath mResPath;

  bool mClearBackground;

  nglMouseInfo mMouseInfo;
  
  nuiWidgetPtr mpWatchedWidget;

  void UpdateWidgetsCSS();
  void UpdateMouseCursor(const nuiWidgetList& rWidgets);
  void DispatchKeyboardFocus(const nuiWidgetList& rWidgets);

protected:
  /* @name Trash management */
  //@{
  bool mFillTrash;
  //@}

  nuiDrawContext* mpDrawContext;
  nuiRenderThread* mpRenderThread;

  static nuiRenderer mRenderer;

  nuiEventSink<nuiTopLevel> mTopLevelSink;
  bool mPartialRedraw;
  bool mIsDrawing;
  bool mIsRendering;

#ifndef DISABLE_TOOLTIP
  void SetToolTipRect();
#endif
  void SetDragFeedbackRect(int X, int Y);

  nglMouseInfo::Flags mLastClickedButton;

  std::map<nglString, nuiHotKey*> mHotKeys;
  
  std::set<nuiWidgetPtr> mHoveredWidgets;
  std::map<nuiWidgetPtr, uint32> mCSSWidgets;
  
  nuiCSS* mpCSS;
  
  std::map<nuiWidgetPtr, nuiWidgetPtr> mTabForward;
  std::map<nuiWidgetPtr, nuiWidgetPtr> mTabBackward;
  std::map<nuiWidgetPtr, std::set<nuiWidgetPtr> > mTabForwardRev;
  std::map<nuiWidgetPtr, std::set<nuiWidgetPtr> > mTabBackwardRev;
};

typedef nuiTopLevel* nuiTopLevelPtr;


#endif // __nuiTopLevel_h__
