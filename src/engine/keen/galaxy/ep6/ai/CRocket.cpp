#include "CRocket.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"


namespace galaxy
{
    
    
/*
$1AEAW  #Rocket needs card window [Best used on map]
$1B08W  #Rocket sit
$1B26W  #Rocket flying
$1B44W  #Rocket flying
$1B62W  #Rocket sit with Keen?
$1B80W  #Keen flying with rocket (Invisible) 
*/

enum ROCKETACTIONS
{
A_ROCKET_SIT = 0,
A_ROCKET_FLY = 1
};


const int MOVE_SPEED = 50;


CRocket::CRocket(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CGalaxyActionSpriteObject(pmap, foeID, x, y, 0),
CMoveTarget(m_Pos, xDirection, yDirection),
mpCarriedPlayer(nullptr)
{
	mActionMap[A_ROCKET_SIT] = (void (CGalaxyActionSpriteObject::*)()) &CRocket::processSitting;
	mActionMap[A_ROCKET_FLY] = (void (CGalaxyActionSpriteObject::*)()) &CRocket::processFlying;
		
	solid = false;
		
	setupGalaxyObjectOnMap(0x1B08, A_ROCKET_SIT);
	
	fetchInitialDir(xDirection, yDirection, *mp_Map);
	detectNextTarget(target, xDirection, yDirection);
}


void CRocket::processSitting()
{

}

void CRocket::processFlying()
{
    int xBlockPos = target.x - getXPosition();
    int yBlockPos = target.y - getYPosition();
    
    const int xBlockPosAbs = (xBlockPos<0) ? -xBlockPos : xBlockPos;
    const int yBlockPosAbs = (yBlockPos<0) ? -yBlockPos : yBlockPos;
    
    
    if( xBlockPosAbs < MOVE_SPEED && yBlockPosAbs < MOVE_SPEED )
    {
	const Uint16 object = mp_Map->getPlaneDataAt(2, target.x, target.y);
	
	Vector2D<int> speed(xBlockPos, yBlockPos);	    
	moveDir(speed);
	playSound(SOUND_ROCKET_DRIVE);
	
	// Happens when the rocket find a place where to stop
	if(object == 0x6A || object == 0x69)
	{
	    xDirection = CENTER;
	    yDirection = UP;
	    setAction(A_ROCKET_SIT);
	    detectNextTarget(target, xDirection, yDirection);
	    
	    Vector2D<int> newPlayerPos = m_Pos;
	    
	    newPlayerPos.y = getYDownPos();
	    newPlayerPos.x = getXRightPos();
	    
	    mpCarriedPlayer->moveToForce(newPlayerPos);
	    mpCarriedPlayer->solid = true;
	    mpCarriedPlayer->dontdraw = false;
	    mpCarriedPlayer = nullptr;
	    return;
	}

	readDirection(object, xDirection, yDirection );
	
	// If there is an object that changes the direction of the rocket, apply it!	
	detectNextTarget(target, xDirection, yDirection);		
    }
    
    Vector2D<int> speed;
    
    if(yDirection == UP)
    {
	speed.y = -MOVE_SPEED;
    }
    else if(yDirection == DOWN)
    {
	speed.y = MOVE_SPEED;
    }    
    
    if(xDirection == RIGHT)
    {
	speed.x = MOVE_SPEED;
    }
    else if(xDirection == LEFT)
    {
	speed.x = -MOVE_SPEED;
    }
    
    if(mpCarriedPlayer != nullptr)
    {
	mpCarriedPlayer->moveDir(speed);
    }
    moveDir(speed);
}

void CRocket::getTouchedBy(CSpriteObject& theObject)
{
	if(getActionNumber(A_ROCKET_FLY))
		return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
	    if(player->m_Inventory.Item.m_special.ep6.rocketKeycard > 0)
	    {
		setAction(A_ROCKET_FLY);
		playSound(SOUND_ROCKET_LAUNCH);
		mpCarriedPlayer = player;
		mpCarriedPlayer->solid = false;
		mpCarriedPlayer->dontdraw = true;
	    }
	    else
	    {
		    // Tell the player he cannot climb yet
            showMsgWithBmp(g_pBehaviorEngine->getString("KEEN_KEYCARD_REQUIRED"), 29, RIGHT);
		    player->moveYDir((1<<CSF)/2);		
	    }
	}
}

void CRocket::process()
{
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

    
    
};
