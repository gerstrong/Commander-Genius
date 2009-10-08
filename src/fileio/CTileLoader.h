/*
 * CTileLoader.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CTILELOADER_H_
#define CTILELOADER_H_

#include "../graphics/CTilemap.h"

class CTileLoader {
public:
	CTileLoader(int episode, int version, unsigned char *data);
	virtual ~CTileLoader();

	bool load();

	stTile *getTileProperties() { return mp_TileProperty; }

private:
	int m_episode;
	int m_version;
	long m_offset;
	unsigned char *m_data;
	int m_numtiles;

	stTile *mp_TileProperty;

	void assignChangeTileAttribute();
	bool setProperOffset();
	bool canbePickedup(int tile);
	bool isaDoor(int tile);
};

#endif /* CTILELOADER_H_ */
