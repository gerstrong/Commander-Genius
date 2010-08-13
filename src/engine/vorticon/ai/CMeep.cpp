#include "../../../sdl/sound/CSound.h"
#include "../../../misc.h"
#include "CMeep.h"
#include "CSoundWave.h"

enum meep_actions{
	MEEP_WALK, MEEP_SING,
	MEEP_DYING
};

#define MEEP_WALK_ANIM_RATE     3
#define MEEP_WALK_SPD           20

#define MEEP_SING_PROB          5
#define MEEP_SING_SHOW_TIME     10

#define MEEP_DYING_SHOW_TIME    6

#define MEEP_WALK_RIGHT_FRAME   118
#define MEEP_WALK_LEFT_FRAME    120
#define MEEP_SING_RIGHT_FRAME   122
#define MEEP_SING_LEFT_FRAME    123
#define MEEP_DYING_FRAME        124
#define MEEP_DEAD_FRAME         125

CMeep::CMeep(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player,
		std::vector<CObject*>& Object) :
CObject(p_map,x,y, OBJ_MEEP),
m_Player(Player),
m_Object(Object)
{
	canbezapped = true;

	state = MEEP_WALK;

	if (m_Player[0].getXPosition() > getXPosition())
		dir = RIGHT;
	else
		dir = LEFT;

	blockedr = blockedl = false;
	animframe = 0;
	animtimer = 0;
}

void CMeep::process()
{
	int not_about_to_fall;

	if (touchPlayer && !m_Player[touchedBy].pdie)
	{
		// don't push the player as he's walking through the exit door
		if (!m_Player[touchedBy].level_done)
		{
			if (m_Player[touchedBy].getXPosition() < getXPosition())
				m_Player[touchedBy].push(*this);
			else
				m_Player[touchedBy].push(*this);
		}
	}

	if (HealthPoints <= 0 && state != MEEP_DYING )
	{
		timer = 0;
		state = MEEP_DYING;
		dying = true;
		g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
	}

	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();

	switch(state)
	{
	case MEEP_WALK:
		if (getProbability(MEEP_SING_PROB))
		{
			if (onscreen)
			{
				state = MEEP_SING;
				timer = 0;
			}
			else
			{
				// try to get onscreen by heading towards the player
				if (m_Player[0].getXPosition() > getXPosition())
					dir = RIGHT;
				else
					dir = LEFT;
			}
		}

		if (dir==RIGHT)
		{
			sprite = MEEP_WALK_RIGHT_FRAME + animframe;

			not_about_to_fall = TileProperties.at(mp_Map->at((getXLeftPos())>>CSF, (getYDownPos()+(1<<STC))>>CSF)).bup;

			if (blockedr || !not_about_to_fall)
				dir = LEFT;
			else
				xinertia = MEEP_WALK_SPD;
		}
		else
		{
			sprite = MEEP_WALK_LEFT_FRAME + animframe;
			not_about_to_fall = TileProperties.at(mp_Map->at((getXRightPos())>>CSF, (getYDownPos()+(1<<STC))>>CSF)).bup;

			if (blockedl || !not_about_to_fall)
				dir = RIGHT;
			else
				xinertia = -MEEP_WALK_SPD;
		}

		// walk animation
		if (animtimer > MEEP_WALK_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;

		break;
	case MEEP_SING:
		if (dir==RIGHT)
		{
			sprite = MEEP_SING_RIGHT_FRAME;
		}
		else
		{
			sprite = MEEP_SING_LEFT_FRAME;
		}

		if (timer > MEEP_SING_SHOW_TIME)
		{
			CSoundWave *newobject;
			if (dir==RIGHT)
				newobject = new CSoundWave(mp_Map, getXRightPos(), getYPosition()+(5<<STC), RIGHT);
			else
				newobject = new CSoundWave(mp_Map, getXLeftPos(), getYPosition()+(5<<STC), LEFT);
			newobject->setOwner(OBJ_MEEP, m_index);
			newobject->solid = false;
			m_Object.push_back(newobject);
			g_pSound->playStereofromCoord(SOUND_MEEP, PLAY_NOW, scrx);
			state = MEEP_WALK;
		}
		else timer++;
		break;
	case MEEP_DYING:
		sprite = MEEP_DYING_FRAME;
		if (timer > MEEP_DYING_SHOW_TIME)
		{
			sprite = MEEP_DEAD_FRAME;
			dead = true;
			timer = 0;
		}
		else timer++;
		break;
	}
}
