#include "nui.h"

#include <QuartzCore/QuartzCore.h>


#include <string.h>

#include "nglWindow_Cocoa.h"
#include "nglDragAndDrop_Cocoa.h"

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <CoreServices/CoreServices.h>

#define NGL_WINDOW_FPS 60.0f

//#include "nglImage.h"

#define NGL_WINDOW_EBASE      (NGL_CONTEXT_ELAST+1)
#define NGL_WINDOW_ESYSWIN    (NGL_WINDOW_EBASE+1)
#define NGL_WINDOW_EVIDMODE   (NGL_WINDOW_EBASE+2)
#define NGL_WINDOW_ERANGE     (NGL_WINDOW_EBASE+3)

const nglChar* gpWindowErrorTable[] =
{
  /*  0 */ _T("No error"),
  /*  1 */ _T("System window creation failed"),
  /*  2 */ _T("Full screen video mode unavailable"),
  /*  3 */ _T("Properties out of range"),
  NULL
};

#ifndef _DEBUG_
#undef NGL_OUT
#define NGL_OUT {}
#endif

#if defined(_MULTI_TOUCHES_) && 0//defined(_DEBUG_)
# define NGL_TOUCHES_OUT NGL_OUT
#else//!_MULTI_TOUCHES_
# define NGL_TOUCHES_OUT {}
#endif//!_MULTI_TOUCHES_

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFRunLoop.h>

////////////////////////////////////////////////////////////////////////////////
// Keycodes ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark Keycodes

nglKeyCode ngl_scode_table[0x80] =
{
  /*  0 */ NK_A, NK_S, NK_D, NK_F, NK_H, NK_G, NK_Z, NK_X, NK_C, NK_V,
  /* 10 */ NK_GRAVE, NK_B, NK_Q, NK_W, NK_E, NK_R, NK_Y, NK_T, NK_1, NK_2,
  /* 20 */ NK_3, NK_4, NK_6, NK_5, NK_EQUAL, NK_9, NK_7, NK_MINUS, NK_8, NK_0,
  /* 30 */ NK_RBRACKET, NK_O, NK_U, NK_LBRACKET, NK_I, NK_P, NK_ENTER, NK_L, NK_J, NK_APOSTROPHE,
  /* 40 */ NK_K, NK_SEMICOLON, NK_NUMBERSIGN, NK_COMMA, NK_SLASH, NK_N, NK_M, NK_PERIOD, NK_TAB, NK_SPACE,
  /* 50 */ NK_BACKSLASH, NK_BACKSPACE, 0, NK_ESC, 0, 0, 0, 0, 0, 0,
  /* 60 */ 0, 0, 0, 0, 0, NK_PAD_PERIOD, 0, NK_PAD_ASTERISK, 0, NK_PAD_PLUS,
  /* 70 */ 0, NK_PAD_LOCK, 0, 0, 0, NK_PAD_SLASH, NK_PAD_ENTER, 0, NK_PAD_MINUS, 0,
  /* 80 */ 0, 0, NK_PAD_0, NK_PAD_1, NK_PAD_2, NK_PAD_3, NK_PAD_4, NK_PAD_5, NK_PAD_6, NK_PAD_7,
  /* 90 */ 0, NK_PAD_8, NK_PAD_9, 0, 0, 0, NK_F5, NK_F6, NK_F7, NK_F3,
  /* 100 */ NK_F8, NK_F9, 0, NK_F11, NK_SYSREQ, 0, NK_SCRLOCK, 0, 0, NK_F10,
  /* 110 */ NK_MENU, NK_F12, 0, NK_PAUSE, 0, NK_HOME, NK_PAGEUP, NK_DELETE, NK_F4, NK_END,
  /* 120 */ NK_F2, NK_PAGEDOWN, NK_F1, NK_LEFT, NK_RIGHT, NK_DOWN, NK_UP 
};

std::map<unichar, nglKeyCode> gKeyCodes;

