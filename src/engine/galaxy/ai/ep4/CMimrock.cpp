/*
 * CMimrock.cpp
 *
 *  Created on: 18.09.2011
 *      Author: gerstrong
 */

#include "CMimrock.h"

namespace galaxy {

#define A_MIMROCK_SIT		0
#define A_MIMROCK_WALK		1
#define A_MIMROCK_JUMP		7
#define A_MIMROCK_BOUNCE	10
#define A_MIMROCK_STUNNED	11

CMimrock::CMimrock(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE),
CStunnable(pmap, x, y, OBJ_NONE)
{
	setupGalaxyObjectOnMap(0x343A, A_MIMROCK_SIT);
	mp_processState = (void (CStunnable::*)()) &CMimrock::processSit;
	m_hDir = NONE;
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
