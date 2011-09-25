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

CPlatformVert::CPlatformVert(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player) :
CPlatform(p_map, x, y, Player)
{
	animframe = 0;
	animtimer = 0;
	movedir = UP;
	state = PLATVERT_MOVE;

	blockedu = false;
	blockedd = true;
	inhibitfall = 1;
	needinit = 0;
	canbezapped = 1;
	SetAllCanSupportPlayer(1);

	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		kickedplayer[it_player->m_index] = 0;
	}
}

void CPlatformVert::process()
{
	// after kicking a player, wait until he falls beneath the platform
	// before turning cansupportplayer back on...just in case we also
	// check for if he stopped falling
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if (kickedplayer[it_player->m_index])
		{
			if (it_player->getYPosition() > getYPosition() ||
					(!it_player->pfalling && !it_player->pjumping))
			{
				cansupportplayer = 1;
				kickedplayer[it_player->m_index] = 0;
			}
		}
	}

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

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->supportedbyobject && it_player->psupportingobject==m_index &&
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
				}
			}
		}
		else if (movedir==DOWN)
		{
			if (blockedd)
				movedir = UP;
			else
			{
				moveDown(PLATVERT_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->supportedbyobject && it_player->psupportingobject==m_index &&
							(it_player->pjumping==PNOJUMP||it_player->pjumping==PPREPAREJUMP||it_player->pjumping==PPREPAREPOGO))
					{
						if (!kickedplayer[it_player->m_index])
							it_player->moveDown(PLATVERT_MOVE_SPD);
					}
				}
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

void CPlatformVert::getTouchedBy(CObject &theObject)
{
	// push player horizontally
	if( theObject.m_type == OBJ_PLAYER )
	{
		CPlayer &Player = dynamic_cast<CPlayer&>(theObject);
		if(Player.pfalling or !Player.supportedbyobject)
			Player.push(*this);
		else if( state == PLATVERT_MOVE )
			Player.moveYDir( (movedir==UP) ? -PLATVERT_MOVE_SPD : PLATVERT_MOVE_SPD);
	}
}
