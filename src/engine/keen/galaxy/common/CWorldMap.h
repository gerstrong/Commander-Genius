/*
 * CWorldMap.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 *
 *  This is in charge of the ai and physics on the worldmap
 */

#ifndef CWORLDMAP_H_
#define CWORLDMAP_H_

#include "../CMapPlayGalaxy.h"
#include <boost/property_tree/ptree.hpp>

namespace galaxy {

class CWorldMap : public CMapPlayGalaxy
{
public:
    CWorldMap(std::vector<CInventory> &inventoryVec);

	void init();
	void loadAndPlayMusic();

    void ponder(const float deltaT);

};

}

#endif /* CWORLDMAP_H_ */
