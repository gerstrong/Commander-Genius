/*
 * CMapLoaderGalaxyEp6.h
 *
 *  Created on: 24.12.2012
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXYEP6_H_
#define CMAPLOADERGALAXYEP6_H_

#include "../common/CMapLoaderGalaxy.h"

namespace galaxy
{

class CMapLoaderGalaxyEp6 : public CMapLoaderGalaxy
{
public:
    CMapLoaderGalaxyEp6(std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
                        std::vector<CInventory> &inventoryVec,
                        bool demo);

    bool isKeenPlayer(const int foeID);

    CGalaxySpriteObject* addFoe(std::shared_ptr<CMap> map, word foe, size_t x, size_t y);
private:
    bool m_demo;
};

}

#endif /* CMAPLOADERGALAXYEP6_H_ */