static void InitCocoaToNGLKeyCodes()
{
  if (!gKeyCodes.empty())
    return;
  gKeyCodes[NSUpArrowFunctionKey] = NK_UP;
  gKeyCodes[NSDownArrowFunctionKey] = NK_DOWN;
  gKeyCodes[NSLeftArrowFunctionKey] = NK_LEFT;
  gKeyCodes[NSRightArrowFunctionKey] = NK_RIGHT;
  gKeyCodes[NSF1FunctionKey] = NK_F1;
  gKeyCodes[NSF2FunctionKey] = NK_F2;
  gKeyCodes[NSF3FunctionKey] = NK_F3;
  gKeyCodes[NSF4FunctionKey] = NK_F4;
  gKeyCodes[NSF5FunctionKey] = NK_F5;
  gKeyCodes[NSF6FunctionKey] = NK_F6;
  gKeyCodes[NSF7FunctionKey] = NK_F7;
  gKeyCodes[NSF8FunctionKey] = NK_F8;
  gKeyCodes[NSF9FunctionKey] = NK_F9;
  gKeyCodes[NSF10FunctionKey] = NK_F10;
  gKeyCodes[NSF11FunctionKey] = NK_F11;
  gKeyCodes[NSF12FunctionKey] = NK_F12;
  //  gKeyCodes[NSF13FunctionKey] = ;
  //  gKeyCodes[NSF14FunctionKey] = ;
  //  gKeyCodes[NSF15FunctionKey] = ;
  //  gKeyCodes[NSF16FunctionKey] = ;
  //  gKeyCodes[NSF17FunctionKey] = ;
  //  gKeyCodes[NSF18FunctionKey] = ;
  //  gKeyCodes[NSF19FunctionKey] = ;
  //  gKeyCodes[NSF20FunctionKey ] = ;
  //  gKeyCodes[NSF21FunctionKey ] = ;
  //  gKeyCodes[NSF22FunctionKey ] = ;
  //  gKeyCodes[NSF23FunctionKey ] = ;
  //  gKeyCodes[NSF24FunctionKey ] = ;
  //  gKeyCodes[NSF25FunctionKey ] = ;
  //  gKeyCodes[NSF26FunctionKey ] = ;
  //  gKeyCodes[NSF27FunctionKey ] = ;
  //  gKeyCodes[NSF28FunctionKey ] = ;
  //  gKeyCodes[NSF29FunctionKey ] = ;
  //  gKeyCodes[NSF30FunctionKey ] = ;
  //  gKeyCodes[NSF31FunctionKey ] = ;
  //  gKeyCodes[NSF32FunctionKey ] = ;
  //  gKeyCodes[NSF33FunctionKey ] = ;
  //  gKeyCodes[NSF34FunctionKey ] = ;
  //  gKeyCodes[NSF35FunctionKey ] = ;
  gKeyCodes[NSInsertFunctionKey ] = NK_INSERT;
  gKeyCodes[NSDeleteFunctionKey ] = NK_DELETE;
  gKeyCodes[NSHomeFunctionKey   ] = NK_HOME;
  //gKeyCodes[NSBeginFunctionKey  ] =;
  //gKeyCodes[NSEndFunctionKey    ] = ;
  gKeyCodes[NSPageUpFunctionKey ] = NK_PAGEUP;
  gKeyCodes[NSPageDownFunctionKey    ] = NK_PAGEDOWN;
  //gKeyCodes[NSPrintScreenFunctionKey ] = ;
  gKeyCodes[NSScrollLockFunctionKey  ] = NK_SCRLOCK;
  gKeyCodes[NSPauseFunctionKey       ] = NK_PAUSE;
  gKeyCodes[NSSysReqFunctionKey      ] = NK_SYSREQ;
  //gKeyCodes[NSBreakFunctionKey       ] = ;
  //gKeyCodes[NSResetFunctionKey       ] = ;
  //gKeyCodes[NSStopFunctionKey        ] = ;
  gKeyCodes[NSMenuFunctionKey        ] = NK_MENU;
  //gKeyCodes[NSUserFunctionKey        ] = ;
  //gKeyCodes[NSSystemFunctionKey      ] = ;
  //gKeyCodes[NSPrintFunctionKey       ] = ;
  //gKeyCodes[NSClearLineFunctionKey   ] = ;
  //gKeyCodes[NSClearDisplayFunctionKey] = ;
  //gKeyCodes[NSInsertLineFunctionKey  ] = ;
  //gKeyCodes[NSDeleteLineFunctionKey  ] = ;
  //gKeyCodes[NSInsertCharFunctionKey  ] = ;
  //gKeyCodes[NSDeleteCharFunctionKey  ] = ;
  //gKeyCodes[NSPrevFunctionKey        ] = ;
  //gKeyCodes[NSNextFunctionKey        ] = ;
  //gKeyCodes[NSSelectFunctionKey      ] = ;
  //gKeyCodes[NSExecuteFunctionKey     ] = ;
  //gKeyCodes[NSUndoFunctionKey        ] = ;
  //gKeyCodes[NSRedoFunctionKey        ] = ;
  //gKeyCodes[NSFindFunctionKey        ] = ;
  //gKeyCodes[NSHelpFunctionKey        ] = ;
  //gKeyCodes[NSModeSwitchFunctionKey  ] = ;
  
  // Normal keys:
  gKeyCodes[' '] = NK_SPACE;
}

nglKeyCode CocoaToNGLKeyCode(unichar c, uint16 scanCode)
{
  InitCocoaToNGLKeyCodes();
  std::map<unichar, nglKeyCode>::const_iterator it = gKeyCodes.find(c);
  if (scanCode < 0x80)
    return ngl_scode_table[scanCode];
  if (it != gKeyCodes.end())
    return it->second;
  return NK_NONE;
}


////////////////////////////////////////////////////////////////////////////////
// Scaling /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark Scaling

static float gScaleFactor = 0.0f;
static float gInvScaleFactor = 0.0f;

float nuiGetScaleFactor()
{
  if (gScaleFactor == 0)
  {
    gScaleFactor = [[NSScreen mainScreen] backingScaleFactor];
  }
  return gScaleFactor;
}

float nuiGetInvScaleFactor()
{
  if (gInvScaleFactor == 0)
  {
    gInvScaleFactor = 1.0f / nuiGetScaleFactor();
  }
  
  return gInvScaleFactor;
}


////////////////////////////////////////////////////////////////////////////////
// CVDisplayLink callback //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark CVDisplayLink callback

CVReturn CVDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* pNGLWindow)
{
///< Add threaded rendering callbacks here
  return kCVReturnSuccess;
}


////////////////////////////////////////////////////////////////////////////////
// nglNSView ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglNSView

@implementation nglNSView
- (id)initWithNGLWindow:(nglWindow*)pNGLWindow
{
  self = [super init];
  if(self == nil)
    return nil;
  
  mpNGLWindow = pNGLWindow;

  [self registerForDraggedTypes: [NSArray arrayWithObjects: @"public.file-url", NSFilenamesPboardType,(NSString*)kUTTypePlainText,(NSString*)kUTTypeUTF8PlainText, NSURLPboardType, NSFilesPromisePboardType, nil]];

  return self;
}

-(void) dealloc
{
  [super dealloc];
}


- (void) windowDidResize: (NSNotification *)notification
{
  NSWindow* win = [notification object];
  NSRect rect = {0};
  rect = [win frame];
  [win resize: [win contentRectForFrameRect: rect].size];
}

-(void)windowWillClose:(NSNotification *)note
{
  //[[NSApplication sharedApplication] terminate:self];
  [super windowWillClose:note];
  mpNGLWindow->CallOnDestruction();
}

- (void)lockFocus
{
  NSOpenGLContext* context = (NSOpenGLContext*)mpNGLWindow->mpNSGLContext;
  
  // make sure we are ready to draw
  [super lockFocus];
  
  // when we are about to draw, make sure we are linked to the view
  // It is not possible to call setView: earlier (will yield 'invalid drawable')
  if ([context view] != self)
  {
    [context setView:self];
  }
  
  if (!mInitiated)
  {
    mInitiated = true;
    mpNGLWindow->CallOnCreation();
  }

  // make us the current OpenGL context
  [context makeCurrentContext];
}

