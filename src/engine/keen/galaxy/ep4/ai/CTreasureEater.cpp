/*
 * CTreasureEater.cpp
 *
 *  Created on: 17.07.2012
 *      Author: gerstong
 */

#include "CTreasureEater.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include "../../common/ai/CSpriteItem.h"
#include "../../common/ai/CItemEffect.h"
#include <base/utils/misc.h>


namespace galaxy {


const int A_SMIRKY_LOOK = 0;
const int A_SMIRKY_TELEPORT = 2;
const int A_SMIRKY_HOP = 10;
const int A_SMIRKY_STUNNED = 14;

const int HOP_INERTIA = 120;
const int SMIRKY_HOP_TIMER = 10;
const int SMIRKY_XSPEED = 40;


CTreasureEater::CTreasureEater(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0),
mTeleported(false),
mStolen(false)
{
	mActionMap[A_SMIRKY_LOOK]     = (GASOFctr) &CTreasureEater::processLooking;
	mActionMap[A_SMIRKY_TELEPORT] = (GASOFctr) &CTreasureEater::processTeleporting;
	mActionMap[A_SMIRKY_HOP]      = (GASOFctr) &CTreasureEater::processHopping;
	mActionMap[A_SMIRKY_STUNNED]  = (GASOFctr) &CStunnable::processGettingStunned;

	setupGalaxyObjectOnMap( 0x31E2, A_SMIRKY_LOOK );
	xDirection = LEFT;
	
    GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);
	processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
	performCollisions();
	
	if(!processActionRoutine())
	{
	  exists = false;
	}
	
	honorPriority = false;
}


bool CTreasureEater::isNearby(CSpriteObject &theObject)
{
  	if( !getProbability(80) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			xDirection = LEFT;
		else
			xDirection = RIGHT;
	}

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
		setActionSprite();
		dead = true;
		theObject.dead = true;
	}

	// Here go the items
	if( dynamic_cast<CSpriteItem*>(&theObject) )
	{
		mStolen = true;
		theObject.exists = false;
	}
}


void CTreasureEater::processLooking()
{
	if( mTimer < SMIRKY_HOP_TIMER )
	{
		mTimer++;
		return;
	}
	else
	{
		mTimer = 0;
	}

	if(mEndOfAction)
	{
		if( getProbability(800) )
		{
			setAction(A_SMIRKY_HOP);
			yinertia = -HOP_INERTIA;
		}
		else
		{
			setActionForce(A_SMIRKY_LOOK);
		}
	}
}

void CTreasureEater::processTeleporting()
{      
	if( getActionStatus(A_SMIRKY_HOP) )
	{
	  moveToForce(mDestination);
	  setAction(A_SMIRKY_HOP);
	  mStolen = false;
	  mTeleported = false;
	  yinertia = -HOP_INERTIA;
	}
}

bool CTreasureEater::lookForNextDestination()
{  
  const unsigned int height = mp_Map->m_height-2;
  const unsigned int width = mp_Map->m_width-2;
  
  for( unsigned int y = 2 ; y<height  ; y++ )
  {
    for( unsigned int x = 2 ; x<width  ; x++ )
    {
	const unsigned int low = 61;
	const unsigned int high = 76;
	for( unsigned int item = low ; item <= high ; item++ )
	{
	   if(mp_Map->at(x,y,2) == item)
	   {
	     unsigned int newY = getYPosition()>>CSF;
	      if(mp_Map->at(x,newY,1) == 0)
	      {
		mDestination.x = x<<CSF;
		mDestination.y = getYPosition();
		return true;
	      }
	   }
	}
    }    
  }
  
  return false;
}


void CTreasureEater::processHopping()
{
	moveXDir(SMIRKY_XSPEED*xDirection);
	if(yinertia >= 0)
	{
		if(blockedd || onslope)
		{
			if(mStolen)
			{
			      setAction( A_SMIRKY_TELEPORT );
				lookForNextDestination();
				mTeleported = true;
			}
			else
			{
			      setAction( A_SMIRKY_LOOK );
			}

			return;
		}
	}
	
}



void CTreasureEater::checkForItem()
{
	int l_x = getXLeftPos();
	int l_y = getYUpPos();
	int l_w = getXRightPos() - l_x;
	int l_h = getYDownPos() - l_y;

	// So far we have tile item support only!
	for( Uint32 i=21 ; i<=27 ; i++ )
	{
		if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 2<<STC))
		{
			const int lc_x = l_x>>CSF;
			const int lc_y = l_y>>CSF;
			const int ep = g_pBehaviorEngine->getEpisode();
			
			mp_Map->setTile( lc_x, lc_y, 0, true, 1 );			
			spawnObj( new CItemEffect(mp_Map, 0, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[ep-4][4+i-21], FADEOUT) );
			mStolen = true;
			
		}
	}
}


void CTreasureEater::process()
{
	performCollisions();

	if(!inhibitfall)
		performGravityMid();

	checkForItem();

	/*if( blockedl )
		xDirection = RIGHT;
	else if( blockedr )
		xDirection = LEFT;*/	

	(this->*mp_processState)();

	if( !getActionNumber(A_SMIRKY_STUNNED) )
	{
		processActionRoutine();
	}
	else // TODO: This a workaround, because the last known action doesn't seem to be triggered
	{
		sprite = 231;
	}
}



} /* namespace galaxy */
