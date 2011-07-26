/*
 * CDopeFish.cpp
 *
 *  Created on: 26.07.2011
 *      Author: gerstrong
 */

#include "CDopeFish.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy {

#define A_DOPEFISH_SWIM		0
#define A_DOPEFISH_EAT		2
#define A_DOPEFISH_BURP		3

CDopeFish::CDopeFish(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	setupGalaxyObjectOnMap(0x35C0, A_DOPEFISH_SWIM);
	mp_processState = &CDopeFish::processSwim;
	m_hDir = RIGHT;

}

void CDopeFish::processSwim()
{

}

void CDopeFish::processEat()
{

}

void CDopeFish::processBurp()
{

}

void CDopeFish::getTouchedBy(CObject &theObject)
{
	if(dead || theObject.dead)
		return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}

}

void CDopeFish::process()
{
	(this->*mp_processState)();
}

} /* namespace galaxy */
