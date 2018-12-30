#include "CGik.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>


namespace galaxy
{

/*
$2604W  #Gik walking 0
$2622W  #Gik walking
$2640W  #Gik walking
$265EW  #Gik jumping 3
$267CW  #Gik land slide 4
$269AW  #Gik sliding 5
$26B8W  #Gik sliding
*/

enum GIK_ACTION
{
A_GIK_WALK = 0,
A_GIK_JUMP = 3,
A_GIK_LAND = 4,
A_GIK_SLIDE = 5
};

//constexpr int MAX_BOUNCE_BOOST = -115;

constexpr int WALK_SPEED = 20;
constexpr int JUMP_X_SPEED = 40;
constexpr int LAND_X_SPEED = 40;
constexpr int SLIDE_X_SPEED = 40;

const int CSF_DISTANCE_TO_JUMP = 10<<CSF;

const int GIK_JUMP_SPEED = 100;
const int GIK_SILDE_TIME = 100;

CGik::CGik(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxyActionSpriteObject(pmap, foeID, x, y, 0)
{
    mActionMap[A_GIK_WALK]  = static_cast<GASOFctr>(&CGik::processWalk);
    mActionMap[A_GIK_JUMP]  = static_cast<GASOFctr>(&CGik::processJump);
    mActionMap[A_GIK_LAND]  = static_cast<GASOFctr>(&CGik::processLand);
    mActionMap[A_GIK_SLIDE] = static_cast<GASOFctr>(&CGik::processSlide);

    setupGalaxyObjectOnMap(gBehaviorEngine.isDemo() ? 0x2000 : 0x2604, A_GIK_WALK);

    xDirection = LEFT;
}


bool CGik::isNearby(CSpriteObject &theObject)
{
    if( !getProbability(10) )
        return false;

    if( !getActionNumber(A_GIK_WALK) )
    {
        return true;
    }


    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        auto dist = player->getXMidPos()-getXMidPos();

        xDirection = ( dist < 0 ) ? LEFT : RIGHT;

        const auto distLen = std::abs(dist);

        if(!mpInteractPlayer)
        {
            if(distLen < CSF_DISTANCE_TO_JUMP)
            {
                m_timer = GIK_SILDE_TIME;
                yinertia = -GIK_JUMP_SPEED;

                setAction(A_GIK_JUMP);
            }
        }

    }

    return true;
}


void CGik::processWalk()
{        
    mXSpeed = WALK_SPEED;
    // If player is standing on bounder, he can control him a bit also
    /*if(mpInteractPlayer)
    {
        if(mpInteractPlayer->pSupportedbyobject)
        {
            const int mMidPX = mpInteractPlayer->getXMidPos();
            const int mMidX = getXMidPos();

            if( mMidPX > mMidX + (4<<STC) )
                xDirection = RIGHT;
            else if( mMidPX < mMidX - (4<<STC) )
                xDirection = LEFT;
            else
                xDirection = 0;
        }
    }*/
}

void CGik::processJump()
{    
    mXSpeed = JUMP_X_SPEED;

    if(yinertia > 0)
    {
        setAction(A_GIK_LAND);
    }

    // Will block the player when bounder touches him, but he is not riding him
    /*if( mpInteractPlayer )
    {
        if(!mpInteractPlayer->pSupportedbyobject)
        {
            const int mMidPX = mpInteractPlayer->getXMidPos();
            const int mMidX = getXMidPos();

            if( mMidPX > mMidX + (4<<STC) )
                mpInteractPlayer->blockedl = 1;
            else if( mMidPX < mMidX - (4<<STC) )
                mpInteractPlayer->blockedr = 1;
        }
    }*/

}

void CGik::processLand()
{
    mXSpeed = LAND_X_SPEED;

    std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    int xMid = getXMidPos();
    int y2 = getYDownPos();

    // When gik hits the floor set to land.
    if( blockedd || TileProperty[mpMap->at(xMid>>CSF, (y2+1)>>CSF)].bup>1 )
    {
        setAction( A_GIK_SLIDE );
        return;
    }
}


void CGik::processSlide()
{
    mXSpeed = SLIDE_X_SPEED;

    m_timer--;
    if( m_timer <= 0 )
    {
        setAction( A_GIK_WALK );
        m_timer = GIK_SILDE_TIME;
    }
}


void CGik::getTouchedBy(CSpriteObject &theObject)
{
    if(mIsDead || theObject.mIsDead)
        return;

    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        if(getActionNumber(A_GIK_LAND) || getActionNumber(A_GIK_SLIDE))
        {
            player->kill();
        }

        if(player->getActionNumber() != A_KEEN_ENTER_DOOR)
        {
            mpInteractPlayer = player;

            const int m_py2 = player->getYDownPos();
            const int m_y2 = getYUpPos()+(4<<STC);
            if( m_py2 <= m_y2 && !player->pSupportedbyobject && !player->m_jumpdownfromobject )
                player->pSupportedbyobject = this;
        }
    }
}

