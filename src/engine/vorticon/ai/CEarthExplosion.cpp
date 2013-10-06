/*
 * CEarthExplosion.cpp
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#include "CEarthExplosion.h"

CEarthExplosion::CEarthExplosion(CMap *p_map, Uint32 x, Uint32 y) :
CRay(p_map, x, y, RIGHT, CENTER, 0),
animframe(0),
animtimer(0)
{
	onscreen = true;
}

#define EXPLODESPRITE           60
#define EXPLODE_ANIM_RATE       5

#define SPACETILE               155

void CEarthExplosion::process()
{
	sprite = EXPLODESPRITE + animframe;
	if (animtimer > EXPLODE_ANIM_RATE)
	{
		if (m_HorDir!=RIGHT && animframe==0)
		{
			exists = false;
		}
		else
		{
			animframe += (animframe%2);
			if (animframe==3)
			{
				int x, y;
				x = getXPosition()>>CSF;
				y = getYPosition()>>CSF;
				m_HorDir = LEFT;
				mp_Map->setTile((x+8)>>4,(y+8)>>4, SPACETILE, true);
			}
		}
		animtimer = 0;
	}
	else animtimer++;
}
