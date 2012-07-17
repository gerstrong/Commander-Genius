/*
 * CLick.cpp
 *
 *  Created on: 04.08.2011
 *      Author: gerstrong
 */

#include "CLick.h"

#include "engine/galaxy/ai/CPlayerBase.h"
#include "misc.h"

namespace galaxy {

enum LICK_ACTIONS
{
A_LICK_HOP = 0,
A_LICK_LAND = 3,
A_LICK_BREATHE = 4,
A_LICK_STUNNED = 12
};

const int CSF_MIN_DISTANCE_TO_BREATHE = 2<<CSF;
const int CSF_DISTANCE_TO_FOLLOW_TOLERANCE = 2<<CSF;

const int LICK_HOP_X_SPEED = 50;
const int LICK_BREATHE_TIMER = 100;

CLick::CLick(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CStunnable(pmap, foeID, x, y),
m_timer(0)
{
	mActionMap[A_LICK_HOP] = (void (CStunnable::*)()) (&CLick::processHop);
	mActionMap[A_LICK_LAND] = (void (CStunnable::*)()) (&CLick::processLand);
	mActionMap[A_LICK_BREATHE] = (void (CStunnable::*)()) (&CLick::processBreathe);
	mActionMap[A_LICK_STUNNED] = &CStunnable::processGettingStunned;

	setupGalaxyObjectOnMap(0x2FC6, A_LICK_HOP);
}

void CLick::process()
{
	(this->*mp_processState)();

	if(!processActionRoutine())
			exists = false;

	processFalling();
}

void CLick::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction( A_LICK_STUNNED );
		theObject.dead = true;
		dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		if(getActionNumber(A_LICK_BREATHE))
		{
			player->kill();
		}
	}
}


bool CLick::isNearby(CSpriteObject &theObject)
{

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		const int dx = player->getXMidPos() - getXMidPos();

		if( getProbability(80) )
		{
			if( dx<-CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
				m_hDir = LEFT;
			else if( dx>+CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
				m_hDir = RIGHT;
		}

		if(getActionNumber(A_LICK_LAND))
		{
			int absdx = (dx<0) ? -dx : dx;

			if( absdx < CSF_MIN_DISTANCE_TO_BREATHE )
			{
				setAction(A_LICK_BREATHE);
				playSound(SOUND_LICK_FIREBREATH);
				m_timer = LICK_BREATHE_TIMER;
			}
		}

	}

	return true;
}

void CLick::processHop()
{
	// Move left or right according to set direction
	if(m_hDir == RIGHT)
		moveRight(LICK_HOP_X_SPEED);
	else if(m_hDir == LEFT)
		moveLeft(LICK_HOP_X_SPEED);

	if(blockedd)
	{
		setAction( A_LICK_LAND );
	}
}

void CLick::processLand()
{
	// After a moment he might hop again
	setAction( A_LICK_HOP );

	yinertia = -100;
	blockedd = false;
}

void CLick::processBreathe()
{
	// Breathe for a brief moment
	m_timer--;
	if(getActionStatus(A_LICK_HOP+2))
		setAction( A_LICK_HOP );
}


} /* namespace galaxy */
