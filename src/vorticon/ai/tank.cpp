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
//#define TANK_LOOKFIRE_PROB    500
#define TANK_LOOKFIRE_PROB    100
#define TANK_MINTRAVELDIST    200

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
		object.ai.tank.dist_traveled = 0;
		object.ai.tank.detectedPlayer = 0;
		object.ai.tank.detectedPlayerIndex = 0;
		object.canbezapped = true;  // will stop bullets but are not harmed
		object.needinit = false;
	}

	int height = object.bboxY2-object.bboxY1;
	int width = object.bboxX2-object.bboxX1;

	// stop keen from walking through our sprite
	if (object.touchPlayer && !m_Player[object.touchedBy].pdie)
	{
		g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, object.scrx);

		if (!((m_Player[object.touchedBy].y) < (object.y - 300))) // give the m_Player a little jump-over room
		{
			if (m_Player[object.touchedBy].x < object.x)
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

		// when time is up go back to moving
		if (object.ai.tank.timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			// did we go into this state for the purpose of turning and firing?
			if (!object.ai.tank.fireafterlook)
			{ // no

				if ( !tank_CanMoveRight(object, width, height) || (rnd()&1) )
					object.ai.tank.movedir = LEFT;
				else if (!tank_CanMoveLeft(object, height) || !(rnd()&1) )
					object.ai.tank.movedir = RIGHT;

				object.ai.tank.state = TANK_WALK;
				object.ai.tank.frame = 0;
				object.ai.tank.animtimer = 0;
				object.ai.tank.timer = 0;
				object.ai.tank.dist_traveled = 0;
			}
			else
			{ // yes, face towards m_Player and fire.
				if (m_Player[object.ai.tank.detectedPlayerIndex].x < object.x)
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
				object.ai.tank.state = TANK_FIRE;
			}
		} else object.ai.tank.timer++;
		break;
	case TANK_WALK:
		// is keen on same level?
		object.ai.tank.detectedPlayer = false;
		for(int i=0;i<m_NumPlayers;i++)
		{
			if (m_Player[i].y >= object.y-(3<<CSF))
			{
				if ( m_Player[i].y+m_Player[i].h <= object.y+height+(3<<CSF) )
				{
					object.ai.tank.detectedPlayer = true;
					object.ai.tank.detectedPlayerIndex = i;
					break;
				}
			}
		}

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
					if (((m_Player[object.ai.tank.detectedPlayerIndex].x < object.x) && object.ai.tank.movedir==LEFT) ||
						((m_Player[object.ai.tank.detectedPlayerIndex].x > object.x) && object.ai.tank.movedir==RIGHT))
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

		if (object.ai.tank.dist_traveled > TANK_MINTRAVELDIST && object.onscreen)
		{
			if (rnd()%TANK_LOOKFIRE_PROB==(TANK_LOOKFIRE_PROB/2))
			{  // we're either going to look or fire
				object.ai.tank.timer = 0;

				if (rnd()&1)
				{ // look
					object.ai.tank.timer = 0;
					object.ai.tank.frame = 0;
					object.ai.tank.state = TANK_LOOK;
				}
				else
				{ // FIRE!
					object.ai.tank.state = TANK_FIRE;
				}
				break;
			}
		}

		if (object.ai.tank.movedir==LEFT)
		{  // move left
			object.sprite = TANK_WALK_LEFT_FRAME + object.ai.tank.frame;
			if( tank_CanMoveLeft(object, height) )
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
			object.sprite = TANK_WALK_RIGHT_FRAME + object.ai.tank.frame;
			if ( tank_CanMoveRight(object, width, height) )
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
			CObject newobject(mp_Map);
			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, object.scrx);
			if (object.ai.tank.movedir==RIGHT)
			{
				newobject.spawn(object.x+object.bboxX2, object.y+(5<<STC), OBJ_RAY, m_Episode);
				newobject.ai.ray.direction = RIGHT;
			}
			else
			{
				newobject.spawn(object.x, object.y+(5<<STC), OBJ_RAY, m_Episode);
				newobject.ai.ray.direction = LEFT;
			}
			newobject.ai.ray.owner = object.m_index;
			newobject.sprite = ENEMYRAY;
			newobject.ai.ray.dontHitEnable = 0;
			m_Objvect.push_back(newobject);

			object.ai.tank.state = TANK_WALK;

		} else object.ai.tank.timer++;
		break;
	}
}

bool CObjectAI::tank_CanMoveLeft(CObject &object, int h)
{
	stTile &currentTile = g_pGfxEngine->Tilemap->mp_tiles[mp_Map->at((object.x-1)>>CSF, (object.y+h+512)>>CSF)];
	if (!object.blockedl && currentTile.bup) return true;
	return false;
}

bool CObjectAI::tank_CanMoveRight(CObject &object, int w, int h)
{
	stTile &currentTile = g_pGfxEngine->Tilemap->mp_tiles[mp_Map->at((object.x+w+1)>>CSF, (object.y+h+512)>>CSF)];
	if (!object.blockedr && currentTile.bup) return true;
	return false;
}
