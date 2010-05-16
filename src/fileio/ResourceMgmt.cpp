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
 * \brief	This function will look for certain files used in the game. It looks on the local dir
 * 			and is it didn't find the file, it will look on the global director GLOBAL_DIR. In case
 * 			it finds something there, it return that alternate path.
 *  \param	filename		the filename for which we are looking
 *  \param	gamepath		if a game started, path to that one
 *  \param	essential		essential means whether the file is really needed or the game can live without it.
 * 	\param	can_be_global	This means, that the file can be read from the global directory. In some cases it must not.
 */
std::string getResourceFilename(const std::string& filename, const std::string& gamepath,
								bool essential, bool can_be_global)
{
	std::string search_path;
	std::string found_path = "";
	std::string text;

	if(gamepath != "")
	{
		search_path = gamepath + "/" + filename;
		text = "Looking for \"" + search_path + "\"... ";
		// try to look at the local path of the game.
		found_path = GetFullFileName(search_path);
		if(!IsFileAvailable(found_path))
			found_path = "";
	}

	if(!can_be_global)
		return found_path;

	if(found_path == "")
	{
		// if it didn't find the file try looking at the global resources
		search_path = GLOBAL_DIR + "/" + filename;
		text = "Looking for \"" + search_path + "\"... ";

		g_pLogFile->textOut(GREEN, "Looking for \"" + search_path + "\"... ");
		found_path = GetFullFileName(search_path);
		if(!IsFileAvailable(found_path))
			found_path = "";
	}

	if( found_path == "" )
	{
		if(essential)
		{
			g_pLogFile->textOut(RED, "missing!\n");
			g_pLogFile->textOut(RED, "The file \" " + search_path + "\" was not found. Please provide that file!\n");
		}
		else
			g_pLogFile->textOut(BLUE, "missing, but not needed!\n");
	}
	g_pLogFile->textOut(GREEN, "found!\n");

	return found_path;
}
