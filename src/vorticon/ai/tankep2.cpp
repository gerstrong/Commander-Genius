#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../game.h"

//#include "enemyai.h"


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
/*

void CObjectAI::tankep2_ai(CObject *p_object, bool hardmode)
{
	int newobject;
	if (p_object->needinit)
	{  // first time initilization
		p_object->ai.tank.state = TANK_WALK;
		p_object->ai.tank.movedir = RIGHT;
		p_object->ai.tank.fireafterlook = 0;
		p_object->ai.tank.animtimer = 0;
		p_object->ai.tank.timer = 0;
		p_object->ai.tank.dist_traveled = 0;
		p_object->ai.tank.pausetime = 0;
		p_object->ai.tank.timetillcanfire = TANK2_MAX_TIME_TILL_CAN_FIRE;
		p_object->ai.tank.firetimes = 0;
		p_object->ai.tank.detectedPlayer = 0;
		p_object->ai.tank.detectedPlayerIndex = primaryplayer;
		p_object->ai.tank.turnaroundtimer = 0;

		p_object->canbezapped = 1;   // will stop bullets but is not harmed
		p_object->inhibitfall = 1;
		p_object->needinit = 0;
	}
	// touched player?
	if (p_object->touchPlayer && !player[p_object->touchedBy].pdie)
		killplayer(p_object->touchedBy);

	switch(p_object->ai.tank.state)
	{
	case TANK_LOOK:
		// animation
		if (p_object->ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
		{
			p_object->ai.tank.frame ^= 1;
			p_object->ai.tank.animtimer = 0;
		}
		else
			p_object->ai.tank.animtimer++;

		p_object->sprite = TANK2_LOOK_FRAME + p_object->ai.tank.frame;


		// when time is up go back to moving
		if (p_object->ai.tank.timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			if (p_object->blockedr)
			{ p_object->ai.tank.movedir = LEFT; }
			else if (p_object->blockedl)
			{ p_object->ai.tank.movedir = RIGHT; }
			else if (p_object->x > player[0].x)
			{ p_object->ai.tank.movedir = LEFT; }
			else
			{ p_object->ai.tank.movedir = RIGHT; }

			p_object->ai.tank.alreadyfiredcauseonsamelevel = 0;
			p_object->ai.tank.timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
			p_object->ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BEFORE_FIRE_WHEN_SEE;
			p_object->ai.tank.firetimes = 0;
			p_object->ai.tank.state = TANK_WALK;
			p_object->ai.tank.frame = 0;
			p_object->ai.tank.animtimer = 0;
			p_object->ai.tank.timer = 0;
			p_object->ai.tank.dist_traveled = 0;
		}
		else
			p_object->ai.tank.timer++;

		break;

	case TANK_WALK:
		// hover animation
		if (p_object->ai.tank.animtimer > TANK_WALK_ANIM_TIME)
		{
			if (p_object->ai.tank.frame>=3) p_object->ai.tank.frame=0;
			else p_object->ai.tank.frame++;
			p_object->ai.tank.animtimer = 0;
		} else p_object->ai.tank.animtimer++;

		if (p_object->ai.tank.movedir==LEFT)
			p_object->sprite = TANK2_WALK_LEFT_FRAME + p_object->ai.tank.frame;
		else
			p_object->sprite = TANK2_WALK_RIGHT_FRAME + p_object->ai.tank.frame;

		// if we're about to, or just did, fire a volley, don't move
		if (!hardmode)
		{
			if (p_object->ai.tank.pausetime)
			{
				p_object->ai.tank.pausetime--;
				return;
			}
		}
		else
			p_object->ai.tank.pausetime = 0;

		// are we firing a volley?
		if (p_object->ai.tank.firetimes)
		{
			// is it time to fire the next shot in the volley?
			if (!p_object->ai.tank.timetillnextshot)
			{
				if (p_object->onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, p_object->scrx);
				if (p_object->ai.tank.movedir==RIGHT)
				{
					newobject = spawn_object(p_object->x+(Sprite[TANK2_WALK_RIGHT_FRAME]->getWidth()<<CSF), p_object->y+(6<<CSF), OBJ_RAY);
					objects[newobject].ai.ray.direction = RIGHT;
				}
				else
				{
					newobject = spawn_object(p_object->x-(Sprite[ENEMYRAYEP2]->getWidth()<<CSF), p_object->y+(6<<CSF), OBJ_RAY);
					objects[newobject].ai.ray.direction = LEFT;
				}
				objects[newobject].sprite = ENEMYRAYEP2;
				objects[newobject].ai.ray.dontHitEnable = 0;

				p_object->ai.tank.timetillnextshot = TANK2_TIME_BETWEEN_SHOTS;
				if (!--p_object->ai.tank.firetimes)
				{
					p_object->ai.tank.pausetime = TANK_FIRE_PAUSE_TIME;
				}
			}
			else
			{
				p_object->ai.tank.timetillnextshot--;
			}

			// don't move when firing except on hard mode
			if (hardmode)
				return;

		}
		else
		{  // not firing a volley
			if (!p_object->ai.tank.timetillcanfire)
			{
				tank2_fire(o);
			}
			else
			{
				p_object->ai.tank.timetillcanfire--;
			}

		}

		// is keen on same level?
		tank_searchplayers(o);

		if (p_object->ai.tank.detectedPlayer)
		{
			// facing keen?
			p_object->ai.tank.alreadyfiredcauseonsamelevel = 1;
			// are we facing him?
			if (((player[p_object->ai.tank.detectedPlayerIndex].x < p_object->x) && p_object->ai.tank.movedir==LEFT) || \
					((player[p_object->ai.tank.detectedPlayerIndex].x > p_object->x) && p_object->ai.tank.movedir==RIGHT))
			{
				// yes, we're facing him! FIRE!!!
				if (!p_object->ai.tank.firetimes)
				{
					if (!p_object->ai.tank.timetillcanfirecauseonsamelevel)
					{
						tank2_fire(o);
						p_object->ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL;
					}
					else p_object->ai.tank.timetillcanfirecauseonsamelevel--;
				}
			}
			else
			{
				// no, we're not facing him, on hard difficulty turn around
				if (hardmode)
				{
					if (!p_object->ai.tank.turnaroundtimer)
					{
						p_object->ai.tank.frame = 0;
						p_object->ai.tank.timer = 0;
						p_object->ai.tank.animtimer = 0;
						p_object->ai.tank.state = TANK_LOOK;
						p_object->ai.tank.turnaroundtimer = 100;
					}
					else p_object->ai.tank.turnaroundtimer--;
				}
			}
		}
		else
		{  // no, not on same level
			p_object->ai.tank.alreadyfiredcauseonsamelevel = 0;
			p_object->ai.tank.turnaroundtimer = 0;
		}


		if (p_object->ai.tank.movedir==LEFT)
		{  // move left
			if (!p_object->blockedl)
			{
				p_object->x -= TANK_WALK_SPEED;
				p_object->ai.tank.dist_traveled++;
			}
			else
			{
				p_object->ai.tank.frame = 0;
				p_object->ai.tank.timer = 0;
				p_object->ai.tank.animtimer = 0;
				p_object->ai.tank.state = TANK_LOOK;
			}
		}
		else
		{  // move right
			p_object->sprite = TANK2_WALK_RIGHT_FRAME + p_object->ai.tank.frame;
			if (!p_object->blockedr)
			{
				p_object->x += TANK_WALK_SPEED;
				p_object->ai.tank.dist_traveled++;
			}
			else
			{
				p_object->ai.tank.frame = 0;
				p_object->ai.tank.timer = 0;
				p_object->ai.tank.animtimer = 0;
				p_object->ai.tank.state = TANK_LOOK;
			}
		}
		break;
	}
}

// makes the tank start firing
void static CObjectAI::tank2_fire(CObject &object)
{
	object.ai.tank.firetimes = TANK2_SHOTS_PER_VOLLEY;
	object.ai.tank.timetillnextshot = 0;
	object.ai.tank.timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
	object.ai.tank.pausetime = TANK_FIRE_PAUSE_TIME;
}


// searches for any players on the same level as the tank
void static CObjectAI::tank_searchplayers(CObject &object)
{
	object.ai.tank.detectedPlayer = 0;
	for( unsigned int i=0 ; i<m_NumPlayers ; i++ )
	{
		if (mp_Player[i].y >= object.y-(12<<STC))
		{
			if ((mp_Player[i].y>>CSF)+Sprite[0]->getHeight() <= (object.y>>STC)+Sprite[object.sprite]->getHeight()+12)
			{
				object.ai.tank.detectedPlayer = 1;
				object.ai.tank.detectedPlayerIndex = i;
				break;
			}
		}
	}
}
*/
