#include "CNospike.h"
#include <base/interface/misc.h>

#include "../../common/ai/CPlayerLevel.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"
#include "../../menu/ComputerWrist.h"
#include "../../common/ai/CBullet.h"
#include "engine/core/mode/Scene.h"
#include "fileio/KeenFiles.h"


/*
$2460W  #Nospike sit 0
$247EW  #Nospike walks 1
$249CW  #Nospike walks
$24BAW  #Nospike walks
$24D8W  #Nospike walks
$24F6W  #Nospike charges 5
$2514W  #Nospike charges
$2532W  #Nospike charges
$2550W  #Nospike charges
$256EW  #Nospike standing (Pause before ?) 9
$258CW  #Nospike start ? 
$25AAW  #Nospike ?
$25C8W  #Nospike fall [Will not hit ground unless it has Nospike property] 12
$25E6W  #Stunned Nospike  13
*/
namespace galaxy {
  

enum NOSPIKEACTIONS
{
A_NOSPIKE_SIT = 0,
A_NOSPIKE_WALK = 1,	// Ordinary walk action 
A_NOSPIKE_CHARGE = 5,
A_NOSPIKE_STAND = 9,
A_NOSPIKE_FALL = 12,
A_NOSPIKE_STUNNED = 13
};


constexpr int WALK_SPEED = 30;
constexpr int CHARGE_SPEED = 60;
constexpr int TIME_UNTIL_WALK = 120;
constexpr int TIME_UNTIL_SIT = 120;
constexpr int CSF_DISTANCE_TO_CHARGE = 6<<CSF;


CNospike::CNospike(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CStunnable(pMap, foeID, x, y),
mHealth(0),
mTimer(0),
mKeenAlignment(LEFT),
mGoodChargeChance(false),
mCanFinishGame(false)
{
    mActionMap[A_NOSPIKE_SIT] = (GASOFctr) &CNospike::processSitting;
    mActionMap[A_NOSPIKE_WALK] = (GASOFctr) &CNospike::processWalking;
    mActionMap[A_NOSPIKE_CHARGE] = (GASOFctr) &CNospike::processCharging;
    mActionMap[A_NOSPIKE_STAND] = (GASOFctr) &CNospike::processStanding;
    //mActionMap[A_NOSPIKE_FALL] = (GASOFctr) &CNospike::processWalking;
    mActionMap[A_NOSPIKE_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
    
    setupGalaxyObjectOnMap(0x2460, A_NOSPIKE_SIT);
    
    xDirection = LEFT;
    

    gs_byte *ptr = gKeenFiles.exeFile.getRawData();
    ptr += 0x10D35;
    memcpy(&mHealth, ptr, 1 );
    
    ptr = gKeenFiles.exeFile.getRawData();
    ptr += 0x1256C;
    
    const gs_byte endpattern[] =
    { 0x02, 0x75, 0x30, 0xC7, 0x06, 0xAC, 0x75, 
      0x0C, 0x00, 0x5F, 0x5E, 0x5D, 0xCB };
              
    if(memcmp(endpattern, ptr, 13) == 0)
    {
      mCanFinishGame = true;
    }
    
}



void CNospike::processSitting()
{
    if(getActionStatus(A_NOSPIKE_WALK))
    {
	setAction(A_NOSPIKE_WALK);
    }
    
    mTimer++;
    if( mTimer < TIME_UNTIL_WALK )  
	return;
    
    mTimer = 0;
    
    setAction(A_NOSPIKE_WALK);    
}


void CNospike::processWalking()
{
    // Move normally in the direction
    if( xDirection == RIGHT )
    {
	moveRight( WALK_SPEED );
    }
    else
    {
	moveLeft( WALK_SPEED );
    }
    
    if(mGoodChargeChance)      
    {
      xDirection = mKeenAlignment;
      setAction(A_NOSPIKE_CHARGE);
    }
    
    mTimer++;
    if( mTimer < TIME_UNTIL_SIT )  
	return;
    
    mTimer = 0;
    
    setAction(A_NOSPIKE_SIT);     
}

void CNospike::processCharging()
{
    // Move normally in the direction
    if( xDirection == RIGHT )
    {
	moveRight( CHARGE_SPEED );
    }
    else
    {
	moveLeft( CHARGE_SPEED );
    }
    
    if(!mGoodChargeChance)      
    {
      xDirection = mKeenAlignment;
      setAction(A_NOSPIKE_STAND);
    }
    
    mTimer++;
    if( mTimer < TIME_UNTIL_WALK )  
	return;
    
    mTimer = 0;
    
    setAction(A_NOSPIKE_CHARGE); 
}

void CNospike::processStanding()
{
    mTimer++;
    if( mTimer < TIME_UNTIL_SIT )  
	return;
    
    mTimer = 0;
    
    setAction(A_NOSPIKE_WALK);     
}



bool CNospike::isNearby(CSpriteObject& theObject)
{
	if( !getProbability(10) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignment = LEFT;
		else
			mKeenAlignment = RIGHT;
		
		
		const int objX = theObject.getXMidPos();
		const int objY = theObject.getYMidPos();
		const int nospikeX = getXMidPos();
		const int nospikeY = getYMidPos();
		
		mGoodChargeChance = false;
		
		if( objX < nospikeX - CSF_DISTANCE_TO_CHARGE ||
			objX > nospikeX + CSF_DISTANCE_TO_CHARGE )
			return false;

		if( objY < nospikeY - CSF_DISTANCE_TO_CHARGE ||
			objY > nospikeY + CSF_DISTANCE_TO_CHARGE )
			return false;
		
		mGoodChargeChance = true;
	}

	return true;
}



void CNospike::getTouchedBy(CSpriteObject& theObject)
{
    if(mIsDead || theObject.mIsDead)
        return;
    
    CStunnable::getTouchedBy(theObject);
    
    // Was it a bullet? Then loose health.
    if( dynamic_cast<CBullet*>(&theObject) )
    {
        mHealth--;
        theObject.mIsDead = true;

        if(mHealth == 0)
        {
            setAction(A_NOSPIKE_STUNNED);

            if(!mCanFinishGame)
            {
                mIsDead = true;
            }
        }
        else
        {
            blink(10);
        }
    }
    
    
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        if( mCanFinishGame && getActionNumber(A_NOSPIKE_STUNNED) ) // This will only happen in the Keen 8 Mod!
        {            
            mIsDead = true;

            gEventManager.add(new OpenComputerWrist(4, true, false, -1, true));
            gEventManager.add(new EventEndGamePlay());
        }
        else
        {
            player->kill(false);
        }
    }
}



int CNospike::checkSolidD(int x1, int x2, int y2, const bool push_mode)
{
    turnAroundOnCliff( x1, x2, y2 );
    
    return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CNospike::process()
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
