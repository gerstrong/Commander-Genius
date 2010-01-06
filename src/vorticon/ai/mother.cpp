#include "../../sdl/sound/CSound.h"
#include "../../misc.h"

#include "CObjectAI.h"

// Vorticon Mother AI (ep3)

enum vortimom_actions{
MOTHER_WALK, MOTHER_SPIT,
MOTHER_HURT, MOTHER_DEAD
};

#define MOTHER_WALK_ANIM_RATE     3
#define MOTHER_WALK_SPD           12

#define MOTHER_SPIT_PROB          10
#define MOTHER_SPIT_PROB_HARD     40
#define MOTHER_SPIT_SHOW_TIME     25

#define MOTHER_HP      5

#define MOTHER_HURT_SHOW_TIME    25

#define SNDWAVE_LEFT_FRAME    128

#define MOTHER_WALK_LEFT_FRAME	 85
#define MOTHER_WALK_RIGHT_FRAME	 87
#define MOTHER_SPIT_LEFT_FRAME	 89
#define MOTHER_SPIT_RIGHT_FRAME	 90
#define MOTHER_HURT_FRAME        91
#define MOTHER_DEAD_FRAME	 92

void CObjectAI::mother_ai( CObject& object, bool hardmode )
{
	int prob;

	if (object.needinit)
	{
		object.ai.mother.state = MOTHER_WALK;
		object.ai.mother.hittimes = 0;
		object.ai.mother.animframe = 0;
		object.ai.mother.animtimer = 0;

		if (m_Player[0].getXPosition() > object.getXPosition())
			object.ai.mother.dir = RIGHT;
		else
			object.ai.mother.dir = LEFT;

		object.blockedr = object.blockedl = 0;
		object.canbezapped = 1;
		object.needinit = 0;
	}
	if (object.ai.mother.state==MOTHER_DEAD) return;

	if (object.touchPlayer && !m_Player[object.touchedBy].pdie)
	{
		// don't push the player as he's walking through the exit door
		if (!m_Player[object.touchedBy].level_done)
		{
			if (m_Player[object.touchedBy].getXPosition() < object.getXPosition())
				m_Player[object.touchedBy].bump(-MOTHER_WALK_SPD, true);
			else
				m_Player[object.touchedBy].bump(MOTHER_WALK_SPD, true);
		}
	}


	if (object.zapped)
	{
		object.zapped = 0;
		object.ai.mother.hittimes++;
		if (object.ai.mother.state != MOTHER_HURT ||
				object.ai.mother.hittimes < MOTHER_HP)
		{
			object.ai.mother.timer = 0;
			object.ai.mother.state = MOTHER_HURT;
		}
	}

	switch(object.ai.mother.state)
	{
	case MOTHER_WALK:

		prob = hardmode ? MOTHER_SPIT_PROB_HARD : MOTHER_SPIT_PROB;
		if (getProbability(prob))
		{
			if (object.onscreen)
			{
				object.ai.mother.dir ^= 1;		// turn around before spitting

				object.ai.mother.state = MOTHER_SPIT;
				object.ai.mother.timer = 0;
			}
		}

		if (object.ai.mother.dir==RIGHT)
		{
			object.sprite = MOTHER_WALK_RIGHT_FRAME + object.ai.mother.animframe;

			if (object.blockedr)
			{
				object.ai.mother.dir = LEFT;
			}
			else
			{
				object.moveRight(MOTHER_WALK_SPD);
			}
		}
		else
		{
			object.sprite = MOTHER_WALK_LEFT_FRAME + object.ai.mother.animframe;

			if (object.blockedl)
				object.ai.mother.dir = RIGHT;
			else
				object.moveLeft(MOTHER_WALK_SPD);
		}

		// walk animation
		if (object.ai.mother.animtimer > MOTHER_WALK_ANIM_RATE)
		{
			object.ai.mother.animframe ^= 1;
			object.ai.mother.animtimer = 0;
		}
		else object.ai.mother.animtimer++;

		break;

	case MOTHER_SPIT:
		object.sprite = (object.ai.mother.dir==RIGHT) ?
				MOTHER_SPIT_RIGHT_FRAME:MOTHER_SPIT_LEFT_FRAME;


		if (object.ai.mother.timer > MOTHER_SPIT_SHOW_TIME)
		{
			CObject newobject(mp_Map);
			if (object.ai.mother.dir==RIGHT)
			{
				newobject.spawn(object.getXRightPos()+1, object.getYPosition()+(11<<STC), OBJ_FIREBALL, 3);
				newobject.ai.ray.direction = RIGHT;
			}
			else
			{
				newobject.spawn(object.getXLeftPos()-1, object.getYPosition()+(11<<STC), OBJ_FIREBALL, 3, LEFT);
				newobject.ai.ray.direction = LEFT;
			}
			newobject.ai.ray.owner = object.m_index;
			m_Objvect.push_back(newobject);

			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, object.scrx);
			object.ai.mother.state = MOTHER_WALK;
		}
		else object.ai.mother.timer++;
		break;
	case MOTHER_HURT:
		object.sprite = MOTHER_HURT_FRAME;
		if (object.ai.mother.timer > MOTHER_HURT_SHOW_TIME)
		{
			if (object.ai.mother.hittimes >= MOTHER_HP)
			{
				object.sprite = MOTHER_DEAD_FRAME;
				object.ai.mother.state = MOTHER_DEAD;
				object.canbezapped = 0;
				object.ai.mother.timer = 0;
				if (object.onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, object.scrx);
			}
			else
			{
				object.ai.mother.state = MOTHER_WALK;
				if (m_Player[0].getXPosition() > object.getXPosition())
					object.ai.mother.dir = RIGHT;
				else
					object.ai.mother.dir = LEFT;
			}
		}
		else object.ai.mother.timer++;
		break;
	}
}
