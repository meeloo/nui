#ifndef __nglUIApplication_h__
#define __nglUIApplication_h__

#include "ngl.h"
#import <UIKit/UIKit.h>

@class nglUIWindow;

@interface nglUIApplicationDelegate : NSObject <UIApplicationDelegate>
{
}

- (void) dealloc;
- (void) applicationDidFinishLaunching:       (UIApplication*) pUIApp;
- (BOOL) application:                         (UIApplication*) pUIApp didFinishLaunchingWithOptions: (NSDictionary *) launchOptions;
- (void) applicationDidBecomeActive:          (UIApplication*) pUIApp;
- (void) applicationWillResignActive:         (UIApplication*) pUIApp;
- (void) applicationDidEnterBackground:       (UIApplication*) pUIApp;
- (void) applicationDidReceiveMemoryWarning:  (UIApplication*) pUIApp;
- (void) applicationSignificantTimeChange:    (UIApplication*) pUIApp;
- (void) applicationWillTerminate:            (UIApplication*) pUIApp;

// Handling Remote Notifications

- (void) application: (UIApplication*) pUIApp didReceiveRemoteNotification: (NSDictionary *)userInfo;
//- (void) application: (UIApplication*) pUIApp didReceiveRemoteNotification: (NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))handler;
- (void) application: (UIApplication*) pUIApp didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
- (void) application: (UIApplication*) pUIApp didFailToRegisterForRemoteNotificationsWithError:(NSError *)error;

// Handling Local Notifications
- (void) application: (UIApplication*) pUIApp didReceiveLocalNotification:(UILocalNotification *)notification;


@end//nglUIApplicationDelegate


#endif//__nglUIApplication_h__
