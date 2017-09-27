#include "nui.h"

#include <QuartzCore/QuartzCore.h>
#include <QuartzCore/CALayer.h>


#include <string.h>

#include "nglWindow_UIKit.h"

#include <GL/gl.h>
#include <Metal/Metal.h>

#define NGL_WINDOW_FPS 60.0f
#define NUI_START_METAL_CAPTURE 0

#define USE_MULTISAMPLE 0

//#include "nglImage.h"

#define NGL_WINDOW_EBASE      (NGL_CONTEXT_ELAST+1)
#define NGL_WINDOW_ESYSWIN    (NGL_WINDOW_EBASE+1)
#define NGL_WINDOW_EVIDMODE   (NGL_WINDOW_EBASE+2)
#define NGL_WINDOW_ERANGE     (NGL_WINDOW_EBASE+3)

const nglChar* gpWindowErrorTable[] =
{
  /*  0 */ "No error",
  /*  1 */ "System window creation failed",
  /*  2 */ "Full screen video mode unavailable",
  /*  3 */ "Properties out of range",
  NULL
};

#ifndef _DEBUG_
#undef NGL_OUT
#define NGL_OUT {}
#endif

#if defined(_MULTI_TOUCHES_) && defined(_DEBUG_)
# define NGL_TOUCHES_OUT
// NGL_OUT
#else//!_MULTI_TOUCHES_
# define NGL_TOUCHES_OUT
#endif//!_MULTI_TOUCHES_

//#define _DEBUG_WINDOW_


////////////////////////////////////////////////////////////////////////////////
// CAEAGLLayer /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark RetainedEAGLLayer

//@interface RetainedEAGLLayer : CAEAGLLayer
//@end
//
//@implementation RetainedEAGLLayer
//- (void)setDrawableProperties:(NSDictionary *)drawableProperties
//{
//  self.opaque = YES;
//
//  // Copy the dictionary and add/modify the retained property
//  NSMutableDictionary *mutableDictionary = [[NSMutableDictionary alloc] initWithCapacity:drawableProperties.count + 1];
//  [drawableProperties enumerateKeysAndObjectsUsingBlock:^(id key, id object, BOOL *stop)
//  {
//    // Copy all keys except the retained backing
//    if (![key isKindOfClass:[NSString class]] || ![(NSString *)key isEqualToString:kEAGLDrawablePropertyRetainedBacking])
//    {
//      [mutableDictionary setObject:object forKey:key];
//    }
//  }];
//  // Add the retained backing setting
//  [mutableDictionary setObject:@(YES) forKey:kEAGLDrawablePropertyRetainedBacking];
//  // Continue
//  [super setDrawableProperties:mutableDictionary];
//  [mutableDictionary release];
//}
//@end


////////////////////////////////////////////////////////////////////////////////
// nglUIView_GL ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglUIView_GL

@implementation nglUIView_GL

+ (Class)layerClass
{
  return [CAEAGLLayer class];
}

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow
{
  self = [super init];
  if (self)
  {
    mpNGLWindow = pNGLWindow;
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                    nil];
    
    self.contentScaleFactor = [[UIScreen mainScreen] scale];
    self.multipleTouchEnabled = YES;

  }
  return self;
}

- (void) layoutSubviews
{
  CGRect frame = [self frame];
//  NGL_OUT("layoutSubviews with frame %d - %d\n", (uint)frame.size.width, (uint)frame.size.height);
  mpNGLWindow->UpdateLayer();
  mpNGLWindow->SetSize((uint)frame.size.width, (uint)frame.size.height);
  [super layoutSubviews];
}
@end

////////////////////////////////////////////////////////////////////////////////
// nglUIView_Metal ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglUIView_Metal

@implementation nglUIView_Metal
{
  CAMetalLayer* _metalLayer;
}

+ (Class)layerClass
{
  return [CAMetalLayer class];
}

- (CAMetalLayer *)metalLayer
{
  return _metalLayer;
}


