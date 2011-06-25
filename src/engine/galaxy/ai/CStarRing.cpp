/*
 * CStarRing.cpp
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 */

#include "CStarRing.h"

namespace galaxy
{

const unsigned int STARRING_ANIMATION_TIME = 5;

CStarRing::CStarRing(CMap *pmap, Uint32 x, Uint32 y, object_t type) :
CObject(pmap, x, y, type),
m_animation_timer(0)
{
	sprite = STARRING_SPRITE;
	honorPriority = false;
}

void CStarRing::process()
{
	const unsigned char anim = m_animation_timer % STARRING_ANIMATION_TIME;

	if(anim == 0)
	{
		sprite++;

		if(sprite > STARRING_SPRITE+2)
		{
			sprite = STARRING_SPRITE;
			m_animation_timer = 0;
		}
	}

	// Animation timer increasing all the time
	m_animation_timer++;
}

};

