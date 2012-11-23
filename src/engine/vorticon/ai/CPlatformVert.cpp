#include "CPlatform.h"
#include "engine/spritedefines.h"
// vertically-moving platform (ep3)

enum platver_actions{
PLATVERT_MOVE, PLATVERT_WAIT
};

#define PLATVERT_WAITTIME       35
#define PLATVERT_MOVE_SPD       23

#define PLATVERT_ANIM_RATE      15

#define PLATVERTPUSHAMOUNT      10

CPlatformVert::CPlatformVert(CMap *p_map, Uint32 x, Uint32 y) :
CPlatform(p_map, x, y)
{
	animframe = 0;
	animtimer = 0;
	movedir = UP;
	state = PLATVERT_MOVE;

	blockedu = false;
	blockedd = true;
	inhibitfall = 1;
	canbezapped = 1;

	/*std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		kickedplayer[it_player->m_index] = 0;
	}*/
}

void CPlatformVert::process()
{
	sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + animframe;

	if (animtimer > PLATVERT_ANIM_RATE)
	{
		animframe ^= 1;
		animtimer = 0;
	}
	else animtimer++;

	switch(state)
	{
	case PLATVERT_MOVE:

		if (movedir==UP)
		{
			if (blockedu)
			{
				movedir = DOWN;
				waittimer = 0;
				state = PLATVERT_WAIT;
			}
			else
			{
				moveUp(PLATVERT_MOVE_SPD);

				/*std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->pSupportedbyobject && it_player->pSupportedbyobject==this &&
							(it_player->pjumping==PNOJUMP||it_player->pjumping==PPREPAREJUMP||it_player->pjumping==PPREPAREPOGO))
					{
						if (!kickedplayer[it_player->m_index])
						{
							it_player->moveUp(PLATVERT_MOVE_SPD);
						}
						// kick player off if we're running him into the ceiling
						if (it_player->blockedu)
						{
							cansupportplayer = 0;
							kickedplayer[it_player->m_index] = 1;
						}
					}
				}*/
			}
		}
		else if (movedir==DOWN)
		{
			if (blockedd)
				movedir = UP;
			else
			{
				moveDown(PLATVERT_MOVE_SPD);

				/*std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->pSupportedbyobject && it_player->pSupportedbyobject==this &&
							(it_player->pjumping==PNOJUMP||it_player->pjumping==PPREPAREJUMP||it_player->pjumping==PPREPAREPOGO))
					{
						if (!kickedplayer[it_player->m_index])
							it_player->moveDown(PLATVERT_MOVE_SPD);
					}
				}*/
			}
		}
		break;
	case PLATVERT_WAIT:
		if (waittimer > PLATVERT_WAITTIME)
			state = PLATVERT_MOVE;

		else
			waittimer++;
		break;
	}
}

void CPlatformVert::getTouchedBy(CSpriteObject &theObject)
{
	// push player horizontally
	if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
	{
		if(player->pfalling or !player->pSupportedbyobject)
			player->push(*this);
		else if( state == PLATVERT_MOVE )
			player->moveYDir( (movedir==UP) ? -PLATVERT_MOVE_SPD : PLATVERT_MOVE_SPD);
	}
}
