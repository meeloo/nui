#include "nui.h"

using namespace std;

#include "nglApplication_Cocoa.h"

#include "../../Window/Cocoa/nglWindow_Cocoa.h"

/*
** nglNSApplication
*/

void objCCallOnInit(void* pNSApplication);
void objCCallOnInitWithURL(void* pNSApplication, const nglString &url);
void objCCallOnExit(int code);
void objCCallOnWillExit();
void objCCallOnPreActivation();
void objCCallOnActivation();
void objCCallOnPreDeactivation();
void objCCallOnDeactivation();


static NSString* GetApplicationName(void)
{
  NSDictionary *dict;
  NSString *appName = 0;
  
  /* Determine the application name */
  dict = (NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
  if (dict)
    appName = [dict objectForKey: @"CFBundleName"];
  
  if (![appName length])
    appName = [[NSProcessInfo processInfo] processName];
  
  return appName;
}


@interface MenuPopulator : NSObject
{}

+(void) populateMainMenu;

+(void) populateApplicationMenu:(NSMenu *)aMenu;
+(void) populateDebugMenu:(NSMenu *)aMenu;
+(void) populateEditMenu:(NSMenu *)aMenu;
+(void) populateFileMenu:(NSMenu *)aMenu;
+(void) populateFindMenu:(NSMenu *)aMenu;
+(void) populateHelpMenu:(NSMenu *)aMenu;
+(void) populateSpellingMenu:(NSMenu *)aMenu;
+(void) populateViewMenu:(NSMenu *)aMenu;
+(void) populateWindowMenu:(NSMenu *)aMenu;

@end

@implementation MenuPopulator

#pragma mark Public

+(void) populateMainMenu
{
	NSMenu * mainMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
	
	NSMenuItem * menuItem;
	NSMenu * submenu;
	
	// The titles of the menu items are for identification purposes only and shouldn't be localized.
	// The strings in the menu bar come from the submenu titles,
	// except for the application menu, whose title is ignored at runtime.
	menuItem = [mainMenu addItemWithTitle:@"Apple" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:@"Apple"];
	[NSApp performSelector:@selector(setAppleMenu:) withObject:submenu];
	[self populateApplicationMenu:submenu];
	[mainMenu setSubmenu:submenu forItem:menuItem];
	
	menuItem = [mainMenu addItemWithTitle:@"File" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"File", @"The File menu")];
	[self populateFileMenu:submenu];
	[mainMenu setSubmenu:submenu forItem:menuItem];
	
	menuItem = [mainMenu addItemWithTitle:@"Edit" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Edit", @"The Edit menu")];
	[self populateEditMenu:submenu];
	[mainMenu setSubmenu:submenu forItem:menuItem];
  [submenu release];

	/* TODO
   menuItem = [mainMenu addItemWithTitle:@"View" action:NULL keyEquivalent:@""];
   submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"View", @"The View menu")];
   [self populateViewMenu:submenu];
   [mainMenu setSubmenu:submenu forItem:menuItem];
   */
	
	menuItem = [mainMenu addItemWithTitle:@"Window" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Window", @"The Window menu")];
	[self populateWindowMenu:submenu];
	[mainMenu setSubmenu:submenu forItem:menuItem];
	[NSApp setWindowsMenu:submenu];
  [submenu release];

	menuItem = [mainMenu addItemWithTitle:@"Help" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Help", @"The Help menu")];
	[self populateHelpMenu:submenu];
	[mainMenu setSubmenu:submenu forItem:menuItem];
  [submenu release];

	/* TODO
   menuItem = [mainMenu addItemWithTitle:@"Debug" action:NULL keyEquivalent:@""];
   submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Debug", @"The Debug menu")];
   [self populateDebugMenu:submenu];
   [mainMenu setSubmenu:submenu forItem:menuItem];
   */
	
	[NSApp setMainMenu:mainMenu];
  [mainMenu release];
}

+(void) populateApplicationMenu:(NSMenu *)aMenu
{
	NSString * applicationName = GetApplicationName();
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"About", nil), applicationName]
                              action:@selector(orderFrontStandardAboutPanel:)
                       keyEquivalent:@""];
	[menuItem setTarget:NSApp];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Preferences...", nil)
                              action:NULL
                       keyEquivalent:@","];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Services", nil)
                              action:NULL
                       keyEquivalent:@""];
	NSMenu * servicesMenu = [[NSMenu alloc] initWithTitle:@"Services"];
	[aMenu setSubmenu:servicesMenu forItem:menuItem];
	[NSApp setServicesMenu:servicesMenu];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Hide", nil), applicationName]
                              action:@selector(hide:)
                       keyEquivalent:@"h"];
	[menuItem setTarget:NSApp];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Hide Others", nil)
                              action:@selector(hideOtherApplications:)
                       keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:NSCommandKeyMask | NSAlternateKeyMask];
	[menuItem setTarget:NSApp];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Show All", nil)
                              action:@selector(unhideAllApplications:)
                       keyEquivalent:@""];
	[menuItem setTarget:NSApp];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Quit", nil), applicationName]
                              action:@selector(terminate:)
                       keyEquivalent:@"q"];
	[menuItem setTarget:NSApp];
}

