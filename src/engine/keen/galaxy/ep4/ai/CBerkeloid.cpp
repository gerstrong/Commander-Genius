/*
 * CBerkeloid.cpp
 *
 *  Created on: 16.07.2012
 *      Author: gerstong
 */

#include "CBerkeloid.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include <base/utils/misc.h>

namespace galaxy {

const int A_BERKELOID_MOVING = 0;
const int A_BERKELOID_THROW = 4;

const int BERKELOID_TIME = 15;
	

CBerkeloid::CBerkeloid(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0)
{
	mActionMap[A_BERKELOID_MOVING] = &CBerkeloid::processMoving;
	mActionMap[A_BERKELOID_THROW] = &CBerkeloid::processThrowing;

	setupGalaxyObjectOnMap(0x2AF8, A_BERKELOID_MOVING);
	inhibitfall = true;

	performCollisions();
	processActionRoutine();
	
	if(blockedl || blockedr || blockedd)
	{
	  moveUp(8<<STC);
	}
}

void CBerkeloid::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
    {
		mpProcessState = mActionMap[ActionNumber];
    }
	else
    {
        setActionForce(0); // When the action-map is incomplete. Some mods have that problem
    }
}


void CBerkeloid::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.mIsDead )
    {
		return;
    }

    if( auto *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		bullet->mIsDead = true;
	}

    if( auto *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}

bool CBerkeloid::isNearby(CSpriteObject &theObject)
{
    if( !getProbability(15) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
        {
			xDirection = LEFT;
        }
		else
        {
			xDirection = RIGHT;
        }
	}

	return true;
}



void CBerkeloid::processMoving()
{
    const auto x1 = m_Pos.x+m_BBox.x1;
    const auto x2 = m_Pos.x+m_BBox.x2;
    const auto y2 = m_Pos.y+m_BBox.y2;

    turnAroundOnCliff( x1, x2, y2+(1<<CSF) );

    moveYDir(30*mVertMoveDir);

	if( mTimer < BERKELOID_TIME )
	{
		mTimer++;
		return;
	}
	else
	{
		mTimer = 0;
	}

    mVertMoveDir = -mVertMoveDir;

	// Chance to throw a flame
    if( getProbability(45) )
	{
		setAction( A_BERKELOID_THROW );
		playSound( SOUND_BERKELOID_WINDUP );
		CBerkFlame *flame = new CBerkFlame( getMapPtr(), getXMidPos(), getYUpPos(), xDirection );
		gEventManager.add( new EventSpawnObject( flame ) );
		return;
	}

}

void CBerkeloid::processThrowing()
{
	if( getActionStatus(A_BERKELOID_MOVING) )
		setAction(A_BERKELOID_MOVING);
}


void CBerkeloid::process()
{  
    calcBoundingBoxes();

    blockedu = true;
    blockedd = true;

    const auto abs_x1 = m_Pos.x+m_BBox.x1;
    const auto abs_x2 = m_Pos.x+m_BBox.x2;
    const auto abs_y1 = m_Pos.y+m_BBox.y1;
    const auto abs_y2 = m_Pos.y+m_BBox.y2;

    // Left/Right walls
    blockedl = checkSolidL(abs_x1, abs_x2, abs_y1, abs_y2);
    blockedr = checkSolidR(abs_x1, abs_x2, abs_y1, abs_y2);

	(this->*mpProcessState)();        

	if( blockedl )
    {
		xDirection = RIGHT;
    }
	else if(blockedr)
    {
		xDirection = LEFT;
    }

	if(!processActionRoutine())
    {
			exists = false;
    }
}



 ////////////////////////////////// End of Berkeloid //////////////////////////////////

/*
 *
 * Flame Part of the Berkeloid
 *
 */
const int A_FLAME_THROWN = 0;
const int A_FLAME_LANDED = 2;

const int FLAME_INERTIAX = 150;

CBerkFlame::CBerkFlame(CMap *pmap, Uint32 x, Uint32 y, const int xDir) :
CGalaxySpriteObject(pmap, 0, x, y, 0)
{
	mActionMap[A_FLAME_THROWN] = &CBerkFlame::processThrown;
	mActionMap[A_FLAME_LANDED] = &CBerkFlame::processLanded;

	xDirection = xDir;
	xinertia = FLAME_INERTIAX;

	setupGalaxyObjectOnMap(0x2CD8, A_FLAME_THROWN);
}


void CBerkFlame::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mpProcessState = mActionMap[ActionNumber];
	else
		setActionForce(0); // This might happen, when the action-map is incomplete
}


void CBerkFlame::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.mIsDead )
		return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


void CBerkFlame::processThrown()
{
	if(blockedd)
		setAction(A_FLAME_LANDED);

	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( xinertia );
	else
		moveLeft( xinertia );

	xinertia--;
}

void CBerkFlame::processLanded()
{

}


void CBerkFlame::process()
{

	processFalling();

	performCollisions();

	(this->*mpProcessState)();

	if(!processActionRoutine())
			exists = false;

}


} /* namespace galaxy */
