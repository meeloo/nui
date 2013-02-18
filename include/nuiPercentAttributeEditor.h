/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/
 
#pragma once

class NUI_API nuiPercentAttributeEditor : public nuiAttributeEditor
{
public:

	nuiPercentAttributeEditor(const nuiAttrib<float>& rAttribute);
	virtual ~nuiPercentAttributeEditor();
	
	

protected : 

	void OnAttributeChanged(float value);
	void OnChanged(const nuiEvent& rEvent);

	
private : 

	nuiSlotsSink mSink;
	nuiEventSink<nuiPercentAttributeEditor> mEventSink;
	nuiAttrib<float> mAttribute;
	nuiSlider* mpSlider;

};
