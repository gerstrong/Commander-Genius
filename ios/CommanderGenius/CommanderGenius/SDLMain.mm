#import <Foundation/Foundation.h>
#include <SDL.h>
#include "FindFile.h"

// Forward-declare the entry point
extern "C" int CommanderGenius_Run(int argc, char** argv);

// Forward declare the app's shared files dir
extern std::string gIOSSandboxSharedFilesDir;

const char* GetIOSDocumentsPath() {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docs = [paths firstObject];
    return strdup([docs UTF8String]);
    // caller frees
}

int SDL_main(int argc, char *argv[])
{
    gIOSSandboxSharedFilesDir = GetIOSDocumentsPath();
    return CommanderGenius_Run(argc, argv);
}

