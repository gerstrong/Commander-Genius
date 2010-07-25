#include "CObjectAI.h"
#include "../../spritedefines.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../keen.h"
#include "CTank.h"
#include "CRay.h"

CTank::CTank(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player, std::vector<CObject*>& Object,
		object_t objtype) :
CObject(p_map, x, y, objtype),
m_Player(Player),
m_Object(Object)
{
	m_type = OBJ_TANK;
	hardmode = (mp_Map->m_Difficulty>1);
	state = TANK_WALK;
	movedir = RIGHT;
	animtimer = 0;
	frame = 0;
	timer = 0;
	ponsameleveltime = 0;
	alreadyfiredcauseonsamelevel = 0;
	dist_to_travel = TANK_MAXTRAVELDIST;
	canbezapped = true;  // will stop bullets but are not harmed
	m_invincible = true;
}

void CTank::process()
{
	switch(state)
	{
	case TANK_WALK: // Walk in a direction
	{
		// is keen on same level?
		if (movedir==LEFT)
		{  // move left
			sprite = TANK_WALK_LEFT_FRAME + frame;
			moveLeft(TANK_WALK_SPEED);
			if( !CanMoveLeft() )
			{
				movedir = RIGHT;
				frame = 0;
				timer = 0;
				animtimer = 0;
				state = TANK_TURN;
			}

			dist_to_travel--;
		}
		else
		{  // move right
			sprite = TANK_WALK_RIGHT_FRAME + frame;
			moveRight(TANK_WALK_SPEED);
			if ( !CanMoveRight() )
			{
				movedir = LEFT;
				frame = 0;
				timer = 0;
				animtimer = 0;
				state = TANK_TURN;
			}

			dist_to_travel--;
		}

		// walk animation
		if (animtimer > TANK_WALK_ANIM_TIME)
		{
			if (frame>=3) frame=0;
			else frame++;
			animtimer = 0;
		} else animtimer++;

		if(dist_to_travel==0)
		{
			frame = 0;
			timer = 0;
			animtimer = 0;
			state = TANK_WAIT;
		}
	default: break;
	}
	break;

	case TANK_WAIT:
		if ( (timer > TANK_PREPAREFIRE_TIME) ||
				(timer > TANK_PREPAREFIRE_TIME_FAST && hardmode) )
		{
			timer = 0;
			state = TANK_FIRE;
		}
		else
			timer++;

		break;

	case TANK_TURN:
		// If it gets stuck somewhere turn around
		sprite = TANK_LOOK_FRAME + frame;
		// animation
		if (animtimer > TANK_LOOK_ANIM_TIME)
		{
			frame ^= 1;
			animtimer = 0;
		} else animtimer++;

		// when time is up go back to moving
		if (timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			state = TANK_WALK;
			animtimer = 0;
			timer = 0;
		} else timer++;
		break;
	case TANK_FIRE:
	{
		CRay *newobject;
		if (onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, scrx);
		if (movedir==RIGHT)
			newobject = new CRay(mp_Map, getXMidPos(), getYUpPos()+(4<<STC), RIGHT);
		else
			newobject = new CRay(mp_Map, getXMidPos(), getYUpPos()+(4<<STC), LEFT);
		newobject->setOwner(OBJ_TANK, m_index);
		newobject->setSpeed(108);
		newobject->sprite = ENEMYRAY;
		newobject->canbezapped = true;
		m_Object.push_back(newobject);

		state = TANK_WAIT_LOOK;
		frame = 0;
		timer = 0;
		animtimer = 0;
		dist_to_travel = TANK_MINTRAVELDIST + (rnd()%10)*(TANK_MAXTRAVELDIST-TANK_MINTRAVELDIST)/10;
	}
	break;

	case TANK_WAIT_LOOK:
		// Happens after Robot has fired
		if ( timer > TANK_WAITAFTER_FIRE )
		{
			timer = 0;
			state = TANK_LOOK;
		}
		else
			timer++;

		break;

	case TANK_LOOK:
		sprite = TANK_LOOK_FRAME + frame;
		// animation
		if (animtimer > TANK_LOOK_ANIM_TIME)
		{
			frame ^= 1;
			animtimer = 0;
		} else animtimer++;

		// when time is up go back to moving
		if (timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go

			if(m_Player[0].getXMidPos() < getXMidPos())
			{
				movedir = LEFT;
				sprite = TANK_WALK_LEFT_FRAME;
			}
			else if(m_Player[0].getXMidPos() > getXMidPos())
			{
				movedir = RIGHT;
				sprite = TANK_WALK_RIGHT_FRAME;
			}
			state = TANK_WALK;
			animtimer = 0;
			timer = 0;
		} else timer++;
		break;
	}
}

bool CTank::CanMoveLeft()
{
	CTileProperties &currentTile = g_pBehaviorEngine->getTileProperties()
			[mp_Map->at((getXLeftPos()-1)>>CSF, (getYDownPos()+256)>>CSF)];
	if (!blockedl && currentTile.bup) return true;
	return false;
}

bool CTank::CanMoveRight()
{
	CTileProperties &currentTile = g_pBehaviorEngine->getTileProperties()
			[mp_Map->at((getXRightPos()+1)>>CSF, (getYDownPos()+256)>>CSF)];
	if (!blockedr && currentTile.bup) return true;
	return false;
}

void CTank::getTouchedBy(CObject &theObject)
{   // push keen
	if(hitdetect(theObject))
	{
		if( theObject.m_type == OBJ_PLAYER )
		{
			CPlayer &Player = dynamic_cast<CPlayer&>(theObject);

			if(Player.dead)
				return;

			if(!((Player.pdir == movedir) && (Player.pwalking)))
			{
				g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, scrx);
				Player.bump( Player.getXPosition() < getXPosition() ?
						TANKPUSHAMOUNT : -TANKPUSHAMOUNT, false);
			}
		}
	}
}
