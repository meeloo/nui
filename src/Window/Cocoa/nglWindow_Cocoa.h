#pragma once

#include <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGL/OpenGL.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

/*
** nglNSView
*/

@interface nglNSView : NSView<NSWindowDelegate>
{
  nglWindow* mpNGLWindow;
  NSTimer* mpRefreshTimer;
  bool mInitiated;
}
@end

/*
** nglNSWindow
*/
@interface nglNSWindow : NSWindow
{
  nglWindow*    mpNGLWindow;
  nglTime       mLastEventTime;
  uint32        mModifiers;
  NSEvent*      mpLastMouseMoved;
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

@end


/*
** nglNSWindowController
*/
@interface nglNSWindowController : NSWindowController
{
  nglNSView *mpView;
  nglWindow* mpNGLWindow;
}
- initWithNGLWindow: (nglWindow*)pNGLWindow;
@end
