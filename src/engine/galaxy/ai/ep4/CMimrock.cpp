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

CMimrock::CMimrock(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE),
CStunnable(pmap, x, y, OBJ_NONE)
{
	setupGalaxyObjectOnMap(0x343A, A_MIMROCK_SIT);
	mp_processState = (void (CStunnable::*)()) &CMimrock::processSit;
	m_hDir = NONE;
}


bool CMimrock::isNearby(CObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{

		const int dx = player->getXMidPos() - getXMidPos();
		const int dy = player->getYMidPos() - getYMidPos();


		if( dx<-CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
		{
			m_hDir = LEFT;
			setAction(A_MIMROCK_WALK);
			mp_processState = (void (CStunnable::*)()) &CMimrock::processWalk;
		}
		else if( dx>+CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
		{
			m_hDir = RIGHT;
			setAction(A_MIMROCK_WALK);
			mp_processState = (void (CStunnable::*)()) &CMimrock::processWalk;
		}
	}

	return true;
}

void CMimrock::processSit()
{

}

void CMimrock::processWalk()
{

}

void CMimrock::processJump()
{

}

void CMimrock::processBounce()
{

}

void CMimrock::processStunned()
{

}

void CMimrock::process()
{
	performCollisions();
	processFalling();

	(this->*mp_processState)();
}


} /* namespace galaxy */
