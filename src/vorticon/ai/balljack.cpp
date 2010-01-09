// ai for the ball and the jack in ep3
#include "CObjectAI.h"
#include "../spritedefines.h"

#define BALL_SPEED      120
#define JACK_SPEED      120
#define JACK_ANIM_RATE  12

#define BALLPUSHAMOUNT	30

char BJ_BlockedD(int o);

unsigned int rnd(void);

void CObjectAI::ballandjack_ai(CObject &object)
{
	if (object.needinit)
	{
		int i = rnd()%4;
		switch(i)
		{
		case 0: object.ai.bj.dir = DUPLEFT; break;
		case 1: object.ai.bj.dir = DUPRIGHT; break;
		case 2: object.ai.bj.dir = DDOWNLEFT; break;
		case 3: object.ai.bj.dir = DDOWNRIGHT; break;
		}

		object.ai.bj.animframe = 0;
		object.ai.bj.animtimer = 0;
		object.blockedl = 0;
		object.blockedr = 0;
		object.blockedu = 0;
		object.blockedd = 0;
		object.inhibitfall = 1;

		if (object.m_type==OBJ_BALL)
		{
			object.ai.bj.speed = BALL_SPEED;
			object.canbezapped = 1;
		}
		else
		{
			object.ai.bj.speed = JACK_SPEED;
			object.canbezapped = 0;
		}
		object.needinit = 0;
	}

	if (object.touchPlayer)
	{
		if (object.m_type==OBJ_BALL)
		{
			if (m_Player[object.touchedBy].getXPosition() < object.getXLeftPos())
			{
				m_Player[object.touchedBy].bump(-BALLPUSHAMOUNT, true);
			}
			else
			{
				m_Player[object.touchedBy].bump(BALLPUSHAMOUNT, true);
			}

			switch(object.ai.bj.dir)
			{
			case DUPRIGHT: object.ai.bj.dir = DUPLEFT; break;
			case DUPLEFT: object.ai.bj.dir = DUPRIGHT; break;
			case DDOWNRIGHT: object.ai.bj.dir = DDOWNLEFT; break;
			case DDOWNLEFT: object.ai.bj.dir = DDOWNRIGHT; break;
			}
		}
		else killplayer(object.touchedBy);
	}

	if (object.zapped)
	{
		// have ball change direction when zapped
		if (object.zapd==LEFT)
		{
			switch(object.ai.bj.dir)
			{
			case DUPRIGHT: object.ai.bj.dir = DUPLEFT; break;
			case DDOWNRIGHT: object.ai.bj.dir = DDOWNLEFT; break;
			}
		}
		else
		{
			switch(object.ai.bj.dir)
			{
			case DUPLEFT: object.ai.bj.dir = DUPRIGHT; break;
			case DDOWNLEFT: object.ai.bj.dir = DDOWNRIGHT; break;
			}
		}
		object.zapped = 0;
	}

	switch(object.ai.bj.dir)
	{
	case DUPLEFT:
		if (object.blockedu) { object.ai.bj.dir = DDOWNLEFT; }
		else object.moveUp(object.ai.bj.speed);

		if (object.blockedl) { object.ai.bj.dir = DUPRIGHT; }
		else object.moveLeft(object.ai.bj.speed);
		break;
	case DUPRIGHT:
		if (object.blockedu) { object.ai.bj.dir = DDOWNRIGHT; }
		else object.moveUp(object.ai.bj.speed);

		if (object.blockedr) { object.ai.bj.dir = DUPLEFT; }
		else object.moveRight(object.ai.bj.speed);
		break;
	case DDOWNLEFT:
		if (BJ_BlockedD(object)) { object.ai.bj.dir = DUPLEFT; }
		else object.moveDown(object.ai.bj.speed);

		if (object.blockedl) { object.ai.bj.dir = DDOWNRIGHT; }
		else object.moveLeft(object.ai.bj.speed);
		break;
	case DDOWNRIGHT:
		if (BJ_BlockedD(object)) { object.ai.bj.dir = DUPRIGHT; }
		else object.moveDown(object.ai.bj.speed);

		if (object.blockedr) { object.ai.bj.dir = DDOWNLEFT; }
		else object.moveRight(object.ai.bj.speed);
		break;
	}

	if (object.m_type==OBJ_BALL)
	{
		object.sprite = OBJ_BALL_DEFSPRITE;
	}
	else
	{
		object.sprite = OBJ_JACK_DEFSPRITE + object.ai.bj.animframe;
		if (object.ai.bj.animtimer > JACK_ANIM_RATE)
		{
			object.ai.bj.animframe++;
			if (object.ai.bj.animframe>3) object.ai.bj.animframe=0;
			object.ai.bj.animtimer = 0;
		}
		else object.ai.bj.animtimer++;
	}
}

bool CObjectAI::BJ_BlockedD(CObject &object)
{
	// we do our own blockedd, because we don't want the ball/jack to
	// bounce off the top of platforms that have only solidfall set--
	// so we test blockedd against solidl/r instead
	if (object.blockedd)
	{	// ensure that the tile common_enemy_ai said we hit also has
		// solid l/r set
		return true;
	}
	return false;
}
