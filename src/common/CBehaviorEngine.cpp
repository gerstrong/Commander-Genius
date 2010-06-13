/*
 * CBehaviorEngine.cpp
 *
 *  Created on: 11.06.2010
 *      Author: gerstrong
 *
 *  This engine stores all the behavior in the games,
 *  that can be read from the game-data files.
 *  Mainly the TileProperties are stored here,
 *  which for example decides whether something
 *  is blocked or not.
 *
 *  Also Information-blocks like some Strings read from the Exe-files
 *  belong to this class
 */

#include "CBehaviorEngine.h"

CBehaviorEngine::CBehaviorEngine()
{
	// TODO Auto-generated constructor stub

}

std::vector<CTileProperties> &CBehaviorEngine::getTileProperties()
{	return m_TileProperties;	}

CPhysicsSettings &CBehaviorEngine::getPhysicsSettings()
{	return m_PhysicsSettings;	}

// returns a pointer to the string with name 'name'
std::string CBehaviorEngine::getString(const std::string& name)
{
	for(int i=0;i<numStrings;i++)
	{
		if (name == strings[i].name)
		{
			// Returning is not enough anymore
			// Convert the /h and /H into proper colours
			// This is necessary since the new graphics engine
			return strings[i].message;
		}
	}

	return "UNKNOWN '" + name + "' STRING";
}

CBehaviorEngine::~CBehaviorEngine()
{
	// TODO Auto-generated destructor stub
}
