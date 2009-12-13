#include "../../sdl/sound/CSound.h"
#include "../../game.h"
#include "../spritedefines.h"

#include "CObjectAI.h"

// Tank Robot (Ep2)
#define TANK_LOOK       0
#define TANK_WALK       1

#define TANK2_SAME_LEVEL_TIME   150
#define TANK_REPEAT_FIRE_TIME  800

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.
#define TANK_LOOKFIRE_PROB    500
#define TANK_MINTRAVELDIST    200

#define TANK_WALK_SPEED         4
#define TANK_WALK_ANIM_TIME     60
#define TANK_LOOK_ANIM_TIME     70
#define TANK_LOOK_TOTALTIME     180
#define TANK2_PREPAREFIRE_TIME  80

// frames
#define TANK2_WALK_LEFT_FRAME       116
#define TANK2_WALK_RIGHT_FRAME      112
#define TANK2_LOOK_FRAME            120

#define TANKPUSHAMOUNT        16

#define TANK_FIRE_PAUSE_TIME		100

#define TANK2_SHOTS_PER_VOLLEY    4
#define TANK2_MIN_TIME_TILL_CAN_FIRE  500
#define TANK2_MAX_TIME_TILL_CAN_FIRE  800
#define TANK2_TIME_BETWEEN_SHOTS  50
#define TANK2_TIME_BEFORE_FIRE_WHEN_SEE      100
#define TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL  400

#define Sprite g_pGfxEngine->Sprite

// reference to ../misc.cpp
unsigned int rnd(void);

//void static tank_searchplayers(int o);
//void static tank2_fire(int o);


