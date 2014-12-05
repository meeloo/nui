#ifndef __nglWindow_UIKit_h__
#define __nglWindow_UIKit_h__

#include <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>


/*
 ** nglUIView
 */

@interface nglUIView : UIView
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
@end///< nglUIWindow

#endif//__nglWindow_UIKit_h__
