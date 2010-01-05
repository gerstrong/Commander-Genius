#include "CObjectAI.h"
#include "../spritedefines.h"
// vertically-moving platform (ep3)

enum platver_actions{
PLATVERT_MOVE, PLATVERT_WAIT
};

#define PLATVERT_WAITTIME       35
#define PLATVERT_MOVE_SPD       16

#define PLATVERT_ANIM_RATE      15

#define PLATVERTPUSHAMOUNT      10

void CObjectAI::platvert_ai(CObject& object)
{
	unsigned int p;
	int i;

	if (object.needinit)
	{  // first time initilization
		object.ai.platform.animframe = 0;
		object.ai.platform.animtimer = 0;
		object.ai.platform.movedir = UP;
		object.ai.platform.state = PLATVERT_MOVE;

		object.blockedu = false;
		object.blockedd = true;
		object.inhibitfall = 1;
		object.needinit = 0;
		object.canbezapped = 1;
		SetAllCanSupportPlayer(object, 1);

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			object.ai.platform.kickedplayer[it_player->m_player_number] = 0;
		}
	}

	// after kicking a player, wait until he falls beneath the platform
	// before turning cansupportplayer back on...just in case we also
	// check for if he stopped falling
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if (object.ai.platform.kickedplayer[it_player->m_player_number])
		{
			if (it_player->getYPosition() > object.getYPosition() ||
					(!it_player->pfalling && !it_player->pjumping))
			{
				object.cansupportplayer = 1;
				object.ai.platform.kickedplayer[it_player->m_player_number] = 0;
			}
		}
	}

	// push player horizontally
	CPlayer &tPlayer = m_Player[object.touchedBy];
	if ( object.touchPlayer && !tPlayer.pdie && tPlayer.psupportingobject != object.m_index)
	{
		if (object.cansupportplayer && !tPlayer.supportedbyobject)
		{
			// if player is standing around minding his own business and we
			// come down on his head, change direction. if player is trying
			// to walk/jump into us horizontally, push him away.
			if( m_Player[object.touchedBy].getYDownPos() > object.getYDownPos() )
			{
				if (m_Player[object.touchedBy].getXPosition() < object.getXPosition())
				{
					m_Player[object.touchedBy].playpushed_x = -PLATVERTPUSHAMOUNT;
					if (m_Player[object.touchedBy].pinertia_x > 0) m_Player[object.touchedBy].pinertia_x = 0;
					m_Player[object.touchedBy].playpushed_decreasetimer = 0;
				}
				else
				{
					m_Player[object.touchedBy].playpushed_x = PLATVERTPUSHAMOUNT;
					if (m_Player[object.touchedBy].pinertia_x < 0) m_Player[object.touchedBy].pinertia_x = 0;
					m_Player[object.touchedBy].playpushed_decreasetimer = 0;
				}
			}
		}
	}

	object.sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + object.ai.platform.animframe;

	if (object.ai.platform.animtimer > PLATVERT_ANIM_RATE)
	{
		object.ai.platform.animframe ^= 1;
		object.ai.platform.animtimer = 0;
	}
	else object.ai.platform.animtimer++;

	switch(object.ai.platform.state)
	{
	case PLATVERT_MOVE:

		if (object.ai.platform.movedir==UP)
		{
			if (object.blockedu)
			{
				object.ai.platform.movedir = DOWN;
				object.ai.platform.waittimer = 0;
				object.ai.platform.state = PLATVERT_WAIT;
			}
			else
			{
				object.moveUp(PLATVERT_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->supportedbyobject && it_player->psupportingobject==object.m_index &&
							(it_player->pjumping==PNOJUMP||it_player->pjumping==PPREPAREJUMP||it_player->pjumping==PPREPAREPOGO))
					{
						if (!object.ai.platform.kickedplayer[it_player->m_player_number])
						{
							it_player->moveUp(PLATVERT_MOVE_SPD);
						}
						// kick player off if we're running him into the ceiling
						if (it_player->blockedu)
						{
							object.cansupportplayer = 0;
							object.ai.platform.kickedplayer[it_player->m_player_number] = 1;
						}
					}
				}
			}
		}
		else if (object.ai.platform.movedir==DOWN)
		{
			if (object.blockedd)
				object.ai.platform.movedir = UP;
			else
			{
				object.moveDown(PLATVERT_MOVE_SPD);

				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if( it_player->supportedbyobject && it_player->psupportingobject==object.m_index &&
							(it_player->pjumping==PNOJUMP||it_player->pjumping==PPREPAREJUMP||it_player->pjumping==PPREPAREPOGO))
					{
						if (!object.ai.platform.kickedplayer[it_player->m_player_number])
							it_player->moveDown(PLATVERT_MOVE_SPD);
					}
				}
			}
		}
		break;
	case PLATVERT_WAIT:
		if (object.ai.platform.waittimer > PLATVERT_WAITTIME)
			object.ai.platform.state = PLATVERT_MOVE;

		else
			object.ai.platform.waittimer++;
		break;
	}
}
