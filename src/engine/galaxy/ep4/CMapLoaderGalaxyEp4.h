/*
 * CMapLoaderGalaxyEp4.h
 *
 *  Created on: 28.07.2012
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXYEP4_H_
#define CMAPLOADERGALAXYEP4_H_

#include "../CMapLoaderGalaxy.h"

namespace galaxy
{

class CMapLoaderGalaxyEp4 : public CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxyEp4(CExeFile &ExeFile,
			std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
			CInventory &Inventory, stCheat &Cheatmode);

	//------------------------------------------------//
	// The following part covers the load of the foes
	// Some defines are hardcoded so it is easier to
	// identify what type of foe/stuff is loaded

	/**
	 * @brief	Loads a foe given by the coordiantes
	 */
	CGalaxySpriteObject* addFoe(CMap &Map, word foe, size_t x, size_t y);
};

};

#endif /* CMAPLOADERGALAXYEP4_H_ */
