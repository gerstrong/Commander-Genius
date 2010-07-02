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
	CTileLoader(int episode, int version, unsigned char *data);
	CTileLoader(CExeFile &Exefile);
	
	bool load(size_t NumUnMaskedTiles, size_t NumMaskedTiles);
	void readVorticonTileinfo(size_t NumTiles);
	void readGalaxyTileinfo(size_t NumUnMaskedTiles, size_t NumMaskedTiles);
	
	virtual ~CTileLoader();

private:
	int m_episode;
	int m_version;
	long m_offset;
	unsigned char *m_data;
	
	//std::map<std::map<int,int>, long> m_offsetMap;
	typedef std::map<int,long> VersionMap;
	std::map<int, VersionMap> m_offsetMap;
	
	void assignChangeTileAttribute(size_t NumTiles);
	void setupOffsetMap();
	bool canbePickedup(int tile);
	bool isaDoor(int tile);
};

#endif /* CTILELOADER_H_ */
