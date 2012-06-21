/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: Gerstrong and FCTW
 */

#include "CPlatform.h"
#include "CLogFile.h"

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, Uint32 x, Uint32 y, object_t type) :
CSpriteObject(pmap, x, y, type),
mp_CarriedPlayer(NULL)
{
	m_ActionBaseOffset = 0x316A;
}

void CPlatform::movePlatLeft(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->moveLeft(amnt);

	// Now move the platform itself.
	moveLeft(amnt);
}

void CPlatform::movePlatRight(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->moveRight(amnt);

	// Now move the platform itself.
	moveRight(amnt);
}

void CPlatform::movePlatUp(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->moveUp(amnt);

	// Now move the platform itself.
	moveUp(amnt);
}

void CPlatform::movePlatDown(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->moveDown(amnt);

	// Now move the platform itself.
	moveDown(amnt);
}

void CPlatform::process()
{
	// check if someone is still standing on the platform
	if(mp_CarriedPlayer)
	{
		if(!hitdetect(*mp_CarriedPlayer) || mp_CarriedPlayer->blockedu)
		{
			mp_CarriedPlayer->supportedbyobject = false;
			mp_CarriedPlayer->m_jumpdownfromobject = false;
			mp_CarriedPlayer = NULL;
		}
	}

	processActionRoutine();
}

void CPlatform::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		const int m_py2 = player->getYDownPos();
		const int m_y2 = getYUpPos()+(4<<STC);
		if( m_py2 <= m_y2 && !player->supportedbyobject && !player->m_jumpdownfromobject )
		{
			mp_CarriedPlayer = player;
			player->supportedbyobject = true;
		}
	}
}

}
