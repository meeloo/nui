#pragma once

#include <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGL/OpenGL.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
** nglNSWindow
*/
@interface nglNSWindow : NSWindow
{
  nglWindow*    mpNGLWindow;
  nglTime       mLastEventTime;
  double        mLastPaintTime;
  bool          mInited;
  bool          mInvalidated;
  id            mDisplayLink;
  NSTimer*      mInvalidationTimer;

  nuiTimer*     mpTimer;
  uint32        mModifiers;

  class nglDragAndDrop* mpDropObject;
}

- (id) initWithFrame: (NSRect) rect andNGLWindow: (nglWindow*) pNGLWindow andSharedContext:(NSOpenGLContext*)sharedContext;
- (void) dealloc;
- (void) invalidate;
- (nglWindow *) getNGLWindow;

- (BOOL)keyDown:(NSEvent *)theEvent;
- (BOOL)keyUp:(NSEvent *)theEvent;
- (BOOL)flagsChanged:(NSEvent *)theEvent;

- (NSDragOperation)draggingEntered: (id <NSDraggingInfo>)sender;
- (NSDragOperation)draggingUpdated: (id <NSDraggingInfo>)sender;
- (void)draggingExited: (id <NSDraggingInfo>)sender;
- (BOOL)prepareForDragOperation: (id <NSDraggingInfo>)sender;
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender;

- (void)resize: (NSSize) size;

@end///< nglNSWindow

