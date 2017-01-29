#include "nui.h"

using namespace std;

#include "nglApplication_UIKit.h"

#include "../../Window/UIKit/nglWindow_UIKit.h"
#include "nglWindow.h"

/*
** nglUIApplication
*/

void objCCallOnInit(void* pUIApplication);
void objCCallOnInitWithURL(void* pUIApplication, const nglString &url);
void objCCallOnExit(int code);
void objCCallOnWillExit();
void objCCallOnPreActivation();
void objCCallOnActivation();
void objCCallOnPreDeactivation();
void objCCallOnDeactivation();
void objCCallOnMemoryWarning();

/*
** nglUIApplicationDelegate
*/
@implementation nglUIApplicationDelegate

- (void) dealloc
{
  //App->TimedPrint("nglUIApplicationDelegate dealloc");
  //NGL_OUT("[nglUIApplicationDelegate dealloc]\n");
  [super dealloc];
}

- (UIWindow*) window
{
  return [[UIApplication sharedApplication] keyWindow];
}

- (void) applicationDidFinishLaunching:       (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationDidFinishLaunching");
  NGL_OUT("[nglUIApplicationDelegate applicationDidFinishLaunching]\n");
  assert(App);

  objCCallOnInit(pUIApplication);
}

- (BOOL)application:(UIApplication *)pUIApplication didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
{
	assert(App);
	NSURL *launchURL = [launchOptions objectForKey:UIApplicationLaunchOptionsURLKey];

	if (launchURL)
	{
    nglString p;
    if ([launchURL isFileURL])
    {
      p = [launchURL.path UTF8String];
    }
    else
    {
      p = [launchURL.absoluteString UTF8String];
    }
		objCCallOnInitWithURL(pUIApplication, p);
	}
  else
  {
		objCCallOnInit(pUIApplication);
	}

  /*
  // Check if we have a notification to display
  NSDictionary *notification = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];

  if (notification)
  {
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:[[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleNameKey]
                                                        message:notification[@"aps"][@"alert"]
                                                       delegate:nil
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:nil];
    [alertView show];
  }
  */

  return YES;
}

// The app is about to enter the foreground.
- (void) applicationWillEnterForeground: (UIApplication*) pUIApplication
{
  NGL_DEBUG( NGL_OUT("[nglUIApplicationDelegate applicationWillEnterForeground]\n"); )
  assert(App);

  objCCallOnPreActivation();

  NSEnumerator *e = [[pUIApplication windows] objectEnumerator];

  id win;
  while ((win = [e nextObject]) )
  {
    if ([win respondsToSelector: @selector(getNGLWindow)] )
    {
      nglWindow* pWindow = [win getNGLWindow];

      NGL_ASSERT(pWindow);
      pWindow->CallOnPreActivation();
    }
  }	
}

// The app has become active
- (void) applicationDidBecomeActive: (UIApplication*) pUIApplication
{
  NGL_DEBUG( NGL_OUT("[nglUIApplicationDelegate applicationDidBecomeActive]\n"); )
  assert(App);

  objCCallOnActivation();
  
	NSEnumerator *e = [[pUIApplication windows] objectEnumerator];
	
	id win;
	while ((win = [e nextObject]) )
  {
		if ([win respondsToSelector: @selector(getNGLWindow)] )
		{
			nglWindow* pWindow = [win getNGLWindow];
			
			NGL_ASSERT(pWindow);
			pWindow->CallOnActivation();			
		}
	}	
}

// The app is about to become inactive
- (void) applicationWillResignActive: (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationDidEnterBackground");
  NGL_DEBUG( NGL_OUT("[nglUIApplicationDelegate applicationWillResignActive]\n"); )
  assert(App);

  NSEnumerator *e = [[pUIApplication windows] objectEnumerator];

  id win;
  while ((win = [e nextObject]))
  {
    if ([win respondsToSelector: @selector(getNGLWindow)])
    {
      nglWindow* pWindow = [win getNGLWindow];

      NGL_ASSERT(pWindow);
      pWindow->CallOnPreDesactivation();
    }
  }

  objCCallOnPreDeactivation();
}

// The app is now in the background
- (void) applicationDidEnterBackground: (UIApplication*) pUIApplication
{
  NGL_DEBUG( NGL_OUT("[nglUIApplicationDelegate applicationDidEnterBackground]\n"); )
  assert(App);

  NSEnumerator *e = [[pUIApplication windows] objectEnumerator];

  id win;
  while ((win = [e nextObject]))
  {
    if ([win respondsToSelector: @selector(getNGLWindow)])
    {
      nglWindow* pWindow = [win getNGLWindow];

      NGL_ASSERT(pWindow);
      pWindow->CallOnDesactivation();
    }
  }

  objCCallOnDeactivation();
}

- (void) applicationDidReceiveMemoryWarning:  (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationDidReceiveMemoryWarning");
	assert(App);
  objCCallOnMemoryWarning();
}

- (void) applicationSignificantTimeChange:    (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationSignificantTimeChange");
//NGL_OUT("[nglUIApplicationDelegate applicationSignificantTimeChange]\n");
}

