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
#include "CExeFile.h"
#include <map>

class CTileLoader {
public:
	CTileLoader(int episode, int version, size_t NumTiles, unsigned char *data);
	CTileLoader(CExeFile &Exefile, size_t NumTiles);
	
	bool load();
	
	int getNumTiles() { return m_numtiles; }
	
	virtual ~CTileLoader();

private:
	int m_episode;
	int m_version;
	long m_offset;
	unsigned char *m_data;
	size_t m_numtiles;
	
	//std::map<std::map<int,int>, long> m_offsetMap;
	typedef std::map<int,long> VersionMap;
	std::map<int, VersionMap> m_offsetMap;

	std::vector<CTileProperties> &m_TileProperties;
	
	void assignChangeTileAttribute();
	void setupOffsetMap();
	bool canbePickedup(int tile);
	bool isaDoor(int tile);
};

#endif /* CTILELOADER_H_ */
