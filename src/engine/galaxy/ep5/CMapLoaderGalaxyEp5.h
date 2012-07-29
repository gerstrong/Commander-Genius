/*
 * CMapLoaderGalaxyEp4.h
 *
 *  Created on: 29.07.2012
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXYEP5_H_
#define CMAPLOADERGALAXYEP5_H_

#include "../CMapLoaderGalaxy.h"

namespace galaxy
{

class CMapLoaderGalaxyEp5 : public CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxyEp5(CExeFile &ExeFile,
			std::vector< SmartPointer<CGalaxySpriteObject> > &ObjectPtr,
			CInventory &Inventory, stCheat &Cheatmode);

	CGalaxySpriteObject* addFoe(CMap &Map, word foe, size_t x, size_t y);
};

};

#endif /* CMAPLOADERGALAXYEP4_H_ */