- (id) initWithNGLWindow: (nglWindow*) pNGLWindow
{
  self = [super init];
  if (self)
  {
    mpNGLWindow = pNGLWindow;
    _metalLayer = (CAMetalLayer *)self.layer;
    _metalLayer.opaque = YES;
    _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
//    _metalLayer.device = MTLCreateSystemDefaultDevice();
//    _metalLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
//                                    [NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,
//                                    kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
//                                    nil];

    double scale = [[UIScreen mainScreen] scale];
    self.contentScaleFactor = scale;
    _metalLayer.contentsScale = scale;
    self.multipleTouchEnabled = YES;

  }
  return self;
}
//- (void) dealloc
//{
//  [super dealloc];
//}

- (void) layoutSubviews
{
  CGRect frame = [self frame];
  //  NGL_OUT("layoutSubviews with frame %d - %d\n", (uint)frame.size.width, (uint)frame.size.height);
  mpNGLWindow->UpdateLayer();
  mpNGLWindow->SetSize((uint)frame.size.width, (uint)frame.size.height);
  [super layoutSubviews];
}

@end


////////////////////////////////////////////////////////////////////////////////
// nglUIViewController /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglUIViewController

@implementation nglUIViewController

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow
{
  self = [super initWithNibName:nil bundle:nil];
  mpDropObject = nil;
  if (self)
  {
    mpNGLWindow = pNGLWindow;
  }
  return self;
}

