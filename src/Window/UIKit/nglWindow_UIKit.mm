#include "nui.h"

#include <QuartzCore/QuartzCore.h>


#include <string.h>

#include "nglWindow_UIKit.h"

#include <GL/gl.h>

#define NGL_WINDOW_FPS 60.0f

#define USE_MULTISAMPLE 0

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

#if defined(_MULTI_TOUCHES_) && defined(_DEBUG_)
# define NGL_TOUCHES_OUT NGL_OUT
#else//!_MULTI_TOUCHES_
# define NGL_TOUCHES_OUT
#endif//!_MULTI_TOUCHES_

//#define _DEBUG_WINDOW_

@interface RetainedEAGLLayer : CAEAGLLayer
@end

@implementation RetainedEAGLLayer
- (void)setDrawableProperties:(NSDictionary *)drawableProperties
{
  // Copy the dictionary and add/modify the retained property
  NSMutableDictionary *mutableDictionary = [[NSMutableDictionary alloc] initWithCapacity:drawableProperties.count + 1];
  [drawableProperties enumerateKeysAndObjectsUsingBlock:^(id key, id object, BOOL *stop)
  {
    // Copy all keys except the retained backing
    if (![key isKindOfClass:[NSString class]] || ![(NSString *)key isEqualToString:kEAGLDrawablePropertyRetainedBacking])
    {
      [mutableDictionary setObject:object forKey:key];
    }
  }];
  // Add the retained backing setting
  [mutableDictionary setObject:@(YES) forKey:kEAGLDrawablePropertyRetainedBacking];
  // Continue
  [super setDrawableProperties:mutableDictionary];
  [mutableDictionary release];
}
@end

@interface RetainedGLKView : GLKView
@end

@implementation RetainedGLKView
+ (Class)layerClass
{
  return [RetainedEAGLLayer class];
}
@end


@implementation NGLViewController

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow
{
  if (self =  [self initWithNibName:nil bundle:nil])
  {
    mpNGLWindow = pNGLWindow;
    return self;
  }

  return nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
  return YES; //(interfaceOrientation == UIInterfaceOrientationLandscapeRight) || (interfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}

-(BOOL)shouldAutorotate
{
  return YES;
}

-(NSUInteger)supportedInterfaceOrientations
{
  NSInteger mask = UIInterfaceOrientationMaskAll;
  return mask;

}
@end

@implementation nglUIWindow

- (nglWindow *) getNGLWindow
{
	return self->mpNGLWindow;
}

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}


