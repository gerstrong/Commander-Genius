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
#include <base/interface/FindFile.h>
#include <base/GsLogging.h>

#include <base/GsLua.h>

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
    // Only for Lua scripts
    if(!gKeenFiles.exeFile.isLuaScript())
        return true;

    GsLua lua;

    const auto fullFname =
            GetFullFileName(JoinPaths(gKeenFiles.gameDir, "keen5.lua"));
    lua.loadFile(fullFname);

    if(!lua)
        return false;

    std::string stdmapheadFilename;
    std::string stdGamemapsFilename;

    if(lua.runFunctionRetOneStr("getMapheadFile", stdmapheadFilename))
    {
        gKeenFiles.mapheadFilename = stdmapheadFilename;
    }

    if(lua.runFunctionRetOneStr("getGamemapsFile", stdGamemapsFilename))
    {
        gKeenFiles.gamemapsFilename = stdGamemapsFilename;
    }

    return true;
}
