/*
 * CMapLoaderGalaxyEp5.h
 *
 *  Created on: 29.09.2012
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXYEP5_H_
#define CMAPLOADERGALAXYEP5_H_

#include "../common/CMapLoaderGalaxy.h"

namespace galaxy
{

class CMapLoaderGalaxyEp5 : public CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxyEp5(CExeFile &ExeFile,
			std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
            std::vector<CInventory> &inventoryVec, stCheat &Cheatmode);

	CGalaxySpriteObject* addFoe(CMap &Map, word foe, size_t x, size_t y);
};

};

#endif /* CMAPLOADERGALAXYEP5_H_ */
