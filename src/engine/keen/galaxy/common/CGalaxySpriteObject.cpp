/*
 * CGalaxySpriteObject.cpp
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#include "CGalaxySpriteObject.h"
#include "engine/core/CBehaviorEngine.h"

#include "../res/EGAStructs.h"

static int spriteOffset;

CGalaxySpriteObject::CGalaxySpriteObject(CMap *pmap, const Uint16 foeID,
                                         Uint32 x, Uint32 y, const int sprVar) :
CSpriteObject(pmap, x, y, sprVar),
mFoeID(foeID),
m_ActionTicker(0),
m_ActionNumber(0),
m_ActionBaseOffset(0x0),
nextX(0),
nextY(0),
topTI(0),
bottomTI(0),
leftTI(0),
rightTI(0),
user1(0),
user2(0),
user3(0),
user4(0),
m_climbing(false),
mClipped(false),
m_jumpdown(false),
mEndOfAction(false)
{
    EpisodeInfoStruct* eiStructPtr = g_pBehaviorEngine->getEpisodeInfoStructRef(g_pBehaviorEngine->getEpisode());
    spriteOffset = eiStructPtr->IndexSprites;
}

void CGalaxySpriteObject::setupGalaxyObjectOnMap(const size_t ActionBaseOffset,
						 const size_t ActionNumber )
{
	m_ActionBaseOffset = ActionBaseOffset;
	m_climbing = false;
	m_jumped = false;

	setActionForce(ActionNumber);

	setActionSprite();
	
	calcBoundingBoxes();
		
	alignToTile();
	
	performCollisions();
	
	if(!processActionRoutine())
			exists = false;
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
	CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
	processFallPhysics(Physics.fallspeed_increase/4);
}

void CGalaxySpriteObject::performGravityMid()
{
	CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
	processFallPhysics(3*Physics.fallspeed_increase/4);
}

void CGalaxySpriteObject::performGravityHigh()
{
	CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
	processFallPhysics(Physics.fallspeed_increase);
}

void CGalaxySpriteObject::performInverseGravityHigh()
{
	CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
	processFallPhysics(-Physics.fallspeed_increase);
}




/**
 * processes falling of an object. Can be player or any other foe
 */
void CGalaxySpriteObject::processFalling()
{
	if(m_climbing)
		return;

	CSpriteObject::processFalling();
}



int CGalaxySpriteObject::checkSolidU(int x1, int x2, int y1, const bool push_mode )
{
	if(hitdetectWithTilePropertyHor(1, x1, x2, y1-COLISION_RES, 1<<CSF))
	    return 0;
    
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y1 -= COLISION_RES;

	// Check for sloped tiles here. They must be handled differently
	if(solid)
	{
		int8_t blocked;

		if(m_climbing)
		{
			x1 += 4*COLISION_RES;
			x2 -= 4*COLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked == 17 && m_climbing)
				return 0;

			if( blocked >= 2 && blocked <= 7 && checkslopedU(c, y1, blocked))
				return blocked;
		}

		blocked = TileProperty[mp_Map->at(x2>>CSF, y1>>CSF)].bdown;
		if( blocked >= 2 && blocked <= 7 && checkslopedU(x2, y1, blocked ))
			return 1;

		if(blocked == 17 && m_climbing)
			return 0;
	}

	return CSpriteObject::checkSolidU(x1, x2, y1+COLISION_RES, push_mode);
}


int CGalaxySpriteObject::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y2 += COLISION_RES;

	// Check for sloped tiles here. They must be handled differently
	if(solid)
	{
		int8_t blockedu;

		if(m_climbing)
		{
			x1 += 4*COLISION_RES;
			x2 -= 4*COLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blockedu = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if( blockedu == 17 && m_climbing)
				return 0;

			if( blockedu >= 2 && blockedu <= 7 && checkslopedD(c, y2, blockedu) )
				return blockedu;
		}

		blockedu = TileProperty[mp_Map->at(x2>>CSF, y2>>CSF)].bup;

		if(blockedu == 17 && m_climbing)
			return 0;

		if( blockedu >= 2 && blockedu <= 7 && checkslopedD(x2, y2, blockedu)  )
			return blockedu;
	}


	if( ( (y2>>STC) != ((y2>>CSF)<<TILE_S) ) && !push_mode )
		return 0;


	// Check for down from the object
	if(solid)
	{
		int8_t blocked;
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if(blocked)
			{
				if( blocked < 2 || blocked > 7 )
				{
					int8_t blockedd = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bdown;

					if(blockedd == 0 && m_jumpdown)
						return 0;

					return blocked;
				}
			}
		}

		blocked = TileProperty[mp_Map->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
		if(blocked)
		{
			if( blocked < 2 || blocked > 7 )
				return blocked;
		}
	}

	if( (Uint32)y2 > ((mp_Map->m_height)<<CSF) )
		exists=false; // Out of map?

	return 0;
}



////
// Action format (Galaxy only now...)
////
/**
 * So far only used in Galaxy. Here we performs some stuff for the Action format
 */
bool CGalaxySpriteObject::getActionNumber(int16_t ActionNumber)
{	return (m_ActionNumber==ActionNumber);	}

bool CGalaxySpriteObject::getActionStatus(int16_t ActionNumber)
{	return (m_Action.getActionFormat(m_ActionBaseOffset + 30*ActionNumber));	}

int16_t CGalaxySpriteObject::getActionNumber()
{	return m_ActionNumber;	}


void CGalaxySpriteObject::setActionForce(const size_t ActionNumber)
{
	mEndOfAction = false;
	m_ActionNumber = ActionNumber;
	m_Action.setActionFormat(m_ActionBaseOffset + 30*m_ActionNumber);
}

void CGalaxySpriteObject::setAction(size_t ActionNumber)
{
	if(m_ActionNumber == ActionNumber) return;
	setActionForce(ActionNumber);
}

// Sets the proper sprite of action format to the local object
void CGalaxySpriteObject::setActionSprite()
{
    int oldBoxY2 = 0;
    int oldBoxY1 = 0;
    
	if(sprite >= 0)
	{
        GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);
	    oldBoxY2 = rSprite.m_bboxY2;
	    oldBoxY1 = rSprite.m_bboxY1;
	}
    
	if(xDirection == LEFT || xDirection == 0)
		sprite = m_Action.spriteLeft-spriteOffset;
	else if(xDirection == RIGHT)
		sprite = m_Action.spriteRight-spriteOffset;
	
	
	// Check the lower box for better collisions and move the sprite whether needed
    GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);
	
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

	if( m_Action.type > 0 )
	{
		if(xDirection == LEFT )
			moveLeft( m_Action.velX<<1 );
		else if(xDirection == RIGHT )
			moveRight( m_Action.velX<<1 );

		if(yDirection == UP)
			moveUp( m_Action.velY<<1 );
		else if(yDirection == DOWN)
			moveDown( m_Action.velY<<1 );
	}

	if(mEndOfAction)
		return false;

	if( m_ActionTicker > m_Action.timer )
	{
		if( m_Action.timer != 0 )
		{
			if(m_Action.Next_action != 0)
				m_Action.setNextActionFormat();
			else
				mEndOfAction = true;
		}
		m_ActionTicker = 0;
	}
	else
	{
		m_ActionTicker += 2;
	}

	return !mEndOfAction;
}