- (id) initWithNGLWindow: (nglWindow*) pNGLWindow andContextInfo: (nglContextInfo*)pContextInfo
{
  mpContextInfo = pContextInfo;
	mInited = false;

	mInvalidationTimer = nil;
	mDisplayLink = nil;
  if ( (self = [super initWithFrame:[[UIScreen mainScreen] bounds]]) )
  {
    mpNGLWindow = pNGLWindow;
  }
  else
  {
    NGL_ASSERT(!"initWithFrame: Could not initialize UIWindow");
  }

  EAGLContext * context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  mpGLKView = [[RetainedGLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:context];
//  mpGLKView = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:context];
  mpGLKView.delegate = self;
  mpGLKView.multipleTouchEnabled = YES;
  mpGLKView.enableSetNeedsDisplay = NO;

  NGLViewController* ctrl = [[NGLViewController alloc] initWithNGLWindow:mpNGLWindow];
  ctrl.view = mpGLKView;
  ctrl.preferredFramesPerSecond = 60;
  ctrl.delegate = self;

  [self setRootViewController:ctrl];

  [mpGLKView bindDrawable];
  //NGL_OUT(_T("[nglUIWindow initWithFrame]\n"));


  int frameInterval = 1;
  //  NSString* sysVersion = [[UIDevice currentDevice] systemVersion];
  //  if ([sysVersion compare:@"3.1" options:NSNumericSearch] != NSOrderedAscending) ///< CADisplayLink requires version 3.1 or greater
  //  {
  //    mDisplayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(Paint)];
  //    [mDisplayLink setFrameInterval:frameInterval];
  //    [mDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
  //  }
  //  else ///< NSTimer is used as fallback
  //  {
  //    mInvalidationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0f / NGL_WINDOW_FPS) target:self selector:@selector(Paint) userInfo:nil repeats:YES];
  //  }

  mpNGLWindow->CallOnRescale(nuiGetScaleFactor());
  mpTimer = nuiAnimation::AcquireTimer();
  mpTimer->Stop();

	[self initializeKeyboard];

  [self makeKeyAndVisible];

  return self;
}

- (void) dealloc
{
  delete mpContextInfo;
  [super dealloc];
}

- (void) disconnect
{
  
  //NGL_OUT(_T("[nglUIWindow dealloc]\n"));
  if (mpNGLWindow)
  {
    nuiAnimation::ReleaseTimer();
    mpNGLWindow->CallOnDestruction();
    mpNGLWindow = NULL;
  }
  
  if (mInvalidationTimer != nil)
  {
    [mInvalidationTimer invalidate];
    mInvalidationTimer = nil;
  }
  if (mDisplayLink != nil)
  {
    [mDisplayLink invalidate];
    mDisplayLink = nil;
  }
  
  UIWindow* next = [[[UIApplication sharedApplication] windows] objectAtIndex:0];
  if (next != nil)
    [next makeKeyAndVisible];
}

- (void) UpdateKeyboard
{
  if (!mKeyboardVisible)
    return;

  [self hideKeyboard];
  [self showKeyboard];
}

- (void) InitNGLWindow
{  
  if (!mInited)
  {
    mInited = true;
    mpNGLWindow->CallOnCreation();
  }

  static int32 counter = 0;
  if (counter)
  {
    counter--;
    return;
  }
  
  counter = 60;
}

- (void) dumpTouch: (UITouch*) pTouch
{
  UITouchPhase touchPhase = [pTouch phase];
  uint touchTapCount = [pTouch tapCount];

  CGPoint newp = [pTouch locationInView: (UIView*)self];
  CGPoint oldp = [pTouch previousLocationInView: (UIView*)self];    

  NGL_OUT(_T("Touch Info [%p] [@%d]: [%s], [%d], [x:%f y:%f]->[x:%f y:%f]\n"),
            pTouch, [pTouch timestamp],
            touchPhase == UITouchPhaseBegan ?       "Clicked"   :
            touchPhase == UITouchPhaseMoved ?       "Moved"     :
            touchPhase == UITouchPhaseStationary ?  "Static"    :
            touchPhase == UITouchPhaseEnded ?       "Unclicked" :
            touchPhase == UITouchPhaseCancelled ?   "Canceled"  : "Unknown",
            touchTapCount,
            oldp.x, oldp.y,
            newp.x, newp.y
          );
}

- (void) dumpTouches: (UIEvent*) pEvent
{
  NSSet* pSet = [pEvent allTouches];
  NGL_ASSERT(pSet);
  NSArray* pArray = [pSet allObjects];
  NGL_ASSERT(pArray);
  NSUInteger count = [pArray count];
  NGL_OUT(_T("Dump touches info: %d\n"), count);

  std::vector<UITouch*> touches;
  touches.resize((uint)count);
  [pArray getObjects: &touches[0]];
  
  std::vector<UITouch*>::const_iterator end = touches.end();
  for (std::vector<UITouch*>::const_iterator it = touches.begin(); it != end; ++it)
  {
    UITouch* pTouch = *it;
    [self dumpTouch: pTouch];
  }
}

//! From GLKViewControllerDelegate
- (void)glkViewControllerUpdate:(GLKViewController *)controller
{
//  if (gNeedInvalidate)
//  {
//    mpNGLWindow->OnInvalidate();
//    gNeedInvalidate=false;
//  }
  
  GLKViewController* ctrl = (GLKViewController*)self.rootViewController;
  double lap = ctrl.timeSinceLastDraw;
  mpTimer->OnTick(lap);
}

//! From GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
  if (!CGRectEqualToRect(oldrect, rect))
  {
    mpNGLWindow->SetSize(CGRectGetWidth(rect), CGRectGetHeight(rect));
    oldrect = rect;
  }
  
  [self Paint];
}