// this is called whenever the view changes (is unhidden or resized)
- (void)drawRect:(NSRect)frameRect
{
  NSOpenGLContext* _context = (NSOpenGLContext*)mpNGLWindow->mpNSGLContext;
  [_context update];
  mpNGLWindow->CallOnPaint();
}

// this tells the window manager that nothing behind our view is visible
-(BOOL) isOpaque
{
  return YES;
}

- (void)keyDown:(NSEvent *)theEvent
{
  if (![(nglNSWindow*)[self window] keyDown:theEvent])
    [super keyDown: theEvent];
}

- (void)keyUp:(NSEvent *)theEvent
{
  if (![(nglNSWindow*)[self window] keyUp:theEvent])
    [super keyUp: theEvent];
}

- (void)flagsChanged:(NSEvent *)theEvent
{
  if (![(nglNSWindow*)[self window] flagsChanged:theEvent])
    [super flagsChanged: theEvent];
}

- (void)viewDidChangeBackingProperties
{
  [super viewDidChangeBackingProperties];
  mpNGLWindow->CallOnRescale([[self window] backingScaleFactor]);
}

NSString *kPrivateDragUTI = @"com.libnui.privatepasteboardtype";


// Drag and drop:
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
  return [(nglNSWindow*)[self window] draggingEntered:sender];
}

- (NSDragOperation)draggingUpdated:(id < NSDraggingInfo >)sender
{
  return [(nglNSWindow*)[self window] draggingUpdated:sender];
}


- (void)draggingExited:(id <NSDraggingInfo>)sender
{
  return [(nglNSWindow*)[self window] draggingExited:sender];
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
  return [(nglNSWindow*)[self window] prepareForDragOperation:sender];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
  return [(nglNSWindow*)[self window] performDragOperation:sender];
}

@end


////////////////////////////////////////////////////////////////////////////////
// nglNSWindow /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglNSWindow

@implementation nglNSWindow

- (id) initWithFrame: (NSRect) rect andNGLWindow: (nglWindow*) pNGLWindow
{
  BOOL deffering = YES;
  uint32 styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
  NSBackingStoreType buffering = NSBackingStoreBuffered;

  self = [self initWithContentRect:rect styleMask:styleMask backing:buffering defer:deffering];
  
  if (!self)
  {
    NGL_ASSERT(0);
    return nil;
  }

  mpNGLWindow = pNGLWindow;
  mModifiers = 0;
  mpDropObject = NULL;

  [self setAcceptsMouseMovedEvents:TRUE];

  return self;
}

- (void) dealloc
{
  mpNGLWindow->CallOnDestruction();

  [super dealloc];
}

- (void)mouseDown:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = 1 << [theEvent buttonNumber];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  info.Buttons |= [theEvent clickCount] > 1 ? nglMouseInfo::ButtonDoubleClick : 0;
  mpNGLWindow->CallOnMouseClick(info);
}

- (void)mouseUp:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = 1 << [theEvent buttonNumber];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseUnclick(info);
}

- (void)mouseMoved:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = (uint64)[NSEvent pressedMouseButtons];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseMove(info);
}

- (void)mouseDragged:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = (uint64)[NSEvent pressedMouseButtons];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseMove(info);
}

////
- (void)rightMouseDown:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = 1 << [theEvent buttonNumber];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  info.Buttons |= [theEvent clickCount] > 1 ? nglMouseInfo::ButtonDoubleClick : 0;
  mpNGLWindow->CallOnMouseClick(info);
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = 1 << [theEvent buttonNumber];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseUnclick(info);
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = (uint64)[NSEvent pressedMouseButtons];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseMove(info);
}

- (void)scrollWheel:(NSEvent *)theEvent
{
  float x = [theEvent deltaX];
  float y = [theEvent deltaY];
  
  nglMouseInfo info;
  info.Buttons = 0;
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  info.DeltaX = x;
  info.DeltaY = -y;

  if (!mpNGLWindow->CallOnMouseWheel(info))
  {
    if (x > 0)
    {
      info.Buttons = nglMouseInfo::ButtonWheelLeft;
      mpNGLWindow->CallOnMouseClick(info);
      mpNGLWindow->CallOnMouseUnclick(info);
    }
    else if (x < 0)
    {
      info.Buttons = nglMouseInfo::ButtonWheelRight;
      mpNGLWindow->CallOnMouseClick(info);
      mpNGLWindow->CallOnMouseUnclick(info);
    }

    if (y > 0)
    {
      info.Buttons = nglMouseInfo::ButtonWheelUp;
      mpNGLWindow->CallOnMouseClick(info);
      mpNGLWindow->CallOnMouseUnclick(info);
    }
    else if (y < 0)
    {
      info.Buttons = nglMouseInfo::ButtonWheelDown;
      mpNGLWindow->CallOnMouseClick(info);
      mpNGLWindow->CallOnMouseUnclick(info);
    }
  }
}
//////

- (void)otherMouseDown:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = 1 << [theEvent buttonNumber];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  info.Buttons |= [theEvent clickCount] > 1 ? nglMouseInfo::ButtonDoubleClick : 0;
  mpNGLWindow->CallOnMouseClick(info);
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = 1 << [theEvent buttonNumber];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseUnclick(info);
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
  nglMouseInfo info;
  info.Buttons = (uint64)[NSEvent pressedMouseButtons];
  info.TouchId = 0;
  NSPoint p = [self mouseLocationOutsideOfEventStream];
  info.X = p.x;
  info.Y = [self contentRectForFrameRect:[self frame]].size.height - p.y;
  mpNGLWindow->CallOnMouseMove(info);
}

