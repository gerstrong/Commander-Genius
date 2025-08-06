/*
 * CMapLoaderGalaxyEp4.h
 *
 *  Created on: 28.07.2012
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXYEP4_H_
#define CMAPLOADERGALAXYEP4_H_

#include "../common/CMapLoaderGalaxy.h"

namespace galaxy
{

class CMapLoaderGalaxyEp4 : public CMapLoaderGalaxy
{
public:
    CMapLoaderGalaxyEp4(std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
            std::vector<CInventory> &inventoryVec);

	//------------------------------------------------//
	// The following part covers the load of the foes
	// Some defines are hardcoded so it is easier to
	// identify what type of foe/stuff is loaded

    bool isKeenPlayer(const int foeID);

	/**
	 * @brief	Loads a foe given by the coordiantes
	 */
    CGalaxySpriteObject* addFoe(std::shared_ptr<CMap> map,
                                word foe, const size_t x, const size_t y);
};

}

#endif /* CMAPLOADERGALAXYEP4_H_ */
