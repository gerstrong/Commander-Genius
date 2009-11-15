#include "CObjectAI.h"
#include "../spritedefines.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "vort.h"
#include "../../game.h"

//#include "enemyai.h"

// The rope holding the stone which kills the final Vorticon (ep1)

enum ropestates{
ROPE_IDLE, ROPE_DROPSTONE
};

#define STONE_WIDTH          9
#define STONE_HEIGHT         2

#define STONE_DROP_RATE      25

void delete_object(int o);
//void static rope_movestone(int o);
void kill_all_intersecting_tile(int mpx, int mpy);

void CObjectAI::rope_ai(CObject *p_object)
{
	int x;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	if (p_object->needinit)
	{  // first time initialization
		p_object->ai.rope.state = ROPE_IDLE;
		p_object->canbezapped = 1;
		p_object->inhibitfall = 1;
		p_object->sprite = OBJ_ROPE_DEFSPRITE;

		p_object->needinit = 0;
	}

	switch(p_object->ai.rope.state)
	{
	case ROPE_IDLE:
		if (p_object->zapped)
		{
			// rope got broke! time to drop the stone
			p_object->ai.rope.state = ROPE_DROPSTONE;
			p_object->ai.rope.droptimer = 0;
			// hide the rope
			p_object->sprite = BLANKSPRITE;
			// get upper left corner of the stone
			p_object->ai.rope.stoneX = (p_object->x >> CSF >> 4) - 4;
			p_object->ai.rope.stoneY = (p_object->y >> CSF >> 4) + 1;
			// get color of background
			p_object->ai.rope.bgtile = mp_Map->at(p_object->x>>CSF, p_object->y>>CSF);
		}
		break;

	case ROPE_DROPSTONE:
		if (!p_object->ai.rope.droptimer)
		{
			p_object->ai.rope.droptimer = STONE_DROP_RATE;
			rope_movestone(p_object);

			// check if we've hit the ground yet
			for(x=2;x<STONE_WIDTH-2;x++)
			{
				if (TileProperty[mp_Map->at(p_object->ai.rope.stoneX+x, p_object->ai.rope.stoneY+2)].bup)
				{
					deleteObj(p_object);
					return;
				}
			}

		}
		else p_object->ai.rope.droptimer--;
		break;
	}
}

void CObjectAI::rope_movestone(CObject *p_object)
{
	int xa,ya;
	int x,y;

	xa = p_object->ai.rope.stoneX;
	ya = p_object->ai.rope.stoneY;

	// move the stone down one space and kill anything in it's path!
	for(y=STONE_HEIGHT;y>0;y--)
	{
		for(x=0;x<STONE_WIDTH;x++)
		{
			mp_Map->setTile(x+xa,y+ya,mp_Map->at(x+xa, y+ya-1), true);

			// if the stone hits any enemies, kill them
			//kill_all_intersecting_tile(x+xa, y+ya);
		}
	}

	// clear the space at the top
	for(x=0;x<STONE_WIDTH;x++)
		mp_Map->setTile(x+xa,ya,p_object->ai.rope.bgtile, true);

	p_object->ai.rope.stoneY++;
}