////////// Keyboard:
- (BOOL)keyDown:(NSEvent *)theEvent
{
  NSString *chars = [theEvent characters];
  NSString *rawchars = [theEvent charactersIgnoringModifiers];
  nglString c((CFStringRef)chars);
  nglString rc((CFStringRef)rawchars);
  //printf("Key Down: '%s' / '%s'.\n", c.GetChars(), rc.GetChars());
  if ( [rawchars length] == 1 )
  {
    unichar keyChar = [chars characterAtIndex:0];
    unichar keyRawChar = [rawchars characterAtIndex:0];
    uint16 scanCode = [theEvent keyCode];
    nglKeyCode keyCode = CocoaToNGLKeyCode(keyRawChar, scanCode);
    nglKeyEvent event(keyCode, keyChar, keyRawChar);
    if (mpNGLWindow->CallOnKeyDown(event))
      return true;
  }
  
  if (c.IsEmpty())
    return false;
  
  return mpNGLWindow->CallOnTextInput(c);
}

- (BOOL)keyUp:(NSEvent *)theEvent
{
  NSString *chars = [theEvent characters];
  NSString *rawchars = [theEvent charactersIgnoringModifiers];
  nglString c((CFStringRef)chars);
  nglString rc((CFStringRef)rawchars);
  //printf("Key Up: '%s' / '%s'.\n", c.GetChars(), rc.GetChars());
  if ( [rawchars length] == 1 )
  {
    unichar keyChar = [chars characterAtIndex:0];
    unichar keyRawChar = [rawchars characterAtIndex:0];
    uint16 scanCode = [theEvent keyCode];
    nglKeyCode keyCode = CocoaToNGLKeyCode(keyRawChar, scanCode);
    nglKeyEvent event(keyCode, keyChar, keyRawChar);
    if (mpNGLWindow->CallOnKeyUp(event))
      return true;
  }
  return false;
}

- (BOOL)flagsChanged:(NSEvent *)theEvent
{
  uint32 modKeys = [theEvent modifierFlags];
  UInt32 upKeys;
  UInt32 dnKeys;
  upKeys = ~modKeys & mModifiers;
  dnKeys = modKeys & ~mModifiers;
  mModifiers = modKeys;
  
  bool res = false;
  if (upKeys & NSShiftKeyMask)
    res |= mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_LSHIFT, 0, 0));
  if (upKeys & NSControlKeyMask)
    res |= mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_LCTRL, 0, 0));
  if (upKeys & NSAlternateKeyMask)
    res |= mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_LALT, 0, 0));
  if (upKeys & NSAlphaShiftKeyMask)
    res |= mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_CAPSLOCK, 0, 0));
  if (upKeys & NSCommandKeyMask)
    res |= mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_META, 0, 0));
  
  if (dnKeys & NSShiftKeyMask)
    res |= mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_LSHIFT, 0, 0));
  if (dnKeys & NSControlKeyMask)
    res |= mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_LCTRL, 0, 0));
  if (dnKeys & NSAlternateKeyMask)
    res |= mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_LALT, 0, 0));
  if (dnKeys & NSAlphaShiftKeyMask)
    res |= mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_CAPSLOCK, 0, 0));
  if (dnKeys & NSCommandKeyMask)
    res |= mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_META, 0, 0));
  
  
  //NSAlphaShiftKeyMask
  //NSShiftKeyMask
  //NSControlKeyMask
  //NSAlternateKeyMask
  //NSCommandKeyMask
  //NSNumericPadKeyMask
  //NSHelpKeyMask
  //NSFunctionKeyMask

  return res;
}

- (void)close
{
  if (mpNGLWindow)
    mpNGLWindow->CallOnClose();
}

- (void)Unregister
{
  mpNGLWindow = NULL;
}

//////////
- (void)resize: (NSSize) size
{
  //printf("resize %f x %f\n", size.width, size.height);
  mpNGLWindow->CallOnResize(size.width, size.height);
}

- (BOOL)acceptsMouseMovedEvents
{
  return YES;
}

