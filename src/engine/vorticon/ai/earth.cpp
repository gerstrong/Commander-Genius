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
		object.onscreen = true;
	}
	if (!object.onscreen) deleteObj(object);

	switch(object.ai.ray.direction)
	{
	case EC_UPLEFTLEFT:
		object.moveLeft(CHUNKSPD);
		object.moveUp(HALFCHUNKSPD);
		break;
	case EC_UPUPLEFT:
		object.moveLeft(HALFCHUNKSPD);
		object.moveUp(CHUNKSPD);
		break;
	case EC_UP:
		object.moveUp(CHUNKSPD);
		break;
	case EC_UPUPRIGHT:
		object.moveRight(HALFCHUNKSPD);
		object.moveUp(CHUNKSPD);
		break;
	case EC_UPRIGHTRIGHT:
		object.moveRight(CHUNKSPD);
		object.moveUp(HALFCHUNKSPD);
		break;
	case EC_DOWNRIGHTRIGHT:
		object.moveRight(CHUNKSPD);
		object.moveDown(HALFCHUNKSPD);
		break;
	case EC_DOWNDOWNRIGHT:
		object.moveRight(HALFCHUNKSPD);
		object.moveDown(CHUNKSPD);
		break;
	case EC_DOWN:
		object.moveDown(CHUNKSPD);
		break;
	case EC_DOWNDOWNLEFT:
		object.moveLeft(HALFCHUNKSPD);
		object.moveDown(CHUNKSPD);
		break;
	case EC_DOWNLEFTLEFT:
		object.moveLeft(CHUNKSPD);
		object.moveDown(HALFCHUNKSPD);
		break;

	case EC_UPLEFT:
		object.moveLeft(BIGCHUNKSPD);
		object.moveUp(BIGCHUNKSPD);
		break;
	case EC_UPRIGHT:
		object.moveRight(BIGCHUNKSPD);
		object.moveUp(BIGCHUNKSPD);
		break;
	case EC_DOWNLEFT:
		object.moveLeft(BIGCHUNKSPD);
		object.moveDown(BIGCHUNKSPD);
		break;
	case EC_DOWNRIGHT:
		object.moveRight(BIGCHUNKSPD);
		object.moveDown(BIGCHUNKSPD);
		break;
	}
}
