/*
 * CItemEffect.h
 *
 *  Created on: 19.11.2010
 *      Author: gerstrong
 */

#ifndef CITEMEFFECT_H_
#define CITEMEFFECT_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"


namespace galaxy {

// Array of chunk numbers for first frames of sprite items (keys, guns, point items)
// All these items have two frames only
const int sprite_item_pics[] =
{
	242,	246,	248,	250,				/* Keygems */
	227,	229,	231,	233,	235,	237,		/* Point items */
	239,	251						/* lifewater and Neural stunner */
};

const int got_sprite_item_pics[3][12] = {
{
	250,	250,	250,	250,				// Got gem sprite
	219,	220,	221,	222,	223,	224,		// 100,200,500,1000,2000,5000 pt sprites
	225,	226						// 1Up and got raygun sprites
},
{
	254,	254,	254,	254,				// Got gem sprite
	217,	218,	219,	220,	221,	222,		// 100,200,500,1000,2000,5000 pt sprites
	223,	224						// 1Up and got raygun sprites
},
{
	254,	254,	254,	254,				// Got gem sprite
	216,	217,	218,	219,	220,	221,		// 100,200,500,1000,2000,5000 pt sprites
	222,	223						// 1Up and got raygun sprites
}
};

enum item_effect_type
{
	FLOAT,
	ANIMATE,
	FADEOUT
};

class CItemEffect : public CGalaxySpriteObject
{
public:
	CItemEffect(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, Uint16 l_sprite, item_effect_type ieffect = FLOAT);
	void process();

private:
	int m_timer;
	item_effect_type m_ieffect;
};

}

#endif /* CITEMEFFECT_H_ */
