/*
 *  VolumeController.h
 *  AudioPlayer
 *
 *  Created by mat on 9/28/09.
 *  Copyright 2009 Mxp4. All rights reserved.
 *
 */

#include "nui.h"

class VolumeController : public nuiWidget
{
public:
  VolumeController(nuiAttrib<float>& rGainAttrib, nuiAttrib<bool>& rMuteAttrib);
  virtual ~VolumeController();
  
private:
  void OnBtnClicked(const nuiEvent& rEvent);
  void OnSliderChanged(const nuiEvent& rEvent);
  
  void OnGainAttribChanged(float gainDb);
  void OnMuteAttribChanged(bool mute);
  
  nuiAttrib<float> mGainAttrib;
  nuiAttrib<bool> mMuteAttrib;
  
  nuiToggleButton* mpBtn;
  nuiSlider* mpSlider;
  
  nuiSlotsSink mSlotsSink;
  nuiEventSink<VolumeController> mEventSink;
};