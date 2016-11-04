/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiRadioButton_h__
#define __nuiRadioButton_h__

//#include "nui.h"
#include "nuiWidget.h"
#include "nuiEvent.h"
#include "nuiToggleButton.h"
#include "nuiImage.h"

class nuiRadioButtonGroup;

/// This class implements a simple push button that acts as a container for a label, an image, or any other inactive widget element.
class nuiRadioButton : public nuiToggleButton
{ 
public:
  nuiRadioButton(); ///< Create a simple button (empty).
  nuiRadioButton(const nglString& rText); ///< Create a simple button with a nuiLabel as a child filling the client area.
  nuiRadioButton(nuiStateDecoration* pDecoration); ///< Create a simple button with a skin.
  nuiRadioButton(const nglImage& rImage); ///< Create a simple button with a nuiImage as a child filling the client area.
  virtual ~nuiRadioButton();

  void SetGroup(const nglString& rGroupName);
  nglString GetGroup() const;

  virtual bool MouseClicked   (const nglMouseInfo& rInfo);
  virtual bool MouseUnclicked (const nglMouseInfo& rInfo);
  virtual bool MouseMoved     (const nglMouseInfo& rInfo);
  virtual bool MouseUngrabbed (nglTouchId Id);
  void OnDelayedActivation(const nuiEvent& rEvent);

  // Keyboard events:
  bool KeyDown(const nglKeyEvent& rEvent);
  bool KeyUp(const nglKeyEvent& rEvent);
  
  virtual bool SetParent(nuiWidgetPtr pParent);

  void                  SetGroupManager(nuiRadioButtonGroup* pGroup);
  nuiRadioButtonGroup*  GetGroupManager();

  void EnableToggleable(bool set);
  bool IsToggleable() const;

  nuiSimpleEventSource<nuiWidgetActivated> Deactivated;
protected:
  nuiRadioButtonGroup* mpGroupManager;
  bool mCanToggle;
  nuiEventSink<nuiRadioButton> mRadioEventSink;
};

#endif // __nuiRadioButton_h__