// Drag and drop:
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
  NSLog(@"Dragging entered at %f,%f\n", [sender draggingLocation].x, [sender draggingLocation].y);
  /*------------------------------------------------------
   method called whenever a drag enters our drop zone
   --------------------------------------------------------*/
  nglNSView* pView = [self contentView];

  mpDropObject = new nglDragAndDrop(eDropEffectCopy, NULL, 0, 0);

  /* When an image from one window is dragged over another, we want to resize the dragging item to
   * preview the size of the image as it would appear if the user dropped it in. */
  [sender enumerateDraggingItemsWithOptions:NSDraggingItemEnumerationConcurrent
                                    forView:pView
                                    classes:[NSArray arrayWithObject:[NSPasteboardItem class]]
                              searchOptions:nil
                                 usingBlock:^(NSDraggingItem *draggingItem, NSInteger idx, BOOL *stop)
   {
     NSArray* array = [[draggingItem item] types];
     for (int i = 0; i < [array count]; i++)
     {
       NSString* str = (NSString*)[array objectAtIndex:i];
       NSLog(@"  Dragging item %d - type %d %@\n", (int)idx, i, str);
     }
   }];

  NSPasteboard* pboard = [sender draggingPasteboard];
  NSArray* types = [pboard types];
  if ( [types containsObject:NSFilenamesPboardType] )
  {
    nglDataFilesObject* pObj = new nglDataFilesObject("ngl/Files");
    NSArray* filenames = [pboard propertyListForType:NSFilenamesPboardType];
    for (int i = 0; i < [filenames count]; i++)
    {
      NSString* fname = [filenames objectAtIndex:i];
      nglString str((CFStringRef)fname);
      pObj->AddFile(str);
      NSLog(@"File %d: %@\n", i, fname);
    }

    mpDropObject->AddType(pObj);
  }
  else if ( [types containsObject:NSURLPboardType] )
  {
    nglDataFilesObject* pObj = new nglDataFilesObject("ngl/Files");
    NSArray* filenames = [pboard propertyListForType:NSURLPboardType];
    for (int i = 0; i < [filenames count]; i++)
    {
      NSString* fname = [filenames objectAtIndex:i];
      nglString str((CFStringRef)fname);
      pObj->AddFile(str);
      NSLog(@"URL %d: %@\n", i, fname);
    }

    mpDropObject->AddType(pObj);
  }

  if ( [types containsObject:(NSString*)kUTTypeUTF8PlainText] )
  {
    NSData* data = [pboard dataForType:(NSString*)kUTTypeUTF8PlainText];
    NSString* string = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
    nglDataTextObject* pObj = new nglDataTextObject("ngl/Text");
    pObj->SetData(nglString((CFStringRef)string));
    mpDropObject->AddType(pObj);
    NSLog(@"UTF8 text String: %@\n", string);
  }
  else if ( [types containsObject:(NSString*)kUTTypePlainText] )
  {
    NSData* data = [pboard dataForType:(NSString*)kUTTypePlainText];
    NSString* string = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
    nglDataTextObject* pObj = new nglDataTextObject("ngl/Text");
    pObj->SetData(nglString((CFStringRef)string));
                  mpDropObject->AddType(pObj);
    NSLog(@"Plain text String: %@\n", string);
  }

  if ( [types containsObject:(NSString*)kUTTypeText] )
  {
    NSData* data = [pboard dataForType:(NSString*)kUTTypeText];
    NSString* string = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
    nglDataTextObject* pObj = new nglDataTextObject("ngl/Text");
    pObj->SetData(nglString((CFStringRef)string));
                  mpDropObject->AddType(pObj);
    NSLog(@"Text String: %@\n", string);
  }

  if ( [types containsObject:NSFilesPromisePboardType] )
  {
    NSArray* filenames = [pboard propertyListForType:NSFilesPromisePboardType];
    for (int i = 0; i < [filenames count]; i++)
    {
      NSString* fname = [filenames objectAtIndex:i];
      NSLog(@"File %d: %@\n", i, fname);
    }
  }

  mpNGLWindow->OnDragEnter();
  nglDropEffect effect = mpNGLWindow->OnCanDrop(mpDropObject, [sender draggingLocation].x, [sender draggingLocation].y, [NSEvent pressedMouseButtons]);
  switch (effect)
  {
    case eDropEffectNone:
    default:
      return NSDragOperationNone;

    case eDropEffectCopy:
      return NSDragOperationCopy;

    case eDropEffectMove:
      return NSDragOperationMove;

    case eDropEffectLink:
      return NSDragOperationLink;

    case eDropEffectScroll:
      return NSDragOperationGeneric;
  }
}

- (NSDragOperation)draggingUpdated:(id < NSDraggingInfo >)sender
{
  //NSLog(@"Dragging updated at %f,%f\n", [sender draggingLocation].x, [sender draggingLocation].y);
  nglDropEffect effect = mpNGLWindow->OnCanDrop(mpDropObject, [sender draggingLocation].x, [sender draggingLocation].y, [NSEvent pressedMouseButtons]);
  switch (effect)
  {
    case eDropEffectNone:
    default:
      return NSDragOperationNone;

    case eDropEffectCopy:
      return NSDragOperationCopy;

    case eDropEffectMove:
      return NSDragOperationMove;

    case eDropEffectLink:
      return NSDragOperationLink;

    case eDropEffectScroll:
      return NSDragOperationGeneric;
  }
}


- (void)draggingExited:(id <NSDraggingInfo>)sender
{
  /*------------------------------------------------------
   method called whenever a drag exits our drop zone
   --------------------------------------------------------*/
  //remove highlight of the drop zone
  NSLog(@"Dragging exited at %f,%f\n", [sender draggingLocation].x, [sender draggingLocation].y);
  delete mpDropObject;
  mpDropObject = NULL;
  mpNGLWindow->OnDragLeave();
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
  NSLog(@"Prepare for drag operation at %f,%f\n", [sender draggingLocation].x, [sender draggingLocation].y);
  /*------------------------------------------------------
   method to determine if we can accept the drop
   --------------------------------------------------------*/
  nglDropEffect effect = mpNGLWindow->OnCanDrop(mpDropObject, [sender draggingLocation].x, [sender draggingLocation].y, [NSEvent pressedMouseButtons]);
  switch (effect)
  {
    case eDropEffectNone:
    default:
      return NO;

    case eDropEffectCopy:
    case eDropEffectMove:
    case eDropEffectLink:
    case eDropEffectScroll:
      return YES;
  }
  return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
  //finished with the drag so remove any highlighting
  mpNGLWindow->OnDropped(mpDropObject, [sender draggingLocation].x, [sender draggingLocation].y, [NSEvent pressedMouseButtons]);
  delete mpDropObject;
  mpDropObject = NULL;
  return YES;
}
@end///< nglNSWindow


////////////////////////////////////////////////////////////////////////////////
// nglNSWindowController ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglNSWindowController

@implementation nglNSWindowController

- (id)initWithNGLWindow:(nglWindow *)pNGLWindow;
{
  self = [super initWithWindow:nil];
  
  if (self)
  {
    mpNGLWindow = pNGLWindow;
  }
  else
  {
    NGL_ASSERT(0);
  }
  
  return self;
}

- (void) keyDown:(NSEvent *)event
{
  //  unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
  //
  //  switch (c)
  //  {
  //      // Handle [ESC] key
  //    case 27:
  //      if(fullscreenWindow != nil)
  //      {
  //        [self goWindow];
  //      }
  //      return;
  //      // Have f key toggle fullscreen
  //    case 'f':
  //      if(fullscreenWindow == nil)
  //      {
  //        [self goFullscreen];
  //      }
  //      else
  //      {
  //        [self goWindow];
  //      }
  //      return;
  //  }
  //
  // Allow other character to be handled (or not and beep)
  [super keyDown:event];
}

@end


////////////////////////////////////////////////////////////////////////////////
// nglWindow ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglWindow

