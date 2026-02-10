
#include "../version.h"
#include "engine/core/CSettings.h"
#include <base/video/CVideoDriver.h>
#include <base/interface/MathLib.h>


#ifdef ANDROID
#include <SDL_main.h>
#endif

#ifdef __SWITCH__
extern "C" {
void switch_init();
void switch_exit();
}
#endif

#include <base/interface/FindFile.h>
#include <base/interface/CrashHandler.h>
#include <base/GsApp.h>
#include <base/GsLogging.h>
#include <base/GsTTFSystem.h>

#include "engine/CGameLauncher.h"

#include <base/audio/Audio.h>

/**
 * @brief registerEvents Register all events, which are
 *        identified through strings mostly using XML data
 */
void registerEvents()
{
    REGISTER_EV_FACTORY(GMQuit);
}


/**
 * @brief  This is the function where CG begins
 *
 * @param	argc   	number of arguments
 * @param	argv   	pointer to  char arrays where
 * 					where the passed arguments are stored
 * 					in the process
 * @return	        This always returns 0. If
 * 					some errors appear, take a look
 * 					at the Logfile.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

int CommanderGenius_Run(int argc, char *argv[]) 
{

#if SDL_VERSION_ATLEAST(2, 0, 0)
#ifdef ANDROID
    SDL_SetMainReady( );
#endif
#endif

#if __SWITCH__
    switch_init();
#endif

    const std::string appName = "Commander Genius";
    gApp.setName(appName);

    // Check if CG should look into a given directory
    std::string binary_dir;
    if(argc >= 1)
    {
        binary_dir = argv[0];
        size_t slashpos = findLastPathSep(binary_dir);

        if(slashpos != std::string::npos)
        {
            binary_dir.erase(slashpos);
            binary_dir = SystemNativeToUtf8(binary_dir);
        }
        else
        {
            binary_dir = ".";
        }
    }
    else
    {
        warnings << "Binary-argument not given, assuming current dir" << endl;
        binary_dir = ".";
    }

    SetBinaryDir( GetAbsolutePath(binary_dir) );

    notes << "Initializing ThreadPool..." << endl;

    CrashHandler::init();

    InitThreadPool();

    registerEvents();

    const auto cfgFName = gSettings.getConfigFileName();
    notes << "Configuration filename is: " << cfgFName << endl;
    InitSearchPaths(cfgFName);
    printSearchPaths();


    bool enableLogfile;
    gSettings.loadGenerals(enableLogfile);


    if( !gLogging.initLogSystem("CGLog.html", appName, CGVERSION, enableLogfile) )
    {
        errors << "Not even able to create \"CGLog.html\"." << endl;
        return 1;
    }

    gLogging.textOut(FONTCOLORS::GREEN,"Created Log file...\n");

    gLogging.textOut(FONTCOLORS::GREEN,"Initializing TTF driver...\n");

    if(!gTTFDriver.init())
    {
        errors << "Failed loading the TTF Driver." << endl;
        gLogging.textOut(FONTCOLORS::RED,"Failed loading the TTF Driver.\n");
        return 1;
    }

    gLogging.textOut(FONTCOLORS::GREEN,"Initializing Video driver...\n");    

    // Init Video Driver with SDL all together
    if( !gVideoDriver.init() )
    {
        errors << "Failed loading the Video Driver." << endl;
        gLogging.textOut(FONTCOLORS::RED,"Failed loading the Video Driver.\n");
        return 1;
    }

    gLogging.textOut(FONTCOLORS::GREEN, "Loading driver settings...\n");

    // Check if there are settings on the PC/Device, otherwise use defaults.
    if( !gSettings.loadDrvCfg() )
    {
        gLogging.textOut(FONTCOLORS::BLUE,"First time message: CG didn't find the driver config file. ");
        gLogging.textOut(FONTCOLORS::BLUE,"However, it generated some default values and will save them now.\n");
        gSettings.saveDrvCfg();
        gSettings.saveGameSpecSettings("Launcher");
    }

    gLogging.textOut(FONTCOLORS::GREEN,"Loading game options...\n");
    if(!gSettings.loadGameOptions())
    {
        gLogging.textOut(FONTCOLORS::RED,"Cannot load defaults...\n");
        gSettings.loadDefaultGameCfg();
    }

    gLogging.textOut(FONTCOLORS::GREEN,"Initializing the Sound system...\n");
    if(!gAudio.init())
    {
        gLogging.textOut(FONTCOLORS::RED,"Failed to init the sound system...\n");
    }


    ////////////////////////////////////////////////////
    // Initialize CG and run the main cycle if worthy //
    ////////////////////////////////////////////////////
    gLogging.textOut(FONTCOLORS::GREEN,"Starting App cycle...\n");

    if( !gApp.init( argc, argv ) )
    {
        gLogging.textOut(FONTCOLORS::RED,"Failed to init the app cycle ...\n");
        gLogging.closeIt();
        return 1;
    }

    setupLookupTables();

    ////////////////////////////////
    // Set GameLauncher as Engine //
    ////////////////////////////////
    gApp.setEngine(new GameLauncher());

    //////////////////////////////
    // Run the Commander Genius //
    //////////////////////////////
    gApp.runMainCycle();

    ////////////////////////////////
    // Tear down Commander Genius //
    ////////////////////////////////
    gApp.deinit();

    gLogging.textOut(FONTCOLORS::GREEN,"Saving Display settings...\n");
    gSettings.saveDispCfg();

    gLogging.textOut(FONTCOLORS::GREEN,"Tearing down thread pool...\n");
    UnInitThreadPool();

    gLogging.closeIt();

#if __SWITCH__
    switch_exit();
#endif
    CrashHandler::uninit();
    return 0;
}


#ifdef __cplusplus
}
#endif
