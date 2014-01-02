/*
 * CDopeFish.cpp
 *
 *  Created on: 26.07.2011
 *      Author: gerstrong
 */

#include "CDopeFish.h"
#include "CPlayerDive.h"
#include "CSchoolFish.h"
#include "CBubbles.h"

namespace galaxy
{

const int A_DOPEFISH_SWIM = 0;
const int A_DOPEFISH_EAT = 2;
const int A_DOPEFISH_START_BURP = 3;
const int A_DOPEFISH_BURPING = 5;
const int A_DOPEFISH_BURP_FINISHED = 6;

const int DOPE_SPEED = 30;
const int DOPE_BITE_SPEED = 60;
const int CSF_MIN_DISTANCE_TO_CHARGE = 1<<CSF;
const int CSF_DISTANCE_TO_FOLLOW_TOLERANCE = 2<<CSF;
const int CSF_DISTANCE_TO_FOLLOW = 20<<CSF;
const int DOPE_EAT_TIMER = 50;


CDopeFish::CDopeFish(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
m_eatTimer(0),
m_burped(false)
{
	setupGalaxyObjectOnMap(0x35C0, A_DOPEFISH_SWIM);
	mp_processState = &CDopeFish::processSwim;
	xDirection = RIGHT;
	yDirection = UP;
}

bool CDopeFish::isNearby(CSpriteObject &theObject)
{
	if( dynamic_cast<CPlayerBase*>(&theObject) ||
		dynamic_cast<CSchoolFish*>(&theObject) )
	{
		const int objX = theObject.getXMidPos();
		const int objY = theObject.getYMidPos();
		const int fishX = getXMidPos();
		const int fishY = getYMidPos();
		const int dx = objX - fishX;
		const int dy = objY - fishY;


		if( objX < fishX - CSF_DISTANCE_TO_FOLLOW ||
			objX > fishX + CSF_DISTANCE_TO_FOLLOW )
			return false;

		if( objY < fishY - CSF_DISTANCE_TO_FOLLOW ||
			objY > fishY + CSF_DISTANCE_TO_FOLLOW )
			return false;


		if( dx<-CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
			xDirection = LEFT;
		else if( dx>+CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
			xDirection = RIGHT;

		if( dy<-CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
			yDirection = (rand()%5) ? UP : DOWN;
		else if( dy>+CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
			yDirection = (rand()%5) ? DOWN : UP;

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

void CDopeFish::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead)
		return;

	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
		if( getActionNumber(A_DOPEFISH_EAT) )
			Player->getEaten();
	}
}

void CDopeFish::moveDope(const int speed)
{
	if(xDirection == LEFT)
		moveLeft(speed);
	else
		moveRight(speed);

	if(yDirection == UP)
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
		CBubbles *Bubble = new CBubbles(mp_Map, 0, getXMidPos()+(1<<CSF), getYMidPos()+(1<<CSF), true);
		gEventManager.add( new EventSpawnObject( Bubble ) );
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
