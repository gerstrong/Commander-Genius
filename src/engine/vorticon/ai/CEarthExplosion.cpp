/*
 * CEarthExplosion.cpp
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#include "CEarthExplosion.h"

/*CEarthExplosion::CEarthExplosion(CMap *p_map) :
CRay(p_map)
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
		if (m_Direction==NONE && animframe==0)
		{
			exists = false;
		}
		else
		{
			//animframe += (animframe%2);
			//if (direction==1 && animframe==3)
			{
				int x, y;
				x = getXPosition()>>CSF;
				y = getYPosition()>>CSF;
				m_Direction = NONE;
				mp_Map->setTile((x+8)>>4,(y+8)>>4, SPACETILE, true);
			}
		}
		animtimer = 0;
	}
	else animtimer++;
}*/
