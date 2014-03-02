/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nuiAndroidBridge.h"

void* gmpNUI_AndroidBridge = 0;

float gScale = 1.0;
float nuiGetScaleFactor() { return gScale;}
float nuiGetInvScaleFactor() { return 1.0 / nuiGetScaleFactor();}
