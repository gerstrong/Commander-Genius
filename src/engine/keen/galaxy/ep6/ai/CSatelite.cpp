#include "CSatelite.h"

#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerWM.h"

/*
$1C70W  #Keen hang from satelite
$1C8EW  #Sateleite
$1CACW  #Sateleite
$1CCAW  #Sateleite
$1CE8W  #Sateleite 
*/

namespace galaxy
{
    

const int MOVE_SPEED = 40;

const int TILES_UNTIL_UNMOUNT = 8;


CSatelite::CSatelite(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CGalaxySpriteObject(pmap, foeID, x, y, 0),
CMoveTarget(m_Pos, xDirection, yDirection),
mpCarriedPlayer(nullptr),
mTilesUntilumount(0)
{
	solid = false;
		
	setupGalaxyObjectOnMap(0x1C8E, 0);
	
	m_Pos = target;
	
	fetchInitialDir(xDirection, yDirection, *mp_Map);
	detectNextTarget(target, xDirection, yDirection);
}


void CSatelite::processFlying()
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
	
	mTilesUntilumount++;

	if(mpCarriedPlayer != nullptr && mTilesUntilumount >= TILES_UNTIL_UNMOUNT)
	{    	    
	    const Uint16 spot1 = mp_Map->getPlaneDataAt(2, target.x, target.y+(1<<CSF) );
	    const Uint16 spot2 = mp_Map->getPlaneDataAt(2, target.x+(1<<CSF), target.y);
	    // Positions where keen migh mount or land
	    if(spot1 == 0x2D || spot2 == 0x2C)
	    {	    
		Vector2D<int> newPlayerPos = target;
		
		if(spot1 == 0x2D)
		{
		    newPlayerPos.y = target.y+(1<<CSF);
		}
		if(spot2 == 0x2C)
		{		    
		    newPlayerPos.x = target.x+(1<<CSF);
		}
		
		mTilesUntilumount = 0;
		mpCarriedPlayer->solid = true;
		mpCarriedPlayer->moveToForce(newPlayerPos);		
		mpCarriedPlayer->setMounted(false);
		mpCarriedPlayer = nullptr;
	    }
	}
	
	if(object == 0x59)
	{
	    xDirection = LEFT;
	    yDirection = CENTER;
	}
	else
	{
	    readDirection(object, xDirection, yDirection );
	}
	
	// If there is an object that changes the direction of the plat, apply it!
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
    
    moveDir(speed);
        
    if(mpCarriedPlayer != nullptr)
    {
	mPlayerPos = m_Pos;
	mPlayerPos.x += (16<<STC);
	mPlayerPos.y = getYDownPos()-(4<<STC);
	mpCarriedPlayer->moveTo(mPlayerPos);
    }
}

void CSatelite::getTouchedBy(CSpriteObject& theObject)
{
	if( CPlayerWM *player = dynamic_cast<CPlayerWM*>(&theObject) )
	{
	    if(mpCarriedPlayer == nullptr)
	    {	    
		if(mTilesUntilumount >= TILES_UNTIL_UNMOUNT)
		{
		    mpCarriedPlayer = player;
		    mpCarriedPlayer->setMounted(true);
		    playSound(SOUND_SATELITE_MOUNT);
		    mpCarriedPlayer->solid = false;
		    mTilesUntilumount = 0;
		}
	    }
	}
}

bool CSatelite::calcVisibility()
{
	return true;
}


void CSatelite::process()
{
	if(!processActionRoutine())
	    exists = false;
	
	if(mTilesUntilumount >= TILES_UNTIL_UNMOUNT)
	{
	    mTilesUntilumount = TILES_UNTIL_UNMOUNT;
	}
	
	processFlying();
}    
    
    
};
