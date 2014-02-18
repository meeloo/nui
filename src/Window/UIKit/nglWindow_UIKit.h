#ifndef __nglWindow_UIKit_h__
#define __nglWindow_UIKit_h__

#include <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/glext.h>
#import <GLKit/GLKit.h>

/*
** Touches tracking ..
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class nglTouchInfo ///< Touch info
{
public:
  UITouch* mpUITouch;
  nglTouchId mTouchId;
  int X, Y;
};

typedef std::map<UITouch*,nglTouchInfo> TouchesInfo;

@interface NGLViewController : GLKViewController
{
@private
  nglWindow* mpNGLWindow;
}

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow;

@end


/*
** nglUIWindow
*/
@interface nglUIWindow : UIWindow<UITextFieldDelegate, GLKViewDelegate>
{
  nglWindow*    mpNGLWindow;
  nglContextInfo* mpContextInfo;
  nglTime       mLastEventTime;
  bool          mInited;
  id            mDisplayLink;
  NSTimer*      mInvalidationTimer;
	UITextField*  mpTextField;
	BOOL          mKeyboardVisible;

  GLKView* mpGLKView;
  nuiTimer*     mpTimer;

  CGRect oldrect;
}

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow;
- (void) setContext: (void*) pContext renderBuffer: (GLint) buffer;
- (void) dealloc;
- (void) invalidate;
- (void) sendEvent: (UIEvent*) pEvent;

- (void) dumpTouch: (UITouch*) pTouch;
- (void) dumpTouches: (UIEvent*) pEvent;
- (nglWindow *) getNGLWindow;
- (void) disconnect;

- (void)showKeyboard;
- (void)hideKeyboard;
- (void)initializeKeyboard;
@end///< nglUIWindow

#endif//__nglWindow_UIKit_h__
