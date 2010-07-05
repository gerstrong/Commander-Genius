/*
 * CEarthExplosion.cpp
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#include "CEarthExplosion.h"

CEarthExplosion::CEarthExplosion() {
	// TODO Auto-generated constructor stub

}

#define EXPLODESPRITE           60
#define EXPLODE_ANIM_RATE       5

#define SPACETILE               155

void CObjectAI::explosion_ai(CObject &object)
{
	if (object.needinit)
	{
		object.ai.ray.animframe = 0;
		object.ai.ray.animtimer = 0;
		object.ai.ray.direction = 1;    // frame forward
		object.inhibitfall = true;
		object.needinit = false;
		object.onscreen = true;
	}

	object.sprite = EXPLODESPRITE + object.ai.ray.animframe;
	if (object.ai.ray.animtimer > EXPLODE_ANIM_RATE)
	{
		if (object.ai.ray.direction==-1 && object.ai.ray.animframe==0)
		{
			deleteObj(object);
		}
		else
		{
			object.ai.ray.animframe += object.ai.ray.direction;
			if (object.ai.ray.direction==1 && object.ai.ray.animframe==3)
			{
				int x, y;
				x = object.getXPosition()>>CSF;
				y = object.getYPosition()>>CSF;
				object.ai.ray.direction = -1;
				mp_Map->setTile((x+8)>>4,(y+8)>>4, SPACETILE, true);
			}
		}
		object.ai.ray.animtimer = 0;
	}
	else object.ai.ray.animtimer++;
}
