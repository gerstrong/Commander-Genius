#include "graphics/effects/CVibrate.h"
#include "engine/spritedefines.h"
#include "sdl/sound/CSound.h"
#include "CVorticon.h"
#include "CRope.h"

// The rope holding the stone which kills the final Vorticon (ep1)
#define STONE_WIDTH          9
#define STONE_HEIGHT         2

#define STONE_DROP_RATE      6

CRope::CRope(CMap *p_map, Uint32 x, Uint32 y) :
CSpriteObject(p_map, x, y, OBJ_ROPE)
{
	state = ROPE_IDLE;
	canbezapped = 1;
	inhibitfall = true;
	sprite = OBJ_ROPE_DEFSPRITE;
	needinit = false;
	solid = false;
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
			g_pGfxEngine->setupEffect(new CVibrate(400));

			// check if we've hit the ground yet
			for(int x=2;x<STONE_WIDTH-2;x++)
			{
				if (falldist >= 4)
				{
					exists = false;
					return;
				}
			}
		}
		else droptimer--;
	default: break;
	}
}

void CRope::getShotByRay(object_t &obj_type)
{
	if(state == ROPE_IDLE)
	{
		if ( exists && canbezapped )
		{
			// rope got broke! time to drop the stone
			state = ROPE_DROPSTONE;
			droptimer = 0;
			// hide the rope
			sprite = BLANKSPRITE;
			// get upper left corner of the stone
			const int x = (getXPosition()>>CSF);
			const int y = (getYPosition()>>CSF);
			stoneX = x - 4;
			stoneY = y + 1;

			// Create a Colision Rect the way, everthing below gets smashed and on the Stone stays unharmed
			moveTo( (stoneX<<CSF), (stoneY<<CSF)+((1<<CSF)/2) );

			// get color of background
			bgtile = g_pBehaviorEngine->getPhysicsSettings().misc.changestoneblock;

			// Set BBox so the getTouched Function works on the enemies.
			m_BBox(0, 0,
				(STONE_WIDTH<<CSF),
				(STONE_HEIGHT<<CSF)-((1<<CSF)/2) );

			falldist = 0;
		}
	}
}


void CRope::rope_movestone()
{
	int xa,ya;

	xa = stoneX;
	ya = stoneY;

	// move the stone down one space and kill anything in it's path!
	for( int y=STONE_HEIGHT ; y>0 ; y-- )
	{
		for( int x=0 ; x<STONE_WIDTH ; x++ )
		{
			mp_Map->setTile(x+xa,y+ya, mp_Map->at(x+xa, y+ya-1), true);
		}
	}

	// clear the space at the top
	for(int x=0; x<STONE_WIDTH ; x++)
		mp_Map->setTile(x+xa,ya,bgtile, true);

	stoneY++;
	moveDown(1<<CSF);

	falldist++;
}

void CRope::getTouchedBy(CSpriteObject &theObject)
{
	if( state == ROPE_DROPSTONE && m_type==OBJ_ROPE )
	{
		const int xa = stoneX;
		const int ya = stoneY;

		// move the stone down one space and kill anything in it's path!
		for(int y=STONE_HEIGHT;y>0;y--)
		{
			for(int x=0;x<STONE_WIDTH;x++)
			{
				// if the stone hits some enemies, kill them
				kill_intersecting_tile(x+xa, y+ya, theObject);
			}
		}
	}
}
