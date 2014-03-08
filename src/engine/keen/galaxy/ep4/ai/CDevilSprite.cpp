/*
 * CDevilSprite.cpp
 *
 *  Created on: 30.07.2011
 *      Author: gerstrong
 */

#include "CDevilSprite.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CEnemyShot.h"

namespace galaxy {

#define A_SPRITE_MOVE		0
#define A_SPRITE_LOOK		1
#define A_SPRITE_SHOOT		2

const int SPRITE_MOVE_DELAY = 60;
const int SPRITE_LOOK_DELAY = 30;
const int SPRITE_SHOOT_DELAY = 20;

const int SPRITE_MOVE_SPEED = 20;

const int CSF_MIN_DISTANCE_X_TO_LOOK = 10<<CSF;
const int CSF_MIN_DISTANCE_Y_TO_LOOK = 1<<CSF;

CDevilSprite::CDevilSprite(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0),
m_timer(0)
{
	setupGalaxyObjectOnMap(0x37A0, A_SPRITE_MOVE);
	mp_processState = &CDevilSprite::processMove;
	xDirection = LEFT;
	yDirection = UP;
}

void CDevilSprite::getTouchedBy(CSpriteObject &theObject)
{
	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
		Player->kill();
	}
}

bool CDevilSprite::isNearby(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		const int dx = player->getXMidPos() - getXMidPos();
		const int dy = player->getYMidPos() - getYMidPos();


		if( dx<0 )
			xDirection = LEFT;
		else
			xDirection = RIGHT;

		if(getActionNumber(A_SPRITE_MOVE))
		{
			int absdx = (dx<0) ? -dx : dx;
			int absdy = (dy<0) ? -dy : dy;

			if( absdx < CSF_MIN_DISTANCE_X_TO_LOOK &&
			    absdy < CSF_MIN_DISTANCE_Y_TO_LOOK )
			{
				setAction(A_SPRITE_LOOK);
				mp_processState = &CDevilSprite::processLook;
				m_timer = SPRITE_LOOK_DELAY;
			}
		}
	}
	return true;
}

void CDevilSprite::processMove()
{
	if(m_timer <= 0)
	{
		m_timer = SPRITE_MOVE_DELAY;
		yDirection = (yDirection==UP) ? DOWN : UP;
	}


	if(yDirection == UP)
		moveUp(SPRITE_MOVE_SPEED);
	else
		moveDown(SPRITE_MOVE_SPEED);

	m_timer--;
}

void CDevilSprite::processLook()
{
	m_timer--;

	if(m_timer <= 0)
	{
		m_timer = SPRITE_SHOOT_DELAY;
		setAction(A_SPRITE_SHOOT);
		mp_processState = &CDevilSprite::processShoot;
		// TODO: Spawn a sprite shot here! Could be inherited by the CBullet class maybe. Check that out...
		return;
	}
}

void CDevilSprite::processShoot()
{
	m_timer--;

	if( m_timer == SPRITE_SHOOT_DELAY/2 )
	{
		g_pSound->playSound(SOUND_SPRITE_SHOT);
		int x_coord = getXMidPos();
		x_coord += (xDirection == LEFT) ? -(8<<STC) : +(8<<STC);
		CEnemyShot *Spark = new CEnemyShot(mp_Map, 0, x_coord, getYMidPos()-(8<<STC),
                                            0x3818, xDirection, 0,  100, 0);
		gEventManager.add( new EventSpawnObject( Spark ) );
	}

	if( m_timer <= 0 )
	{
		m_timer = SPRITE_MOVE_DELAY;
		setAction(A_SPRITE_MOVE);
		mp_processState = &CDevilSprite::processMove;
	}
}

void CDevilSprite::process()
{
	(this->*mp_processState)();

	processActionRoutine();
}

} /* namespace galaxy */
