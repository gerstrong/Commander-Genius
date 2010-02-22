#include "../../sdl/sound/CSound.h"

#include "CObjectAI.h"

// Ninja AI (the black, bear-like karate-kicking creature in ep3)
enum ninja_actions{
	NINJA_STAND, NINJA_KICK, NINJA_DYING, NINJA_DEAD
};

#define NINJA_STAND_ANIM_RATE          10
#define NINJA_DYING_SHOW_TIME          6
#define NINJA_MIN_TIME_TILL_KICK       80
#define NINJA_MAX_TIME_TILL_KICK       100

#define NINJA_KICK_MOVE_RATE           2

#define NINJA_STAND_LEFT_FRAME         77
#define NINJA_STAND_RIGHT_FRAME        79
#define NINJA_KICK_LEFT_FRAME          81
#define NINJA_KICK_RIGHT_FRAME         82
#define NINJA_DYING_FRAME              83
#define NINJA_DEAD_FRAME               84

unsigned int rnd(void);

void CObjectAI::ninja_ai(CObject &object, bool hardmode)
{
	int onsamelevel;

	if (object.needinit)
	{
		object.ai.ninja.state = NINJA_STAND;
		object.ai.ninja.timetillkick = (rnd()%(NINJA_MAX_TIME_TILL_KICK-NINJA_MIN_TIME_TILL_KICK))+NINJA_MIN_TIME_TILL_KICK;
		if (hardmode) object.ai.ninja.timetillkick /= 3;

		if (m_Player[0].getXPosition() < object.getXPosition())
			object.ai.ninja.dir = LEFT;
		else
			object.ai.ninja.dir = RIGHT;

		object.ai.ninja.animtimer = 0;
		object.ai.ninja.animframe = 0;
		object.ai.ninja.isdying = 0;
		object.canbezapped = 1;
		object.inhibitfall = 0;
		object.needinit = 0;
	}
	if (object.ai.ninja.state==NINJA_DEAD) return;

	if (object.touchPlayer && !m_Player[object.touchedBy].pdie && \
			object.ai.ninja.state != NINJA_DYING)
		killplayer(object.touchedBy);

	if (object.zapped >= 4)
	{
		object.ai.ninja.isdying = 1;
		object.ai.ninja.dietimer = 0;
		object.ai.ninja.YFrictionRate = 2;
		if (object.ai.ninja.YInertia < 0) object.ai.ninja.YInertia = 0;
		object.zapped = 0;
		object.canbezapped = 0;
		g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, object.scrx);
	}


	if (object.ai.ninja.isdying)
	{
		if (object.ai.ninja.state == NINJA_STAND)
			object.ai.ninja.state = NINJA_DYING;

		object.ai.ninja.dietimer++;
		if (object.ai.ninja.dietimer > NINJA_DYING_SHOW_TIME)
			object.sprite = NINJA_DEAD_FRAME;
	}

	switch(object.ai.ninja.state)
	{
	case NINJA_STAND:
		if (m_Player[0].getXPosition() < object.getXPosition()+(8<<STC))
			object.ai.ninja.dir = LEFT;
		else
			object.ai.ninja.dir = RIGHT;

		if (!object.ai.ninja.timetillkick)
		{
			object.ai.ninja.state = NINJA_KICK;
			object.inhibitfall = 1;

			if (rnd()&1)
			{
				// high, short jump
				object.ai.ninja.XInertia = (hardmode) ? 100 : 50;
				object.ai.ninja.YInertia = -100;
				object.ai.ninja.XFrictionTimer = 0;
				object.ai.ninja.YFrictionTimer = 0;
				object.ai.ninja.XFrictionRate = 8;
				object.ai.ninja.YFrictionRate = 2;
			}
			else
			{
				// low, long jump
				object.ai.ninja.XInertia = (hardmode) ? 168 : 84;
				object.ai.ninja.YInertia = -30;
				object.ai.ninja.XFrictionTimer = 0;
				object.ai.ninja.YFrictionTimer = 0;
				object.ai.ninja.XFrictionRate = 5;
				object.ai.ninja.YFrictionRate = 1;
			}

			if (object.ai.ninja.dir==LEFT)
				object.ai.ninja.XInertia = -object.ai.ninja.XInertia;
		}
		else
		{
			// find out if a player is on the same level
			onsamelevel = 0;

			std::vector<CPlayer>::iterator it_player = m_Player.begin();
			for( ; it_player != m_Player.end() ; it_player++ )
			{
				if ((it_player->getYPosition() >= object.getYPosition()-(96<<STC)) &&
					(it_player->getYDownPos() <= (object.getYDownPos()+(96<<STC))))
				{
					onsamelevel = 1;
					break;
				}
			}

			if (onsamelevel)
				object.ai.ninja.timetillkick--;
		}

		if (object.ai.ninja.dir==LEFT)
			object.sprite = NINJA_STAND_LEFT_FRAME + object.ai.ninja.animframe;
		else
			object.sprite = NINJA_STAND_RIGHT_FRAME + object.ai.ninja.animframe;

		if (object.ai.ninja.animtimer > NINJA_STAND_ANIM_RATE)
		{
			object.ai.ninja.animframe ^= 1;
			object.ai.ninja.animtimer = 0;
		}
		else
			object.ai.ninja.animtimer++;
		break;
	case NINJA_KICK:
		if (object.blockedu && object.ai.ninja.YInertia < 0)
			object.ai.ninja.YInertia *= 0.5;

		if (!object.ai.ninja.isdying)
		{
			if (object.ai.ninja.dir==LEFT)
				object.sprite = NINJA_KICK_LEFT_FRAME;
			else
				object.sprite = NINJA_KICK_RIGHT_FRAME;
		}
		else
			object.sprite = NINJA_DYING_FRAME;
		if (object.ai.ninja.YInertia > 0 && object.blockedd)
		{
			if (!object.ai.ninja.isdying)
				object.needinit = true;
			else
				object.ai.ninja.state = NINJA_DYING;

			break;
		}
		else
		{
			if ((object.ai.ninja.XInertia > 0 && !object.blockedr) || \
					(object.ai.ninja.XInertia < 0 && !object.blockedl))
			{
				if (!object.ai.ninja.isdying)
					object.moveXDir(object.ai.ninja.XInertia);
			}

			if (object.ai.ninja.YInertia > 0 || !object.blockedu)
				object.moveYDir(object.ai.ninja.YInertia);
		}


		if (object.ai.ninja.KickMoveTimer < NINJA_KICK_MOVE_RATE)
		{
			object.ai.ninja.KickMoveTimer++;
			break;
		}
		object.ai.ninja.KickMoveTimer = 0;

		if (object.ai.ninja.XFrictionTimer > object.ai.ninja.XFrictionRate)
		{
			if (object.ai.ninja.XInertia>0)
			{
				if(object.ai.ninja.XInertia-16 < 0)
					object.ai.ninja.XInertia = 0;
				else object.ai.ninja.XInertia-= 16;
			}
			else
			{
				if(object.ai.ninja.XInertia+16 > 0)
					object.ai.ninja.XInertia = 0;
				else object.ai.ninja.XInertia+= 16;
			}

			object.ai.ninja.XFrictionTimer = 0;
		}
		else
			object.ai.ninja.XFrictionTimer++;

		if (object.ai.ninja.YFrictionTimer > object.ai.ninja.YFrictionRate)
		{
			if(!object.blockedd) object.ai.ninja.YInertia+=20;
			else{
				object.ai.ninja.YInertia=0;
				object.ai.ninja.state = NINJA_STAND;
				object.needinit = true;
			}
			object.ai.ninja.YFrictionTimer = 0;
		}
		else
			object.ai.ninja.YFrictionTimer++;

		break;
	case NINJA_DYING:
		object.sprite = NINJA_DYING_FRAME;

		if (object.ai.ninja.dietimer > NINJA_DYING_SHOW_TIME)
		{
			object.sprite = NINJA_DEAD_FRAME;
			object.ai.ninja.state = NINJA_DEAD;
		}
		break;
	}
}
