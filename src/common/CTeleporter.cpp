/*
 * CTeleporter.cpp
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 *
 *  This class was part of the player structure,
 *  but as it got too complex, I decided to separate that teleporter code.
 */

//#include "CTeleporter.h"
//#include "../sdl/sound/CSound.h"
//#include "../fileio/TypeDefinitions.h"
//#include "../keen.h"
//
//CTeleporter::CTeleporter(std::vector<stTeleporterTable> &TeleportTable, short episode):
//m_episode(episode),
//m_TeleportTable(TeleportTable)
//{}
//
//// This function checks, whether it is a teleporter or not.
//int CTeleporter::getTeleporterInfo(int objectID)
//{
//	if(m_episode == 1) {
//		if( objectID > 33 && objectID < 47 ) return objectID;
//	}
//	else if(m_episode == 3) {
//		if( (objectID & 0xF00) == 0xF00) return objectID;
//	}
//	return 0;
//}
//
//void CTeleporter::readTeleportDestCoordinatesEP1(int objectID, int &destx, int &desty)
//{
//	destx = desty = 0;
//
//	for(size_t i=0 ; i<m_TeleportTable.size() ; i++)
//	{
//		if(m_TeleportTable[i].objectnumber2 == objectID || m_TeleportTable[i].objectnumber1 == objectID)
//		{
//			destx = m_TeleportTable[i].x;
//			desty = m_TeleportTable[i].y;
//			return;
//		}
//	}
//}
//
//void CTeleporter::readTeleportDestCoordinatesEP3(int objectID, CMap &map,int &destx, int &desty)
//{
//	destx = desty = 0;
//
//	int newObject = objectID & 0x00F;
//	newObject <<= 4;
//	newObject += 0xF00; // Now its a teleporter, we only need to find the right one on the map
//
//	for(int i=newObject; i<newObject+0x10 ; i++)
//	{
//		if(map.findObject(i, &destx, &desty))
//		{
//			destx <<= TILE_S;
//			desty <<= TILE_S;
//			return;
//		}
//	}
//}
//
//void CTeleporter::teleportPlayer(int objectID, CMap &map, std::vector<CObject*> &p_vect_object, CPlayer &player)
//{
//	/*int destx, desty;
//
//	int origx, origy;
//	CObject *teleporter = CObject(&map);
//	map.findObject(objectID, &origx, &origy);
//	teleporter.spawn( origx<<CSF, origy<<CSF, OBJ_TELEPORTER, m_episode );
//	teleporter.solid = false;
//	teleporter.ai.teleport.direction = TELEPORTING_IN;
//	if(m_episode == 1)
//		readTeleportDestCoordinatesEP1(objectID, destx, desty);
//	else if(m_episode == 3)
//		readTeleportDestCoordinatesEP3(objectID, map, destx, desty);
//	teleporter.ai.teleport.destx = destx>>TILE_S;
//	teleporter.ai.teleport.desty = desty>>TILE_S;
//	teleporter.ai.teleport.whichplayer = player.m_index;
//	p_vect_object.push_back(teleporter);*/
//}
//
//void CTeleporter::teleportPlayerFromLevel(CMap &map, std::vector<CObject*> &p_vect_object, CPlayer &player, int origx, int origy)
//{
//	/*int destx, desty;
//
//	CObject teleporter(&map);
//	player.beingteleported = true;
//	player.solid = false;
//	destx = m_TeleportTable[5].x;
//	desty = m_TeleportTable[5].y;
//	teleporter.spawn( origx, origy, OBJ_TELEPORTER, m_episode );
//	teleporter.solid = false;
//	teleporter.ai.teleport.direction = TELEPORTING_SCROLL;
//	teleporter.ai.teleport.destx = destx>>TILE_S;
//	teleporter.ai.teleport.desty = desty>>TILE_S;
//	teleporter.ai.teleport.whichplayer = player.m_index;
//	p_vect_object.push_back(teleporter);*/
//}
