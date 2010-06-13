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

enum teleporation_state{
TELEPORTING_OUT,
TELEPORTING_SCROLL,
TELEPORTING_IN
};


typedef struct{
	Uint8 objectnumber1; // Andy (Adurdin) calls that sprite, not all his numbers seems to be right
	Uint8 objectnumber2; // Alternate number
	Uint16 x, y;
}stTeleporterTable;

class CTeleporter {
public:
	CTeleporter(std::vector<stTeleporterTable> &TeleportTable, short episode);
	
	Uint16 getWord(unsigned char *p_exedata);
	void createTeleporterTable(unsigned char *p_exedata);
	int getTeleporterInfo(int objectID);
	void teleportPlayer(int objectID, CMap &map, std::vector<CObject> &p_vect_object, CPlayer &player);
	void teleportPlayerFromLevel(CMap &map, std::vector<CObject> &p_vect_object, CPlayer &player, int origx, int origy);
	
private:
	//CMap &m_map;
    short m_episode;
    std::vector<stTeleporterTable> &m_TeleportTable;

	void readTeleportDestCoordinatesEP1(int objectID, int &destx, int &desty);
	void readTeleportDestCoordinatesEP3(int objectID, CMap &map,int &destx, int &desty);
};

#endif /* CTELEPORTER_H_ */
