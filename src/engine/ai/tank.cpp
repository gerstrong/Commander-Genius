#include "CObjectAI.h"
#include "../spritedefines.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"

// Comments by Tulip
/*

  Walk in a direction (TANK_WALK)
-> after a random time has passed stop for maybe half a second (TANK_WAIT)
-> a shoot in that same direction it has been walking (TANK_FIRE)
-> Show frontal animation for half a second (while doing that the AI should seek Keen) (TANK_LOOK)

    -> if Keen is anywhere on the screen left of the Tank -> walk left
    -> if Keen is anywhere right of the Tankbot on screen -> walk right

        (the Y position of Keen is completely irrelevant, the Tank should only check for the X position)

-> go to the top of this loop again.

 */

// Tank Robot (ep1)
enum{
	TANK_WALK,TANK_TURN,TANK_WAIT,TANK_WAIT_LOOK,TANK_FIRE,TANK_LOOK
};

#define TANK_SAME_LEVEL_TIME   25
#define TANK_REPEAT_FIRE_TIME  50

#define TANK_SAME_LEVEL_TIME_FAST    50
#define TANK_REPEAT_FIRE_TIME_FAST   300

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.
#define TANK_MINTRAVELDIST    100
#define TANK_MAXTRAVELDIST    200

#define TANK_WALK_SPEED         32
#define TANK_WALK_ANIM_TIME     2
#define TANK_LOOK_ANIM_TIME     4
#define TANK_LOOK_TOTALTIME     44
#define TANK_PREPAREFIRE_TIME   30
#define TANK_PREPAREFIRE_TIME_FAST   22
#define TANK_WAITAFTER_FIRE		14

// frames
#define TANK_WALK_LEFT_FRAME        102
#define TANK_WALK_RIGHT_FRAME       98
#define TANK_LOOK_FRAME             106

#define TANKPUSHAMOUNT        64

//reference to ../misc.cpp
unsigned int rnd(void);

