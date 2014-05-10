/*
 * CTileLoader.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CTILELOADER_H_
#define CTILELOADER_H_

#include "graphics/GsTilemap.h"
//#include "core/CBehaviorEngine.h"
#include "CTileProperties.h"
#include "CExeFile.h"
#include <map>

class CTileLoader {
public:
	CTileLoader(int episode, int version, unsigned char *data);
	CTileLoader(CExeFile &Exefile);
	
	bool load(size_t NumUnMaskedTiles, size_t NumMaskedTiles);
	void readVorticonTileinfo(std::vector<CTileProperties> &TileProperties);
	void readGalaxyTileinfo(size_t NumUnMaskedTiles, size_t NumMaskedTiles);

private:
	int m_episode;
	int m_version;
	long m_offset;
	byte *m_data;
	
	//std::map<std::map<int,int>, long> m_offsetMap;
	typedef std::map<int,long> VersionMap;
	std::map<int, VersionMap> m_offsetMap;
	
	void assignChangeTileAttribute(std::vector<CTileProperties> &TilePropertiesVec);
	void setupOffsetMap();
	bool canbePickedup(const signed char behaviour);
	bool isaDoor(const signed char behaviour);
	bool smartTileChangerEnabled();
};

#endif /* CTILELOADER_H_ */