+(void) populateDebugMenu:(NSMenu *)aMenu
{
	// TODO
}

+(void) populateEditMenu:(NSMenu *)aMenu
{
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Undo", nil)
                              action:@selector(undo:)
                       keyEquivalent:@"z"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Redo", nil)
                              action:@selector(redo:)
                       keyEquivalent:@"Z"];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Cut", nil)
                              action:@selector(cut:)
                       keyEquivalent:@"x"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Copy", nil)
                              action:@selector(copy:)
                       keyEquivalent:@"c"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Paste", nil)
                              action:@selector(paste:)
                       keyEquivalent:@"v"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Paste and Match Style", nil)
                              action:@selector(pasteAsPlainText:)
                       keyEquivalent:@"V"];
	[menuItem setKeyEquivalentModifierMask:NSCommandKeyMask | NSAlternateKeyMask];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Delete", nil)
                              action:@selector(delete:)
                       keyEquivalent:@""];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Select All", nil)
                              action:@selector(selectAll:)
                       keyEquivalent:@"a"];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Find", nil)
                              action:NULL
                       keyEquivalent:@""];
	NSMenu * findMenu = [[NSMenu alloc] initWithTitle:@"Find"];
	[self populateFindMenu:findMenu];
	[aMenu setSubmenu:findMenu forItem:menuItem];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Spelling", nil)
                              action:NULL
                       keyEquivalent:@""];
	NSMenu * spellingMenu = [[NSMenu alloc] initWithTitle:@"Spelling"];
	[self populateSpellingMenu:spellingMenu];
	[aMenu setSubmenu:spellingMenu forItem:menuItem];
}

+(void) populateFileMenu:(NSMenu *)aMenu
{
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"New", nil)
                              action:NULL
                       keyEquivalent:@"n"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Open...", nil)
                              action:NULL
                       keyEquivalent:@"o"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Open Recent", nil)
                              action:NULL
                       keyEquivalent:@""];
	NSMenu * openRecentMenu = [[NSMenu alloc] initWithTitle:@"Open Recent"];
	[openRecentMenu performSelector:@selector(_setMenuName:) withObject:@"NSRecentDocumentsMenu"];
	[aMenu setSubmenu:openRecentMenu forItem:menuItem];
	
	menuItem = [openRecentMenu addItemWithTitle:NSLocalizedString(@"Clear Menu", nil)
                                       action:@selector(clearRecentDocuments:)
                                keyEquivalent:@""];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Close", nil)
                              action:@selector(performClose:)
                       keyEquivalent:@"w"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Save", nil)
                              action:NULL
                       keyEquivalent:@"s"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Save As...", nil)
                              action:NULL
                       keyEquivalent:@"S"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Revert", nil)
                              action:NULL
                       keyEquivalent:@""];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Page Setup...", nil)
                              action:@selector(runPageLayout:)
                       keyEquivalent:@"P"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Print...", nil)
                              action:@selector(print:)
                       keyEquivalent:@"p"];
}

+(void) populateFindMenu:(NSMenu *)aMenu
{
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Find...", nil)
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"f"];
	[menuItem setTag:NSFindPanelActionShowFindPanel];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Find Next", nil)
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"g"];
	[menuItem setTag:NSFindPanelActionNext];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Find Previous", nil)
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"G"];
	[menuItem setTag:NSFindPanelActionPrevious];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Use Selection for Find", nil)
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"e"];
	[menuItem setTag:NSFindPanelActionSetFindString];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Jump to Selection", nil)
                              action:@selector(centerSelectionInVisibleArea:)
                       keyEquivalent:@"j"];
}

