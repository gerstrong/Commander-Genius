/**
 CGenius.cpp
 This is Commander Genius main source file.

 The Commander Genius source may be freely distributed and
 modified as per the GPL but please give credit to
 the original authors.

 If you make any changes or improvements to the code that
 you feel merit inclusion in the source tree email them
 to me at gerstrong@gmail.com.

 Thanks to ID Software for the "Commander Keen" series.
 "Commander Keen" and it's associated
 graphics, level, and sound files are the property of ID
 Software. Commander Genius requires the original version of a
 Commander Keen game in order to be able to interpret them.

 Enjoy our hard work!
 - The Commander Genius Team

 CloneKeen          2003-2005   Caitlin Shaw
 CloneKeenPlus      2008-2019   Gerstrong
 Commander Genius   2009-2014   Tulip,
 Commander Genius   2009-2013   NY00123,
 Commander Genius   2009-2013   Pickle, DaVince, Albert and Pizza2004
 Commander Genius   2010-2012   Lemm, Commander Spleen, Zear, Pelya and FCTW
 Commander Genius   2013-2015   Hagel
 Commander Genius   2018        Zilem
 */

#include "../version.h"
#include "engine/core/CSettings.h"
#include "base/video/CVideoDriver.h"


#ifdef ANDROID
#include <SDL_main.h>
#endif

#include <base/utils/FindFile.h>
#include <base/GsApp.h>
#include <base/GsLogging.h>
#include <base/GsTTFSystem.h>

#include "engine/CGameLauncher.h"

#include "sdl/audio/Audio.h"


/**
 * @brief  This is the function where CG beings
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
int main(int argc, char *argv[])
{

#if SDL_VERSION_ATLEAST(2, 0, 0)
#ifdef ANDROID
    SDL_SetMainReady( );
#endif
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

    InitThreadPool();

    const auto cfgFName = gSettings.getConfigFileName();
    notes << "Configuration filename is: " << cfgFName << endl;
    InitSearchPaths(cfgFName);
    printSearchPaths();

    if( !gLogging.CreateLogfile("CGLog.html", appName, CGVERSION) )
    {
        errors << "Not even able to create \"CGLog.html\"." << endl;
        return 1;
    }

	gLogging.textOut(FONTCOLORS::GREEN,"Created Log file...\n");

    printSearchPaths();

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

    // Check if there are settings on the PC, otherwise use defaults.
    if( !gSettings.loadDrvCfg() )
    {
        gLogging.textOut(FONTCOLORS::BLUE,"First time message: CG didn't find the driver config file. ");
        gLogging.textOut(FONTCOLORS::BLUE,"However, it generated some default values and will save them now.\n");
        gSettings.saveDrvCfg();
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

    if( gApp.init( argc, argv ) )
    {
        ////////////////////////////////
        // Set GameLauncher as Engine //
        ////////////////////////////////
        gApp.setEngine(new CGameLauncher());

        //////////////////////////////
        // Run the Commander Genius //
        //////////////////////////////
        gApp.runMainCycle();

        ////////////////////////////////
        // Tear down Commander Genius //
        ////////////////////////////////
        gApp.deinit();
    }
	else
	{
		gLogging.textOut(FONTCOLORS::RED,"Failed to init the app cycle ...\n");
		return 1;
    }

	gLogging.textOut(FONTCOLORS::GREEN,"Saving Display settings...\n");
    gSettings.saveDispCfg();

	gLogging.textOut(FONTCOLORS::GREEN,"Tearing down thread pool...\n");
    UnInitThreadPool();
    return 0;
}