- (void)loadView
{
  UIView* pView = nil;
  nglContextInfo Info;
  mpNGLWindow->GetContextInfo(Info);
  if (Info.TargetAPI == eTargetAPI_OpenGL2)
  {
    pView = [[nglUIView_GL alloc] initWithNGLWindow:mpNGLWindow];
  }
  else if (Info.TargetAPI == eTargetAPI_Metal)
  {
    pView = [[nglUIView_Metal alloc] initWithNGLWindow:mpNGLWindow];
  }
  
  self.view = pView;
}
-(void) viewDidLoad
{
  [super viewDidLoad];
  
  if (@available(iOS 11, *))
  {
    UIDropInteraction* dropInteraction = [[UIDropInteraction alloc] initWithDelegate:self];
    [self.view addInteraction:dropInteraction];
  }
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator
                                :(id <UIViewControllerTransitionCoordinator>)coordinator
{
  [super viewWillTransitionToSize: size withTransitionCoordinator: coordinator];
  mpNGLWindow->CallOnOrientation(size.width > size.height ? nuiHorizontal : nuiVertical);
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
  return YES;
}

-(BOOL)shouldAutorotate
{
  return YES;
}

-(NSUInteger)supportedInterfaceOrientations
{
  NSInteger mask = UIInterfaceOrientationMaskAll;
  return (NSUInteger)mask;
}

// We assume we can handle the drag'n'drop if at least one item is available. No file data is available at this point
- (BOOL)dropInteraction:(UIDropInteraction *)interaction canHandleSession:(id<UIDropSession>)session
{
  return session.items.count > 0;
}

// Started dropping. Since we cannot access file data now, we add empty nglDataFilesObject
- (void)dropInteraction:(UIDropInteraction *)interaction sessionDidEnter:(id<UIDropSession>)session
{
  mpDropObject.reset(new nglDragAndDrop(eDropEffectCopy, nullptr, 0, 0));
  auto file_objs = new nglDataFilesObject("ngl/Files");
  mpDropObject->AddType(file_objs);
  mpNGLWindow->OnDragEnter();
}

// Update, for hit testing
- (UIDropProposal *)dropInteraction:(UIDropInteraction *)interaction sessionDidUpdate:(id<UIDropSession>)session
{
  CGPoint p = [session locationInView:self.view];
  nglDropEffect effect = mpNGLWindow->OnCanDrop(mpDropObject.get(), p.x, p.y, nglMouseInfo::ButtonLeft);

  UIDropOperation op;
  switch (effect)
  {
    case eDropEffectCopy: op = UIDropOperationCopy; break;
    case eDropEffectMove: op = UIDropOperationMove; break;
    default: op = UIDropOperationCancel; break;
  }
  
  return [[UIDropProposal alloc] initWithDropOperation:op];
}

// Convenience function to generate a temporary directory with suggested file name
std::pair<nglPath, bool> GetTemporaryDropFile(nglPath FileName) noexcept
{
  auto tmp = std::unique_ptr<char[]>{ new char[PATH_MAX] };
  snprintf(tmp.get(), PATH_MAX - 1, "%s/XXXXXXXXX", nglPath { ePathTemp }.GetChars());
  auto tmp_dir = mkdtemp(tmp.get());
  if (!tmp_dir)
  {
    return { {}, false };
  }
  nglPath ret { tmp_dir };
  ret += FileName.GetNodeName();
  return { ret, true };
}

- (void)dropInteraction:(UIDropInteraction *)interaction performDrop:(id<UIDropSession>)session
{
  if (!mpDropObject)
  {
    NGL_ASSERT(mpDropObject);
    return;
  }
  
  if (!session.items.count)
  {
    NGL_ASSERT(session.items.count);
    return;
  }
  
  CGPoint p = [session locationInView:self.view];
  __block auto file_objs = new nglDataFilesObject("ngl/Files");
  __block auto window = mpNGLWindow;
  __block auto drop_object = mpDropObject.get();
  __block float x = static_cast<float>(p.x);
  __block float y = static_cast<float>(p.y);
  mpDropObject->AddType(file_objs);
  for (UIDragItem* item in session.items)
  {
    __block NSItemProvider* provider = item.itemProvider;
    __block NSString* UTI = provider.registeredTypeIdentifiers.lastObject;
    
    //GetTemporaryDropFile
    [provider loadFileRepresentationForTypeIdentifier:UTI
                                    completionHandler:^(NSURL* _Nullable url, NSError* _Nullable error) {
                                      if (!url || error != nil)
                                      {
                                        return;
                                      }
                                      
                                      nglPath src_file { url.fileSystemRepresentation };
                                      auto [path, success] = GetTemporaryDropFile(src_file);
                                      if (success)
                                      {
                                        __block nglPath dst_file { path };
                                        if (src_file.Copy(dst_file))
                                        {
                                          dispatch_async(dispatch_get_main_queue(), ^{
                                            file_objs->AddFile(dst_file.GetPathName());
                                            window->OnDropped(drop_object, x, y, nglMouseInfo::ButtonLeft);
                                          });
                                        }
                                      }
                                    }];
  }
}

// Drop session ended
- (void)dropInteraction:(UIDropInteraction *)interaction sessionDidEnd:(id<UIDropSession>)session
{
  mpNGLWindow->OnDragLeave();
}

//- (void)dropInteraction:(UIDropInteraction *)interaction sessionDidExit:(id<UIDropSession>)session
//{
//}

//- (void)dropInteraction:(UIDropInteraction *)interaction concludeDrop:(id<UIDropSession>)session
//{
//}

//- (nullable UITargetedDragPreview *)dropInteraction:(UIDropInteraction *)interaction previewForDroppingItem:(UIDragItem *)item withDefault:(UITargetedDragPreview *)defaultPreview
//{
//  NSLog(@"dropInteraction:previewForDroppingItem:withDefault");
//  return nil;
//}
//
//- (void)dropInteraction:(UIDropInteraction *)interaction item:(UIDragItem *)item willAnimateDropWithAnimator:(id<UIDragAnimating>)animator
//{
//  NSLog(@"dropInteraction:item:willAnimateDropWithAnimator:");
//}

@end


////////////////////////////////////////////////////////////////////////////////
// nglUIWindow /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglUIWindow

@implementation nglUIWindow

- (nglWindow *) getNGLWindow
{
	return self->mpNGLWindow;
}

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow
{
  if ( (self = [super initWithFrame:[[UIScreen mainScreen] bounds]]) )
  {
    mpNGLWindow = pNGLWindow;
  }
  else
  {
    NGL_ASSERT(!"initWithFrame: Could not initialize UIWindow");
  }

	[self initializeKeyboard];

  return self;
}

- (void) dealloc
{
  [self stopDisplayLink];
}

- (void) startDisplayLink
{
  if (mDisplayLink == nil)
  {
    mDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(displayTicked)];
    [mDisplayLink setFrameInterval:1];
    [mDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
  }
}

- (void) stopDisplayLink
{
  if (mDisplayLink != nil)
  {
    [mDisplayLink invalidate];
    mDisplayLink = nil;
  }
}

- (void) displayTicked
{
  NGL_ASSERT(mpNGLWindow);
  mpNGLWindow->DisplayTicked();
}

- (void) UpdateKeyboard
{
  if (!mKeyboardVisible)
    return;

  [self hideKeyboard];
  [self showKeyboard];
}

- (void) dumpTouch: (UITouch*) pTouch
{
  UITouchPhase touchPhase = [pTouch phase];
  uint touchTapCount = [pTouch tapCount];

  CGPoint newp = [pTouch locationInView: (UIView*)self];
  CGPoint oldp = [pTouch previousLocationInView: (UIView*)self];    

  NGL_OUT("Touch Info [%p] [@%d]: [%s], [%d], [x:%f y:%f]->[x:%f y:%f]\n",
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
  NGL_OUT("Dump touches info: %d\n", count);

  std::vector<UITouch*> touches;
  touches.resize((uint)count);
  for (UITouch* touch in pArray)
  {
    touches.push_back(touch);
  }
  
  std::vector<UITouch*>::const_iterator end = touches.end();
  for (std::vector<UITouch*>::const_iterator it = touches.begin(); it != end; ++it)
  {
    UITouch* pTouch = *it;
    [self dumpTouch: pTouch];
  }
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
    info.Force = [pTouch force] / [pTouch maximumPossibleForce];

    info.SwipeInfo = nglMouseInfo::eNoSwipe;
    info.TouchId = (int64)pTouch;

    if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
      info.Buttons |= nglMouseInfo::ButtonDoubleClick;

    //NGL_TOUCHES_OUT("[%p][%d] Begin X:%d Y:%d\n", pTouch, info.TouchId, x, y);
    NGL_TOUCHES_OUT("[%p][%d] BEGIN X:%d Y:%d Force:%f/%f\n", pTouch, info.TouchId, x, y, [pTouch force], [pTouch maximumPossibleForce]);
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

    if (mpNGLWindow->IsDragging())
    {
      mpNGLWindow->OnDropped(mpNGLWindow->GetDraggedObject(), info.X, info.Y, info.Buttons);
      mpNGLWindow->OnDragStop(false);
    }
    else
    {
      mpNGLWindow->CallOnMouseUnclick(info);
    }
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
    info.Force = [pTouch force] / [pTouch maximumPossibleForce];
    info.SwipeInfo = nglMouseInfo::eNoSwipe;
    info.TouchId = (int64)pTouch;

//    NGL_TOUCHES_OUT("[%p][%d] MOVED X:%d Y:%d Force:%f/%f\n", pTouch, info.TouchId, x, y, [pTouch force], [pTouch maximumPossibleForce]);

    ///< if tapcount > 1, unclicked from a double click
    //        if (touchTapCount > 1)// && ([pTouch timestamp] - sOldTimestamp < DOUBLE_TAP_DELAY))
    //          info.Buttons |= nglMouseInfo::ButtonDoubleClick;
    
    if (mpNGLWindow->IsDragging())
    {
      mpNGLWindow->OnCanDrop(mpNGLWindow->GetDraggedObject(), info.X, info.Y, info.Buttons);
    }
    else
    {
      mpNGLWindow->CallOnMouseMove(info);
    }
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
    nglString str { [string UTF8String] };
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
    if (!mpNGLWindow->CallOnTextInput("\n"))
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

// Hardware keyboard support
- (BOOL)canBecomeFirstResponder
{
  return YES;
}

- (NSArray*)keyCommands
{
//  return [mHotkeys copy];

  return @[
           [UIKeyCommand keyCommandWithInput:UIKeyInputLeftArrow modifierFlags:0 action:@selector(handleHotKey) discoverabilityTitle:@"Left"],
           [UIKeyCommand keyCommandWithInput:UIKeyInputRightArrow modifierFlags:0 action:@selector(handleHotKey) discoverabilityTitle:@"Right"],
           [UIKeyCommand keyCommandWithInput:UIKeyInputUpArrow modifierFlags:0 action:@selector(handleHotKey) discoverabilityTitle:@"Up"],
           [UIKeyCommand keyCommandWithInput:UIKeyInputDownArrow modifierFlags:0 action:@selector(handleHotKey) discoverabilityTitle:@"Down"],
           ];
}

inline static UIKeyModifierFlags GetUIKeyModifier(const nuiKeyModifier& M)
{
  UIKeyModifierFlags ret = 0;
  if (M & nuiControlKey)
  {
    ret |= UIKeyModifierControl;
  }
  if (M & nuiAltKey)
  {
    ret |= UIKeyModifierAlternate;
  }
  if (M & nuiShiftKey)
  {
    ret |= UIKeyModifierShift;
  }
  if (M & nuiMetaKey)
  {
    ret |= UIKeyModifierCommand;
  }
  return ret;
}

- (void)addHotKeyChar:(nuiHotKeyChar*)hotkey
{
  /*
  char c = hotkey->GetTrigger();
  UIKeyCommand* key_command = [UIKeyCommand keyCommandWithInput:[NSString stringWithUTF8String:&c]
                                                  modifierFlags:GetUIKeyModifier(hotkey->GetModifiers())
                                                         action:@selector(handleHotKey:)
                                           discoverabilityTitle:[NSString stringWithUTF8String:hotkey->GetName().GetChars()]];

  if (!mHotkeys)
  {
    mHotkeys = [[NSMutableArray alloc] initWithArray:@[key_command]];
  }
  else
  {
    [mHotkeys addObject:key_command];
  }
  */
}

- (void)delHotKeyChar:(nuiHotKeyChar*)hotkey
{
}
- (void)handleHotKey:(UIKeyCommand *)keyCommand
{
  NSLog(@"---------> keyCommand received! %@", keyCommand);
}


@end///< nglUIWindow



////////////////////////////////////////////////////////////////////////////////
// nglWindow ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma mark nglWindow

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
  nglContextInfo context; // Get default context
  nglWindowInfo info(Width, Height, IsFullScreen);
  InternalInit (context, info, nullptr);
}

nglWindow::nglWindow (const nglContextInfo& rContext, const nglWindowInfo& rInfo, const nglContext* pShared)
{
  Register();
  InternalInit (rContext, rInfo, pShared);
}

void nglWindow::InternalInit (const nglContextInfo& rContext, const nglWindowInfo& rInfo, const nglContext* pShared)
{
  mFramebuffer = 0;
  mRenderbuffer= 0;
  mInited = false;

  mState = eHide;
  mAngle = 0;

  SetError (NGL_WINDOW_ENONE);
  SetEventMask(nglWindow::AllEvents);

  mAutoRotate = true;
  mAngle = rInfo.Rotate;

  mContextInfo = rContext;
  
  nglUIWindow* _window = [[nglUIWindow alloc] initWithNGLWindow: this];
  mpUIWindow = (__bridge void *) _window;
  mOSInfo.mpUIWindow = (__bridge void *) _window;
  [_window setMultipleTouchEnabled: YES];
  [_window makeKeyAndVisible];
  
  nglUIViewController* _viewctrl = [[nglUIViewController alloc] initWithNGLWindow: this];
  mpUIViewCtrl = (__bridge void *) _viewctrl;

  [_window setRootViewController: _viewctrl];

  NGL_ASSERT(_viewctrl.view);
  UIView* _view = (UIView*)_viewctrl.view;
  mpUIView = (__bridge void *) _view;
  mpCALayer = (__bridge void *)_view.layer;

  NGL_LOG("window", NGL_LOG_INFO, "trying to create GLES context");
  rContext.Dump(NGL_LOG_INFO);
  
  if ((rContext.TargetAPI != eTargetAPI_OpenGL2) && (rContext.TargetAPI != eTargetAPI_Metal))
  {
    // UIKit Implementation only supports OpenGLES renderer so far
    NGL_LOG("window", NGL_LOG_INFO, "bad renderer");
    NGL_ASSERT(0);
    return;
  }
	

  if (rContext.TargetAPI != eTargetAPI_Metal)
  {
    mpEAGLContext = (void *) CFBridgingRetain([[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2]);
    UpdateGLLayer();
    bool currentOk = MakeCurrent();
    NGL_ASSERT(currentOk);
  }
  else
  {
    mMetalDevice = (void*)CFBridgingRetain(MTLCreateSystemDefaultDevice());
    UpdateMetalLayer();
  }
  Build(rContext);

	CGRect r = [(__bridge nglUIWindow*)mpUIWindow frame];
	NSLog(@"currentFrame: %f, %f - %f, %f\n", r.origin.x, r.origin.y, r.size.width, r.size.height);
	r = [UIScreen mainScreen].applicationFrame;
	NSLog(@"applicationFrame: %f, %f - %f, %f\n", r.origin.x, r.origin.y, r.size.width, r.size.height);

//  CallOnCreation();
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
//  if (mAngle == 270 || mAngle == 90)
//  {
//    w = rect.size.height;
//    h = rect.size.width;
//  }
//  else
//  {
//    w = rect.size.width;
//    h = rect.size.height;
//  }
  

  CallOnRescale(nuiGetScaleFactor());
	SetSize((uint)r.size.width, (uint)r.size.height);


  mpAnimationTimer = nuiAnimation::AcquireTimer();
  mpAnimationTimer->Stop();
  mCurrentThread = 0;

  [_window startDisplayLink];
}

nglWindow::~nglWindow()
{
  CallOnDestruction();

  if (mpEAGLContext)
  {
    if (mpEAGLContext == (__bridge void *) [EAGLContext currentContext])
    {
      [EAGLContext setCurrentContext:nil];
    }
    
    CFBridgingRelease(mpEAGLContext);
    mpEAGLContext = nullptr;
  }

  if (mpUIWindow)
  {
    nglUIWindow* win = (__bridge nglUIWindow *) mpUIWindow;
    [win stopDisplayLink];

    UIWindow* oldwin = [[UIApplication sharedApplication].windows objectAtIndex:0];
    if (win != oldwin)
    {
      [oldwin makeKeyWindow];
    }
    //[win release];
  }
  Unregister();
}

void nglWindow::UpdateLayer()
{
  if (mpEAGLContext)
  {
    UpdateGLLayer();
  }
  else if (GetMetalDevice())
  {
    UpdateMetalLayer();
  }
  else
  {
    NGL_ASSERT(0); // Whoops?
  }
}

void nglWindow::UpdateGLLayer()
{
  GetLock().Lock();

  NGL_ASSERT(mpEAGLContext);
  [EAGLContext setCurrentContext: (__bridge EAGLContext *) mpEAGLContext];

  if (mFramebuffer)
  {
    if (mFramebuffer) {
      glDeleteFramebuffers(1, &mFramebuffer);
      mFramebuffer = 0;
    }
    if (mRenderbuffer) {
      glDeleteRenderbuffers(1, &mRenderbuffer);
      mRenderbuffer = 0;
    }
  }

  NGL_ASSERT(mpEAGLContext);
  NGL_ASSERT(mpCALayer);
  EAGLContext* _context = (__bridge EAGLContext *) mpEAGLContext;
  CAEAGLLayer* _layer = (__bridge CAEAGLLayer *) mpCALayer;

// Create default framebuffer object.
  glGenFramebuffers(1, &mFramebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
// Create color render buffer and allocate storage for CAEAGLLayer
  glGenRenderbuffers(1, &mRenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer);
  bool res = [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_layer] == YES;
  NGL_ASSERT(res);
  GLint renderbufferWidth, renderbufferHeight;
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderbufferWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderbufferHeight);
//  NGL_ASSERT(mWidth == renderbufferWidth && mHeight == renderbufferHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderbuffer);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    NGL_LOG("window", NGL_LOG_ERROR, "Failed to make complete framebuffer object\n");
    NGL_ASSERT(0);
  }

//  [EAGLContext setCurrentContext: nil];

  GetLock().Unlock();
}

void nglWindow::UpdateMetalLayer()
{
  GetLock().Lock();
  
  NGL_ASSERT(!mpEAGLContext);
  UIWindow* window = (__bridge UIWindow*)GetOSInfo()->mpUIWindow;
  nglUIView_Metal* metalView = (__bridge nglUIView_Metal*)mpUIView;
  CAMetalLayer* metalLayer = (CAMetalLayer*)metalView.layer;
  double scale = [[UIScreen mainScreen] scale];
  metalLayer.device = (__bridge id<MTLDevice>)GetMetalDevice();
  metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  metalLayer.contentsScale = scale;
  metalLayer.opaque = YES;
  metalLayer.framebufferOnly = YES;
  metalLayer.drawableSize = CGSizeMake(metalLayer.frame.size.width * scale, metalLayer.frame.size.height * scale);

  GetLock().Unlock();
}

void nglWindow::DisplayTicked()
{
  if (!mInited)
  {
    mInited = true;
    CallOnCreation();
  }
  
  nglTime now;
  mpAnimationTimer->OnTick(now - mLastTick);
  mLastTick = now;
  OnPaint();
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
  //NGL_OUT("nglWindow::SetSize(%d, %d)\n", Width, Height);
  if (mWidth == (GLint)Width &&
      mHeight == (GLint)Height)
    return false;

  mWidth  = Width;
  mHeight = Height;

  CallOnResize(Width, Height);
  return true;
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
  return "UIKit Window\n";
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

static int frame = 0;

void nglWindow::BeginSession()
{
#ifdef _DEBUG_WINDOW_
  NGL_LOG("window", NGL_LOG_INFO, "BeginSession\n");
#endif
  if (mpEAGLContext)
  {
    [EAGLContext setCurrentContext: (__bridge EAGLContext*)mpEAGLContext];
    NGL_ASSERT(mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
  }
  else
  {
    id<MTLCommandQueue> commandQueue = (__bridge id<MTLCommandQueue>)mMetalCommandQueue;
    if (!commandQueue)
    {
      id<MTLDevice> device = (__bridge id<MTLDevice>)GetMetalDevice();
#if NUI_START_METAL_CAPTURE
      [MTLCaptureManager.sharedCaptureManager startCaptureWithDevice:device];
#endif
      commandQueue = [device newCommandQueue];
      mMetalCommandQueue = (void*)CFBridgingRetain(commandQueue);
    }
    commandQueue.label = [NSString stringWithFormat:@"nui queue frame %d", frame];
    
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
    commandBuffer.label = [NSString stringWithFormat:@"nui buffer frame %d", frame];
    mMetalCommandBuffer = (void*)CFBridgingRetain(commandBuffer);
    
    frame++;
  }
}

void* nglWindow::CreateMetalPass()
{
  id<MTLDevice> device = (__bridge id<MTLDevice>)GetMetalDevice();
  id<MTLTexture> texture = (__bridge id<MTLTexture>)mMetalDestinationTexture;
  
  if (!mMetalDrawable)
  {
    CAMetalLayer* layer = (__bridge CAMetalLayer*)GetMetalLayer();
    id<CAMetalDrawable> drawable = [layer nextDrawable];
    NGL_ASSERT(drawable != nil);
    mMetalDrawable = (void*)CFBridgingRetain(drawable);
    texture = drawable.texture;
    NGL_ASSERT(texture.width == layer.drawableSize.width);
    NGL_ASSERT(texture.height == layer.drawableSize.height);
    //      NGL_OUT("Next drawable size is %d x %d (%d x %d [%f])\n", (int)texture.width, (int)texture.height, (int)layer.frame.size.width, (int)layer.frame.size.height, (float)layer.contentsScale);
    mMetalDestinationTexture = (void*)CFBridgingRetain(texture);
  }
  
  if (!mDrawableMetalCommandEncoder)
  {
    MTLRenderPassDescriptor *passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    passDescriptor.colorAttachments[0].texture = texture;
    passDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    passDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    passDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(1.0, 1.0, 1.0, 1.0);
    
    id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)mMetalCommandBuffer;
    NGL_ASSERT(commandBuffer);
    id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:passDescriptor];
    NGL_ASSERT(commandEncoder);
    commandEncoder.label = [NSString stringWithFormat:@"nui encoder for frame %d", frame];
    mDrawableMetalCommandEncoder = (void*)CFBridgingRetain(commandEncoder);
    NGL_ASSERT(mCurrentMetalCommandEncoder == nil);
  }
  return mDrawableMetalCommandEncoder;
}

void nglWindow::EndSession()
{
#ifndef __NOGLCONTEXT__

#ifdef _DEBUG_WINDOW_
  NGL_LOG("window", NGL_LOG_INFO, "EndSession\n");
#endif
	
  if (mpEAGLContext)
  {
    EAGLContext* _context = (__bridge EAGLContext*)mpEAGLContext;
    NGL_ASSERT(_context == [EAGLContext currentContext]);

    NGL_ASSERT(mRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer);

    BOOL success = [_context presentRenderbuffer:GL_RENDERBUFFER];
    NGL_ASSERT(success == YES);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glFlush();
//  [EAGLContext setCurrentContext:nil];
  }
  else
  {
    SetCurrentMetalCommandEncoder(nullptr);
    
    id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)GetMetalCommandBuffer();
    id<CAMetalDrawable> drawable = (__bridge id<CAMetalDrawable>)GetMetalDrawable();
    
    if (drawable)
      [commandBuffer presentDrawable:drawable];
    
    [commandBuffer commit];
    
    if (mMetalDrawable)
    {
      CFBridgingRelease(mMetalDrawable);
      mMetalDrawable = nullptr;
    }
    
    if (mMetalDestinationTexture)
    {
      CFBridgingRelease(mMetalDestinationTexture);
      mMetalDestinationTexture = nullptr;
    }
    
    if (mMetalCommandBuffer)
    {
      CFBridgingRelease(mMetalCommandBuffer);
      mMetalCommandBuffer = nullptr;
    }
#if NUI_START_METAL_CAPTURE
    [MTLCaptureManager.sharedCaptureManager stopCapture];
#endif
  }

#endif
}

bool nglWindow::MakeCurrent() const
{
  if (mpEAGLContext)
  {
    return [EAGLContext setCurrentContext: (__bridge EAGLContext*)mpEAGLContext] == YES;
  }
  return true;
}

void* nglWindow::GetMetalLayer() const
{
  return (__bridge void*)((__bridge nglUIView_Metal*)mpUIView).metalLayer;
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
  NGL_OUT("nglWindow::EnterModalState NOT IMPLENTED YET!");
//  NGL_ASSERT(!"Not Implemented");
}

void nglWindow::ExitModalState()
{
  NGL_OUT("nglWindow::ExitModalState NOT IMPLENTED YET!");
//  NGL_ASSERT(!"Not Implemented");  
}

void nglWindow::StartTextInput(int32 X, int32 Y, int32 W, int32 H)
{
  [(__bridge nglUIWindow*)mpUIWindow showKeyboard];
}

void nglWindow::EndTextInput()
{
  [(__bridge nglUIWindow*)mpUIWindow hideKeyboard];
}

bool nglWindow::IsEnteringText() const
{
  //[(nglUIWindow*)mpUIWindow frame]
  return false;
}

/// Drag and Drop:
bool nglWindow::Drag(nglDragAndDrop* pDragObject)
{
  mDragging = true;
  mpDragged = pDragObject;
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
  mDragging = false;
  delete mpDragged;
  mpDragged = nullptr;
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