+(void) populateHelpMenu:(NSMenu *)aMenu
{
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", GetApplicationName(), NSLocalizedString(@"Help", nil)]
                              action:@selector(showHelp:)
                       keyEquivalent:@"?"];
	[menuItem setTarget:NSApp];
}

+(void) populateSpellingMenu:(NSMenu *)aMenu
{
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Spelling...", nil)
                              action:@selector(showGuessPanel:)
                       keyEquivalent:@":"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Check Spelling", nil)
                              action:@selector(checkSpelling:)
                       keyEquivalent:@";"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Check Spelling as You Type", nil)
                              action:@selector(toggleContinuousSpellChecking:)
                       keyEquivalent:@""];
}

+(void) populateViewMenu:(NSMenu *)aMenu
{
	// TODO
}

+(void) populateWindowMenu:(NSMenu *)aMenu
{
	NSMenuItem * menuItem;
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Minimize", nil)
                              action:@selector(performMinimize:)
                       keyEquivalent:@"m"];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Zoom", nil)
                              action:@selector(performZoom:)
                       keyEquivalent:@""];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Bring All to Front", nil)
                              action:@selector(arrangeInFront:)
                       keyEquivalent:@""];
}

@end



@implementation nglNSApplication

- (void) dealloc
{
//  NGL_OUT("[nglNSApplication dealloc]\n");
  [super dealloc];
}

- (BOOL) openURL: (NSURL*) pUrl
{
  //NGL_OUT("[nglNSApplication openURL]\n");
  [super openURL: pUrl];
  return true;
}

- (void) sendEvent: (NSEvent*) pEvent
{
//NGL_DEBUG( NGL_OUT("[nglNSApplication sendEvent]\n") );
  [super sendEvent: pEvent];
}


@end///< nglNSApplication

@implementation nglNSApplicationDelegate

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
  printf("nglNSApplicationDelegate applicationWillFinishLaunching\n");
  [MenuPopulator populateMainMenu];
  //  [pItem setTarget:NSApp];
  //  [pItem setAction: @selector(terminate:)];
  
}

- (void) applicationDidFinishLaunching:       (NSApplication*) pNSApplication
{
  //NGL_OUT("[nglNSApplicationDelegate applicationDidFinishLaunching]\n");
  NGL_ASSERT(App);
  
  objCCallOnInit(pNSApplication);
}

- (void)applicationDidReceiveMemoryWarning:  (NSApplication*) pUIApp
{

}


- (BOOL)application:(NSApplication *)pNSApplication didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	NGL_ASSERT(App);
	//NSURL *launchURL = [launchOptions objectForKey:NSApplicationLaunchOptionsURLKey];	
	//objCCallOnInitWithURL(pNSApplication, nglString ((CFStringRef)@"bleepbox://oauth?oauth_verifier=fffff"));
	
  //	if(launchURL)
  //	{
  //		NSString *urlstr = [launchURL absoluteString];
  //		
  //		objCCallOnInitWithURL(pNSApplication, nglString ((CFStringRef)urlstr));
  //	} else {
  //		
  //		objCCallOnInit(pNSApplication);
  //	}
  
  return YES;
}

- (void) applicationWillBecomeActive: (NSNotification *) pNotification
{
  NGL_DEBUG( NGL_OUT("[nglNSApplicationDelegate applicationWillBecomeActive]\n"); )
  NGL_ASSERT(App);

  objCCallOnPreActivation();

  NSApplication* pNSApplication = [pNotification object];
  if (pNSApplication != nil)
  {
    NSArray* array = [pNSApplication windows];
    if (array != nil)
    {
      NSEnumerator *e = [array objectEnumerator];

      id win;
      while (e != nil && (win = [e nextObject]) )
      {
        if ([win respondsToSelector: @selector(getNGLWindow)] )
        {
          nglWindow* pWindow = [win getNGLWindow];

          NGL_ASSERT(pWindow);
          pWindow->CallOnPreActivation();
        }
      }
    }
  }
}

- (void) applicationDidBecomeActive: (NSNotification*) pNotification
{
  NGL_DEBUG( NGL_OUT("[nglNSApplicationDelegate applicationDidBecomeActive]\n"); )
  NGL_ASSERT(App);

  objCCallOnActivation();

  NSApplication* pNSApplication = [pNotification object];
  if (pNSApplication != nil)
  {
    NSArray* array = [pNSApplication windows];
    if (array != nil)
    {
      NSEnumerator *e = [array objectEnumerator];
      
      id win;
      while (e != nil && (win = [e nextObject]) )
      {
        if ([win respondsToSelector: @selector(getNGLWindow)] )
        {
          nglWindow* pWindow = [win getNGLWindow];
          
          NGL_ASSERT(pWindow);
          pWindow->CallOnActivation();			
        }
      }
    }
  }
}

