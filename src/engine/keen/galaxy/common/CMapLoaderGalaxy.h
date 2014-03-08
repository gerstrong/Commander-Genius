/*
 * CVorticonMapLoaderGalaxy.h
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#ifndef CVorticonMapLoaderGALAXY_H_
#define CVorticonMapLoaderGALAXY_H_

#include <memory>
#include <vector>
#include <string>
#include <SDL.h>

#include "fileio/CExeFile.h"
#include "fileio/TypeDefinitions.h"
#include "common/CMap.h"
#include "common/Cheat.h"
#include "CInventory.h"
#include "CGalaxySpriteObject.h"

namespace galaxy
{

class CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxy(CExeFile &ExeFile,
			std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
            std::vector<CInventory> &inventoryVec, stCheat &Cheatmode);
	
	size_t getMapheadOffset();
	bool gotoNextSignature(std::ifstream &MapFile);
	bool loadMap(CMap &Map, Uint8 level);
	void spawnFoes(CMap &Map);
	
	std::string getLevelName()
	{ return mLevelName; }
	
	
	virtual CGalaxySpriteObject* addFoe(CMap &Map, word foe, size_t x, size_t y) = 0;
	virtual ~CMapLoaderGalaxy() {}

    int numLoadedPlayers()
    {
        return mNumLoadedPlayers;
    }

    void resetNumLoadedPlayers()
    {
        mNumLoadedPlayers = 0;
    }

protected:
	void unpackPlaneData(std::ifstream &MapFile,
			CMap &Map, size_t PlaneNumber,
			longword offset, longword length,
			word magic_word);

	CExeFile &m_ExeFile;
	std::vector< std::shared_ptr<CGalaxySpriteObject> > &m_ObjectPtr;
    std::vector<CInventory> &mInventoryVec;
	stCheat &m_Cheatmode;
	std::string mLevelName;
    int mNumLoadedPlayers;
};

}

#endif /* CVorticonMapLoaderGALAXY_H_ */
