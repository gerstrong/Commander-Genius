/*
 * CTileLoader.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CTILELOADER_H_
#define CTILELOADER_H_

#include "../graphics/CTilemap.h"
#include "../common/CBehaviorEngine.h"

class CTileLoader {
public:
	CTileLoader(int episode, int version, size_t NumTiles, unsigned char *data);
	
	bool load();
	
	int getNumTiles() { return m_numtiles; }
	
	virtual ~CTileLoader();

private:
	int m_episode;
	int m_version;
	long m_offset;
	unsigned char *m_data;
	size_t m_numtiles;
	
	std::vector<CTileProperties> &m_TileProperties;
	
	void assignChangeTileAttribute();
	bool setProperOffset();
	bool canbePickedup(int tile);
	bool isaDoor(int tile);
};

#endif /* CTILELOADER_H_ */
