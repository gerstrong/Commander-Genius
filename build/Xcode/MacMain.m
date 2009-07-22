// Mac OS X SDL Code
// Used to register our program, import SDL, and hand off the rest of the code to the game
// No support for Mac OS Leopard or higher as of yet...

#import <SDL/SDL.h>
#import <Cocoa/Cocoa.h> // Needed
#import <sys/param.h> // for MAXPATHLEN
#import <unistd.h>

@interface SDLMain : NSObject
@end

// To avoid warnings, we declare
// it ourselves here.
@interface NSApplication(SDL_Missing_Methods)
- (void)setAppleMenu:(NSMenu *)menu;
@end

// Used to determine whether we use SDLMain.nib or not, in this case
// we want to use our own interface file instead, to be changed later..
#define         SDL_USE_NIB_FILE        0

/* Use this flag to determine whether we use CPS (docking) or not */
#define         SDL_USE_CPS             1
#if SDL_USE_CPS
// Portions of CPS.h
typedef struct CPSProcessSerNum
	{
        UInt32          lo;
        UInt32          hi;
	} CPSProcessSerNum;

extern OSErr    CPSGetCurrentProcess( CPSProcessSerNum *psn);
extern OSErr    CPSEnableForegroundOperation( CPSProcessSerNum *psn, UInt32 _arg2, UInt32 _arg3, UInt32 _arg4, UInt32 _arg5);
extern OSErr    CPSSetFrontProcess( CPSProcessSerNum *psn);

#endif // SDL_USE_CPS

static int    gArgc;
static char  **gArgv;
static BOOL   gFinderLaunch; // See if the app had been launched by the finder

static void addArgument(const char *value)
{
	if(!gArgc)
		gArgv = (char **)malloc(sizeof(*gArgv));
	else
		gArgv = (char **)realloc(gArgv, sizeof(*gArgv) * (gArgc + 1));
	gArgc++;
	gArgv[gArgc - 1] = (char *)malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(gArgv[gArgc - 1], value);
}

