#include "engine/spritedefines.h"

#include "CPlatform.h"

#define PLATFORM_MOVE   0
#define PLATFORM_WAIT   1

#define PLATFORM_WAITTIME       50
#define PLATFORM_MOVE_SPD       23

#define PLATFORM_ANIM_RATE      15

#define PLATFORMPUSHAMOUNT      40

CPlatform::CPlatform(CMap *p_map, Uint32 x, Uint32 y) :
CCarrier(p_map, x, y, OBJ_PLATFORM)
{
	animframe = 0;
	animtimer = 0;
	movedir = RIGHT;
	state = PLATFORM_MOVE;

	blockedl = blockedr = false;
	inhibitfall = 1;
	canbezapped = 1;
}

void CPlatform::process()
{
    CCarrier::process();
    
	sprite = (g_pBehaviorEngine->getEpisode()==2) ? OBJ_PLATFORM_DEFSPRITE_EP2 : OBJ_PLATFORM_DEFSPRITE_EP3;
	sprite += animframe;

	if (animtimer > PLATFORM_ANIM_RATE)
	{
		animframe ^= 1;
		animtimer = 0;
	}
	else animtimer++;

	switch(state)
	{
	case PLATFORM_MOVE:

		if (movedir==RIGHT)
		{
			if (blockedr || getXRightPos() > (mp_Map->m_width<<CSF) )
			{
				movedir = LEFT;
				waittimer = 0;
				state = PLATFORM_WAIT;
			}
			else
			{
				moveCarrierRight(PLATFORM_MOVE_SPD);
			}
		}
		else if (movedir==LEFT)
		{
			if (blockedl || getXLeftPos() < (2<<CSF) )
			{
				movedir = RIGHT;
				waittimer = 0;
				state = PLATFORM_WAIT;
			}
			else
			{
				moveCarrierLeft(PLATFORM_MOVE_SPD);
			}
		}
		break;
	case PLATFORM_WAIT:
		if (waittimer > PLATFORM_WAITTIME)
		{
			state = PLATFORM_MOVE;
		}
		else waittimer++;
		break;
	}
}
