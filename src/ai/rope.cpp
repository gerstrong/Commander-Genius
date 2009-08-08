#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "vort.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// The rope holding the stone which kills the final Vorticon (ep1)
// (Oh shit, was that a spoiler? I'm sorry)

// ;)

#define ROPE_IDLE            0
#define ROPE_DROPSTONE       1

#define STONE_WIDTH          9
#define STONE_HEIGHT         2

#define STONE_DROP_RATE      100

void delete_object(int o);
void static rope_movestone(int o);
void kill_all_intersecting_tile(int mpx, int mpy);

void rope_ai(int o)
{
int x;
	if (objects[o].needinit)
	{  // first time initialization
		objects[o].ai.rope.state = ROPE_IDLE;
		objects[o].canbezapped = 1;
		objects[o].inhibitfall = 1;
		objects[o].sprite = OBJ_ROPE_DEFSPRITE;

		objects[o].needinit = 0;
	}

	switch(objects[o].ai.rope.state)
	{
		case ROPE_IDLE:
			if (objects[o].zapped)
			{
				// rope got broke! time to drop the stone
				objects[o].ai.rope.state = ROPE_DROPSTONE;
				objects[o].ai.rope.droptimer = 0;
				// hide the rope
				objects[o].sprite = BLANKSPRITE;
				// get upper left corner of the stone
				objects[o].ai.rope.stoneX = (objects[o].x >> CSF >> 4) - 4;
				objects[o].ai.rope.stoneY = (objects[o].y >> CSF >> 4) + 1;
				// get color of background
				objects[o].ai.rope.bgtile = getmaptileat(objects[o].x>>CSF, objects[o].y>>CSF);
			}
		break;

		case ROPE_DROPSTONE:
			if (!objects[o].ai.rope.droptimer)
			{
				objects[o].ai.rope.droptimer = STONE_DROP_RATE;
				rope_movestone(o);

				// check if we've hit the ground yet
				for(x=0;x<STONE_WIDTH;x++)
				{
					if (TileProperty[map.mapdata[objects[o].ai.rope.stoneX+x][objects[o].ai.rope.stoneY+2]][BUP])
					{
						delete_object(o);
						return;
					}
				}
			}
			else objects[o].ai.rope.droptimer--;
     break;
   }
}

void static rope_movestone(int o)
{
int xa,ya;
int x,y;

	xa = objects[o].ai.rope.stoneX;
	ya = objects[o].ai.rope.stoneY;

	// move the stone down one space and kill anything in it's path!
	for(y=STONE_HEIGHT;y>0;y--)
	{
		for(x=0;x<STONE_WIDTH;x++)
		{
			map_chgtile(x+xa,y+ya,map.mapdata[x+xa][y+ya-1]);

			// if the stone hits any enemies, kill them
			kill_all_intersecting_tile(x+xa, y+ya);
		}
	}

	// clear the space at the top
	for(x=0;x<STONE_WIDTH;x++)
		map_chgtile(x+xa,ya,objects[o].ai.rope.bgtile);

	objects[o].ai.rope.stoneY++;
}

