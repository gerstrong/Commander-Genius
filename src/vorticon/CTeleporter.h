/*
 * CTeleporter.h
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 */

#ifndef CTELEPORTER_H_
#define CTELEPORTER_H_

#define TELEPORT_GRAY_BASEFRAME_EP1  342
#define TELEPORT_GRAY_IDLEFRAME_EP1  99

#define TELEPORT_RED_BASEFRAME_EP1   338
#define TELEPORT_RED_IDLEFRAME_EP1   325

#define TELEPORT_BASEFRAME_EP3  130
#define TELEPORT_IDLEFRAME_EP3  134

#include "../common/CPlayer.h"
#include "../common/CObject.h"
#include "../common/CMap.h"
#include <vector>

class CTeleporter {
public:
	CTeleporter(CMap &map, short episode);

	bool readTeleporterInfo(int lvl);
	void teleportPlayer(std::vector<CObject> &p_vect_object, CPlayer &player);

private:
	CMap &m_map;
    int telfrom;
    int teldest;
    int telsnap;
    short m_episode;
};

#endif /* CTELEPORTER_H_ */
