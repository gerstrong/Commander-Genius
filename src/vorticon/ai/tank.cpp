#include "CObjectAI.h"
#include "../spritedefines.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../game.h"

// Tank Robot (ep1)
enum{
	TANK_LOOK,TANK_WALK,TANK_FIRE
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
#define TANK_LOOK_TOTALTIME     11
#define TANK_PREPAREFIRE_TIME   44
#define TANK_PREPAREFIRE_TIME_FAST   22

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
		object.ai.tank.fireafterlook = 0;
		object.ai.tank.animtimer = 0;
		object.ai.tank.frame = 0;
		object.ai.tank.timer = 0;
		object.ai.tank.ponsameleveltime = 0;
		object.ai.tank.alreadyfiredcauseonsamelevel = 0;
		object.ai.tank.dist_to_travel = TANK_MAXTRAVELDIST;
		object.ai.tank.detectedPlayer = false;
		object.ai.tank.detectedPlayerIndex = 0;
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
	case TANK_LOOK:
		object.sprite = TANK_LOOK_FRAME + object.ai.tank.frame;
		// animation
		if (object.ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
		{
			object.ai.tank.frame ^= 1;
			object.ai.tank.animtimer = 0;
		} else object.ai.tank.animtimer++;

		object.ai.tank.detectedPlayer = false;
		for(size_t i=0 ; i<m_Player.size() ; i++)
		{
			if (m_Player[i].getYPosition() >= object.getYUpPos()-(3<<CSF))
			{
				if ( m_Player[i].getYDownPos() <= object.getYDownPos()+(3<<CSF) )
				{
					object.ai.tank.detectedPlayer = true;
					object.ai.tank.detectedPlayerIndex = i;
					break;
				}
			}
		}

		// when time is up go back to moving
		if (object.ai.tank.timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			// did we go into this state for the purpose of turning and firing?
			if (!object.ai.tank.fireafterlook)
			{ // no
				if ( !tank_CanMoveRight(object) || (rnd()&1) )
					object.ai.tank.movedir = LEFT;
				else if (!tank_CanMoveLeft(object) || !(rnd()&1) )
					object.ai.tank.movedir = RIGHT;

				object.ai.tank.state = TANK_WALK;
				object.ai.tank.frame = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.timer = 0;
			}
			else
			{ // yes, face towards m_Player.
				if (m_Player[object.ai.tank.detectedPlayerIndex].getXPosition() < object.getXLeftPos())
				{
					object.ai.tank.movedir = LEFT;
					object.sprite = TANK_WALK_LEFT_FRAME;
				}
				else
				{
					object.ai.tank.movedir = RIGHT;
					object.sprite = TANK_WALK_RIGHT_FRAME;
				}
				object.ai.tank.timer = 0;
				object.ai.tank.fireafterlook = 0;
				object.ai.tank.state = TANK_WALK;
			}
		} else object.ai.tank.timer++;
		break;
	case TANK_WALK:
		// is keen on same level?

		if (object.ai.tank.detectedPlayer)
		{
			object.ai.tank.ponsameleveltime++;
			if ((object.ai.tank.ponsameleveltime > TANK_SAME_LEVEL_TIME) ||
					(object.ai.tank.ponsameleveltime > TANK_SAME_LEVEL_TIME_FAST && hardmode))
			{   // keen would be a good target now (hard mode).
				if (!object.ai.tank.alreadyfiredcauseonsamelevel ||
						object.ai.tank.ponsameleveltime > TANK_REPEAT_FIRE_TIME ||
						(object.ai.tank.ponsameleveltime > TANK_REPEAT_FIRE_TIME_FAST && hardmode))
				{
					// facing keen?
					object.ai.tank.timer = 0;
					object.ai.tank.alreadyfiredcauseonsamelevel = 1;
					if (((m_Player[object.ai.tank.detectedPlayerIndex].getXPosition() < object.getXLeftPos()) && object.ai.tank.movedir==LEFT) ||
						((m_Player[object.ai.tank.detectedPlayerIndex].getXPosition() > object.getXLeftPos()) && object.ai.tank.movedir==RIGHT))
					{ // yes, we're facing him, we see him, we fire!
						object.ai.tank.state = TANK_FIRE;
						object.ai.tank.ponsameleveltime = 0;
					}
					else
					{ // no, we're not facing him. turn and fire!
						object.ai.tank.frame = 0;
						object.ai.tank.fireafterlook = 1;
						object.ai.tank.state = TANK_LOOK;
					}
				}
			}
		}
		else
		{  // no, not on same level
			object.ai.tank.ponsameleveltime = 0;
			object.ai.tank.alreadyfiredcauseonsamelevel = 0;
		}

		if (object.ai.tank.dist_to_travel <= 0)
		{
			object.ai.tank.timer = 0;
			object.ai.tank.state = TANK_FIRE;
		}

		if (object.ai.tank.movedir==LEFT)
		{  // move left
			object.sprite = TANK_WALK_LEFT_FRAME + object.ai.tank.frame;
			if( tank_CanMoveLeft(object) )
			{
				object.moveLeft(TANK_WALK_SPEED);
				object.ai.tank.dist_to_travel--;
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
			object.sprite = TANK_WALK_RIGHT_FRAME + object.ai.tank.frame;
			if ( tank_CanMoveRight(object) )
			{
				object.moveRight(TANK_WALK_SPEED);
				object.ai.tank.dist_to_travel--;
			}
			else
			{
				object.ai.tank.frame = 0;
				object.ai.tank.timer = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.state = TANK_LOOK;
			}
		}
		// walk animation
		if (object.ai.tank.animtimer > TANK_WALK_ANIM_TIME)
		{
			if (object.ai.tank.frame>=3) object.ai.tank.frame=0;
			else object.ai.tank.frame++;
			object.ai.tank.animtimer = 0;
		} else object.ai.tank.animtimer++;
		break;
	case TANK_FIRE:
		if (object.ai.tank.timer > TANK_PREPAREFIRE_TIME ||
				(object.ai.tank.timer > TANK_PREPAREFIRE_TIME_FAST && hardmode))
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

			object.ai.tank.state = TANK_LOOK;
			object.ai.tank.frame = 0;
			object.ai.tank.timer = 0;
			object.ai.tank.animtimer = 0;
			object.ai.tank.dist_to_travel = TANK_MINTRAVELDIST + (rnd()%10)*(TANK_MAXTRAVELDIST-TANK_MINTRAVELDIST)/10;
		} else object.ai.tank.timer++;
		break;
	}
}

bool CObjectAI::tank_CanMoveLeft(CObject &object)
{
	stTile &currentTile = g_pGfxEngine->Tilemap->mp_tiles[mp_Map->at((object.getXLeftPos()-1)>>CSF, (object.getYDownPos()+256)>>CSF)];
	if (!object.blockedl && currentTile.bup) return true;
	return false;
}

bool CObjectAI::tank_CanMoveRight(CObject &object)
{
	stTile &currentTile = g_pGfxEngine->Tilemap->mp_tiles[mp_Map->at((object.getXRightPos()+1)>>CSF, (object.getYDownPos()+256)>>CSF)];
	if (!object.blockedr && currentTile.bup) return true;
	return false;
}
