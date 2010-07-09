#include "../../spritedefines.h"

#include "CPlatform.h"

#define PLATFORM_MOVE   0
#define PLATFORM_WAIT   1

#define PLATFORM_WAITTIME       50
#define PLATFORM_MOVE_SPD       12

#define PLATFORM_ANIM_RATE      15

#define PLATFORMPUSHAMOUNT      40

CPlatform::CPlatform(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player) :
CObject(p_map, x, y, OBJ_PLATFORM),
m_Player(Player)
{
	animframe = 0;
	animtimer = 0;
	movedir = RIGHT;
	state = PLATFORM_MOVE;

	blockedl = blockedr = false;
	inhibitfall = 1;
	needinit = 0;
	canbezapped = 1;
	SetAllCanSupportPlayer(1);
}

void CPlatform::process()
{
	// push player horizontally
	if (touchPlayer && !m_Player[touchedBy].pdie && m_Player[touchedBy].psupportingobject!=m_index)
	{
		if (m_Player[touchedBy].getXLeftPos() < getXLeftPos())
		{
			m_Player[touchedBy].playpushed_x = -PLATFORMPUSHAMOUNT;
			if (m_Player[touchedBy].xinertia > 0) m_Player[touchedBy].xinertia = 0;
			m_Player[touchedBy].playpushed_decreasetimer = 0;
		}
		else if (m_Player[touchedBy].getXRightPos() > getXRightPos())
		{
			m_Player[touchedBy].playpushed_x = PLATFORMPUSHAMOUNT;
			if (m_Player[touchedBy].xinertia < 0) m_Player[touchedBy].xinertia = 0;
			m_Player[touchedBy].playpushed_decreasetimer = 0;
		}
	}

	if (g_pBehaviorEngine->getEpisode()==2)
	{
		sprite = OBJ_PLATFORM_DEFSPRITE_EP2 + animframe;
	}
	else
	{
		sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + animframe;
	}

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
			if (blockedr)
			{
				movedir = LEFT;
				waittimer = 0;
				state = PLATFORM_WAIT;
			}
			else
			{
				moveRight(PLATFORM_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->supportedbyobject &&
							it_player->psupportingobject==m_index &&
							(it_player->pjumping==PNOJUMP||
							it_player->pjumping==PPREPAREJUMP||
							it_player->pjumping==PPREPAREPOGO) )
					{
						if (!it_player->blockedr)
							it_player->moveRight(PLATFORM_MOVE_SPD);
					}
				}
			}
		}
		else if (movedir==LEFT)
		{
			if (blockedl)
			{
				movedir = RIGHT;
				waittimer = 0;
				state = PLATFORM_WAIT;
			}
			else
			{
				moveLeft(PLATFORM_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->supportedbyobject &&
							it_player->psupportingobject==m_index &&
							(it_player->pjumping==PNOJUMP||
							 it_player->pjumping==PPREPAREJUMP||
							 it_player->pjumping==PPREPAREPOGO))
					{
						if (!it_player->blockedl)
							it_player->moveLeft(PLATFORM_MOVE_SPD);
					}
				}
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

void CPlatform::SetAllCanSupportPlayer(bool state)
{
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		cansupportplayer = state;
		if(!state && it_player->supportedbyobject)
		{
			it_player->pfalling=true;
			it_player->moveDown(1);
			it_player->blockedd=false;
		}
	}
}

