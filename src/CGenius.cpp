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
 CloneKeenPlus      2008-2014   Gerstrong
 Commander Genius   2009-2014   Tulip,
 Commander Genius   2009-2013   NY00123,
 Commander Genius   2009-2013   Pickle, DaVince, Albert and Pizza2004
 Commander Genius   2010-2012   Lemm, Commander Spleen, Zear, Pelya and FCTW
 Commander Genius   2013-2014   Hagel
 */

#include "../version.h"
#include "engine/core/CSettings.h"

#ifdef ANDROID
	#include <SDL_main.h>
#endif

#include <base/utils/FindFile.h>
#include <base/GsApp.h>
#include <base/GsLogging.h>

#include "engine/CGameLauncher.h"

#include "sdl/audio/Audio.h"

/**
 * \brief  This is the function where CG beings
 *
 * \param	argc   	number of arguments
 * \param	argv   	pointer to  char arrays where
 * 					where the passed arguments are stored
 * 					in the process
 * \return	        This always returns 0. If
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

	InitThreadPool();
    InitSearchPaths(g_pSettings->getConfigFileName());

    gLogging.CreateLogfile("CGLog.html", APP_NAME, CGVERSION);

    // Check if there are settings on the PC, otherwise use defaults.
    if(!g_pSettings->loadDrvCfg())
    {
        //m_firsttime = true;
        gLogging.textOut(RED,"First time message: CG didn't find the driver config file. ");
        gLogging.textOut(RED,"However, it generated some default values and will save them now.\n");
        g_pSettings->saveDrvCfg();
    }

    if(!g_pSettings->loadGameOptions())
    {
        g_pSettings->loadDefaultGameCfg();
    }

    // Init the Game sound
    g_pSound->init();

    ////////////////////////////////////////////////////
    // Initialize CG and run the main cycle if worthy //
    ////////////////////////////////////////////////////
    if( gApp.init( argc, argv ) )
	{
        ////////////////////////////////
        // Set GameLauncher as Engine //
        ////////////////////////////////
        gApp.setEngine(new CGameLauncher(false));

        //////////////////////////////
        // Run the Commander Genius //
        //////////////////////////////
        gApp.runMainCycle();
	}

    g_pSettings->saveDispCfg();

	UnInitThreadPool();
	return 0;
}
