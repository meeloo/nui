/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "WidgetDelegates.h"
#include "nuiTitledPane.h"
#include "nuiFolderPane.h"

void WidgetDelegateLabel(nuiWidget* pWidget)
{
	nuiLabel* pLabel = (nuiLabel*) pWidget;
	
	pLabel->SetObjectName("unnamed label");
	pLabel->SetText("empty label");
}


void WidgetDelegateTitledPane(nuiWidget* pWidget)
{
	nuiTitledPane* pPane = (nuiTitledPane*) pWidget;
	
	pPane->SetObjectName("unnamed titledpane");
	pPane->SetTitle("unnamed titledpane");
}

void WidgetDelegateFolderPane(nuiWidget* pWidget)
{
	nuiFolderPane* pFolder = (nuiFolderPane*) pWidget;
	
	pFolder->SetObjectName("unnamed folderpane");
	pFolder->SetTitle("unnamed folderpane");
}
