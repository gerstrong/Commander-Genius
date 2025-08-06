#include "CFleex.h"

#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"

namespace galaxy
{

/*
$2C78W  #Fleex walking 0
$2C96W  #Fleex walking
$2CB4W  #Fleex track Keen 2
$2CD2W  #Fleex track Keen
$2CF0W  #Fleex track Keen
$2D0EW  #Fleex track Keen
$2D2CW  #Fleex look 6
$2D4AW  #Fleex look
$2D68W  #Fleex stunned 8
*/

enum FLEEXACTIONS
{
    A_FLEEX_WALK = 0,
    A_FLEEX_TRACK = 2,
    A_FLEEX_LOOK = 6,
    A_FLEEX_STUNNED = 8
};


constexpr int WALK_SPEED = 30;
constexpr int TRACK_SPEED = 120;
//constexpr int TIME_LOOKING = 200;
constexpr int TIME_UNTIL_LOOK = 200;
constexpr int DISTANCE_UNTIL_TRACK = 8<<CSF;


CFleex::CFleex(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
    CStunnable(pMap, foeID, x, y),
    mHealth(4),
    mTimer(0),
    mKeenAlignment(LEFT),
    mGoodTrackChance(false)
{

    mapFunction(A_FLEEX_WALK, &CFleex::processWalk);
    mapFunction(A_FLEEX_TRACK, &CFleex::processTrack);
    mapFunction(A_FLEEX_LOOK, &CFleex::processLook);
    mapFunction(A_FLEEX_STUNNED, &CFleex::processGettingStunned);
    
    setupGalaxyObjectOnMap(0x2C78, A_FLEEX_WALK);
    
    xDirection = LEFT;
}



void CFleex::processWalk()
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
    
    mTimer++;
    if( mTimer < TIME_UNTIL_LOOK )
        return;
    
    mTimer = 0;
    
    setAction(A_FLEEX_LOOK);
}


void CFleex::processTrack()
{
    // Move normally in the direction
    if( xDirection == RIGHT )
    {
        moveRight( TRACK_SPEED );
    }
    else
    {
        moveLeft( TRACK_SPEED );
    }
    
    if(getActionStatus(A_FLEEX_WALK))
    {
        setAction(A_FLEEX_WALK);
    }
}


void CFleex::processLook()
{
    if(getActionStatus(A_FLEEX_TRACK))
    {
        xDirection = mKeenAlignment;
        setAction(A_FLEEX_TRACK);
    }
}



bool CFleex::isNearby(CSpriteObject& theObject)
{
    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        if( player->getXMidPos() < getXMidPos() )
            mKeenAlignment = LEFT;
        else
            mKeenAlignment = RIGHT;


        const int objX = theObject.getXMidPos();
        const int objY = theObject.getYMidPos();
        const int fleexX = getXMidPos();
        const int fleexY = getYMidPos();

        mGoodTrackChance = false;

        if( objX < fleexX - DISTANCE_UNTIL_TRACK ||
                objX > fleexX + DISTANCE_UNTIL_TRACK )
            return false;

        if( objY < fleexY - DISTANCE_UNTIL_TRACK ||
                objY > fleexY + DISTANCE_UNTIL_TRACK )
            return false;

        mGoodTrackChance = true;
    }

    return true;
}


void CFleex::getTouchedBy(CSpriteObject& theObject)
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
            setAction(A_FLEEX_STUNNED);
            mIsDead = true;
        }
        else
        {
            blink(10);
        }
    }
    
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        player->kill();
    }
}



int CFleex::checkSolidD(int x1, int x2, int y2, const bool push_mode)
{
    turnAroundOnCliff( x1, x2, y2 );
    
    return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}



void CFleex::process()
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




  
  
}