/*
 * OS specific info
 */

nglWindow::OSInfo::OSInfo()
{
  mpNSWindow = NULL;
}

/*
 * Constructors
 */

nglWindow::nglWindow (uint Width, uint Height, bool IsFullScreen)
{
  Register();
  nglContextInfo context; // Get default context
  nglWindowInfo info(Width, Height, IsFullScreen);
  InternalInit (context, info, NULL); 
}

nglWindow::nglWindow (const nglContextInfo& rContext, const nglWindowInfo& rInfo, const nglContext* pShared)
{
  Register();
  InternalInit (rContext, rInfo, pShared);
}


void OnCFRunLoopTicked(CFRunLoopTimerRef pTimer, void* pUserData)
{
  nglWindow* pWindow = (nglWindow*)pUserData;
  NGL_ASSERT(pWindow);
  nglTime now;
  pWindow->mpAnimationTimer->OnTick(now - pWindow->mLastTick);
  pWindow->mLastTick = now;
  pWindow->CallOnPaint();
}


void nglWindow::InternalInit (const nglContextInfo& rContext, const nglWindowInfo& rInfo, const nglContext* pShared)
{
  //[self registerForDraggedTypes: [NSArray arrayWithObjects: @"public.file-url", NSFilenamesPboardType,(NSString*)kUTTypePlainText,(NSString*)kUTTypeUTF8PlainText, NSURLPboardType, NSFilesPromisePboardType, nil]];
  App->GetDataTypesRegistry().RegisterDataType(_T("ngl/Text"), kUTTypeText, nglDataTextObject::Create);
  App->GetDataTypesRegistry().RegisterDataType(_T("ngl/Text"), kUTTypePlainText, nglDataTextObject::Create);
  App->GetDataTypesRegistry().RegisterDataType(_T("ngl/Text"), kUTTypeUTF8PlainText, nglDataTextObject::Create);
  App->GetDataTypesRegistry().RegisterDataType(_T("ngl/Files"), "public.file-url", nglDataFilesObject::Create);
  //App->GetDataTypesRegistry().RegisterDataType(_T("ngl/PromiseFiles"), kDragFlavorTypePromiseHFS, nglDataFilesObject::Create);

  mState = eHide;
  mAngle = 0;

  SetError (NGL_WINDOW_ENONE);
  SetEventMask(nglWindow::AllEvents);

  mAutoRotate = true;
  
  mAngle = rInfo.Rotate;
  NSRect rect = [[NSScreen mainScreen] visibleFrame];
  mWidth = 0;
  mHeight = 0;
  rect.origin.x = rInfo.XPos;
  rect.origin.y = rect.size.height - rInfo.YPos;
  rect.size.width = rInfo.Width;
  rect.size.height = rInfo.Height;

  switch (rInfo.Pos)
  {
    case nglWindowInfo::ePosUser:
      break;
    case nglWindowInfo::ePosCenter:
      rect.origin.x = (rect.origin.x - rect.size.width) / 2;
      rect.origin.y = (rect.origin.y - rect.size.height) / 2;
      break;
    case nglWindowInfo::ePosMouse:
      break;
    case nglWindowInfo::ePosAuto:
      break;
  }
  
  NSOpenGLContext* _shared_ctx = nil;
  if (pShared)
  {
    _shared_ctx = (NSOpenGLContext*)((nglWindow*)pShared)->mpNSGLContext;
  }

  
  nglNSWindow* _window = [[nglNSWindow alloc] initWithFrame:rect andNGLWindow: this];
  mOSInfo.mpNSWindow = _window;
  mpNSWindow = _window;

  SetTitle(rInfo.Title);

  nglNSView* _view = [[nglNSView alloc] initWithNGLWindow:this];
  mpNSView = _view;
  [_window setContentView: _view];
  [_window setDelegate: _view];
  
  if ([_view respondsToSelector:@selector(setWantsBestResolutionOpenGLSurface:)])
    [_view setWantsBestResolutionOpenGLSurface: YES];
  
  NSOpenGLPixelFormatAttribute attribs[] =
  {
    NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)24,
    NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFABackingStore,
    NSOpenGLPFAMultisample,
    NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
    NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,
    //NSOpenGLPFAWindow,
    (NSOpenGLPixelFormatAttribute)0
  };
  NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
  NSOpenGLContext* _context = [[NSOpenGLContext alloc] initWithFormat: format shareContext: _shared_ctx];

  GLint v = 1;
  [_context setValues:&v forParameter:NSOpenGLCPSwapInterval];
  [_context setView: _view];
  [_context makeCurrentContext];
  mpNSGLContext = _context;

  CallOnRescale([_window backingScaleFactor]);

  //[pNSWindow makeKeyAndVisible];
  
  
  if (rContext.TargetAPI != eTargetAPI_OpenGL && rContext.TargetAPI != eTargetAPI_OpenGL2)
  {
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("bad renderer"));
    NGL_ASSERT(0);
    return;
  }

  mWidth = rect.size.width;
  mHeight = rect.size.height;
	
  Build(rContext);


  
  mpAnimationTimer = nuiAnimation::AcquireTimer();
  mpAnimationTimer->Stop();

  CFRunLoopTimerContext _timer_ctx;
  _timer_ctx.version = 0;
  _timer_ctx.info = (void*)this;
  _timer_ctx.retain = NULL;
  _timer_ctx.release = NULL;
  _timer_ctx.copyDescription = NULL;
  
  CFAbsoluteTime absTime = CFAbsoluteTimeGetCurrent() + (1.0/60.0);
//CFAllocatorRef allocator, CFAbsoluteTime fireDate, CFTimeInterval interval, CFOptionFlags flags, CFIndex order, CFRunLoopTimerCallBack callout, CFRunLoopTimerContext *context)
  mpCFRunLoopTimer = CFRunLoopTimerCreate( kCFAllocatorDefault,
                                           absTime,
                                           1.0/60.0,
                                           0,// option flags, not implemented so far
                                           0,// timer order, not implemented so far
                                           &OnCFRunLoopTicked,
                                           &_timer_ctx);

  if (!mpCFRunLoopTimer)
  {
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("Couldn't start runloop timer"));
    NGL_ASSERT(0);
  }
  
  mLastTick = nglTime();
  CFRunLoopAddTimer(CFRunLoopGetCurrent(), mpCFRunLoopTimer, kCFRunLoopCommonModes);
}




