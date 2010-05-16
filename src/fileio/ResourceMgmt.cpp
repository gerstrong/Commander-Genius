/*
 * ResourceMgmt.cpp
 *
 *  Created on: 16.05.2010
 *      Author: gerstrong
 */

#include "ResourceMgmt.h"
#include "../FindFile.h"
#include "../CLogFile.h"

const std::string GLOBAL_DIR = "data";

/**
 * This function will look for certain files used in the game. It looks on the local dir
 * and is it didn't find the file, it will look on the global director GLOBAL_DIR. In case
 * it finds something there, it return that alternate path.
 */
std::string getResourceFilename(const std::string& filename, const std::string& gamepath, bool essential)
{
	std::string search_path;
	std::string found_path = "";

	if(gamepath != "")
	{
		search_path = gamepath + "/" + filename;

		g_pLogFile->textOut(GREEN, "Looking for \" " + search_path + "\"... ");
		// try to look at the local path of the game.
		found_path = GetFullFileName(search_path);
	}

	if(found_path == "")
	{
		// if it didn't find the file try looking at the global resources
		search_path = GLOBAL_DIR + "/" + filename;
		g_pLogFile->textOut(GREEN, "Looking for \"" + search_path + "\"... ");
		found_path = GetFullFileName(search_path);
	}

	if( essential && found_path == "" )
	{
		g_pLogFile->textOut(RED, "missing!\n");
		g_pLogFile->textOut(RED, "The file \" " + search_path + "\" was not found. Please provide that file!\n");
	}

	if(found_path != "")
		g_pLogFile->textOut(GREEN, "found!\n");

	return found_path;
}
