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

