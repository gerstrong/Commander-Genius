/*
 * CGalaxySpriteObject.cpp
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#include "CGalaxySpriteObject.h"
#include "engine/core/CBehaviorEngine.h"

#include "../res/EGAStructs.h"

#include <graphics/GsSprite.h>
#include <graphics/GsGraphics.h>


CGalaxySpriteObject::CGalaxySpriteObject(std::shared_ptr<CMap> pMap, const Uint16 foeID,
                                         const int x, const int y, const int sprVar) :
CSpriteObject(pMap, x, y, sprVar),
mFoeID(foeID)
{
    EpisodeInfoStruct* eiStructPtr = gBehaviorEngine.getEpisodeInfoStructRef();
    mSpriteOffset = int(eiStructPtr->IndexSprites);
}

void CGalaxySpriteObject::setupGalaxyObjectOnMap(const size_t ActionBaseOffset,
						 const size_t ActionNumber )
{
	m_ActionBaseOffset = ActionBaseOffset;
	mIsClimbing = false;
	m_jumped = false;

	setActionForce(ActionNumber);

	setActionSprite();
	
	calcBoundingBoxes();
		
	alignToTile();
	
	performCollisions();
	
	if(!processActionRoutine())
    {
        exists = false;
    }
}


/**
 * Some Physics
 */

void CGalaxySpriteObject::performPhysAccelHor( const int accX, const int velLimit )
{

	bool isNegative = (xinertia < 0);

	xinertia += accX;

	if( xinertia != isNegative )
	{
		isNegative = (xinertia < 0);
		xDirection = isNegative?-1:1;
	}

	if(xinertia < 0)
	{
		if( xinertia < -velLimit )
			xinertia = -velLimit;
	}
	else
	{
		if( xinertia > velLimit )
			xinertia = velLimit;
	}
}


void CGalaxySpriteObject::performPhysDampHorz()
{
	if (xinertia < 0)
	{
		xinertia++;
	}
	else if (xinertia > 0)
	{
		xinertia--;
	}
}

void CGalaxySpriteObject::performGravityLow()
{
	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
	processFallPhysics(Physics.fallspeed_increase/4);
}

void CGalaxySpriteObject::performGravityMid()
{
	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
	processFallPhysics(3*Physics.fallspeed_increase/4);
}

void CGalaxySpriteObject::performGravityHigh()
{
	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
	processFallPhysics(Physics.fallspeed_increase);
}

void CGalaxySpriteObject::performInverseGravityHigh()
{
	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
	processFallPhysics(-Physics.fallspeed_increase);
}




/**
 * processes falling of an object. Can be player or any other foe
 */
void CGalaxySpriteObject::processFalling()
{
	if(mIsClimbing)
		return;

	CSpriteObject::processFalling();
}



int CGalaxySpriteObject::checkSolidU(int x1, int x2, int y1, const bool push_mode )
{
	if(hitdetectWithTilePropertyHor(1, x1, x2, y1-COLLISION_RES, 1<<CSF))
	    return 0;
    
	std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

	y1 -= COLLISION_RES;

	// Check for sloped tiles here. They must be handled differently
    if(solid && !noclipping)
	{
		int8_t blocked;

		if(mIsClimbing)
		{
			x1 += 4*COLLISION_RES;
			x2 -= 4*COLLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLLISION_RES)
		{
            blocked = TileProperty[mpMap->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked == 17 && mIsClimbing)
				return 0;

			if( blocked >= 2 && blocked <= 7 && checkslopedU(c, y1, blocked))
				return blocked;
		}

        blocked = TileProperty[mpMap->at(x2>>CSF, y1>>CSF)].bdown;
		if( blocked >= 2 && blocked <= 7 && checkslopedU(x2, y1, blocked ))
			return 1;

		if(blocked == 17 && mIsClimbing)
			return 0;
	}

	return CSpriteObject::checkSolidU(x1, x2, y1+COLLISION_RES, push_mode);
}


int CGalaxySpriteObject::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    y2 += COLLISION_RES;

	// Check for sloped tiles here. They must be handled differently
    if(solid && !noclipping)
	{
		int8_t blockedu;

		if(mIsClimbing)
		{
			x1 += 4*COLLISION_RES;
			x2 -= 4*COLLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLLISION_RES)
		{
            blockedu = TileProperty[mpMap->at(c>>CSF, y2>>CSF)].bup;

			if( blockedu == 17 && mIsClimbing)
				return 0;

			if( blockedu >= 2 && blockedu <= 7 && checkslopedD(c, y2, blockedu) )
            {
				return blockedu;
            }
		}

        blockedu = TileProperty[mpMap->at(x2>>CSF, y2>>CSF)].bup;

		if(blockedu == 17 && mIsClimbing)
			return 0;

		if( blockedu >= 2 && blockedu <= 7 && checkslopedD(x2, y2, blockedu)  )
        {
			return blockedu;
        }
	}

    const int y2CSFed = (y2>>CSF);
    const int y2TILEed = (y2CSFed<<TILE_S);
    const int y2STCed = (y2>>STC);

    if( ( y2STCed != y2TILEed ) && !push_mode )
		return 0;


	// Check for down from the object
    if(solid && !noclipping)
	{
		int8_t blocked;
		for(int c=x1 ; c<=x2 ; c += COLLISION_RES)
		{
            blocked = TileProperty[mpMap->at(c>>CSF, y2>>CSF)].bup;

			if(blocked)
			{
				if( blocked < 2 || blocked > 7 )
				{
                    int8_t blockedd = TileProperty[mpMap->at(c>>CSF, y2>>CSF)].bdown;

					if(blockedd == 0 && m_jumpdown)
						return 0;

					return blocked;
				}
			}
		}

        blocked = TileProperty[mpMap->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
		if(blocked)
		{
			if( blocked < 2 || blocked > 7 )
				return blocked;
		}
	}

    checkMapBoundaryD(y2);

	return 0;
}



