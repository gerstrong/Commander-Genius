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
    CWorldMap(CExeFile &ExeFile, std::vector<CInventory> &inventoryVec, stCheat &Cheatmode);

	void init();
	void loadAndPlayMusic();

    // Saves the inventory using the Savegamecontroller.
    //void operator>>(boost::property_tree::ptree &invNode);

    // This is for loading the game
    //void operator<<(boost::property_tree::ptree &invNode);

};

}

#endif /* CWORLDMAP_H_ */