static NSString *getApplicationName(void)
{
    NSDictionary *dict;
    NSString *appName = NULL;
	
    // Determine the application name
    dict = ( NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
    if (dict)
        appName = [dict objectForKey: @"CFBundleName"];
	
    if (![appName length])
        appName = [[NSProcessInfo processInfo] processName];
	
    return appName;
}

#if SDL_USE_NIB_FILE
// A helper category for NSString
@interface NSString (ReplaceSubString)
- (NSString *)stringByReplacingRange:(NSRange)aRange with:(NSString *)aString;
@end
#endif

@interface SDLApplication : NSApplication
@end

@implementation SDLApplication
// Invoked from the Quit menu item
- (void)terminate:(id)sender
{
    (void)sender;
    // Post a SDL_QUIT event
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}
@end

// The main class of the application, the application's delegate
@implementation SDLMain

// Set the working directory to the .app's parent directory
- (void) setupWorkingDirectory:(BOOL)shouldChdir
{
    if (shouldChdir)
    {
        char parentdir[MAXPATHLEN];
        CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFURLRef url2 = CFURLCreateCopyDeletingLastPathComponent(0, url);
        if (CFURLGetFileSystemRepresentation(url2, true, (UInt8 *)parentdir, MAXPATHLEN))
        {
			assert ( chdir (parentdir) == 0 );   // chdir to the binary app's parent
        }
        CFRelease(url);
        CFRelease(url2);
    }
	
}

#if SDL_USE_NIB_FILE

// Fix menu to contain the real app name instead of "SDL App"
- (void)fixMenu:(NSMenu *)aMenu withAppName:(NSString *)appName
{
    NSRange aRange;
    NSEnumerator *enumerator;
    NSMenuItem *menuItem;
	
    aRange = [[aMenu title] rangeOfString:@"SDL App"];
    if (aRange.length != 0)
        [aMenu setTitle: [[aMenu title] stringByReplacingRange:aRange with:appName]];
	
    enumerator = [[aMenu itemArray] objectEnumerator];
    while ((menuItem = [enumerator nextObject]))
    {
        aRange = [[menuItem title] rangeOfString:@"SDL App"];
        if (aRange.length != 0)
            [menuItem setTitle: [[menuItem title] stringByReplacingRange:aRange with:appName]];
        if ([menuItem hasSubmenu])
            [self fixMenu:[menuItem submenu] withAppName:appName];
    }
    [ aMenu sizeToFit ];
}

#else

static void setApplicationMenu(void)
{
    /* warning: this code is very odd */
    NSMenu *appleMenu;
    NSMenuItem *menuItem;
    NSString *title;
    NSString *appName;
	
    appName = getApplicationName();
    appleMenu = [[NSMenu alloc] initWithTitle:@""];
	
    // Add menu items
    title = [@"About " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
	
    [appleMenu addItem:[NSMenuItem separatorItem]];
	
    title = [@"Hide " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];
	
    menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];
	
    [appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
	
    [appleMenu addItem:[NSMenuItem separatorItem]];
	
    title = [@"Quit " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
	
	
    // Put menu into the menubar
    menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:appleMenu];
    [[NSApp mainMenu] addItem:menuItem];
	
    // Tell the application object that this is now the application menu
    [NSApp setAppleMenu:appleMenu];
	
    // Finally give up our references to the objects
    [appleMenu release];
    [menuItem release];
}

// Create a window menu
static void setupWindowMenu(void)
{
    NSMenu      *windowMenu;
    NSMenuItem  *windowMenuItem;
    NSMenuItem  *menuItem;
	
    windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
	
    // "Minimize" item
    menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    [windowMenu addItem:menuItem];
    [menuItem release];
	
    // Put menu into the menubar
    windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
    [windowMenuItem setSubmenu:windowMenu];
    [[NSApp mainMenu] addItem:windowMenuItem];
	
    // Tell the application object that this is now the window menu
    [NSApp setWindowsMenu:windowMenu];
	
    // Finally give up our references to the objects
    [windowMenu release];
    [windowMenuItem release];
}

// Replacement for NSApplicationMain
static void CustomApplicationMain (int argc, char **argv)
{
    (void)argc;
    (void)argv;
    NSAutoreleasePool   *pool = [[NSAutoreleasePool alloc] init];
    SDLMain                             *sdlMain;
	
    // Ensure the application object is initialised
    [SDLApplication sharedApplication];
	
#if SDL_USE_CPS
    {
        CPSProcessSerNum PSN;
        // Tell the dock about us
        if (!CPSGetCurrentProcess(&PSN))
            if (!CPSEnableForegroundOperation(&PSN,0x03,0x3C,0x2C,0x1103))
                if (!CPSSetFrontProcess(&PSN))
                    [SDLApplication sharedApplication];
    }
#endif // SDL_USE_CPS
	
    // Set up the menubar
    [NSApp setMainMenu:[[NSMenu alloc] init]];
    setApplicationMenu();
    setupWindowMenu();
	
    // Create SDLMain and make it the app delegate
    sdlMain = [[SDLMain alloc] init];
    [NSApp setDelegate:sdlMain];
	
    // Start the main event loop
    [NSApp run];
	
    [sdlMain release];
    [pool release];
}

#endif

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
    (void)theApplication;
    //addArgument("-iwad");
    //addArgument([filename cString]); // Deprecated in Mac OS Leopard
    return YES;
}

// Called when the internal event loop has just started running
- (void) applicationDidFinishLaunching: (NSNotification *) note
{
    (void)note;
    int status;
	
    // Set the working directory to the .app's parent directory
    [self setupWorkingDirectory:gFinderLaunch];
	
#if SDL_USE_NIB_FILE
    // Set the main menu to contain the real app name instead of "SDL App"
    [self fixMenu:[NSApp mainMenu] withAppName:getApplicationName()];
#endif
	
    //setenv("SRB2PATH", [[[NSBundle mainBundle] resourcePath] cString], 1); // Deprecated Mac OS Leopard
	
    // Hand off to main application code
    status = SDL_main (gArgc, gArgv);
	
    // We're done, thanks for playing
    exit(status);
}
@end


@implementation NSString (ReplaceSubString)

- (NSString *)stringByReplacingRange:(NSRange)aRange with:(NSString *)aString
{
    unsigned int bufferSize;
    unsigned int selfLen = [self length];
    unsigned int aStringLen = [aString length];
    unichar *buffer;
    NSRange localRange;
    NSString *result;
	
    bufferSize = selfLen + aStringLen - aRange.length;
    buffer = NSAllocateMemoryPages(bufferSize*sizeof(unichar));
	
    // Get first part into buffer
    localRange.location = 0;
    localRange.length = aRange.location;
    [self getCharacters:buffer range:localRange];
	
    // Get middle part into buffer
    localRange.location = 0;
    localRange.length = aStringLen;
    [aString getCharacters:(buffer+aRange.location) range:localRange];
	
    // Get last part into buffer
    localRange.location = aRange.location + aRange.length;
    localRange.length = selfLen - localRange.location;
    [self getCharacters:(buffer+aRange.location+aStringLen) range:localRange];
	
    // Build output string
    result = [NSString stringWithCharacters:buffer length:bufferSize];
	
    NSDeallocateMemoryPages(buffer, bufferSize);
	
    return result;
}

@end



#ifdef main
#  undef main
#endif


// Main entry point to the game - shouldn't be SDL_main
int main (int argc, char **argv)
{
	
    // Copy the arguments into a global variable
	
    // This is passed if we are launched by double-clicking
    if ( argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
        gArgc = 1;
        gFinderLaunch = YES;
    } else {
        gArgc = argc;
        gFinderLaunch = NO;
    }
    gArgv = argv;
	
#if SDL_USE_NIB_FILE
    [SDLApplication poseAsClass:[NSApplication class]];
    NSApplicationMain (argc, argv);
#else
    CustomApplicationMain (argc, argv);
#endif
    return 0;
}
