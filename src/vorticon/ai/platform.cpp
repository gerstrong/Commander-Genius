#include "../spritedefines.h"

#include "CObjectAI.h"

#define PLATFORM_MOVE   0
#define PLATFORM_WAIT   1

#define PLATFORM_WAITTIME       50
#define PLATFORM_MOVE_SPD       12

#define PLATFORM_ANIM_RATE      15

#define PLATFORMPUSHAMOUNT      40


void CObjectAI::platform_ai(CObject &object)
{
	if (object.needinit)
	{  // first time initilization
		object.ai.platform.animframe = 0;
		object.ai.platform.animtimer = 0;
		object.ai.platform.movedir = RIGHT;
		object.ai.platform.state = PLATFORM_MOVE;

		object.blockedl = object.blockedr = true;
		object.inhibitfall = 1;
		object.needinit = 0;
		object.canbezapped = 1;
		SetAllCanSupportPlayer(object, 1);
	}

	// push player horizontally
	if (object.touchPlayer && !m_Player[object.touchedBy].pdie && m_Player[object.touchedBy].psupportingobject!=object.m_index)
	{
		if (m_Player[object.touchedBy].getXPosition() < object.getXPosition())
		{
			m_Player[object.touchedBy].playpushed_x = -PLATFORMPUSHAMOUNT;
			if (m_Player[object.touchedBy].pinertia_x > 0) m_Player[object.touchedBy].pinertia_x = 0;
			m_Player[object.touchedBy].playpushed_decreasetimer = 0;
		}
		else
		{
			m_Player[object.touchedBy].playpushed_x = PLATFORMPUSHAMOUNT;
			if (m_Player[object.touchedBy].pinertia_x < 0) m_Player[object.touchedBy].pinertia_x = 0;
			m_Player[object.touchedBy].playpushed_decreasetimer = 0;
		}
	}

	if (m_Episode==2)
	{
		object.sprite = OBJ_PLATFORM_DEFSPRITE_EP2 + object.ai.platform.animframe;
	}
	else
	{
		object.sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + object.ai.platform.animframe;
	}

	if (object.ai.platform.animtimer > PLATFORM_ANIM_RATE)
	{
		object.ai.platform.animframe ^= 1;
		object.ai.platform.animtimer = 0;
	}
	else object.ai.platform.animtimer++;

	switch(object.ai.platform.state)
	{
	case PLATFORM_MOVE:

		if (object.ai.platform.movedir==RIGHT)
		{
			if (object.blockedr)
			{
				object.ai.platform.movedir = LEFT;
				object.ai.platform.waittimer = 0;
				object.ai.platform.state = PLATFORM_WAIT;
			}
			else
			{
				object.moveRight(PLATFORM_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if(it_player->psupportingobject==object.m_index &&
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
		else if (object.ai.platform.movedir==LEFT)
		{
			if (object.blockedl)
			{
				object.ai.platform.movedir = RIGHT;
				object.ai.platform.waittimer = 0;
				object.ai.platform.state = PLATFORM_WAIT;
			}
			else
			{
				object.moveLeft(PLATFORM_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if(it_player->psupportingobject==object.m_index &&
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
		if (object.ai.platform.waittimer > PLATFORM_WAITTIME)
		{
			object.ai.platform.state = PLATFORM_MOVE;
		}
		else object.ai.platform.waittimer++;
		break;
	}
}
