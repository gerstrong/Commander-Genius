/*
 * CMapLoader.h
 *
 *  Created on: 09.10.2009
 *      Author: gerstrong
 */

#ifndef CMAPLOADER_H_
#define CMAPLOADER_H_

#include "CMap.h"
#include "CPlayer.h"
#include "CObject.h"
#include "options.h"
#include "sdl/music/CMusic.h"
#include <string>
#include <vector>

class CMapLoader {
public:
	CMapLoader(CMap* p_map, std::vector<CPlayer> *p_PlayerVect = NULL, stOption *mp_option = NULL);
	
	bool load( Uint8 episode, Uint8 level, const std::string& path, bool loadNewMusic=true, bool stategame=false );
	
	void addTile( Uint16 t, Uint16 x, Uint16 y );
	void addWorldMapObject(unsigned int t, Uint16 x, Uint16 y, int episode);
	void addEnemyObject(unsigned int t, Uint16 x, Uint16 y, int episode, int level);
	int fixLevelTiles(int currentTile, Uint16 curmapX, Uint16 curmapY, int episode, int level);
	
	bool m_checkpointset;
	bool m_NessieAlreadySpawned;
	std::vector<CObject*> *mp_objvect;

	stOption *mp_option;

private:
	CMap *mp_map;
	std::vector<CPlayer> *mp_vec_Player;
};

#endif /* CMAPLOADER_H_ */
