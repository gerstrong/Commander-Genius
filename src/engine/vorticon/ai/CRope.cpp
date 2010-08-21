#include "../../../graphics/effects/CVibrate.h"
#include "../../spritedefines.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../keen.h"
#include "CVorticon.h"
#include "CRope.h"

// The rope holding the stone which kills the final Vorticon (ep1)
#define STONE_WIDTH          9
#define STONE_HEIGHT         2

#define STONE_DROP_RATE      6

CRope::CRope(CMap *p_map, Uint32 x, Uint32 y) :
CObject(p_map, x, y, OBJ_ROPE)
{
	state = ROPE_IDLE;
	canbezapped = 1;
	inhibitfall = true;
	sprite = OBJ_ROPE_DEFSPRITE;
	needinit = false;
	droptimer = 0;
}

void CRope::process()
{
	switch(state)
	{
	case ROPE_DROPSTONE:
		if (!droptimer)
		{
			droptimer = STONE_DROP_RATE;
			rope_movestone();
			g_pGfxEngine->pushEffectPtr(new CVibrate(400));

			// check if we've hit the ground yet
			for(int x=2;x<STONE_WIDTH-2;x++)
			{
				if (g_pBehaviorEngine->getTileProperties().at(mp_Map->at(stoneX+x, stoneY+2)).bup)
				{
					exists=false;
					return;
				}
			}
		}
		else droptimer--;
	default: break;
	}
}

void CRope::getShotByRay()
{
	if(state == ROPE_IDLE)
	{
		if ( exists && canbezapped )
		{
			int x, y;
			// rope got broke! time to drop the stone
			state = ROPE_DROPSTONE;
			droptimer = 0;
			// hide the rope
			sprite = BLANKSPRITE;
			// get upper left corner of the stone
			x = getXPosition()>>CSF;
			y = getYPosition()>>CSF;
			stoneX = x - 4;
			stoneY = y + 1;
			// get color of background
			bgtile = mp_Map->at(x, y);
		}
	}
}


void CRope::rope_movestone()
{
	int xa,ya;

	xa = stoneX;
	ya = stoneY;

	// move the stone down one space and kill anything in it's path!
	for(int y=STONE_HEIGHT;y>0;y--)
	{
		for(int x=0;x<STONE_WIDTH;x++)
		{
			mp_Map->setTile(x+xa,y+ya, mp_Map->at(x+xa, y+ya-1), true);
		}
	}

	// clear the space at the top
	for(int x=0;x<STONE_WIDTH;x++)
		mp_Map->setTile(x+xa,ya,bgtile, true);

	stoneY++;
}

void CRope::getTouchedBy(CObject &theObject)
{
	if(state == ROPE_DROPSTONE && m_type==OBJ_ROPE)
	{
		const int xa = stoneX;
		const int ya = stoneY;

		// move the stone down one space and kill anything in it's path!
		for(int y=STONE_HEIGHT;y>0;y--)
		{
			for(int x=0;x<STONE_WIDTH;x++)
			{
				// if the stone hits any enemies, kill them
				kill_intersecting_tile(x+xa, y+ya, theObject);
			}
		}
	}
}
