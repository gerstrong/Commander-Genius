/*
 * CShikadiMaster.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */

#include "CShikadiMaster.h"
#include "../../common/ai/CEnemyShot.h"
#include "../../common/ai/CBullet.h"
#include "CRedShot.h"

#include <base/interface/misc.h>
#include <engine/core/CBehaviorEngine.h>

/*
$2AF4W #Shikadi master stand
$2B12W #Shikadi master stand
$2B30W #Shikadi master stand
$2B4EW #Shikadi master stand
$2B6CW #Shikadi master shoot 4
$2B8AW #Shikadi master shoot (Make shot)
$2BA8W #Shikadi master teleport 6
$2BC6W #Shikadi master teleport
$2BE4W #Shikadi master teleport
$2C02W #Shikadi master teleport
 */


namespace galaxy {  
  
enum MASTERACTIONS
{
A_MASTER_STAND = 0,
A_MASTER_SHOOT = 4,
A_MASTER_TELEPORT = 6
};

constexpr int TIME_UNTIL_REACT = 100;
constexpr int TIME_UNTIL_SHOOT = 100;
constexpr int TIME_UNTIL_TELEPORTED = 50;


  
CShikadiMaster::CShikadiMaster(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y)
{  
    mActionMap[A_MASTER_STAND] = (GASOFctr) &CShikadiMaster::processStanding;
    mActionMap[A_MASTER_SHOOT] = (GASOFctr) &CShikadiMaster::processShooting;
    mActionMap[A_MASTER_TELEPORT] = (GASOFctr) &CShikadiMaster::processTeleporting;

    auto diff = gBehaviorEngine.mDifficulty;

    if(diff > HARD)
    {
        mMoreAgressive = true;
    }
    
    // Adapt this AI
    setupGalaxyObjectOnMap(0x2AF4, A_MASTER_STAND);

    xDirection = LEFT;
    mKeenAlignment = LEFT;

    loadLuaScript("shikadimaster");
}


void CShikadiMaster::processStanding()
{
    mTimer++;
    if(mTimer < TIME_UNTIL_REACT)
    {
        return;
    }

    mTimer = 0;
    
    if(getProbability(250))
        return;
    
    if(!blockedd)
        return;

    
    if(mTeleport)
    {
        setAction(A_MASTER_TELEPORT);
        mTeleport = false;
    }
    else
    {
        mTeleport = true;
        xDirection = mKeenAlignment;


        if(mMoreAgressive)
        {
            mp_processState = static_cast<GASOFctr>(&CShikadiMaster::processShootingAgressive);
        }
        else
        {
            setAction(A_MASTER_SHOOT);

            // ... and spawn a shot that might hurt Keen
            const int newX = (xDirection == LEFT) ? getXLeftPos()+(4<<STC) : getXRightPos()-(4<<STC);
            spawnObj( new CEnemyShot(mpMap, 0,
                                        newX, getYUpPos()+(8<<STC),
                                        0x2C3E, xDirection, CENTER,  150, mSprVar) );

        }
    }
}


void CShikadiMaster::processShootingAgressive()
{
  // Shoot many times.
  if(mTimer%16 == 0)
  {
      direction_t newXDir = xDirection<0 ? LEFT : RIGHT;
      direction_t newYDir = swapYDir ? UP : DOWN;
      swapYDir = !swapYDir;
      int newX = 	xDirection == RIGHT ? getXRightPos() : getXLeftPos();
      int newY = 	getYPosition() + 0x300;

          playSound(GameSound::ROBORED_SHOOT);

          spawnObj( new CRedShot( getMapPtr(),
                                  0,
                                  newX, newY,
                                  newXDir, newYDir ) );


  }

  mTimer++;
  if(mTimer < TIME_UNTIL_SHOOT)
    return;

  mTimer = 0;

  setAction(A_MASTER_STAND);
  mp_processState = (GASOFctr) &CShikadiMaster::processStanding;
}



void CShikadiMaster::processShooting()
{
    mTimer++;
    if(mTimer < TIME_UNTIL_SHOOT)
        return;
    
    mTimer = 0;
        
    this->playSound(GameSound::MASTERSHOT);
    setAction(A_MASTER_STAND);        
}


void CShikadiMaster::processTeleporting()
{
    if(!mpPlayer)
	return;
    
    mTimer++;
    if(mTimer < TIME_UNTIL_TELEPORTED)
        return;
    
    mTimer = 0;
    
    
/*
 
	int xpos_0, ypos_0, tx, ty, tx2, ty2, tries;
	int _far *t;

	xpos_0 = o->xpos;
	ypos_0 = o->ypos;

	GetNewObj(1);
	new_object->xpos = o->xpos;
	new_object->ypos = o->ypos;
	o->xmove = 0x30;
	CheckGround(new_object,ACTION_MASTERSPARKS0);

	GetNewObj(1);
	new_object->xpos = o->xpos;
	new_object->ypos = o->ypos;
	o->xmove = -0x30;
	CheckGround(new_object,ACTION_MASTERSPARKS0);
	*/

    this->playSound(GameSound::MASTERTELE);

	int triesLeft = 10;

	while (triesLeft > 0) 
	{
	    const unsigned int tx = ((rand()%(mpMap->m_width<<CSF))/8 + mpPlayer->getXMidPos() - (0x10<<STC))>>CSF;
	    const unsigned int ty = ((rand()%(mpMap->m_height<<CSF))/8 + mpPlayer->getYUpPos() - (0x10<<STC))>>CSF;
	    
        if (ty < 2 || tx < 2 || mpMap->m_width-5 < tx || mpMap->m_height-5 < ty)
        {
            rand();
            triesLeft--;
            continue;
        }

	    const int testBoxX1 = (tx - 1)<<CSF;
	    const int testBoxX2 = (tx + 4)<<CSF;
	    const int testBoxY1 = (ty - 1)<<CSF;
	    const int testBoxY2 = (ty + 4)<<CSF;
	    
	    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
	    
	    bool allow_teleport = true;
	    
	    for (int newy = testBoxY1; newy <= testBoxY2; newy += (1<<CSF) ) 
	    {
			for (int newx = testBoxX1; newx <= testBoxX2; newx += (1<<CSF) )
			{
				const int tile = mpMap->getPlaneDataAt(1, newx, newy);				
				auto &prop = Tile[tile];
				
				if ( (prop.behaviour & 0x80) || 
				    prop.bup || prop.bright || 
                    prop.bleft || prop.bdown )
				{
                  // don't spawn inside a tile, or behind a hidden tile
				  triesLeft--;
				  allow_teleport = false;
				  break;			  
				}
			}
			
			if(!allow_teleport)  break;			
	    }
	    
	    if(!allow_teleport)  continue;
	    
        // make it through previous nested loop == successful tele
	    //KeenXVel = KeenYVel = 0;
	    moveToForce(tx<<CSF, ty<<CSF);
	    setAction(A_MASTER_STAND);
	    return;
	}

	// couldn't find a suitable spawn point, so reset to default master behaviour
	rand();
	
	setAction(A_MASTER_STAND);
	/*Rand();
	o->action = ACTION_MASTER0;
	o->xpos = xpos_0 - 1; o->ypos = ypos_0;
	KeenXVel = 1; KeenYVel = 0;
	return; 
 
	*/
}



bool CShikadiMaster::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignment = LEFT;
		else
			mKeenAlignment = RIGHT;
		
		mpPlayer = player;
	}
	
	return true;
}

void CShikadiMaster::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.mIsDead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}


void CShikadiMaster::process()
{
	performCollisions();
	
	performGravityMid();

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
