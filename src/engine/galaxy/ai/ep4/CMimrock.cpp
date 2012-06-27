/*
 * CMimrock.cpp
 *
 *  Created on: 18.09.2011
 *      Author: gerstrong
 */

#include "CMimrock.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy {

#define A_MIMROCK_SIT		0
#define A_MIMROCK_WALK		1
#define A_MIMROCK_JUMP		7
#define A_MIMROCK_BOUNCE	10
#define A_MIMROCK_STUNNED	11

const int CSF_DISTANCE_TO_FOLLOW_TOLERANCE = 2<<CSF;
const int WALK_SPEED = 10;

CMimrock::CMimrock(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
CStunnable(pmap, foeID, x, y)
{
	setupGalaxyObjectOnMap(0x343A, A_MIMROCK_SIT);
	mp_processState = (void (CStunnable::*)()) &CMimrock::processSit;
	m_hDir = NONE;
}


bool CMimrock::isNearby(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		const int dx = player->getXMidPos() - getXMidPos();

		if( dx>-CSF_DISTANCE_TO_FOLLOW_TOLERANCE &&
			dx<+CSF_DISTANCE_TO_FOLLOW_TOLERANCE	)
		{
			if( dx<0 )
				m_hDir = LEFT;
			else
				m_hDir = RIGHT;

			setAction(A_MIMROCK_WALK);
			mp_processState = (void (CStunnable::*)()) &CMimrock::processWalk;
		}
	}

	return true;
}

void CMimrock::processSit()
{
	// When sitting the rock doesn't do any thing, so this stays empty for now.
}

void CMimrock::processWalk()
{
	if(m_hDir == LEFT)
		moveLeft(WALK_SPEED);
	else
		moveRight(WALK_SPEED);

	if(getActionStatus(A_MIMROCK_SIT))
	{
		setAction(A_MIMROCK_SIT);
		mp_processState = (void (CStunnable::*)()) &CMimrock::processSit;
	}
}

void CMimrock::processJump()
{
	// TODO: Code here!
}

void CMimrock::processBounce()
{
	// TODO: Code here!
}

void CMimrock::processStunned()
{
	// TODO: Code here!
}

void CMimrock::process()
{
	performCollisions();
	processFalling();

	(this->*mp_processState)();

	processActionRoutine();
}


} /* namespace galaxy */
