/*
 * CSpriteItem.h
 *
 *  Created on: 24.11.2010
 *      Author: gerstrong
 *
 *  In Keen Galaxy there tile items and sprite items. The basically do the same and the only difference
 *  is that the sprite items can be hidden in secret areas. This is not always done strangely.
 *  Both item types exist altough I think we really don't need items, oh well...
 */

#ifndef CSpriteItem_H_
#define CSpriteItem_H_

#include "../CGalaxySpriteObject.h"

namespace galaxy {


class CSpriteItem : public CGalaxySpriteObject
{
public:
    CSpriteItem(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y,
                Uint32 mSpriteIdx, const int sprVar, const bool gravity = false);
	void process();
	void getTouchedBy(CSpriteObject &theObject);

private:
	int m_timer;
	int m_basesprite;
	const bool mGravity;
};

}

#endif /* CSpriteItem_H_ */
