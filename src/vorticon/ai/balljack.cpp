// ai for the ball and the jack in ep3
#include "CObjectAI.h"
#include "../spritedefines.h"

#define BALL_SPEED      60
#define JACK_SPEED      40
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
			if (m_Player[object.touchedBy].x < object.x)
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
		else object.y -= object.ai.bj.speed;

		if (object.blockedl) { object.ai.bj.dir = DUPRIGHT; }
		else object.x -= object.ai.bj.speed;
		break;
	case DUPRIGHT:
		if (object.blockedu) { object.ai.bj.dir = DDOWNRIGHT; }
		else object.y -= object.ai.bj.speed;

		if (object.blockedr) { object.ai.bj.dir = DUPLEFT; }
		else object.x += object.ai.bj.speed;
		break;
	case DDOWNLEFT:
		if (BJ_BlockedD(object)) { object.ai.bj.dir = DUPLEFT; }
		else object.y += object.ai.bj.speed;

		if (object.blockedl) { object.ai.bj.dir = DDOWNRIGHT; }
		else object.x -= object.ai.bj.speed;
		break;
	case DDOWNRIGHT:
		if (BJ_BlockedD(object)) { object.ai.bj.dir = DUPRIGHT; }
		else object.y += object.ai.bj.speed;

		if (object.blockedr) { object.ai.bj.dir = DDOWNLEFT; }
		else object.x += object.ai.bj.speed;
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

char CObjectAI::BJ_BlockedD(CObject &object)
{
	// we do our own blockedd, because we don't want the ball/jack to
	// bounce off the top of platforms that have only solidfall set--
	// so we test blockedd against solidl/r instead
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	int x1 = object.bboxX1;
	int x2 = object.bboxX2;
	int y2 = object.bboxY2;

	if (object.blockedd)
	{
		// ensure that the tile common_enemy_ai said we hit also has
		// solid l/r set
		if (TileProperty[mp_Map->at((object.x+x1)>>CSF, (object.y+y2)>>CSF)].bleft)
		{ return 1; }
		if (TileProperty[mp_Map->at((object.x+x2)>>CSF, (object.y+y2)>>CSF)].bleft)
		{ return 1; }

	}

	// ensure it's not a ball no-pass point
	if (mp_Map->getObjectat((object.x+x1)>>CSF, (object.y+y2)>>CSF)==BALL_NOPASSPOINT)
	{ return 1; }
	if (mp_Map->getObjectat((object.x+x2)>>CSF, (object.y+y2)>>CSF)==BALL_NOPASSPOINT)
	{ return 1; }

	return 0;
}
