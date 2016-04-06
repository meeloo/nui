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
void objCCallOnActivation();
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

- (void) applicationDidFinishLaunching:       (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationDidFinishLaunching");
  NGL_OUT("[nglUIApplicationDelegate applicationDidFinishLaunching]\n");
  assert(App);

  objCCallOnInit(pUIApplication);
}

- (BOOL)application:(UIApplication *)pUIApplication didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
{
  //App->TimedPrint("nglUIApplicationDelegate didFinishLaunchingWithOptions");
	assert(App);
	NSURL *launchURL = [launchOptions objectForKey:UIApplicationLaunchOptionsURLKey];	
	//objCCallOnInitWithURL(pUIApplication, nglString ((CFStringRef)@"bleepbox://oauth?oauth_verifier=fffff"));
	
	if(launchURL)
	{
		NSString *urlstr = [launchURL absoluteString];
		
		objCCallOnInitWithURL(pUIApplication, nglString ((CFStringRef)urlstr));
	} else {
		
		objCCallOnInit(pUIApplication);
	}
}

- (void) applicationDidBecomeActive:          (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationDidBecomeActive");
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

- (void) applicationDidEnterBackground:       (UIApplication*) pUIApplication
{
  //App->TimedPrint("nglUIApplicationDelegate applicationDidEnterBackground");
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

	NSEnumerator *e = [[pUIApplication windows] objectEnumerator];
	
	id win;
	while ((win = [e nextObject]))
  {
		[win close];
	}	
 

///< advise the kernel we're quiting
  objCCallOnExit(0);
}

///////// Notifications:
- (void) application: (UIApplication*) pUIApp didReceiveRemoteNotification: (NSDictionary *)userInfo
{
  std::map<nglString, nglString> infos;
  for (id key in userInfo)
  {
    nglString _key((CFStringRef)key);
    nglString _value((CFStringRef)[userInfo objectForKey:key]);
    infos[_key] = _value;

    NGL_OUT("Notif params: %s -> %s\n", _key.GetChars(), _value.GetChars());
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
