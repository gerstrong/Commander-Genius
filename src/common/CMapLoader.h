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
#include <string>

class CMapLoader {
public:
	CMapLoader(CMap* p_map, CPlayer *p_Player = NULL);
	virtual ~CMapLoader();

	bool load( Uint8 episode, Uint8 level, const std::string& path );

	void addTile( Uint16 t, Uint16 x, Uint16 y );
	void addWorldMapObject(unsigned int t, Uint16 x, Uint16 y, int episode);
	void addEnemyObject(unsigned int t, Uint16 x, Uint16 y, int episode, int chglevelto);

private:
	CMap *mp_map;
	CPlayer *mp_Player;
};

#endif /* CMAPLOADER_H_ */