////
// Action format (Keen Galaxy only)
////
/**
 * So far only used in Galaxy. Here we performs some stuff for the Action format
 */
bool CGalaxySpriteObject::getActionNumber(int16_t ActionNumber)
{
    return (m_ActionNumber==ActionNumber);
}

bool CGalaxySpriteObject::isOneOfActionNumbers(const int from, const int to)
{
    for(int i=from ; i<to ; i++)
    {
        if(m_ActionNumber==i)
            return true;
    }
    return false;
}

bool CGalaxySpriteObject::getActionStatus(const int16_t ActionNumber)
{
    return (m_Action.getActionFormat(m_ActionBaseOffset + 30*ActionNumber));
}

int16_t CGalaxySpriteObject::getActionNumber()
{
    return int16_t(m_ActionNumber);
}


void CGalaxySpriteObject::setActionForce(const size_t ActionNumber)
{
	mEndOfAction = false;
	m_ActionNumber = ActionNumber;
	m_Action.setActionFormat(m_ActionBaseOffset + 30*m_ActionNumber);
}

void CGalaxySpriteObject::setAction(const size_t ActionNumber)
{
	if(m_ActionNumber == ActionNumber) return;
	setActionForce(ActionNumber);
}

// Sets the proper sprite of action format to the local object
void CGalaxySpriteObject::setActionSprite()
{
    int oldBoxY2 = 0;
    int oldBoxY1 = 0;
    
	if(mSpriteIdx >= 0)
	{
        GsSprite &rSprite = gGraphics.getSprite(mSprVar, mSpriteIdx);
	    oldBoxY2 = rSprite.m_bboxY2;
	    oldBoxY1 = rSprite.m_bboxY1;
	}
    
	if(xDirection == LEFT || xDirection == 0)
        mSpriteIdx = m_Action.spriteLeft-mSpriteOffset;
	else if(xDirection == RIGHT)
        mSpriteIdx = m_Action.spriteRight-mSpriteOffset;
	
	
	// Check the lower box for better collisions and move the sprite whether needed
    GsSprite &rSprite = gGraphics.getSprite(mSprVar, mSpriteIdx);
	
    int newBoxY2 = rSprite.m_bboxY2;
    int newBoxY1 = rSprite.m_bboxY1;
	
    const int diffY2 = oldBoxY2-newBoxY2;
    const int diffY1 = oldBoxY1-newBoxY1;

    if(diffY2 && oldBoxY2 && blockedd)
	{
        processMove(0, diffY2);
	}

    if(diffY1 && oldBoxY1 && blockedu)
	{
        processMove(0, diffY1);
	}
}


// This new function will setup the sprite based on the Action format
bool CGalaxySpriteObject::processActionRoutine()
{
	mEndOfAction = false;
	setActionSprite();

	// Check the Movement Parameter
	/*
	 *	This is how the game handles the sprite's movement;
	 *	and is important only with sprites that have movement
	 *	not associated with animation (See below.)The variable
	 *	can have any value between 0-4; zero is used for
	 *	sprites that don't move (Most common occurrence.),
	 *	1 for nonanimating sprites that may still need to move or hover in place,
	 *	2 is the stunned sprite value and is also used where smooth movement is needed
	 *	3 is used for 'fall' moves such as the Bounder or Mad Mushroom and
	 *	4 is used for sprites that must hit or land on the ground.
	 */


    /*
    Movement Parm       Behaviour Called...    Action Movements Applied...        Delay field is used?
    0                   Once                   Once                               Yes
    1                   Once                   Every frame                        Yes
    2                   Every frame            Not applied                        No
    3                   Every frame            Once                               Yes
    4                   Every frame            Every frame                        Yes
    */
	if( m_Action.movement_param > 0 )
	{
		if(xDirection == LEFT )
			moveLeft( m_Action.h_anim_move<<1 );
		else if(xDirection == RIGHT )
			moveRight( m_Action.h_anim_move<<1 );

		if(yDirection == UP)
			moveUp( m_Action.v_anim_move<<1 );
		else if(yDirection == DOWN)
			moveDown( m_Action.v_anim_move<<1 );
	}

	if(mEndOfAction)
		return false;


    // Calculate this timer correctly to the applied LPS value
    if(m_Action.delay)
    {
        if( m_ActionTicker > m_Action.delay )
        {
            if(m_Action.Next_action)
            {
                m_Action.setNextActionFormat();
            }
            else
            {
                mEndOfAction = true;
            }

            m_ActionTicker = 0;

            // In order to enable this, the AI Code of all the implementations must be changed first
            /*const int moveX = (m_Action.h_anim_move<<STC);
            const int moveY = (m_Action.v_anim_move<<STC);

            moveXDir(moveX);
            moveYDir(moveY);*/
        }

        m_ActionTicker++;
    }

	return !mEndOfAction;
}