- (void) applicationWillTerminate:            (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationWillTerminate");
//	NGL_DEBUG( NGL_OUT("[nglUIApplicationDelegate applicationWillTerminate]\n") );

	objCCallOnWillExit();

  for (UIWindow* cur_win in pUIApplication.windows)
  {
    cur_win = nil;
  }

///< advise the kernel we're quiting
  objCCallOnExit(0);
}

///////// Notifications:
- (void) application: (UIApplication*) pUIApp didReceiveRemoteNotification: (NSDictionary *)userInfo
{
  /*
  if (pUIApp.applicationState == UIApplicationStateActive)
  {
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:[[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleNameKey]
                                                        message:userInfo[@"aps"][@"alert"]
                                                       delegate:nil
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:nil];

    [alertView show];
  }
  else if (pUIApp.applicationState == UIApplicationStateBackground || pUIApp.applicationState == UIApplicationStateInactive)
  {
    // Do something else rather than showing an alert view, because it won't be displayed.
  }
  */

  std::map<nglString, nglString> infos;
  for (id key in userInfo)
  {
    id _value = [userInfo objectForKey:key];
    if (_value && [_value isKindOfClass:[NSString class]])
    {
      infos[[key UTF8String]] = [_value UTF8String];
#ifdef _DEBUG_
      NGL_OUT("Notif params: %s -> %s\n", [key UTF8String], [_value UTF8String]);
#endif
    }
  }

  App->DidReceiveNotification(infos);
}


//- (void) application: (UIApplication*) pUIApp didReceiveRemoteNotification: (NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))handler
//{
//
//}

- (void) application: (UIApplication*) pUIApp didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
  std::vector<uint8> token;
  token.resize(deviceToken.length);
  [deviceToken getBytes:&token[0] length:token.size()];
  App->DidRegisterForRemoteNotifications(token);

}

- (void) application: (UIApplication*) pUIApp didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
  std::vector<uint8> token;
  App->DidRegisterForRemoteNotifications(token); // Send an empty token
}


// Handling Local Notifications
- (void) application: (UIApplication*) pUIApp didReceiveLocalNotification:(UILocalNotification *)notification
{
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options
{
  if (url)
  {
    if ([url isFileURL])
    {
      nglPath p { [url.path UTF8String] };
      std::list<nglPath> paths { p };
      ((nglApplication *) App)->OpenDocuments(paths);
    }
    else
    {
      ((nglApplication *) App)->OpenURL([url.absoluteString UTF8String]);
    }
  }
  return YES;
}

@end///< nglUIApplicationDelegate

/*
** nglApplication
*/

// #define NGL_APP_ENONE  0 (in nglApplicationBase.h)

const nglChar* gpApplicationErrorTable[] =
{
/*  0 */ "No error",
  NULL
};

nglApplication::nglApplication()
{
  mExitPosted = false;
  mUseIdle = false;
//  mIdleTimer = NULL;

// nglApplication is a kernel's client, just as plugin instances
  IncRef();
}

nglApplication::~nglApplication()
{
  SetIdle(false);
}


/*
 * Public methods
 */

void nglApplication::Quit (int Code)
{
  mExitPosted = true;
  
  CallOnWillExit();
  CallOnExit(Code);
  exit(Code);
}


/*
 * Internals
 */

/* Startup
 */

int nglApplication::Main(int argc, const char** argv)
{
  //  NSAutoreleasePool *pPool = [NSAutoreleasePool new];
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];  
  {
    //App->TimedPrint("nglApplication::Main Init");

    Init(argc, argv);
  }

  //App->TimedPrint("nglApplication::Main UIApplication");
  UIApplicationMain(argc, const_cast<char**>(argv), nil, @"nglUIApplicationDelegate");

  [pPool release];

  return 0;
}


bool nglApplication::Init(int ArgCnt, const char** pArg)
{
  int i;

//Fetch application's name (App->mName) from argv[0]
  nglString arg0(pArg[0]);
  nglString Name = arg0;

//Only keep file name if it's a full path
  i = Name.Find ('/');
  if (i != -1)
    Name = Name.Extract (i + 1, Name.GetLength());
  SetName(Name);

  // Store user args in mArgs
  for (i = 1; i < ArgCnt; i++)
    AddArg( nglString(pArg[i]) );

  return true;
}


/* Event management / main loop
 */

///////////////////////////////////////////////
/*
nglApplication::nglApplication(nglApplication* pApp);
nglApplication::~nglApplication();
void nglApplication::Quit (int Code);
void nglApplication::MakeMenu();
void nglApplication::DoMenuCommand(long menuResult);
int nglApplication::Run();
void nglApplication::OnEvent(int Flags);
void nglApplication::AddEvent(nglEvent* pEvent);
void nglApplication::DelEvent(nglEvent* pEvent);
bool nglApplication::AddTimer(nglTimer* pTimer);
bool nglApplication::DelTimer(nglTimer* pTimer);
bool nglApplication::AddWindow (nglWindow* pWin);
bool nglApplication::DelWindow (nglWindow* pWin);
void nglApplication::DoEvent(EventRecord *event);
OSErr nglApplication::QuitAppleEventHandler( const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon );
pascal void TimerAction (EventLoopTimerRef  theTimer, void* userData);
nglString nglApplication::GetClipboard();
bool nglApplication::SetClipboard(const nglString& rString);
*/
