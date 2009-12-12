/*
 * CTeleporter.h
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 */

#ifndef CTELEPORTER_H_
#define CTELEPORTER_H_

#include "../common/CPlayer.h"
#include "../common/CObject.h"
#include "../common/CMap.h"
#include <vector>

typedef struct{
	Uint8 objectnumber1; // Andy (Adurdin) calls that sprite, not all his number seems to be right
	Uint8 objectnumber2; // Alternate number
	Uint16 x, y;
}stTeleporterTable;

class CTeleporter {
public:
	//CTeleporter(CMap &map, short episode);
	CTeleporter(std::vector<stTeleporterTable> &TeleportTable, short episode);
	
	void createTeleporterTable(unsigned char *p_exedata);
	int getTeleporterInfo(int objectID);
	void readTeleportDestCoordinates(int objectID, int &destx, int &desty);
	void teleportPlayer(int objectID, CMap &map, std::vector<CObject> &p_vect_object, CPlayer &player);
	
private:
	//CMap &m_map;
    short m_episode;
    std::vector<stTeleporterTable> &m_TeleportTable;
};

#endif /* CTELEPORTER_H_ */