void CObjectAI::tankep2_ai(CObject &object, bool hardmode)
{
	if (object.needinit)
	{  // first time initilization
		object.ai.tank.state = TANK_WALK;
		object.ai.tank.movedir = RIGHT;
		object.ai.tank.fireafterlook = 0;
		object.ai.tank.animtimer = 0;
		object.ai.tank.timer = 0;
		object.ai.tank.dist_traveled = 0;
		object.ai.tank.pausetime = 0;
		object.ai.tank.timetillcanfire = TANK2_MAX_TIME_TILL_CAN_FIRE;
		object.ai.tank.firetimes = 0;
		object.ai.tank.detectedPlayer = 0;
		object.ai.tank.detectedPlayerIndex = m_Player[0].m_player_number;
		object.ai.tank.turnaroundtimer = 0;

		object.canbezapped = 1;   // will stop bullets but is not harmed
		object.inhibitfall = 1;
		object.needinit = 0;
	}
	// touched player?
	if (object.touchPlayer && !m_Player[object.touchedBy].pdie)
		killplayer(object.touchedBy);

	switch(object.ai.tank.state)
	{
	case TANK_LOOK:
		// animation
		if (object.ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
		{
			object.ai.tank.frame ^= 1;
			object.ai.tank.animtimer = 0;
		}
		else
			object.ai.tank.animtimer++;

		object.sprite = TANK2_LOOK_FRAME + object.ai.tank.frame;


		// when time is up go back to moving
		if (object.ai.tank.timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			if (object.blockedr)
			{ object.ai.tank.movedir = LEFT; }
			else if (object.blockedl)
			{ object.ai.tank.movedir = RIGHT; }
			else if (object.x > m_Player[0].x)
			{ object.ai.tank.movedir = LEFT; }
			else
			{ object.ai.tank.movedir = RIGHT; }

			object.ai.tank.alreadyfiredcauseonsamelevel = 0;
			object.ai.tank.timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
			object.ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BEFORE_FIRE_WHEN_SEE;
			object.ai.tank.firetimes = 0;
			object.ai.tank.state = TANK_WALK;
			object.ai.tank.frame = 0;
			object.ai.tank.animtimer = 0;
			object.ai.tank.timer = 0;
			object.ai.tank.dist_traveled = 0;
		}
		else
			object.ai.tank.timer++;

		break;

	case TANK_WALK:
		// hover animation
		if (object.ai.tank.animtimer > TANK_WALK_ANIM_TIME)
		{
			if (object.ai.tank.frame>=3) object.ai.tank.frame=0;
			else object.ai.tank.frame++;
			object.ai.tank.animtimer = 0;
		} else object.ai.tank.animtimer++;

		if (object.ai.tank.movedir==LEFT)
			object.sprite = TANK2_WALK_LEFT_FRAME + object.ai.tank.frame;
		else
			object.sprite = TANK2_WALK_RIGHT_FRAME + object.ai.tank.frame;

		// if we're about to, or just did, fire a volley, don't move
		if (!hardmode)
		{
			if (object.ai.tank.pausetime)
			{
				object.ai.tank.pausetime--;
				return;
			}
		}
		else
			object.ai.tank.pausetime = 0;

		// are we firing a volley?
		if (object.ai.tank.firetimes)
		{
			// is it time to fire the next shot in the volley?
			if (!object.ai.tank.timetillnextshot)
			{
				CObject newobject;
				if (object.onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, object.scrx);
				if (object.ai.tank.movedir==RIGHT)
				{
					newobject.spawn(object.x+(Sprite[TANK2_WALK_RIGHT_FRAME]->getWidth()<<CSF), object.y+(6<<CSF), OBJ_RAY);
					newobject.ai.ray.direction = RIGHT;
				}
				else
				{
					newobject.spawn(object.x-(Sprite[ENEMYRAYEP2]->getWidth()<<CSF), object.y+(6<<CSF), OBJ_RAY);
					newobject.ai.ray.direction = LEFT;
				}
				newobject.sprite = ENEMYRAYEP2;
				newobject.ai.ray.dontHitEnable = 0;

				m_Objvect.push_back(newobject);

				object.ai.tank.timetillnextshot = TANK2_TIME_BETWEEN_SHOTS;
				if (!--object.ai.tank.firetimes)
				{
					object.ai.tank.pausetime = TANK_FIRE_PAUSE_TIME;
				}
			}
			else
			{
				object.ai.tank.timetillnextshot--;
			}

			// don't move when firing except on hard mode
			if (hardmode)
				return;

		}
		else
		{  // not firing a volley
			if (!object.ai.tank.timetillcanfire)
			{
				tank2_fire(object);
			}
			else
			{
				object.ai.tank.timetillcanfire--;
			}

		}

		// is keen on same level?
		tank_searchplayers(object);

		if (object.ai.tank.detectedPlayer)
		{
			// facing keen?
			object.ai.tank.alreadyfiredcauseonsamelevel = 1;
			// are we facing him?
			if (((m_Player[object.ai.tank.detectedPlayerIndex].x < object.x) && object.ai.tank.movedir==LEFT) || \
					((m_Player[object.ai.tank.detectedPlayerIndex].x > object.x) && object.ai.tank.movedir==RIGHT))
			{
				// yes, we're facing him! FIRE!!!
				if (!object.ai.tank.firetimes)
				{
					if (!object.ai.tank.timetillcanfirecauseonsamelevel)
					{
						tank2_fire(object);
						object.ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL;
					}
					else object.ai.tank.timetillcanfirecauseonsamelevel--;
				}
			}
			else
			{
				// no, we're not facing him, on hard difficulty turn around
				if (hardmode)
				{
					if (!object.ai.tank.turnaroundtimer)
					{
						object.ai.tank.frame = 0;
						object.ai.tank.timer = 0;
						object.ai.tank.animtimer = 0;
						object.ai.tank.state = TANK_LOOK;
						object.ai.tank.turnaroundtimer = 100;
					}
					else object.ai.tank.turnaroundtimer--;
				}
			}
		}
		else
		{  // no, not on same level
			object.ai.tank.alreadyfiredcauseonsamelevel = 0;
			object.ai.tank.turnaroundtimer = 0;
		}


		if (object.ai.tank.movedir==LEFT)
		{  // move left
			if (!object.blockedl)
			{
				object.x -= TANK_WALK_SPEED;
				object.ai.tank.dist_traveled++;
			}
			else
			{
				object.ai.tank.frame = 0;
				object.ai.tank.timer = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.state = TANK_LOOK;
			}
		}
		else
		{  // move right
			object.sprite = TANK2_WALK_RIGHT_FRAME + object.ai.tank.frame;
			if (!object.blockedr)
			{
				object.x += TANK_WALK_SPEED;
				object.ai.tank.dist_traveled++;
			}
			else
			{
				object.ai.tank.frame = 0;
				object.ai.tank.timer = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.state = TANK_LOOK;
			}
		}
		break;
	}
}

// makes the tank start firing
void CObjectAI::tank2_fire(CObject &object)
{
	object.ai.tank.firetimes = TANK2_SHOTS_PER_VOLLEY;
	object.ai.tank.timetillnextshot = 0;
	object.ai.tank.timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
	object.ai.tank.pausetime = TANK_FIRE_PAUSE_TIME;
}


// searches for any players on the same level as the tank
void CObjectAI::tank_searchplayers(CObject &object)
{
	object.ai.tank.detectedPlayer = 0;
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		if (m_Player[i].y >= object.y-(12<<STC))
		{
			if ((m_Player[i].y>>CSF)+Sprite[0]->getHeight() <= (object.y>>STC)+Sprite[object.sprite]->getHeight()+12)
			{
				object.ai.tank.detectedPlayer = 1;
				object.ai.tank.detectedPlayerIndex = i;
				break;
			}
		}
	}
}
