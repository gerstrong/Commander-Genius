// explosion and earth chunk objects, used for the "earth explodes"
// sequence when you press the switch on a Tantalus Ray in ep2.

#include "CObjectAI.h"

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
				object.ai.ray.direction = -1;
				mp_Map->setTile(((object.x>>CSF)+8)>>4,((object.y>>CSF)+8)>>4, SPACETILE, true);
			}
		}
		object.ai.ray.animtimer = 0;
	}
	else object.ai.ray.animtimer++;
}

#define EARTHCHUNK_ANIM_RATE       5

#define CHUNKSPD        20
#define HALFCHUNKSPD    (CHUNKSPD/2)
#define BIGCHUNKSPD     16

void CObjectAI::earthchunk_ai(CObject &object)
{
	if (object.needinit)
	{
		object.inhibitfall = true;
		object.needinit = false;
	}
	if (!object.onscreen) deleteObj(object);

	switch(object.ai.ray.direction)
	{
	case EC_UPLEFTLEFT:
		object.x -= CHUNKSPD;
		object.y -= HALFCHUNKSPD;
		break;
	case EC_UPUPLEFT:
		object.x -= HALFCHUNKSPD;
		object.y -= CHUNKSPD;
		break;
	case EC_UP:
		object.y -= CHUNKSPD;
		break;
	case EC_UPUPRIGHT:
		object.x += HALFCHUNKSPD;
		object.y -= CHUNKSPD;
		break;
	case EC_UPRIGHTRIGHT:
		object.x += CHUNKSPD;
		object.y -= HALFCHUNKSPD;
		break;
	case EC_DOWNRIGHTRIGHT:
		object.x += CHUNKSPD;
		object.y += HALFCHUNKSPD;
		break;
	case EC_DOWNDOWNRIGHT:
		object.x += HALFCHUNKSPD;
		object.y += CHUNKSPD;
		break;
	case EC_DOWN:
		object.y += CHUNKSPD;
		break;
	case EC_DOWNDOWNLEFT:
		object.x -= HALFCHUNKSPD;
		object.y += CHUNKSPD;
		break;
	case EC_DOWNLEFTLEFT:
		object.x -= CHUNKSPD;
		object.y += HALFCHUNKSPD;
		break;

	case EC_UPLEFT:
		object.x -= BIGCHUNKSPD;
		object.y -= BIGCHUNKSPD;
		break;
	case EC_UPRIGHT:
		object.x += BIGCHUNKSPD;
		object.y -= BIGCHUNKSPD;
		break;
	case EC_DOWNLEFT:
		object.x -= BIGCHUNKSPD;
		object.y += BIGCHUNKSPD;
		break;
	case EC_DOWNRIGHT:
		object.x += BIGCHUNKSPD;
		object.y += BIGCHUNKSPD;
		break;
	}
}
