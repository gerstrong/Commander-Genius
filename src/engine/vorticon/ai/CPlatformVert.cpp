#include "CPlatform.h"
#include "../../spritedefines.h"
// vertically-moving platform (ep3)

enum platver_actions{
PLATVERT_MOVE, PLATVERT_WAIT
};

#define PLATVERT_WAITTIME       35
#define PLATVERT_MOVE_SPD       16

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

	// push player horizontally
	CPlayer &tPlayer = m_Player[touchedBy];
	if ( touchPlayer && !tPlayer.pdie && tPlayer.psupportingobject != m_index)
	{
		if (cansupportplayer && !tPlayer.supportedbyobject)
		{
			// if player is standing around minding his own business and we
			// come down on his head, change direction. if player is trying
			// to walk/jump into us horizontally, push him away.
			if( m_Player[touchedBy].getYDownPos() > getYDownPos() )
			{
				if (m_Player[touchedBy].getXPosition() < getXPosition())
				{
					m_Player[touchedBy].playpushed_x = -PLATVERTPUSHAMOUNT;
					if (m_Player[touchedBy].xinertia > 0) m_Player[touchedBy].xinertia = 0;
					m_Player[touchedBy].playpushed_decreasetimer = 0;
				}
				else
				{
					m_Player[touchedBy].playpushed_x = PLATVERTPUSHAMOUNT;
					if (m_Player[touchedBy].xinertia < 0) m_Player[touchedBy].xinertia = 0;
					m_Player[touchedBy].playpushed_decreasetimer = 0;
				}
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