nglWindow::~nglWindow()
{
  NGL_ASSERT(mpCFRunLoopTimer);
  CFRunLoopRef currentRunLoop = CFRunLoopGetCurrent();
  CFRunLoopRemoveTimer(currentRunLoop, mpCFRunLoopTimer, kCFRunLoopCommonModes);
  if (mpCFRunLoopTimer)
    CFRelease(mpCFRunLoopTimer);
  mpCFRunLoopTimer = NULL;
  nuiAnimation::ReleaseTimer();

  [(id)mpNSWindow Unregister];
  Unregister();
}

/*
 * All services
 */

void nglWindow::SetState (StateChange State)
{
  if (mState == State)
    return;

	switch (State)
	{
		case eHide:
      [(id)mpNSWindow hide:nil];
			break;
		case eShow:
      [(id)mpNSWindow makeKeyAndOrderFront:nil];
			break;
		case eMinimize:
      [(id)mpNSWindow makeKeyAndOrderFront:nil];
			break;
		case eMaximize:
      [(id)mpNSWindow makeKeyAndOrderFront:nil];
			break;
	};
}

nglWindow::StateInfo nglWindow::GetState() const
{
  return eVisible;
}

void nglWindow::GetSize (uint& rWidth, uint& rHeight) const
{
  rWidth  = mWidth;
  rHeight = mHeight;
}

uint nglWindow::GetWidth () const
{
	return mWidth;
}

uint nglWindow::GetHeight () const
{
  return mHeight;
}

bool nglWindow::SetSize (uint Width, uint Height)
{
  NGL_OUT(_T("nglWindow::SetSize(%d, %d)\n"), Width, Height);
  mWidth  = Width;
  mHeight = Height;
  CallOnResize(Width, Height);
  return false;
}

void nglWindow::GetPosition (int& rXPos, int& rYPos) const
{
  // FIXME
}

bool nglWindow::SetPosition (int XPos, int YPos)
{
  // FIXME
  return false;
}

nglString nglWindow::GetTitle() const
{
  NSString* pR = [(id)mpNSWindow title];
  nglString r((CFStringRef)pR);
  [pR release];
  return r;
}

void nglWindow::SetTitle (const nglString& rTitle)
{
  [(id)mpNSWindow setTitle: (NSString*)rTitle.ToCFString()];
}

bool IsThisKeyDown(const short theKey)
{
  return CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, theKey);
}


bool IsCommandKeyDown()
{
  const short kCommandKey = 55;
  return IsThisKeyDown(kCommandKey);
}

bool IsControlKeyDown()
{
  const short kCtlKey = 0x3B;
  return IsThisKeyDown(kCtlKey);
}

bool IsOptionKeyDown()
{
  const short kOptionKey = 58;
  return IsThisKeyDown(kOptionKey);
}

bool IsShiftKeyDown()
{
  const short kShiftKey = 56;
  return IsThisKeyDown(kShiftKey);
}

bool nglWindow::IsKeyDown (nglKeyCode Key) const
{
  if (Key == NK_LMETA || Key == NK_RMETA || Key == NK_META)
    return IsCommandKeyDown();
  else if (Key == NK_LCTRL || Key == NK_RCTRL || Key == NK_CTRL)
    return IsControlKeyDown();
  else if (Key == NK_LSHIFT || Key == NK_RSHIFT || Key == NK_SHIFT)
    return IsShiftKeyDown();
  else if (Key == NK_LALT || Key == NK_RALT || Key == NK_ALT)
    return IsOptionKeyDown();
  else if ((Key > 0) && (Key <= NGL_KEY_MAX))
    return mpKeyState[Key-1];
  else
    return false;
}

void nglWindow::GetMouse (nglMouseInfo& rMouseInfo, bool Local) const
{
}

const nglWindow::OSInfo* nglWindow::GetOSInfo() const
{
  return &mOSInfo;
}


#pragma mark CVDisplayLink setup

void nglWindow::AcquireDisplayLink()
{
  mpAnimationTimer = nuiAnimation::AcquireTimer();
  mpAnimationTimer->Stop();
  
  if (kCVReturnSuccess != CVDisplayLinkCreateWithActiveCGDisplays(&mDisplayLink))
  {
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("CVDisplayLinkCreateWithActiveCGDisplays returned an error"));
    NGL_ASSERT(0);
  }
  if (kCVReturnSuccess != CVDisplayLinkSetOutputCallback(mDisplayLink, &CVDisplayLinkCallback, this))
  {
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("CVDisplayLinkSetOutputCallback returned an error"));
    NGL_ASSERT(0);
  }

  NSOpenGLContext* _context = (NSOpenGLContext*)mpNSGLContext;
  CGLPixelFormatObj cglPixelFormat = [[_context pixelFormat] CGLPixelFormatObj];
  CGLContextObj cglContext = [_context CGLContextObj];

  if (kCVReturnSuccess != CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(mDisplayLink, cglContext, cglPixelFormat))
  {
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext returned an error"));
    NGL_ASSERT(0);
  }

  if (kCVReturnSuccess != CVDisplayLinkStart(mDisplayLink))
  {
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("CVDisplayLinkStart returned an error"));
    NGL_ASSERT(0);
  }
}

void nglWindow::ReleaseDisplayLink()
{
  if (mDisplayLink != nil)
  {
    CVDisplayLinkStop(mDisplayLink);
    CVDisplayLinkRelease(mDisplayLink);
    mDisplayLink = nil;
  }
  nuiAnimation::ReleaseTimer();
}


#pragma mark GL Context handling

