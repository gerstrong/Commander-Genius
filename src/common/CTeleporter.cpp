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

// 3859 <-> 3889
// 3911 -> 3955
// 3877 <-> 3922

// 4032 -> 3842
// 3842 -> 3877
// 4063 -> 4094
// 4094 -> 4072
// 4072 -> 3980
// 3980 -> 4032 nosnap

// table of teleporters (mostly for ep3). player will teleport from
// a teleporter in a teleport_from entry, to the teleporter
// in the matching teleport_dest entry. Thus 3859 leads to 3889, etc.
// if the snap entry is 1 the screen will snap to center on the new
// position, otherwise it will smoothly scroll over to it
#define NUM_TPORTS    20
const int teleport_from[NUM_TPORTS+1] = {4014,4032,3842,4063,4094,4072,3980,3859,3889,3911,3877,3922,3947,4025,3988,3955,41,38,0};
const int teleport_dest[NUM_TPORTS+1] = {4072,3842,3877,4094,4072,3980,4032,3889,3859,3955,3922,3877,4025,3988,3911,3889,38,41,0};
// TODO: This structure is held for compatibility reasons when compiling. In future it will be removed.

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
	else if(m_episode == 3)
	{
		// TODO: This code is missing!! It's far more complex. Be careful!
	}
}

// This function checks, whether it is a teleporter or not.
int CTeleporter::getTeleporterInfo(int objectID)
{
	if(m_episode == 1)
	{
		if(objectID > 33 && objectID < 47) return objectID;
	}
	else if(m_episode == 3)
	{
		// TODO: Ya know!
	}
	return 0;
}

void CTeleporter::readTeleportDestCoordinates(int objectID, int &destx, int &desty)
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
	// TODO: Warn here, if the the entry was not found!
}

void CTeleporter::teleportPlayer(int objectID, CMap &map, std::vector<CObject> &p_vect_object, CPlayer &player)
{
	int destx, desty;
	
	int origx, origy;
	CObject teleporter;
	map.findObject(objectID, &origx, &origy);
	teleporter.spawn( origx<<CSF, origy<<CSF, OBJ_TELEPORTER );
	teleporter.ai.teleport.direction = TELEPORTING_IN;
	readTeleportDestCoordinates(objectID, destx, desty);
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