- (void)Paint
{
  [self InitNGLWindow];
  mpNGLWindow->OnPaint();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
  for (id item in touches)
  {
    UITouch* pTouch = item;
    UITouchPhase touchPhase = [pTouch phase];
    uint32 touchTapCount = (uint)[pTouch tapCount];

    CGPoint newp = [pTouch locationInView: self.rootViewController.view];

    int x = (int)newp.x;
    int y = (int)newp.y;

    nglMouseInfo info;
    info.Buttons = nglMouseInfo::ButtonLeft;
    info.X = x;
    info.Y = y;

    info.SwipeInfo = nglMouseInfo::eNoSwipe;
    info.TouchId = (int64)pTouch;

    if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
      info.Buttons |= nglMouseInfo::ButtonDoubleClick;

    //NGL_TOUCHES_OUT("[%p][%d] Begin X:%d Y:%d\n", pTouch, info.TouchId, x, y);

    ///< if tapcount > 1, unclicked from a double click
    //        if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
    //          info.Buttons |= nglMouseInfo::ButtonDoubleClick;

    mpNGLWindow->CallOnMouseClick(info);
  }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
  for (id item in touches)
  {
    UITouch* pTouch = item;
    UITouchPhase touchPhase = [pTouch phase];
    uint32 touchTapCount = (uint)[pTouch tapCount];

    CGPoint newp = [pTouch locationInView: self.rootViewController.view];

    int x = (int)newp.x;
    int y = (int)newp.y;

    nglMouseInfo info;
    info.Buttons = nglMouseInfo::ButtonLeft;
    info.X = x;
    info.Y = y;

    info.SwipeInfo = nglMouseInfo::eNoSwipe;
    info.TouchId = (int64)pTouch;

    //NGL_TOUCHES_OUT("[%p][%d] Release X:%d Y:%d\n", pTouch, info.TouchId, x, y);

    ///< if tapcount > 1, unclicked from a double click
    //        if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
    //          info.Buttons |= nglMouseInfo::ButtonDoubleClick;

    mpNGLWindow->CallOnMouseUnclick(info);
  }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
  for (id item in touches)
  {
    UITouch* pTouch = item;
    UITouchPhase touchPhase = [pTouch phase];
    uint32 touchTapCount = (uint)[pTouch tapCount];

    CGPoint newp = [pTouch locationInView: self.rootViewController.view];

    int x = (int)newp.x;
    int y = (int)newp.y;

    nglMouseInfo info;
    info.Buttons = nglMouseInfo::ButtonLeft;
    info.X = x;
    info.Y = y;

    info.SwipeInfo = nglMouseInfo::eNoSwipe;
    info.TouchId = (int64)pTouch;

    //NGL_TOUCHES_OUT("[%p][%d] Cancel X:%d Y:%d\n", pTouch, info.TouchId, x, y);

    ///< if tapcount > 1, unclicked from a double click
    //        if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
    //          info.Buttons |= nglMouseInfo::ButtonDoubleClick;

    mpNGLWindow->CallOnMouseCanceled(info);
  }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
  for (id item in touches)
  {
    UITouch* pTouch = item;
    UITouchPhase touchPhase = [pTouch phase];
    uint32 touchTapCount = (uint)[pTouch tapCount];

    CGPoint newp = [pTouch locationInView: self.rootViewController.view];

    int x = (int)newp.x;
    int y = (int)newp.y;

    nglMouseInfo info;
    info.Buttons = nglMouseInfo::ButtonLeft;
    info.X = x;
    info.Y = y;

    info.SwipeInfo = nglMouseInfo::eNoSwipe;
    info.TouchId = (int64)pTouch;

    //NGL_TOUCHES_OUT("[%p][%d] Moved X:%d Y:%d\n", pTouch, info.TouchId, x, y);

    ///< if tapcount > 1, unclicked from a double click
    //        if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
    //          info.Buttons |= nglMouseInfo::ButtonDoubleClick;

    mpNGLWindow->CallOnMouseMove(info);
  }
}

/////// Keyboard support:
- (void)initializeKeyboard
{
	mpTextField = NULL;

	// Apparently CGRectZero breaks backspace in OS3.2 .. weird.
	mpTextField = [[UITextField alloc] initWithFrame: CGRectMake(0,0,160,50)];
	
	mpTextField.delegate = self;
	/* placeholder so there is something to delete! */
	mpTextField.text = @"x";	
	
	/* set UITextInputTrait properties, mostly to defaults */
	mpTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	mpTextField.autocorrectionType = UITextAutocorrectionTypeNo;
	mpTextField.enablesReturnKeyAutomatically = NO;
	mpTextField.keyboardAppearance = UIKeyboardAppearanceAlert;
	mpTextField.keyboardType = UIKeyboardTypeDefault;
	mpTextField.clearsOnBeginEditing = NO;
	mpTextField.returnKeyType = UIReturnKeyDefault;
	mpTextField.secureTextEntry = NO;	
	
	mpTextField.hidden = YES;
	mKeyboardVisible = NO;
	/* add the UITextField (hidden) to our view */
	[self addSubview: mpTextField];
	
}

// Show Keyboard
- (void)showKeyboard
{
	mKeyboardVisible = YES;
	[mpTextField becomeFirstResponder];
}