void nglWindow::BeginSession()
{
#ifdef _DEBUG_WINDOW_
  NGL_LOG(_T("window"), NGL_LOG_INFO, _T("BeginSession\n"));
#endif
  NGL_ASSERT(mpNSGLContext);
  [(NSOpenGLContext*)mpNSGLContext makeCurrentContext];
}

void nglWindow::EndSession()
{
#ifndef __NOGLCONTEXT__

#ifdef _DEBUG_WINDOW_
  NGL_LOG(_T("window"), NGL_LOG_INFO, _T("EndSession\n"));
#endif

  NGL_ASSERT(mpNSGLContext);
glFlush();
//  [(NSOpenGLContext*)mpNSGLContext flushBuffer];
  [NSOpenGLContext clearCurrentContext];
#endif
}

bool nglWindow::MakeCurrent() const
{
  NGL_ASSERT(mpNSGLContext);
  [(NSOpenGLContext*)mpNSGLContext makeCurrentContext];
  return true;
}

void nglWindow::Invalidate()
{
  //printf("nglWindow::Invalidate()\n");
  [(nglNSWindow*)mpNSWindow invalidate];
}

bool nglWindow::SetCursor(nuiMouseCursor Cursor)
{
  mCursor = Cursor;
  switch (Cursor)
  {
    case eCursorNone:
      [NSCursor hide]; break;

    case eCursorArrow:
      [[NSCursor arrowCursor] set]; break;
    case eCursorCross:
      [[NSCursor crosshairCursor] set]; break;
    case eCursorIBeam:
      [[NSCursor IBeamCursor] set]; break;
    case eCursorHand:
      [[NSCursor openHandCursor] set]; break;
    case eCursorClosedHand:
      [[NSCursor closedHandCursor] set]; break;
    case eCursorPointingHand:
      [[NSCursor pointingHandCursor] set]; break;
    case eCursorHelp:
      [[NSCursor contextualMenuCursor] set]; break;
    case eCursorWait:
      [[NSCursor arrowCursor] set]; break;
    case eCursorCaret:
      [[NSCursor IBeamCursor] set]; break;
    case eCursorDnD:
      [[NSCursor dragLinkCursor] set]; break;
    case eCursorForbid:
      [[NSCursor operationNotAllowedCursor] set]; break;
    case eCursorMove:
      [[NSCursor openHandCursor] set]; break;
    case eCursorResize:
      [[NSCursor openHandCursor] set]; break;
    case eCursorResizeNS:
      [[NSCursor resizeUpDownCursor] set]; break;
    case eCursorResizeWE:
      [[NSCursor resizeLeftRightCursor] set]; break;
    case eCursorResizeN:
      [[NSCursor resizeUpCursor] set]; break;
    case eCursorResizeS:
      [[NSCursor resizeDownCursor] set]; break;
    case eCursorResizeW:
      [[NSCursor resizeLeftCursor] set]; break;
    case eCursorResizeE:
      [[NSCursor resizeRightCursor] set]; break;
    case eCursorResizeNW:
      [[NSCursor pointingHandCursor] set]; break;
    case eCursorResizeNE:
      [[NSCursor pointingHandCursor] set]; break;
    case eCursorResizeSW:
      [[NSCursor pointingHandCursor] set]; break;
    case eCursorResizeSE:
      [[NSCursor pointingHandCursor] set]; break;
    default:
      break;
  }
  return true;
}

nuiMouseCursor nglWindow::GetCursor() const
{
///< No Cursor relevance here
  return mCursor;
}

nglWindow::EventMask nglWindow::GetEventMask() const
{
  return mEventMask;
}

void nglWindow::SetEventMask(EventMask Events)
{
  mEventMask = Events;
}

bool nglWindow::GetResolution(float& rHorizontal, float& rVertical) const
{
  rHorizontal = rVertical = 72.f;
  return false;
}


void nglWindow::EnterModalState()
{
  SetState(eShow);
  NGL_OUT(_T("nglWindow::EnterModalState NOT IMPLENTED YET!"));
//  NGL_ASSERT(!"Not Implemented");
}

void nglWindow::ExitModalState()
{
  NGL_OUT(_T("nglWindow::ExitModalState NOT IMPLENTED YET!"));
//  NGL_ASSERT(!"Not Implemented");  
}

void nglWindow::StartTextInput(int32 X, int32 Y, int32 W, int32 H)
{
  //[(nglNSWindow*)mpNSWindow showKeyboard];
}

void nglWindow::EndTextInput()
{
  //[(nglNSWindow*)mpNSWindow hideKeyboard];
}

bool nglWindow::IsEnteringText() const
{
  //[(nglNSWindow*)mpNSWindow frame]
  return false;
}


////////////////////////////////////////////////////////////////////////////////
// Drag and drop ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark Drag and drop

/// Drag and Drop:
bool nglWindow::Drag(nglDragAndDrop* pDragObject)
{
  return false;
}

nglDropEffect nglWindow::OnCanDrop(nglDragAndDrop* pDragObject, int X,int Y, nglMouseInfo::Flags Button)
{
  return eDropEffectNone;
}

void nglWindow::OnDragEnter()
{
}

void nglWindow::OnDragLeave()
{
}

void nglWindow::OnDragRequestData(nglDragAndDrop* pDragObject, const nglString& rMimeType)
{
}

void nglWindow::OnDragStop(bool canceled)
{
}

void nglWindow::OnDropped(nglDragAndDrop* pDragObject, int X,int Y, nglMouseInfo::Flags Button)
{
}

int nglWindow::GetStatusBarSize() const
{
  return 0;
}

nglCocoaDragAndDrop::nglCocoaDragAndDrop(nglWindow* pWin)
{
  mpWin = pWin;
}

nglCocoaDragAndDrop::~nglCocoaDragAndDrop()
{
  mpWin = NULL;
}

bool nglCocoaDragAndDrop::Drag(nglDragAndDrop* pDragObject)
{
  return false;
}
