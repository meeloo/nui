#ifndef __nglWindow_UIKit_h__
#define __nglWindow_UIKit_h__

#include <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>


/*
 ** nglUIView_GL
 */

@interface nglUIView_GL : UIView
{
@private
  nglWindow* mpNGLWindow;
}
- (id)  initWithNGLWindow: (nglWindow*) pNGLWindow;

@end

/*
 ** nglUIView_Metal
 */

@interface nglUIView_Metal : UIView
{
@private
  nglWindow* mpNGLWindow;
}
- (id)  initWithNGLWindow: (nglWindow*) pNGLWindow;

@end


/*
 ** nglUIViewController
 */
@interface nglUIViewController : UIViewController
{
@private
  nglWindow* mpNGLWindow;
}
- (id)  initWithNGLWindow: (nglWindow*) pNGLWindow;

@end


/*
** nglUIWindow
*/
@interface nglUIWindow : UIWindow<UITextFieldDelegate>
{
  nglWindow*      mpNGLWindow;
  CADisplayLink*  mDisplayLink;
	UITextField*    mpTextField;
	BOOL            mKeyboardVisible;
  NSMutableArray<UIKeyCommand *>* mHotkeys;
}

- (id) initWithNGLWindow: (nglWindow*) pNGLWindow;
- (void) dealloc;
- (void) startDisplayLink;
- (void) stopDisplayLink;
- (void) displayTicked;
- (void) dumpTouch: (UITouch*) pTouch;
- (void) dumpTouches: (UIEvent*) pEvent;
- (nglWindow *) getNGLWindow;

- (void)showKeyboard;
- (void)hideKeyboard;
- (void)initializeKeyboard;

// Hotkeys
- (void)addHotKeyChar:(nuiHotKeyChar*)hotkey;
- (void)delHotKeyChar:(nuiHotKeyChar*)hotkey;

@end///< nglUIWindow

#endif//__nglWindow_UIKit_h__
