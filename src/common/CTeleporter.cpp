/*
 * CTeleporter.cpp
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 *
 *  This class was part of the player structure,
 *  but as it got too complex, I decided to separate that teleporter code.
 */

#include "CTeleporter.h"
#include "../sdl/sound/CSound.h"
#include "../keen.h"


CTeleporter::CTeleporter(std::vector<stTeleporterTable> &TeleportTable, short episode):
m_episode(episode),
m_TeleportTable(TeleportTable)
{}

Uint16 getWord(unsigned char *p_exedata){
	return ((*p_exedata) + (*(p_exedata+1)<<8));
}
// This is called in the beginning and will decrypt the teleporter behavior.
// The Teleporter Table itself will be globally avialable somewhere else. Maybe PlayGame.
// It has to be read from the beginning...
void CTeleporter::createTeleporterTable(unsigned char *p_exedata)
{
	if(m_episode == 1)
	{
		// We have three destinations.
		// When player uses a teleporter, readTeleporterInfo will retrieve that information
		// We can have 6 teleporters cases: dest 1, 2, 3 and snow and dirt in that combination
		for(int i=0 ; i<6 ; i++)
		{
			stTeleporterTable teleport;
			teleport.objectnumber1 = 0x21 + i;
			m_TeleportTable.push_back(teleport);
		}

		m_TeleportTable[0].objectnumber2 = 41;
		m_TeleportTable[1].objectnumber2 = 38;
		m_TeleportTable[2].objectnumber2 = 0;
		m_TeleportTable[3].objectnumber2 = 46;
		m_TeleportTable[4].objectnumber2 = 46;
		m_TeleportTable[5].objectnumber2 = 46; // Here we still don't about the others...

		// Now get the coordinates...
		m_TeleportTable[3].x = m_TeleportTable[0].x = getWord(p_exedata+0x158DF);
		m_TeleportTable[3].y = m_TeleportTable[0].y = getWord(p_exedata+0x158E3);
		m_TeleportTable[4].x = m_TeleportTable[1].x = getWord(p_exedata+0x158E9);
		m_TeleportTable[4].y = m_TeleportTable[1].y = getWord(p_exedata+0x158ED);
		m_TeleportTable[5].x = m_TeleportTable[2].x = getWord(p_exedata+0x158F3);
		m_TeleportTable[5].y = m_TeleportTable[2].y = getWord(p_exedata+0x158F7);
	}
}

// This function checks, whether it is a teleporter or not.
int CTeleporter::getTeleporterInfo(int objectID)
{
	if(m_episode == 1) {
		if( objectID > 33 && objectID < 47 ) return objectID;
	}
	else if(m_episode == 3) {
		if( (objectID & 0xF00) == 0xF00) return objectID;
	}
	return 0;
}

void CTeleporter::readTeleportDestCoordinatesEP1(int objectID, int &destx, int &desty)
{
	destx = desty = 0;

	for(size_t i=0 ; i<m_TeleportTable.size() ; i++)
	{
		if(m_TeleportTable[i].objectnumber2 == objectID || m_TeleportTable[i].objectnumber1 == objectID)
		{
			destx = m_TeleportTable[i].x;
			desty = m_TeleportTable[i].y;
			return;
		}
	}
}

void CTeleporter::readTeleportDestCoordinatesEP3(int objectID, CMap &map,int &destx, int &desty)
{
	destx = desty = 0;

	int newObject = objectID & 0x00F;
	newObject <<= 4;
	newObject += 0xF00; // Now its a teleporter, we only need to find the right one on the map

	for(int i=newObject; i<newObject+0x10 ; i++)
	{
		if(map.findObject(i, &destx, &desty))
		{
			destx <<= TILE_S;
			desty <<= TILE_S;
			return;
		}
	}
}

void CTeleporter::teleportPlayer(int objectID, CMap &map, std::vector<CObject> &p_vect_object, CPlayer &player)
{
	int destx, desty;

	int origx, origy;
	CObject teleporter;
	map.findObject(objectID, &origx, &origy);
	teleporter.spawn( origx<<CSF, origy<<CSF, OBJ_TELEPORTER );
	teleporter.ai.teleport.direction = TELEPORTING_IN;
	if(m_episode == 1)
		readTeleportDestCoordinatesEP1(objectID, destx, desty);
	else if(m_episode == 3)
		readTeleportDestCoordinatesEP3(objectID, map, destx, desty);
	teleporter.ai.teleport.destx = destx>>TILE_S;
	teleporter.ai.teleport.desty = desty>>TILE_S;
	teleporter.ai.teleport.whichplayer = player.m_player_number;
	teleporter.ai.teleport.NoExitingTeleporter = 0;
	p_vect_object.push_back(teleporter);

	// ep1 bonus teleporter
	// TODO: This must be coded more extensive
	/*case LVLS_TELEPORTER_BONUS:
	o = spawn_object(x<<CSF,y<<CSF,OBJ_TELEPORTER);
	m_Object[o].ai.teleport.direction = TELEPORTING_OUT;
	m_Object[o].ai.teleport.destx = TELEPORT_BONUS_DESTX;
	m_Object[o].ai.teleport.desty = TELEPORT_BONUS_DESTY;
	m_Object[o].ai.teleport.whichplayer = cp;
	m_Object[o].ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
	m_Object[o].ai.teleport.idleframe = TELEPORT_RED_IDLEFRAME_EP1;
	m_Object[o].ai.teleport.NoExitingTeleporter = 1;
	m_Object[o].ai.teleport.snap = 1;
	mp_Player[cp].hideplayer = 1;
	break;*/
}
