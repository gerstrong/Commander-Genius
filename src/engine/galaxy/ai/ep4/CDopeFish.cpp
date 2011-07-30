/*
 * CDopeFish.cpp
 *
 *  Created on: 26.07.2011
 *      Author: gerstrong
 */

#include "CDopeFish.h"
#include "engine/galaxy/ai/ep4/CPlayerDive.h"
#include "CBubbles.h"

namespace galaxy {

#define A_DOPEFISH_SWIM				0
#define A_DOPEFISH_EAT				2
#define A_DOPEFISH_START_BURP		3
#define A_DOPEFISH_BURPING			5
#define A_DOPEFISH_BURP_FINISHED	6

const int DOPE_SPEED = 20;
const int DOPE_BITE_SPEED = 40;
const int CSF_MIN_DISTANCE_TO_CHARGE = 6<<CSF;
const int DOPE_EAT_TIMER = 50;


CDopeFish::CDopeFish(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE),
m_eatTimer(0),
m_burped(false)
{
	setupGalaxyObjectOnMap(0x35C0, A_DOPEFISH_SWIM);
	mp_processState = &CDopeFish::processSwim;
	m_hDir = RIGHT;
	m_vDir = UP;
}

bool CDopeFish::isNearby(CObject &theObject)
{
	if( CPlayerDive *player = dynamic_cast<CPlayerDive*>(&theObject) )
	{

		const int dx = player->getXMidPos() - getXMidPos();
		const int dy = player->getYMidPos() - getYMidPos();


		if( dx<0 )
			m_hDir = LEFT;
		else
			m_hDir = RIGHT;

		if( dy<0 )
			m_vDir = UP;
		else
			m_vDir = DOWN;

		if(getActionNumber(A_DOPEFISH_SWIM))
		{
			int absdx = (dx<0) ? -dx : dx;
			int absdy = (dy<0) ? -dy : dy;

			if( absdx < CSF_MIN_DISTANCE_TO_CHARGE &&
			    absdy < CSF_MIN_DISTANCE_TO_CHARGE )
			{
				setAction(A_DOPEFISH_EAT);
				mp_processState = &CDopeFish::processEat;
				m_eatTimer = DOPE_EAT_TIMER;
			}
		}

	}

	return true;
}

void CDopeFish::getTouchedBy(CObject &theObject)
{
	if(theObject.dead)
		return;

	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
		Player->getEaten();
	}
}

void CDopeFish::moveDope(const int speed)
{
	if(m_hDir == LEFT)
		moveLeft(speed);
	else
		moveRight(speed);

	if(m_vDir == UP)
		moveUp(speed);
	else
		moveDown(speed);

}

void CDopeFish::processSwim()
{
	moveDope(DOPE_SPEED);
}

void CDopeFish::processEat()
{
	moveDope(DOPE_BITE_SPEED);

	if(m_eatTimer>0)
	{
		m_eatTimer--;
	}
	else
	{
		m_eatTimer = 0;
		setAction(A_DOPEFISH_START_BURP);

		m_burped = false;
		mp_processState = &CDopeFish::processBurp;
	}
}

void CDopeFish::processBurp()
{
	if(!m_burped && getActionStatus(A_DOPEFISH_BURPING))
	{
		g_pSound->playSound(SOUND_DOPEFISH_BURP);
		CBubbles *Bubble = new CBubbles(mp_Map, getXMidPos()+(1<<CSF), getYMidPos()+(1<<CSF), true);
		g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( Bubble ) );
		m_burped = true;
	}

	if(getActionStatus(A_DOPEFISH_BURP_FINISHED))
	{
		setAction(A_DOPEFISH_SWIM);
		mp_processState = &CDopeFish::processSwim;
	}
}

void CDopeFish::process()
{
	(this->*mp_processState)();

	processActionRoutine();
}

} /* namespace galaxy */
