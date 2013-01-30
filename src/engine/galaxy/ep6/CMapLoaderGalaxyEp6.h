/*
 * CMapLoaderGalaxyEp6.h
 *
 *  Created on: 24.12.2012
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXYEP6_H_
#define CMAPLOADERGALAXYEP6_H_

#include "engine/galaxy/common/CMapLoaderGalaxy.h"

namespace galaxy
{

class CMapLoaderGalaxyEp6 : public CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxyEp6(CExeFile &ExeFile,
			std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
			CInventory &Inventory, stCheat &Cheatmode);

	CGalaxySpriteObject* addFoe(CMap &Map, word foe, size_t x, size_t y);
};

};

#endif /* CMAPLOADERGALAXYEP6_H_ */