void CObjectAI::tank_ai(CObject &object, bool hardmode)
{
	if (object.needinit)
	{  // first time initialization
		object.ai.tank.state = TANK_WALK;
		object.ai.tank.movedir = RIGHT;
		object.ai.tank.animtimer = 0;
		object.ai.tank.frame = 0;
		object.ai.tank.timer = 0;
		object.ai.tank.ponsameleveltime = 0;
		object.ai.tank.alreadyfiredcauseonsamelevel = 0;
		object.ai.tank.dist_to_travel = TANK_MAXTRAVELDIST;
		object.canbezapped = true;  // will stop bullets but are not harmed
		object.needinit = false;
	}

	// stop keen from walking through our sprite
	if (object.touchPlayer && !m_Player[object.touchedBy].pdie)
	{
		g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, object.scrx);

		if (!((m_Player[object.touchedBy].getYPosition()) < (object.getYUpPos() - 300))) // give the m_Player a little jump-over room
		{
			if (m_Player[object.touchedBy].getXPosition() < object.getXLeftPos())
			{
				m_Player[object.touchedBy].playpushed_x = -TANKPUSHAMOUNT;
				m_Player[object.touchedBy].playpushed_decreasetimer = 0;
				m_Player[object.touchedBy].pdir = m_Player[object.touchedBy].pshowdir = LEFT;
			}
			else
			{
				m_Player[object.touchedBy].playpushed_x = TANKPUSHAMOUNT;
				m_Player[object.touchedBy].playpushed_decreasetimer = 0;
				m_Player[object.touchedBy].pdir = m_Player[object.touchedBy].pshowdir = RIGHT;
			}
		}
	}

	switch(object.ai.tank.state)
	{

	// Walk in a direction
	case TANK_WALK:
	{
		// is keen on same level?
		if (object.ai.tank.movedir==LEFT)
		{  // move left
			object.sprite = TANK_WALK_LEFT_FRAME + object.ai.tank.frame;
			object.moveLeft(TANK_WALK_SPEED);
			if( !tank_CanMoveLeft(object) )
			{
				object.ai.tank.movedir = RIGHT;
				object.ai.tank.frame = 0;
				object.ai.tank.timer = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.state = TANK_TURN;
			}

			object.ai.tank.dist_to_travel--;
		}
		else
		{  // move right
			object.sprite = TANK_WALK_RIGHT_FRAME + object.ai.tank.frame;
			object.moveRight(TANK_WALK_SPEED);
			if ( !tank_CanMoveRight(object) )
			{
				object.ai.tank.movedir = LEFT;
				object.ai.tank.frame = 0;
				object.ai.tank.timer = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.state = TANK_TURN;
			}

			object.ai.tank.dist_to_travel--;
		}

		// walk animation
		if (object.ai.tank.animtimer > TANK_WALK_ANIM_TIME)
		{
			if (object.ai.tank.frame>=3) object.ai.tank.frame=0;
			else object.ai.tank.frame++;
			object.ai.tank.animtimer = 0;
		} else object.ai.tank.animtimer++;

		if(object.ai.tank.dist_to_travel==0)
		{
			object.ai.tank.frame = 0;
			object.ai.tank.timer = 0;
			object.ai.tank.animtimer = 0;
			object.ai.tank.state = TANK_WAIT;
		}

	}
	break;

	case TANK_WAIT:
		if ( (object.ai.tank.timer > TANK_PREPAREFIRE_TIME) ||
				(object.ai.tank.timer > TANK_PREPAREFIRE_TIME_FAST && hardmode) )
		{
			object.ai.tank.timer = 0;
			object.ai.tank.state = TANK_FIRE;
		}
		else
			object.ai.tank.timer++;

		break;

	case TANK_TURN:
		// If it gets stuck somewhere turn around
		object.sprite = TANK_LOOK_FRAME + object.ai.tank.frame;
		// animation
		if (object.ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
		{
			object.ai.tank.frame ^= 1;
			object.ai.tank.animtimer = 0;
		} else object.ai.tank.animtimer++;

		// when time is up go back to moving
		if (object.ai.tank.timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			object.ai.tank.state = TANK_WALK;
			object.ai.tank.animtimer = 0;
			object.ai.tank.timer = 0;
		} else object.ai.tank.timer++;
		break;
	case TANK_FIRE:
	{
		CObject newobject(mp_Map, m_Objvect.size());
		if (object.onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, object.scrx);
		if (object.ai.tank.movedir==RIGHT)
		{
			newobject.spawn(object.getXMidPos(), object.getYUpPos()+(4<<STC), OBJ_RAY, m_Episode, RIGHT);
			newobject.ai.ray.direction = RIGHT;
		}
		else
		{
			newobject.spawn(object.getXMidPos(), object.getYUpPos()+(4<<STC), OBJ_RAY, m_Episode, LEFT);
			newobject.ai.ray.direction = LEFT;
		}
		newobject.ai.ray.owner = object.m_index;
		newobject.sprite = ENEMYRAY;
		newobject.ai.ray.dontHitEnable = 0;
		newobject.canbezapped = true;
		m_Objvect.push_back(newobject);

		object.ai.tank.state = TANK_WAIT_LOOK;
		object.ai.tank.frame = 0;
		object.ai.tank.timer = 0;
		object.ai.tank.animtimer = 0;
		object.ai.tank.dist_to_travel = TANK_MINTRAVELDIST + (rnd()%10)*(TANK_MAXTRAVELDIST-TANK_MINTRAVELDIST)/10;
	}
	break;

	case TANK_WAIT_LOOK:
		// Happens after Robot has fired
		if ( object.ai.tank.timer > TANK_WAITAFTER_FIRE )
		{
			object.ai.tank.timer = 0;
			object.ai.tank.state = TANK_LOOK;
		}
		else
			object.ai.tank.timer++;

		break;

	case TANK_LOOK:
		object.sprite = TANK_LOOK_FRAME + object.ai.tank.frame;
		// animation
		if (object.ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
		{
			object.ai.tank.frame ^= 1;
			object.ai.tank.animtimer = 0;
		} else object.ai.tank.animtimer++;

		// when time is up go back to moving
		if (object.ai.tank.timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go

			if(m_Player[0].getXMidPos() < object.getXMidPos())
			{
				object.ai.tank.movedir = LEFT;
				object.sprite = TANK_WALK_LEFT_FRAME;
			}
			else if(m_Player[0].getXMidPos() > object.getXMidPos())
			{
				object.ai.tank.movedir = RIGHT;
				object.sprite = TANK_WALK_RIGHT_FRAME;
			}
			object.ai.tank.state = TANK_WALK;
			object.ai.tank.animtimer = 0;
			object.ai.tank.timer = 0;
		} else object.ai.tank.timer++;
		break;
	}
}

bool CObjectAI::tank_CanMoveLeft(CObject &object)
{
	CTileProperties &currentTile = g_pBehaviorEngine->getTileProperties()
			[mp_Map->at((object.getXLeftPos()-1)>>CSF, (object.getYDownPos()+256)>>CSF)];
	if (!object.blockedl && currentTile.bup) return true;
	return false;
}

bool CObjectAI::tank_CanMoveRight(CObject &object)
{
	CTileProperties &currentTile = g_pBehaviorEngine->getTileProperties()
			[mp_Map->at((object.getXRightPos()+1)>>CSF, (object.getYDownPos()+256)>>CSF)];
	if (!object.blockedr && currentTile.bup) return true;
	return false;
}
