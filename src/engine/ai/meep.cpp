#include "../../sdl/sound/CSound.h"
#include "../../misc.h"
#include "CObjectAI.h"

enum meep_actions{
	MEEP_WALK, MEEP_SING,
	MEEP_DYING, MEEP_DEAD
};

#define MEEP_WALK_ANIM_RATE     8
#define MEEP_WALK_SPD           26

#define MEEP_SING_PROB          5
#define MEEP_SING_SHOW_TIME     10

#define MEEP_DYING_SHOW_TIME    6

#define MEEP_WALK_RIGHT_FRAME   118
#define MEEP_WALK_LEFT_FRAME    120
#define MEEP_SING_RIGHT_FRAME   122
#define MEEP_SING_LEFT_FRAME    123
#define MEEP_DYING_FRAME        124
#define MEEP_DEAD_FRAME         125

void CObjectAI::meep_ai(CObject& object)
{
	int not_about_to_fall;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	if (object.needinit)
	{
		object.ai.meep.state = MEEP_WALK;
		object.ai.meep.animframe = 0;
		object.ai.meep.animtimer = 0;

		if (m_Player[0].getXPosition() > object.getXPosition())
			object.ai.meep.dir = RIGHT;
		else
			object.ai.meep.dir = LEFT;

		object.blockedr = object.blockedl = false;
		object.canbezapped = 1;
		object.needinit = 0;
	}
	if (object.ai.meep.state==MEEP_DEAD) return;

	if (object.touchPlayer && !m_Player[object.touchedBy].pdie)
	{
		// don't push the player as he's walking through the exit door
		if (!m_Player[object.touchedBy].level_done)
		{
			if (m_Player[object.touchedBy].getXPosition() < object.getXPosition())
				m_Player[object.touchedBy].bump(-MEEP_WALK_SPD, true);
			else
				m_Player[object.touchedBy].bump(MEEP_WALK_SPD, true);
		}
	}

	if (object.zapped)
	{
		object.zapped = 0;
		object.canbezapped = 0;
		object.ai.meep.timer = 0;
		object.ai.meep.state = MEEP_DYING;
		g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
	}

	switch(object.ai.meep.state)
	{
	case MEEP_WALK:
		if (getProbability(MEEP_SING_PROB))
		{
			if (object.onscreen)
			{
				object.ai.meep.state = MEEP_SING;
				object.ai.meep.timer = 0;
			}
			else
			{
				// try to get onscreen by heading towards the player
				if (m_Player[0].getXPosition() > object.getXPosition())
					object.ai.meep.dir = RIGHT;
				else
					object.ai.meep.dir = LEFT;
			}
		}

		if (object.ai.meep.dir==RIGHT)
		{
			object.sprite = MEEP_WALK_RIGHT_FRAME + object.ai.meep.animframe;

			not_about_to_fall = TileProperty[mp_Map->at((object.getXLeftPos())>>CSF, (object.getYDownPos()+(1<<STC))>>CSF)].bup;

			if (object.blockedr || !not_about_to_fall)
				object.ai.meep.dir = LEFT;
			else
				object.moveRight(MEEP_WALK_SPD);
		}
		else
		{
			object.sprite = MEEP_WALK_LEFT_FRAME + object.ai.meep.animframe;
			not_about_to_fall = TileProperty[mp_Map->at((object.getXRightPos())>>CSF, (object.getYDownPos()+(1<<STC))>>CSF)].bup;

			if (object.blockedl || !not_about_to_fall)
			{
				object.ai.meep.dir = RIGHT;
			}
			else
			{
				object.moveLeft(MEEP_WALK_SPD);
			}
		}

		// walk animation
		if (object.ai.meep.animtimer > MEEP_WALK_ANIM_RATE)
		{
			object.ai.meep.animframe ^= 1;
			object.ai.meep.animtimer = 0;
		}
		else object.ai.meep.animtimer++;

		break;
	case MEEP_SING:
		if (object.ai.meep.dir==RIGHT)
		{
			object.sprite = MEEP_SING_RIGHT_FRAME;
		}
		else
		{
			object.sprite = MEEP_SING_LEFT_FRAME;
		}

		if (object.ai.meep.timer > MEEP_SING_SHOW_TIME)
		{
			CObject newobject(mp_Map);
			if (object.ai.meep.dir==RIGHT)
			{
				newobject.spawn(object.getXRightPos(), object.getYPosition()+(5<<STC), OBJ_SNDWAVE, 3);
				newobject.ai.ray.direction = RIGHT;
			}
			else
			{
				newobject.spawn(object.getXLeftPos(), object.getYPosition()+(5<<STC), OBJ_SNDWAVE, 3, LEFT);
				newobject.ai.ray.direction = LEFT;
			}
			newobject.solid = false;
			m_Objvect.push_back(newobject);
			g_pSound->playStereofromCoord(SOUND_MEEP, PLAY_NOW, object.scrx);
			object.ai.meep.state = MEEP_WALK;
		}
		else object.ai.meep.timer++;
		break;
	case MEEP_DYING:
		object.sprite = MEEP_DYING_FRAME;
		if (object.ai.meep.timer > MEEP_DYING_SHOW_TIME)
		{
			object.sprite = MEEP_DEAD_FRAME;
			object.ai.meep.state = MEEP_DEAD;
			object.ai.meep.timer = 0;
		}
		else object.ai.meep.timer++;
		break;
	}
}
