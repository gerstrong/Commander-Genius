#include "CGik.h"

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

const int MAX_BOUNCE_BOOST = -115;
const int HOR_SPEED = 20;


CGik::CGik(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxyActionSpriteObject(pmap, foeID, x, y, 0),
mpInteractPlayer(nullptr)
{
    mActionMap[A_GIK_WALK] = (GASOFctr) &CGik::processWalk;
    mActionMap[A_GIK_JUMP] = (GASOFctr) &CGik::processJump;
    mActionMap[A_GIK_LAND] = (GASOFctr) &CGik::processLand;
    mActionMap[A_GIK_SLIDE] = (GASOFctr) &CGik::processSlide;

    setupGalaxyObjectOnMap(0x2604, A_GIK_WALK);

    xDirection = LEFT;
}

void CGik::processWalk()
{
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
    std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

    int xMid = getXMidPos();
    int y2 = getYDownPos();

    // When gik hits the floor set to land.
    if( blockedd || TileProperty[mp_Map->at(xMid>>CSF, (y2+1)>>CSF)].bup>1 )
    {
        setAction( A_GIK_LAND );
        return;
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

}


void CGik::processSlide()
{

}


void CGik::getTouchedBy(CSpriteObject &theObject)
{
    if(dead || theObject.dead)
        return;

    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        mpInteractPlayer = player;

        const int m_py2 = player->getYDownPos();
        const int m_y2 = getYUpPos()+(4<<STC);
        if( m_py2 <= m_y2 && !player->pSupportedbyobject && !player->m_jumpdownfromobject )
            player->pSupportedbyobject = this;
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
            mpInteractPlayer = NULL;
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
        moveLeft(HOR_SPEED, false);
    }
    else if(xDirection == RIGHT)
    {
        moveRight(HOR_SPEED, false);
    }


    if(!processActionRoutine())
            exists = false;
}

}
