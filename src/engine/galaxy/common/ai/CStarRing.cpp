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

CStarRing::CStarRing(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
m_animation_timer(0)
{
        starSpriteBase = STARRING_SPRITE;
      
	if(g_pBehaviorEngine->getEpisode() == 5)
	  starSpriteBase = STARRING_SPRITE_EP5;
  
	sprite = starSpriteBase;
	honorPriority = false;
}

void CStarRing::process()
{
	const unsigned char anim = m_animation_timer % STARRING_ANIMATION_TIME;

	if(anim == 0)
	{
		sprite++;

		if(sprite > starSpriteBase+2)
		{
			sprite = starSpriteBase;
			m_animation_timer = 0;
		}
	}

	// Animation timer increasing all the time
	m_animation_timer++;
}

};

