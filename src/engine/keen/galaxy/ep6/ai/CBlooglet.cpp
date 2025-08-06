#include "CBlooglet.h"

#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CSpriteItem.h"
#include "../../common/ai/CBullet.h"

namespace galaxy {
    
    
enum BLOOGLETACTIONS
{
A_BLOOGLET_RUN = 0,	/* Ordinary run action */
A_BLOOGLET_STUNNED = 4
};

constexpr int RUN_SPEED = 45;


CBlooglet::CBlooglet(std::shared_ptr<CMap> pMap, const Uint16 foeID, const int actionOff, const bool carriesGem,
		     const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y),
mCarriesGem(carriesGem)
{
	mActionMap[A_BLOOGLET_RUN] = (GASOFctr) &CBlooglet::processRunning;
	mActionMap[A_BLOOGLET_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(actionOff, A_BLOOGLET_RUN);
	
	xDirection = LEFT;
}



void CBlooglet::processRunning()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		moveRight( RUN_SPEED );
	}
	else
	{
		moveLeft( RUN_SPEED );
	}
}


void CBlooglet::getTouchedBy(CSpriteObject &theObject)
{
  if(mIsDead || theObject.mIsDead)
    return;
  
  CStunnable::getTouchedBy(theObject);
  
  // Was it a bullet? Than make it stunned.
  if( dynamic_cast<CBullet*>(&theObject) )
  {
    if(mCarriesGem)
    {
      // If blooglet carried a gem it should fall that case it must jump out! That is a sprite item which can fall on the floor.
      const int newX = getXMidPos();
      const int newY = getYUpPos();
      const Uint32 newSprite = 118+2*( mFoeID-0xB );
      auto *gem = new CSpriteItem(mpMap, mFoeID+0x2E, newX, newY, newSprite, 0,true);
      gem->honorPriority = false;
      spawnObj( gem );
      playSound(GameSound::JUMPED_GEM);
    }
    
    setAction(A_BLOOGLET_STUNNED);
    mIsDead = true;
    theObject.mIsDead = true;
  }
  
  if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
  {
    player->push(*this);
  }
}


int CBlooglet::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CBlooglet::process()
{
	performCollisions();
	
	performGravityHigh();

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();

}

    
    
};
