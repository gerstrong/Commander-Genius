/*
 * ResourceMgmt.cpp
 *
 *  Created on: 16.05.2010
 *      Author: gerstrong
 *
 *  This small but effective code guarantees that the file the game is looking for are really loaded.
 *  It looks into global and local as needed.
 */

#include "KeenFiles.h"
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>

#include <base/GsPython.h>

/**
 * \brief retrieves Ressource ID from a file
 */
int getRessourceID(const std::string& filename, const std::string& namefilter)
{
	int value;
	std::string text = filename.substr(namefilter.size());

	text.erase(text.find("."));

	value = atoi(text.c_str());
	return value;
}

bool loadGamefileNames()
{
    // Only for python scripts
    if(!gKeenFiles.exeFile.isPythonScript())
        return true;

#if USE_PYTHON3

auto pModule = gPython.loadModule( "keen5", gKeenFiles.gameDir );

if (pModule != nullptr)
{
    bool ok = true;
    std::string stdmapheadFilename;
    ok &= loadStrFunction(pModule, "getMapheadFile", stdmapheadFilename);

    std::string stdGamemapsFilename;
    ok &= loadStrFunction(pModule, "getGamemapsFile", stdGamemapsFilename);

    gKeenFiles.mapheadFilename = stdmapheadFilename;
    gKeenFiles.gamemapsFilename = stdGamemapsFilename;

    return ok;
}

return false;

#endif

}
