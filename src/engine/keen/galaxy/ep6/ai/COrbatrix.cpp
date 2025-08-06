#include "COrbatrix.h"

#include "../../common/ai/CBullet.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CItemEffect.h"
#include "fileio/KeenFiles.h"

#include <base/interface/misc.h>

namespace galaxy
{

/*
$27E6W  #Orbatrix float 0
$2804W  #Orbatrix moving 1
$2822W  #Orbatrix moving
$2840W  #Orbatrix curls 3
$285EW  #Orbatrix curls
$287CW  #Orbatrix uncurls 5
$289AW  #Orbatrix uncurls
$28B8W  #Orbatrix moving 7
$28D6W  #Orbatrix moving
$28F4W  #Orbatrix moving
$2912W  #Orbatrix moving
$2930W  #Orbatrix bouncing 11
$294EW  #Orbatrix bouncing
$296CW  #Orbatrix bouncing
$298AW  #Orbatrix bouncing 
*/


enum ORBATRIX_ACTION
{
A_ORBATRIX_FLOAT = 0,
A_ORBATRIX_MOVE = 1,
A_ORBATRIX_CURL = 3,
A_ORBATRIX_UNCURL = 5,
A_ORBATRIX_MOVE_ALT = 7,
A_ORBATRIX_BOUNCE = 11,
A_ORBATRIX_LAND = 4,
A_ORBATRIX_SLIDE = 5
};

constexpr int MAX_BOUNCE_BOOST = -150;
//constexpr int FLY_SPEED = 10;
constexpr int HOR_SPEED = 80;
//constexpr int VER_SPEED = 40;

constexpr int TIME_UNTIL_MOVE = 20;
constexpr int TIME_UNTIL_CURL = 20;
//constexpr int TIME_UNTIL_SLIDE = 20;
constexpr int TIME_UNTIL_UNCURL = 200;
constexpr int TIME_UNTIL_FLOAT = 20;

COrbatrix::COrbatrix(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar) :
CGalaxyActionSpriteObject(pMap, foeID, x, y, sprVar),
mTimer(0),
mGivesKey(false)
{
    mapFunction(A_ORBATRIX_FLOAT, &COrbatrix::processFloat);
    mapFunction(A_ORBATRIX_MOVE, &COrbatrix::processMove);
    mapFunction(A_ORBATRIX_CURL, &COrbatrix::processCurl);
    mapFunction(A_ORBATRIX_UNCURL, &COrbatrix::processUncurl);
    mapFunction(A_ORBATRIX_MOVE_ALT, &COrbatrix::processMove);
    mapFunction(A_ORBATRIX_BOUNCE, &COrbatrix::processBounce);
    mapFunction(A_ORBATRIX_LAND, &COrbatrix::processLand);
    mapFunction(A_ORBATRIX_SLIDE, &COrbatrix::processSlide);

    setupGalaxyObjectOnMap(0x27E6, A_ORBATRIX_FLOAT);

    xDirection = LEFT;        
    
    gs_byte *ptr = gKeenFiles.exeFile.getRawData();
    ptr += 0x114F2;
    
    const gs_byte endpattern[] =
    { 0x05, 0x00 };
              
    if(memcmp(endpattern, ptr, 2) == 0)
    {
      mGivesKey = true;
    }    
}

void COrbatrix::processFloat()
{
    mTimer++;
    if( mTimer < TIME_UNTIL_MOVE )  
	return;
    
    mTimer = 0;
    
    setAction(A_ORBATRIX_MOVE);
}
   
void COrbatrix::processMove()
{
    mTimer++;
    if( mTimer < TIME_UNTIL_CURL )  
	return;
    
    mTimer = 0;
    
    if(!mGivesKey)
    {
      setAction(A_ORBATRIX_CURL);    
    }
}

void COrbatrix::processCurl()
{
    if(getActionStatus(A_ORBATRIX_BOUNCE))
    {
	yinertia = MAX_BOUNCE_BOOST;
	setAction(A_ORBATRIX_BOUNCE);
    }
}

void COrbatrix::processUncurl()
{
    if(getActionStatus(A_ORBATRIX_SLIDE))
    {
	setAction(A_ORBATRIX_SLIDE);
    }    
}

void COrbatrix::processBounce()
{
    performGravityHigh();
    
    // Move normally in the direction
    if( xDirection == RIGHT )
    {
        moveRight( HOR_SPEED );
    }
    else
    {
        moveLeft( HOR_SPEED );
    }
    
    if(blockedd && yinertia >= 0)
    {
        yinertia = MAX_BOUNCE_BOOST;
        xDirection = -xDirection;
        playSound(GameSound::ORBATRIX_BUMP);
    }
    
    mTimer++;
    if( mTimer < TIME_UNTIL_UNCURL )
        return;
    
    mTimer = 0;
    
    setAction(A_ORBATRIX_SLIDE);
}

void COrbatrix::processLand()
{
    if(getActionStatus(A_ORBATRIX_SLIDE))
    {
	setAction(A_ORBATRIX_SLIDE);
    }    
}

void COrbatrix::processSlide()
{
    mTimer++;
    if( mTimer < TIME_UNTIL_FLOAT )  
	return;
    
    mTimer = 0;
    
    setAction(A_ORBATRIX_FLOAT);        
}



bool COrbatrix::isNearby(CSpriteObject &theObject)
{
    if(!getActionStatus(A_ORBATRIX_FLOAT))
	return true;
    
    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {	
	if( getProbability(600) )
	{	    
	    if( player->getXMidPos() < getXMidPos() )
		xDirection = LEFT;
	    else
		xDirection = RIGHT;
	}
    }	
	
    return true;
}



void COrbatrix::getTouchedBy(CSpriteObject& theObject)
{
    if(mIsDead || theObject.mIsDead)
        return;

    // Was it a bullet, make it zap/zot
    if( dynamic_cast<CBullet*>(&theObject) )
    {
        setAction(A_ORBATRIX_FLOAT);
        theObject.mIsDead = true;
        return;
    }
    
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        if(!mGivesKey)
        {
            // Only kill Keen, if Orbatrix bounces
            if(getActionNumber(A_ORBATRIX_BOUNCE))
            {
                player->kill();
            }
        }
        else
        {
            stItemGalaxy &Item = player->m_Inventory.Item;
            Item.m_gem.red++;
            spawnObj(new CItemEffect(mpMap, 0,
                                     getXPosition(), getYPosition(),
                                     got_sprite_item_pics[2][2], FADEOUT));
            mIsDead = true;
            exists = false;
        }
    }
}

void COrbatrix::process()
{
    performCollisions();
        
    if( blockedl )
	xDirection = RIGHT;
    else if(blockedr)
	xDirection = LEFT;
    
    if(!processActionRoutine())
	exists = false;
    
    (this->*mp_processState)();        
}


};