// hide onscreen virtual keyboard
- (void)hideKeyboard
{
	mKeyboardVisible = NO;
	[mpTextField resignFirstResponder];
}

// UITextFieldDelegate method.  Invoked when user types something.
- (BOOL)textField:(UITextField *)_textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
  NGL_OUT("change chars in range %d - %d\n", range.location, range.length);
	if ([string length] == 0)
  {
    mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_BACKSPACE, 8, 8)); // 8 = BS = BackSpace
    mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_BACKSPACE, 8, 8));
	}
	else
  {
    nglString str((CFStringRef)string);
    mpNGLWindow->CallOnTextInput(str);
	}

	return NO; /* don't allow the edit! (keep placeholder text there) */
}

/* Terminates the editing session */
- (BOOL)textFieldShouldReturn:(UITextField*)_textField
{
  bool res = false;
  
  res |= mpNGLWindow->CallOnKeyDown(nglKeyEvent(NK_ENTER, '\n', '\n'));
  res |= mpNGLWindow->CallOnKeyUp(nglKeyEvent(NK_ENTER, '\n', '\n'));

  if (!res)
  {
    if (!mpNGLWindow->CallOnTextInput(_T("\n")))
    {
      [self hideKeyboard];
      mpNGLWindow->CallOnTextInputCancelled();
      return YES;
    }
  }
	return NO;
}

- (BOOL)textFieldShouldClear:(UITextField*)_textField
{
	return NO;
}

- (void) MakeCurrent
{
  BOOL res = [EAGLContext setCurrentContext:(EAGLContext*) mpGLKView.context];
}

- (void) BeginSession
{
  [mpGLKView bindDrawable];
}

- (void) EndSession
{
  //[glView EndSession];
}


@end///< nglUIWindow



/////////////////////////////////////////////////////////////////////////////////////////////////// NGL WINDOW:



/*
 * OS specific info
 */

nglWindow::OSInfo::OSInfo()
{
  mpUIWindow = NULL;
}

/*
 * Constructors
 */

nglWindow::nglWindow (uint Width, uint Height, bool IsFullScreen)
{
  Register();
  mpUIWindow = NULL;
  nglContextInfo context; // Get default context
  nglWindowInfo info(Width, Height, IsFullScreen);
  InternalInit (context, info, NULL); 
}

nglWindow::nglWindow (const nglContextInfo& rContext, const nglWindowInfo& rInfo, const nglContext* pShared)
{
  Register();
  mpUIWindow = NULL;
  InternalInit (rContext, rInfo, pShared);
}

void nglWindow::InternalInit (const nglContextInfo& rContext, const nglWindowInfo& rInfo, const nglContext* pShared)
{
  mState = eHide;
  mAngle = 0;

  SetError (NGL_WINDOW_ENONE);
  SetEventMask(nglWindow::AllEvents);

  mAutoRotate = true;
  
  mAngle = rInfo.Rotate;

  // Create the actual window
  nglUIWindow* pUIWindow = [[nglUIWindow alloc] initWithNGLWindow: this andContextInfo: new nglContextInfo(rContext)];
  mOSInfo.mpUIWindow = pUIWindow;
  mpUIWindow = pUIWindow;

  // Build
  
  // Enable multitouch
  [pUIWindow setMultipleTouchEnabled: YES];
  
  [pUIWindow makeKeyAndVisible];


  NGL_LOG(_T("window"), NGL_LOG_INFO, _T("trying to create GLES context"));
  rContext.Dump(NGL_LOG_INFO);
  
  if ((rContext.TargetAPI != eTargetAPI_OpenGL) && (rContext.TargetAPI != eTargetAPI_OpenGL2))
  {
    // UIKit Implementation only supports OpenGLES renderer so far
    NGL_LOG(_T("window"), NGL_LOG_INFO, _T("bad renderer"));
    NGL_ASSERT(0);
    return;
  }
	
	Build(rContext);
  CGRect rect = pUIWindow.frame;
	CGRect r = [(nglUIWindow*)mpUIWindow frame];
	NSLog(@"currentFrame: %f, %f - %f, %f\n", r.origin.x, r.origin.y, r.size.width, r.size.height);
	r = [UIScreen mainScreen].applicationFrame;
	NSLog(@"applicationFrame: %f, %f - %f, %f\n", r.origin.x, r.origin.y, r.size.width, r.size.height);

  float w, h;
  mWidth = 0;
  mHeight = 0;

  UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
  switch (orientation)
  {
    case UIInterfaceOrientationPortrait:
      mAngle = 0;
      break;
    case UIInterfaceOrientationPortraitUpsideDown:
      mAngle = 180;
      break;
    case UIInterfaceOrientationLandscapeLeft:
      mAngle = 90;
      break;
    case UIInterfaceOrientationLandscapeRight:
      mAngle = 270;
      break;
  }
  if (mAngle == 270 || mAngle == 90)
  {
    w = rect.size.height;
    h = rect.size.width;
  }
  else
  {
    w = rect.size.width;
    h = rect.size.height;
  }

	SetSize(w, h);
	
/* Ultra basic UIKit view integration on top of nuiWidgets
  UIWebView* pWebView = [[UIWebView alloc] initWithFrame: CGRectMake(50, 50, 200, 200)];
	[pUIWindow addSubview: pWebView];
	pWebView.hidden = NO;
  NSURL* pURL = [[NSURL alloc] initWithString: @"http://libnui.net"];
  NSURLRequest* pReq = [[NSURLRequest alloc] initWithURL: pURL];
  [pWebView loadRequest: pReq];
 */
}