void CGik::movePlatLeft(const int amnt)
{
    // First move the object on platform if any
    if(mpInteractPlayer)
    {
        if(!mpInteractPlayer->m_jumpdownfromobject && mpInteractPlayer->pSupportedbyobject)
            mpInteractPlayer->moveLeft(amnt);
    }
}

void CGik::movePlatRight(const int amnt)
{
    // First move the object on platform if any
    if(mpInteractPlayer)
    {
        if(!mpInteractPlayer->m_jumpdownfromobject && mpInteractPlayer->pSupportedbyobject)
            mpInteractPlayer->moveRight(amnt);
    }
}

void CGik::movePlayerUp(const int amnt)
{
    // First move the object on platform if any
    if(mpInteractPlayer)
    {
        if(!mpInteractPlayer->m_jumpdownfromobject && mpInteractPlayer->pSupportedbyobject)
        {
            mpInteractPlayer->moveUp(amnt);
        }
    }
}

void CGik::movePlayerDown(const int amnt)
{
    // First move the object on platform if any
    if( mpInteractPlayer )
    {
        if(!mpInteractPlayer->m_jumpdownfromobject && mpInteractPlayer->pSupportedbyobject)
        {
            mpInteractPlayer->moveDown(amnt);
        }
    }
}

void CGik::moveLeft(const int amnt, const bool force)
{
    CSpriteObject::moveLeft(amnt, force);
    movePlatLeft(amnt);
}

void CGik::moveRight(const int amnt, const bool force)
{
    CSpriteObject::moveRight(amnt, force);
    movePlatRight(amnt);
}

void CGik::moveUp(const int amnt)
{
    CSpriteObject::moveUp(amnt);
    movePlayerUp(amnt);
}

void CGik::moveDown(const int amnt)
{
    CSpriteObject::moveDown(amnt);
    movePlayerDown(amnt);
}

void CGik::process()
{
    // Bounce
    performCollisions();
    performGravityMid();

    (this->*mp_processState)();

    if( blockedl )
    {
        xDirection = RIGHT;
    }
    else if(blockedr)
    {
        xDirection = LEFT;
    }

    // check if someone is still standing on the platform
    if( mpInteractPlayer )
    {
        if( !hitdetect(*mpInteractPlayer) )
        {
            mpInteractPlayer->pSupportedbyobject = nullptr;
            mpInteractPlayer->m_jumpdownfromobject = false;
            mpInteractPlayer = nullptr;
        }
        else if(mpInteractPlayer->pSupportedbyobject)
        {

            const int m_py2 = mpInteractPlayer->getYDownPos();
            const int m_y2 = getYUpPos()+(2<<STC);
            const int moveY = m_y2 - m_py2;

            if( moveY < 0 )
                movePlayerUp(-moveY);
            else if( moveY > 0 )
                movePlayerDown(moveY);

        }
    }

    
    if(xDirection == LEFT)
    {
        moveLeft(mXSpeed, false);
    }
    else if(xDirection == RIGHT)
    {
        moveRight(mXSpeed, false);
    }


    if(!processActionRoutine())
            exists = false;
}

}
