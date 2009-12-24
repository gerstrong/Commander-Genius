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
	unsigned int i,p;

	if (object.needinit)
	{  // first time initilization
		object.ai.platform.animframe = 0;
		object.ai.platform.animtimer = 0;
		object.ai.platform.movedir = UP;
		object.ai.platform.state = PLATVERT_MOVE;

		object.inhibitfall = 1;
		object.needinit = 0;
		object.canbezapped = 1;
		SetAllCanSupportPlayer(object, 1);
		for(i=0;i<m_NumPlayers;i++)
		{
			object.ai.platform.kickedplayer[i] = 0;
		}
	}

	// after kicking a player, wait until he falls beneath the platform
	// before turning cansupportplayer back on...just in case we also
	// check for if he stopped falling
	for(p=0;p<m_NumPlayers;p++)
	{
		if (object.ai.platform.kickedplayer[p])
		{
			if (m_Player[p].y > object.y || (!m_Player[p].pfalling && !m_Player[p].pjumping))
			{
				object.cansupportplayer[p] = 1;
				object.ai.platform.kickedplayer[p] = 0;
			}
		}
	}

	// push player horizontally
	if (object.touchPlayer && !m_Player[object.touchedBy].pdie && m_Player[object.touchedBy].psupportingobject!=object.m_index)
	{
		if (object.cansupportplayer[object.touchedBy])
		{
			// if player is standing around minding his own business and we
			// come down on his head, change direction. if player is trying
			// to walk/jump into us horizontally, push him away.
			// push him away
			if (m_Player[object.touchedBy].x < object.x)
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
				object.y -= PLATVERT_MOVE_SPD;
				for(i=0;i<m_NumPlayers;i++)
				{
					if(m_Player[i].psupportingobject==object.m_index && (m_Player[i].pjumping==PNOJUMP||m_Player[i].pjumping==PPREPAREJUMP||m_Player[i].pjumping==PPREPAREPOGO))
					{
						if (!object.ai.platform.kickedplayer[i])
						{
							m_Player[i].goto_y -= PLATVERT_MOVE_SPD;
						}
						// kick player off if we're running him into the ceiling
						if (m_Player[i].blockedu)
						{
							object.cansupportplayer[i] = 0;
							object.ai.platform.kickedplayer[i] = 1;
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
				object.y += PLATVERT_MOVE_SPD;
				for(i=0;i<m_NumPlayers;i++)
				{
					if(m_Player[i].psupportingobject==object.m_index && (m_Player[i].pjumping==PNOJUMP||m_Player[i].pjumping==PPREPAREJUMP||m_Player[i].pjumping==PPREPAREPOGO))
					{
						if (!object.ai.platform.kickedplayer[i])
							m_Player[i].goto_y += PLATVERT_MOVE_SPD;
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
