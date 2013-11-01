/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

/*!
\file  nglDragAndDrop_Cocoa.h
\brief Cocoa specific Handlers and Procedures for Drag and drop operations
*/

#pragma once

#include "nui.h"
#include "nglKernel.h"
#include "nglWindow.h"
#include "nglDragAndDropObjects.h"
#include "nglDataObjects.h"

class nglCocoaDragAndDrop
{
public:
  nglCocoaDragAndDrop(nglWindow* pWin);
  ~nglCocoaDragAndDrop();

  bool Drag(nglDragAndDrop* pDragObject);

  void SetDropObject(nglDragAndDrop* pDrop) { mpDropObject = pDrop; }
  void SetDragObject(nglDragAndDrop* pDrag) { mpDragObject = pDrag; }
  nglDragAndDrop* GetDropObject()           { return mpDropObject; }
  nglDragAndDrop* GetDragObject()           { return mpDragObject; }
  
  bool HasDropObject()                      { return mpDropObject ? true : false; }
  bool HasDragObject()                      { return mpDragObject ? true : false; }

  void SetCanDrop(bool can)                 { mCanDrop = can; }
  bool CanDrop()                            { return mCanDrop; }
//protected:
  
public:
  nglWindow*  mpWin;
  void*   mWindow;

  bool        mCanDrop;
  
  nglDragAndDrop* mpDragObject;
  nglDragAndDrop* mpDropObject;

  void* mNSPasteboard;
};


