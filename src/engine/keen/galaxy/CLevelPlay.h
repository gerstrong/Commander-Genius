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
    CLevelPlay(CExeFile &ExeFile, std::vector<CInventory> &inventory, stCheat &Cheatmode);

    /**
     * @brief loadLevel Makes this class load the map of the given level and the loading message. calls loadMap
     * @param level level number
     * @return true opon success, otherwise false
     */
    bool loadLevel(const Uint16 level);

    /**
     * @brief reloadLevel load the opened level again
     */
    void reloadLevel();


private:

    /**
     * @brief loadMap will load the given level
     * @param level level number to load
     */
	void loadMap(const int level);



};

}

#endif /* CLEVELPLAY_H_ */