nglWindow::~nglWindow()
{
  if (mpUIWindow)
  {

    UIWindow* win = (UIWindow*)mpUIWindow;
    [win disconnect];
    UIWindow* oldwin = [[UIApplication sharedApplication].windows objectAtIndex:0];
    if (win != oldwin)
    {
      [oldwin makeKeyWindow];
    }
    //[win removeFromSuperview];
    [win release];
  }
  Unregister();
}



/*
 * All services
 */

void nglWindow::SetState (StateChange State)
{
  if (mState == State)
    return;

  //  static bool was_mapped = false;
  //  static int last_x, last_y;
  UIApplication* pApp = [UIApplication sharedApplication];

	switch (State)
	{
		case eHide:
			[pApp setStatusBarHidden:FALSE animated:TRUE];
			break;
		case eShow:
			[pApp setStatusBarHidden:FALSE animated:TRUE];
			break;
		case eMinimize:
			[pApp setStatusBarHidden:FALSE animated:TRUE];
			break;
		case eMaximize:
		{
			[pApp setStatusBarHidden:TRUE animated:TRUE];
		}
			break;
	};

//  [(nglUIWindow*)mpUIWindow recreateWindow];
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
  //NGL_OUT(_T("nglWindow::SetSize(%d, %d)\n"), Width, Height);
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
  return _T("UIKit Window\n");
}

void nglWindow::SetTitle (const nglString& rTitle)
{
}

bool nglWindow::IsKeyDown (nglKeyCode Key) const
{
  return false;
}

void nglWindow::GetMouse (nglMouseInfo& rMouseInfo, bool Local) const
{
}

const nglWindow::OSInfo* nglWindow::GetOSInfo() const
{
  return &mOSInfo;
}

void nglWindow::BeginSession()
{
#ifdef _DEBUG_WINDOW_
  NGL_LOG(_T("window"), NGL_LOG_INFO, _T("BeginSession\n"));
#endif
  //  MakeCurrent();
  NGL_ASSERT(mpUIWindow);
  [mpUIWindow BeginSession];
}

void nglWindow::EndSession()
{
#ifndef __NOGLCONTEXT__

#ifdef _DEBUG_WINDOW_
  NGL_LOG(_T("window"), NGL_LOG_INFO, _T("EndSession\n"));
#endif
	
  NGL_ASSERT(mpUIWindow);
  [mpUIWindow EndSession];
#endif
}

bool nglWindow::MakeCurrent() const
{
  NGL_ASSERT(mpUIWindow);
  [mpUIWindow MakeCurrent];

  return true;
}

void nglWindow::Invalidate()
{
}

bool nglWindow::SetCursor(nuiMouseCursor Cursor)
{
///< No Cursor relevance here
  return true;
}

nuiMouseCursor nglWindow::GetCursor() const
{
///< No Cursor relevance here
  return eCursorNone;
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
  [(nglUIWindow*)mpUIWindow showKeyboard];
}

void nglWindow::EndTextInput()
{
  [(nglUIWindow*)mpUIWindow hideKeyboard];
}

bool nglWindow::IsEnteringText() const
{
  //[(nglUIWindow*)mpUIWindow frame]
  return false;
}

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
  if (NSFoundationVersionNumber > NSFoundationVersionNumber_iOS_6_1)
  {
    return MIN([UIApplication sharedApplication].statusBarFrame.size.height, [UIApplication sharedApplication].statusBarFrame.size.width);
  }
  return 0;
}