- (void) applicationWillResignActive: (NSNotification*) pNotification
{
  NGL_DEBUG( NGL_OUT("[nglNSApplicationDelegate applicationWillResignActive]\n"); )
  NGL_ASSERT(App);

  NSApplication* pNSApplication = [pNotification object];
  if (pNSApplication != nil)
  {
    NSArray* array = [pNSApplication windows];
    if (array != nil)
    {
      NSEnumerator *e = [array objectEnumerator];

      id win;
      while (e != nil && (win = [e nextObject]) )
      {
        if ([win respondsToSelector: @selector(getNGLWindow)] )
        {
          nglWindow* pWindow = [win getNGLWindow];

          NGL_ASSERT(pWindow);
          pWindow->CallOnPreDesactivation();
        }
      }
    }
  }

  objCCallOnPreDeactivation();
}


- (void) applicationDidResignActive: (NSNotification*) pNotification
{
  NGL_DEBUG( NGL_OUT("[nglNSApplicationDelegate applicationDidResignActive]\n"); )
  NGL_ASSERT(App);
	
  NSApplication* pNSApplication = [pNotification object];
  if (pNSApplication != nil)
  {
    NSArray* array = [pNSApplication windows];
    if (array != nil)
    {
      NSEnumerator *e = [array objectEnumerator];

      id win;
      while (e != nil && (win = [e nextObject]) )
      {
        if ([win respondsToSelector: @selector(getNGLWindow)] )
        {
          nglWindow* pWindow = [win getNGLWindow];

          NGL_ASSERT(pWindow);
          pWindow->CallOnDesactivation();
        }
      }
    }
  }

  objCCallOnDeactivation();
}

- (void) applicationSignificantTimeChange:    (NSNotification*) pNotification
{
  //NGL_OUT("[nglNSApplicationDelegate applicationSignificantTimeChange]\n");
}

- (void) applicationWillTerminate:            (NSNotification*) pNotification
{
  //	NGL_DEBUG( NGL_OUT("[nglNSApplicationDelegate applicationWillTerminate]\n") );
  
	objCCallOnWillExit();
  
  NSApplication* pNSApplication = [pNotification object];
  if (pNSApplication != nil)
  {
    NSArray* array = [pNSApplication windows];
    if (array != nil)
    {
      NSEnumerator *e = [[pNSApplication windows] objectEnumerator];
      
      id win;
      while ((win = [e nextObject]))
      {
        [win release];
      }	
    }
  }
  
  ///< advise the kernel we're quiting
  objCCallOnExit(0);
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
  if (filename)
  {
    std::list<nglPath> paths { [filename UTF8String] };
    ((nglApplication *) App)->OpenDocuments(paths);
  }
  return filename != nil;
}

/*
- (void)application:(NSApplication *)sender openFiles:(NSArray *)filenames
{
  NSLog(@"[NSApplication openFiles: %@", filenames);
}
*/

- (BOOL)application:(id)sender openFileWithoutUI:(NSString *)filename
{
  NSLog(@"[NSApplication openFileWithoutUI: %@", filename);
  return YES;
}

- (BOOL)application:(NSApplication *)theApplication openTempFile:(NSString *)filename
{
  NSLog(@"[NSApplication openTempFile: %@", filename);
  return YES;
}

@end



/*
** nglNSApplicationDelegate
*/
//@implementation nglNSApplicationDelegate

//- (void) dealloc
//{
//  //NGL_OUT("[nglNSApplicationDelegate dealloc]\n");
//  [super dealloc];
//}

//
//@end///< nglNSApplicationDelegate

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
//  mExitPosted = false;
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
//  mExitPosted = true;
//  mExitCode = Code;
  
//  QuitApplicationEventLoop();
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

  Init(argc, argv);

  //GetLog().SetLevel("window", 100);
  
  nglNSApplication *applicationObject = (nglNSApplication *)[nglNSApplication sharedApplication];

  nglNSApplicationDelegate* appDelegate = [[nglNSApplicationDelegate alloc] init];
  [applicationObject setDelegate:appDelegate];
  [applicationObject run];
  
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
