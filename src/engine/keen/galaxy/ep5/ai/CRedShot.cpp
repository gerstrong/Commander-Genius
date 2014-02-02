#include "CRedShot.h"

#include "../../common/ai/CPlayerBase.h"

/*
$27ACW #Robo Red shot
$27CAW #Robo Red shot
$27E8W #Robo Red shot smash
$2806W #Robo Red shot smash
 */

namespace galaxy {  

const Uint32 speed = 10;
const Uint32 hittime = 100;

enum REDSHOTACTIONS
{
A_REDSHOT_MOVE = 0,
A_REDSHOT_SMASH = 2
};

const int MOVE_X_SPEED = 200;
const int MOVE_Y_SPEED = 30;

const int TIME_UNTIL_SMASH = 20;

CRedShot::CRedShot(CMap* pmap, 
		   const Uint16 foeID, 
		   const Uint32 x, const Uint32 y, 
		   const int xDir, const int yDir) : 
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	xDirection = xDir;
	yDirection = yDir;
	
	mActionMap[A_REDSHOT_MOVE] = (GASOFctr) &CRedShot::processMove;
	mActionMap[A_REDSHOT_SMASH] = (GASOFctr) &CRedShot::smash;

	setupGalaxyObjectOnMap(0x27AC, A_REDSHOT_MOVE);
	setActionSprite();
	calcBoundingBoxes();
}

void CRedShot::processMove()
{
  if( blockedl || blockedr || blockedu || blockedd )
    setAction(A_REDSHOT_SMASH);

  moveXDir(xDirection*MOVE_X_SPEED);
  moveYDir(yDirection*MOVE_Y_SPEED);  
}

void CRedShot::smash()
{
  // just wait 
  mTimer++;
  if(mTimer < TIME_UNTIL_SMASH)
    return;
  
  dead = true;
  exists = false;
  
  mTimer = 0;  
}

void CRedShot::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
	  player->kill();
	  setAction(A_REDSHOT_SMASH);
	}
}


void CRedShot::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}


};
