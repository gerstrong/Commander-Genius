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
{}

void CBehaviorEngine::addMessage(const std::string &name,
								const std::string &message)
{	stringmap[name] = message;	}


std::vector<CTileProperties> &CBehaviorEngine::getTileProperties()
{	return m_TileProperties;	}

CPhysicsSettings &CBehaviorEngine::getPhysicsSettings()
{	return m_PhysicsSettings;	}

// returns a pointer to the string with name 'name'
std::string CBehaviorEngine::getString(const std::string& name)
{
	std::string text;
	std::map<std::string, std::string>::iterator it = stringmap.find(name);

	if( it != stringmap.end() )
		text = it->second;
	else
		text = "UNKNOWN '" + name + "' STRING";

	return text;
}

CBehaviorEngine::~CBehaviorEngine()
{
}
