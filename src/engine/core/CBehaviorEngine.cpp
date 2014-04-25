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
#include <base/GsLogging.h>

void CBehaviorEngine::setMessage(const std::string &name, const std::string &message)
{	
  stringmap[name] = message;	
}

/**
 * \brief This function reads the teleporter table in case there is any.
 * 		  As far I'm concerned, only episode 1 has one and it is patched
 * 		  by many mods
 * \param	episode		Episode of the Game. Only Episode 1 should be used if it has really
 * 						a table. Other the function will fail, what is okay.
 * 						Episode 3 uses another algorithm for the teleporters.
 * \param	p_exedata	pointer to the data where the exe is located at.
 */
bool CBehaviorEngine::readTeleporterTable(byte *p_exedata)
{
	if(m_Episode == 1)
	{
		// clear the teleporter table
		if(!m_TeleporterTable.empty())
			m_TeleporterTable.clear();

		// We have three destinations.
		// When player uses a teleporter, readTeleporterInfo will retrieve that information
		// We can have 6 teleporters cases: dest 1, 2, 3 and snow and dirt in that combination
		for(int i=0 ; i<6 ; i++)
		{
			stTeleporterTable teleport;
			teleport.objectnumber1 = 0x21 + i;
			m_TeleporterTable.push_back(teleport);
		}

		m_TeleporterTable[0].objectnumber2 = 41;
		m_TeleporterTable[1].objectnumber2 = 38;
		m_TeleporterTable[2].objectnumber2 = 0;
		m_TeleporterTable[3].objectnumber2 = 46;
		m_TeleporterTable[4].objectnumber2 = 46;
		m_TeleporterTable[5].objectnumber2 = 46; // Here we still don't know about the others...

		// Now get the coordinates...
		m_TeleporterTable[0].x = GETWORD(p_exedata+0x158DF);
		m_TeleporterTable[0].y = GETWORD(p_exedata+0x158E3);
		m_TeleporterTable[1].x = GETWORD(p_exedata+0x158E9);
		m_TeleporterTable[1].y = GETWORD(p_exedata+0x158ED);
		m_TeleporterTable[3].x = GETWORD(p_exedata+0x158E9);
		m_TeleporterTable[3].y = GETWORD(p_exedata+0x158ED);
		m_TeleporterTable[4].x = GETWORD(p_exedata+0x158E9);
		m_TeleporterTable[4].y = GETWORD(p_exedata+0x158ED);
		m_TeleporterTable[5].x = m_TeleporterTable[2].x = GETWORD(p_exedata+0x158F3);
		m_TeleporterTable[5].y = m_TeleporterTable[2].y = GETWORD(p_exedata+0x158F7);
		return true;
	}
	return false;
}

std::vector<CTileProperties> &CBehaviorEngine::getTileProperties(size_t tmnum)
{	return m_TileProperties[tmnum];	}

// returns a pointer to the string with name 'name'
std::string CBehaviorEngine::getString(const std::string& name)
{
	std::string text;
	std::map<std::string, std::string>::iterator it = stringmap.find(name);

	if( it != stringmap.end() )
		text = it->second;
	else
		text = "";

	return text;
}

size_t CBehaviorEngine::getEpisode()
{	return m_Episode;	}

EngineType CBehaviorEngine::getEngine()
{
	if( m_Episode >= 4 && m_Episode <= 6 )
		return ENGINE_GALAXY;
	else if(m_Episode >= 1 && m_Episode <= 3)
		return ENGINE_VORTICON;
	else
		return ENGINE_LAUNCHER;
}

void CBehaviorEngine::setEpisode(size_t Episode)
{	m_Episode = Episode;	}

