#ifndef __nglWindow_UIKit_h__
#define __nglWindow_UIKit_h__

#include <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

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

#ifdef _METAL_
@interface nglUIView_Metal : UIView
{
@private
  nglWindow* mpNGLWindow;
}
- (id)  initWithNGLWindow: (nglWindow*) pNGLWindow;

@end
#endif // NUI_HAS_METAL_IOS

/*
 ** nglUIViewController
 */
@interface nglUIViewController : UIViewController<UIDropInteractionDelegate>
{
@private
  nglWindow* mpNGLWindow;
  std::unique_ptr<nglDragAndDrop> mpDropObject;
}
- (id)  initWithNGLWindow: (nglWindow*) pNGLWindow;
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id <UIViewControllerTransitionCoordinator>)coordinator NS_AVAILABLE_IOS(8_0);

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
