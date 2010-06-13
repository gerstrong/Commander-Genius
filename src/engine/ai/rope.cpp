#include "CObjectAI.h"
#include "../../graphics/effects/CVibrate.h"
#include "../spritedefines.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "vort.h"

// The rope holding the stone which kills the final Vorticon (ep1)

enum ropestates{
ROPE_IDLE, ROPE_DROPSTONE
};

#define STONE_WIDTH          9
#define STONE_HEIGHT         2

#define STONE_DROP_RATE      6

void CObjectAI::rope_ai(CObject &object)
{
	int x;
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
			int x, y;
			// rope got broke! time to drop the stone
			object.ai.rope.state = ROPE_DROPSTONE;
			object.ai.rope.droptimer = 0;
			// hide the rope
			object.sprite = BLANKSPRITE;
			// get upper left corner of the stone
			x = object.getXPosition()>>CSF;
			y = object.getYPosition()>>CSF;
			object.ai.rope.stoneX = x - 4;
			object.ai.rope.stoneY = y + 1;
			// get color of background
			object.ai.rope.bgtile = mp_Map->at(x, y);
		}
		break;

	case ROPE_DROPSTONE:
		if (!object.ai.rope.droptimer)
		{
			object.ai.rope.droptimer = STONE_DROP_RATE;
			rope_movestone(object);
			g_pGfxEngine->pushEffectPtr(new CVibrate(400));

			std::vector<CPlayer>::iterator it_player = m_Player.begin();
			for(; it_player!=m_Player.end() ; it_player++)
				it_player->blockedd=false;

			// check if we've hit the ground yet
			for(x=2;x<STONE_WIDTH-2;x++)
			{
				if (g_pBehaviorEngine->getTileProperties().at(mp_Map->at(object.ai.rope.stoneX+x, object.ai.rope.stoneY+2)).bup)
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
			mp_Map->setTile(x+xa,y+ya, mp_Map->at(x+xa, y+ya-1), true);

			// if the stone hits any enemies, kill them
			kill_all_intersecting_tile(x+xa, y+ya);
		}
	}

	// clear the space at the top
	for(x=0;x<STONE_WIDTH;x++)
		mp_Map->setTile(x+xa,ya,object.ai.rope.bgtile, true);

	object.ai.rope.stoneY++;
}

