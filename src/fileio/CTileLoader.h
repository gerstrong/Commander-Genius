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
	CTileLoader(int episode, bool demo, int version, unsigned char *data);
    CTileLoader();

    bool load(const size_t NumUnMaskedTiles,
              const size_t NumMaskedTiles);

private:
	int m_episode;
	bool m_demo;
	int m_version;
	long m_offset;
	byte *m_data;

	//std::map<std::map<int,int>, long> m_offsetMap;
	typedef std::map<int,long> VersionMap;
	std::map<int, VersionMap> m_offsetMap;

    /**
     * @brief readVorticonTileinfo
     * @param data
     * @param TileProperties
     * @return
     */
    bool readVorticonTileinfo(byte *data,
                              std::vector<CTileProperties> &TileProperties);

    bool readGalaxyTileinfo(byte *data,
                            const size_t NumUnMaskedTiles,
                            const size_t NumMaskedTiles);

	void assignChangeTileAttribute(std::vector<CTileProperties> &TilePropertiesVec);

    /**
     * @brief readOffsetMapfromFile If python script is given, a tli file.
     * @param tileFname Tileset file to consider.
     * @param tileData vector of bytes where the loaded data is stored.
     * @return true if everything went fine. otherwise false
     */
    bool readOffsetMapfromFile(const std::string &tliFname,
                               std::vector<byte> &tileData);

	void setupOffsetMap();
	bool canbePickedup(const signed char behaviour);
	bool isaDoor(const signed char behaviour);
	bool smartTileChangerEnabled();
	size_t getOffset();
};

#endif /* CTILELOADER_H_ */
