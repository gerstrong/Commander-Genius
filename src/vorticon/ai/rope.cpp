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

#define STONE_DROP_RATE      6

void delete_object(int o);
//void static rope_movestone(int o);
void kill_all_intersecting_tile(int mpx, int mpy);

void CObjectAI::rope_ai(CObject &object)
{
	int x;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	if (object.needinit)
	{  // first time initialization
		object.ai.rope.state = ROPE_IDLE;
		object.canbezapped = 1;
		object.inhibitfall = 1;
		object.sprite = OBJ_ROPE_DEFSPRITE;
		object.needinit = false;
	}

	switch(object.ai.rope.state)
	{
	case ROPE_IDLE:
		if (object.zapped)
		{
			// rope got broke! time to drop the stone
			object.ai.rope.state = ROPE_DROPSTONE;
			object.ai.rope.droptimer = 0;
			// hide the rope
			object.sprite = BLANKSPRITE;
			// get upper left corner of the stone
			object.ai.rope.stoneX = (object.x >> CSF) - 4;
			object.ai.rope.stoneY = (object.y >> CSF) + 1;
			// get color of background
			object.ai.rope.bgtile = mp_Map->at(object.x>>CSF, object.y>>CSF);
		}
		break;

	case ROPE_DROPSTONE:
		if (!object.ai.rope.droptimer)
		{
			object.ai.rope.droptimer = STONE_DROP_RATE;
			rope_movestone(object);

			// check if we've hit the ground yet
			for(x=2;x<STONE_WIDTH-2;x++)
			{
				if (TileProperty[mp_Map->at(object.ai.rope.stoneX+x, object.ai.rope.stoneY+2)].bup)
				{
					deleteObj(object);
					return;
				}
			}

		}
		else object.ai.rope.droptimer--;
		break;
	}
}

void CObjectAI::rope_movestone(CObject &object)
{
	int xa,ya;
	int x,y;

	xa = object.ai.rope.stoneX;
	ya = object.ai.rope.stoneY;

	// move the stone down one space and kill anything in it's path!
	for(y=STONE_HEIGHT;y>0;y--)
	{
		for(x=0;x<STONE_WIDTH;x++)
		{
			mp_Map->setTile(x+xa,y+ya,mp_Map->at(x+xa, y+ya-1), true);

			// if the stone hits any enemies, kill them
			kill_all_intersecting_tile(x+xa, y+ya);
		}
	}

	// clear the space at the top
	for(x=0;x<STONE_WIDTH;x++)
		mp_Map->setTile(x+xa,ya,object.ai.rope.bgtile, true);

	object.ai.rope.stoneY++;
}

