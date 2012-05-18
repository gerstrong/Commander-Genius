#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"
#include "engine/spritedefines.h"

#include "CGuardRobot.h"
#include "CRay.h"

const int PREPAREFIRE_TIME = 6;
// frames
const int WALK_LEFT_FRAME = 116;
const int WALK_RIGHT_FRAME = 112;
const int LOOK_FRAME = 120;

const int PREPAREFIRE_TIME_FAST = 22;
const int WAITAFTER_FIRE = 14;

const int TRAVELDIST = 100;

const int SHOTS_PER_VOLLEY = 4;
const int MIN_TIME_TILL_CAN_FIRE = 31;
const int MAX_TIME_TILL_CAN_FIRE = 200;
const int TIME_BETWEEN_SHOTS = 12;
const int TIME_BEFORE_FIRE_WHEN_SEE = 6;
const int TIME_BETWEEN_FIRE_CAUSE_LEVEL = 25;

const int WALK_SPEED = 31;
const unsigned int WALK_ANIM_TIME = 6;
const unsigned int LOOK_ANIM_TIME = 4;
const unsigned int LOOK_TOTALTIME = 25;

const int FIRE_PAUSE_TIME = 25;

CGuardRobot::CGuardRobot(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CObject*>& Object) :
CObject(p_map, x, y, OBJ_GUARDROBOT),
m_ObjectVect(Object)
{
	// First time initialization
	state = WALK;
	movedir = RIGHT;
	fireafterlook = 0;
	animtimer = 0;
	timer = 0;
	dist_to_travel = TRAVELDIST;
	pausetime = 0;
	timetillcanfire = MAX_TIME_TILL_CAN_FIRE;
	firetimes = 0;
	turnaroundtimer = 0;

	canbezapped = true;   // will stop bullets but is not harmed
	m_invincible = true;
	inhibitfall = true;
}

void CGuardRobot::process()
{
	switch(state)
	{
	case LOOK:
		// animation
		if (animtimer > LOOK_ANIM_TIME)
		{
			frame ^= 1;
			animtimer = 0;
		}
		else
			animtimer++;

		sprite = LOOK_FRAME + frame;

		// when time is up go back to moving
		if (timer > LOOK_TOTALTIME)
		{
			timetillcanfire = (rnd()%(MAX_TIME_TILL_CAN_FIRE-MIN_TIME_TILL_CAN_FIRE))+MIN_TIME_TILL_CAN_FIRE;
			timetillcanfirecauseonsamelevel = TIME_BEFORE_FIRE_WHEN_SEE;
			firetimes = 0;
			state = WALK;
			frame = 0;
			animtimer = 0;
			timer = 0;
			dist_to_travel = TRAVELDIST;
		}
		else
			timer++;

		break;

	case WALK:
		// hover animation
		if (animtimer > WALK_ANIM_TIME)
		{
			if (frame>=3) frame=0;
			else frame++;
			animtimer = 0;
		} else animtimer++;

		if (movedir==LEFT)
			sprite = WALK_LEFT_FRAME + frame;
		else
			sprite = WALK_RIGHT_FRAME + frame;

		// if we're about to, or just did, fire a volley, don't move
		if (!hardmode)
		{
			if (pausetime)
			{
				pausetime--;
				return;
			}
		}
		else
			pausetime = 0;

		// are we firing a volley?
		if (firetimes)
		{
			// is it time to fire the next shot in the volley?
			if (!timetillnextshot)
			{
				CRay *newobject;
				if (onscreen)
					playSound(SOUND_TANK_FIRE);
				if (movedir==RIGHT)
					newobject = new CRay(mp_Map,getXRightPos()+(8<<STC), getYUpPos()+(5<<STC), RIGHT);
				else
					newobject = new CRay(mp_Map,getXPosition(), getYUpPos()+(5<<STC), LEFT);
				newobject->setOwner(OBJ_GUARDROBOT, m_index);
				newobject->sprite = ENEMYRAYEP2;

				m_ObjectVect.push_back(newobject);

				timetillnextshot = TIME_BETWEEN_SHOTS;
				if (!--firetimes)
				{
					pausetime = FIRE_PAUSE_TIME;
				}
			}
			else
			{
				timetillnextshot--;
			}

			// don't move when firing except on hard mode
			if (hardmode)
				return;

		}
		else
		{  // not firing a volley
			if (!timetillcanfire)
			{
				guard_fire();
			}
			else
			{
				timetillcanfire--;
			}

		}

		turnaroundtimer = 0;

		if (movedir==LEFT)
		{  // move left
			if (!blockedl)
			{
				xinertia = -WALK_SPEED;
				dist_to_travel--;
			}
			else
			{
				frame = 0;
				timer = 0;
				animtimer = 0;
				state = LOOK;
				movedir = RIGHT;
			}
		}
		else
		{  // move right
			sprite = WALK_RIGHT_FRAME + frame;
			if (!blockedr)
			{
				xinertia = WALK_SPEED;
				dist_to_travel--;
			}
			else
			{
				frame = 0;
				timer = 0;
				animtimer = 0;
				state = LOOK;
				movedir = LEFT;
			}
		}
		break;
	default : break;
	}
}

// makes the tank start firing
void CGuardRobot::guard_fire()
{
	firetimes = SHOTS_PER_VOLLEY;
	timetillnextshot = 0;
	timetillcanfire = (rnd()%(MAX_TIME_TILL_CAN_FIRE-MIN_TIME_TILL_CAN_FIRE))+MIN_TIME_TILL_CAN_FIRE;
	pausetime = FIRE_PAUSE_TIME;
}

void CGuardRobot::getTouchedBy(CObject &theObject)
{
	if (theObject.m_type == OBJ_PLAYER && g_pBehaviorEngine->mDifficulty>=NORMAL)
		theObject.kill(); // Only on normal and hard mode when keen touches it, he can die
}
