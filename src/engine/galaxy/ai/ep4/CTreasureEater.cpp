/*
 * CTreasureEater.cpp
 *
 *  Created on: 17.07.2012
 *      Author: gerstong
 */

#include "CTreasureEater.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/CBullet.h"
#include "misc.h"

namespace galaxy {


const int A_SMIRKY_LOOK = 0;
const int A_SMIRKY_TELEPORT = 2;
const int A_SMIRKY_HOP = 10;
const int A_SMIRKY_STUNNED = 14;


CTreasureEater::CTreasureEater(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CStunnable(pmap, foeID, x, y)
{
	mActionMap[A_SMIRKY_LOOK]    = (void (CStunnable::*)()) &CTreasureEater::processLooking;
	mActionMap[A_SMIRKY_TELEPORT]    = (void (CStunnable::*)()) &CTreasureEater::processTeleporting;
	mActionMap[A_SMIRKY_HOP]     = (void (CStunnable::*)()) &CTreasureEater::processHopping;
	mActionMap[A_SMIRKY_STUNNED] = &CStunnable::processGettingStunned;

	setupGalaxyObjectOnMap( 0x31E2, A_SMIRKY_LOOK );
	m_hDir = LEFT;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	performCollisions();
	processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
	if(!processActionRoutine())
			exists = false;

}


bool CTreasureEater::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(80) )
		return false;

	/*if( getActionNumber(A_SMIRKY_LOOK) )
	{

	}*/

	// identify one item and try to get it!
	/*if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			m_hDir = LEFT;
		else
			m_hDir = RIGHT;
	}*/

	return true;
}


void CTreasureEater::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead )
		return;


	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction( A_SMIRKY_STUNNED );
		dead = true;
		theObject.dead = true;
	}

	// Here go the items
}


void CTreasureEater::processLooking()
{
	if( getActionStatus(A_SMIRKY_HOP) )
	{
		setAction(A_SMIRKY_HOP);
	}

	if( getActionStatus(A_SMIRKY_TELEPORT) )
	{
		setAction(A_SMIRKY_TELEPORT);
	}

}

void CTreasureEater::processTeleporting()
{
	/*if( getActionStatus(A_SMIRKY_TELEPORT) )
		setAction(A_SMIRKY_TELEPORT);*/
}


void CTreasureEater::processHopping()
{
	if( getActionStatus(A_SMIRKY_LOOK) )
		setAction(A_SMIRKY_LOOK);
}

void CTreasureEater::process()
{
	performCollisions();
	processFalling();

	if( blockedl )
		m_hDir = RIGHT;
	else if( blockedr )
		m_hDir = LEFT;

	(this->*mp_processState)();

	processActionRoutine();
}



} /* namespace galaxy */
