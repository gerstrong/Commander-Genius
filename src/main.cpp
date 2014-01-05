/* MAIN.C
 This is CloneKeen's main source file.

 The CloneKeen source may be freely distributed and
 modified as per the GPL but please give credit to
 the original author, Caitlin Shaw and to the new author
 Gerstrong.

 If you make any changes or improvements to the code that
 you feel merit inclusion in the source tree email them
 to me at gerstrong@gmail.com or get my latest email
 from the SourceForge site.

 Thanks to ID Software for the "Commander Keen: Invasion of
 the Vorticons" games. "Commander Keen" and it's associated
 graphics, level, and sound files are the property of ID
 Software. Commander Genius requires the original version of a
 Commander Keen game in order to be able to interpret that
 episode.

 Enjoy the Code
 - The Commander Genius Team

 CloneKeen          2003-2005   Caitlin Shaw
 CloneKeenPlus      2008-2013   Gerstrong
 Commander Genius   2009-2012   Tulip, Pickle, DaVince, Albert and Pizza2004
 Commander Genius   2010-2012   Lemm, Commander Spleen, Zear, Pelya and FCTW
 Commander Genius   2012-2013   Hagel
 */

#include "../version.h"

#ifdef ANDROID
	#include <SDL_main.h>
#endif

#include <base/FindFile.h>
#include <base/GsApp.h>
#include <lib/base/GsLogging.h>

#include "ThreadPool.h"
#include "common/CGameLauncher.h"

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

	binary_dir = GetAbsolutePath(binary_dir);

	InitThreadPool();
	InitSearchPaths();

    gLogging.CreateLogfile("CGLog.html", APP_NAME, CGVERSION);

    GsApp app;
	
	////////////////////////////
	// Initialize Game Engine //
	////////////////////////////
    if( app.init( argc, argv ) )
	{
        ////////////////////////////////
        // Set GameLauncher as Engine //
        ////////////////////////////////
        app.setEngine(new CGameLauncher(false));

        //////////////////////////////
        // Run the Commander Genius //
        //////////////////////////////
        app.runMainCycle();
	}

	UnInitThreadPool();
	return 0;
}
