/*
 * CLevelPlay.h
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 *
 *  This class will manage the entire gameplay in one level
 */

#ifndef CLEVELPLAY_H_
#define CLEVELPLAY_H_

#include "CMapPlayGalaxy.h"

namespace galaxy {

class CLevelPlay : public CMapPlayGalaxy
{
public:
	CLevelPlay(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode);

	void loadMap(const int level);
	void reloadLevel();

	bool loadLevel(const Uint16 level);

    // Saves the inventory using the Savegamecontroller.
    //void operator>>(boost::property_tree::ptree &invNode);

    // This is for loading the game
    //void operator<<(boost::property_tree::ptree &invNode);

};

}

#endif /* CLEVELPLAY_H_ */
