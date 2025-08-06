/*
 * CCamera.h
 *
 *  Created on: 09.03.2010
 *      Author: pizza2004 and gerstrong
 *
 *  This camera class is used for the scrolling during the "gameplay".
 *  The old values of that used that class are now a structure part of CVideoDriver
 *  This class will be another object that will following the player
 */

#include "CCamera.h"
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include "engine/core/spritedefines.h"
#include "CBehaviorEngine.h"

int CCamera::mCamlead = 0;
bool CCamera::mCamLeadChange = false;
std::array<bool, 4> CCamera::mDontUseThisLead;


CCamera::CCamera(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y, CSpriteObject *p_attacher) :
CSpriteObject(pMap, x, y, 0)
{
    mRelcam.x = 0;
    mRelcam.y = 0;
    mSpriteIdx = BLANKSPRITE;
    solid = false;

    attachObject(p_attacher);
}

void CCamera::cycleCamlead()
{
    const int numPlayers = gBehaviorEngine.numPlayers();

    if( numPlayers == 1 ) // For one player this doesn't make sense to change
    {        
        playSound(GameSound::CANT_DO);
        return;
    }

    for(int i=0 ; i<numPlayers ; i++)
    {
        if( mCamlead<(numPlayers-1) )
            mCamlead++;
        else
            mCamlead = 0;

        if(!mDontUseThisLead[mCamlead])
            break;
    }


    mCamLeadChange = true;
}

void CCamera::attachObject(CSpriteObject *p_attacher)
{
    mp_AttachedObject = p_attacher;

    const int camId = mp_AttachedObject->getSpriteVariantIdx();
    mDontUseThisLead[camId] = false;
    resetCamLead();

}

void CCamera::setPosition(const GsVec2D<int>& newpos)
{
    const auto gameRes = gVideoDriver.getGameResolution();

    const auto screenWidth = gameRes.dim.x<<STC;
    const auto screenHeight = gameRes.dim.y<<STC;

    // Upper-left corner where the screen rendering start point
    int cam_x = newpos.x-(screenWidth/2);
    int cam_y = newpos.y-(screenHeight/2);

    int upCoord, downCoord, leftCoord, rightCoord;
    mpMap->fetchNearestHorBlockers(newpos.y, upCoord, downCoord);
    mpMap->fetchNearestVertBlockers(newpos.x, leftCoord, rightCoord);

    const int minimumEdgeDistX = leftCoord+(1<<CSF)+1;
    const int minimumEdgeDistY = upCoord+(1<<CSF)+1;
    const int maxWidth = rightCoord-(leftCoord+(1<<CSF));
    const int maxHeight = downCoord-(upCoord+(1<<CSF));

    if(cam_x<minimumEdgeDistX)
    {
        cam_x = minimumEdgeDistX;
    }
    else if(cam_x+screenWidth>rightCoord)
    {
        cam_x = rightCoord-screenWidth;
    }
    else if(cam_x+maxWidth>rightCoord)
    {
        cam_y = rightCoord-screenWidth;
    }

    if(maxHeight < screenHeight)
    {
        cam_y = minimumEdgeDistY;
    }
    else if(cam_y<minimumEdgeDistY)
    {
        cam_y = minimumEdgeDistY;
    }
    else if(cam_y+screenHeight>downCoord)
    {
        cam_y = downCoord-screenHeight;
    }

    moveToForce(newpos);

    // Only the lead camera may change the scroll position
    if(mp_AttachedObject && mCamlead != mp_AttachedObject->getPlayerIdx())
    {
        return;
    }

    mpMap->gotoPos(cam_x>>STC, cam_y>>STC);

    mpMap->mGamePlayPos = newpos;

    reAdjust();
}

void CCamera::process()
{
    if(mp_AttachedObject)
    {
        // Cycle Cam Code
        if( !mp_AttachedObject->mIsDead )
        {
            if(gInput.getPressedCommand(mCamlead, IC_CAMLEAD))
                cycleCamlead();
        }

        const int playerIdx = mp_AttachedObject->getPlayerIdx();

        if(playerIdx != mCamlead)
            return;
    }


    if(mCamLeadChange)
    {
        mCamLeadChange = false;

        setPosition(m_Pos);
    }

    if(mFreeze)
        return;

    SDL_Rect gamerect = gVideoDriver.getGameResolution().SDLRect();
    const int maxscrollx = (mpMap->m_width<<4) - gamerect.w - 32;
    const int maxscrolly = (mpMap->m_height<<4) - gamerect.h - 32;


    if(!mAttached)
    {	// This means, that there is no attached object. Let the camera scroll freely!
        const size_t movespeed = 100;
        if(gInput.getHoldedCommand(IC_LEFT))
            moveLeft(movespeed);
        else if(gInput.getHoldedCommand(IC_RIGHT))
            moveRight(movespeed);
        if(gInput.getHoldedCommand(IC_UP))
            moveUp(movespeed);
        else if(gInput.getHoldedCommand(IC_DOWN))
            moveDown(movespeed);
    }
    else
    {
        if(mp_AttachedObject == nullptr)
            return;

        mMoving = false;

        mpMap->mGamePlayPos = mp_AttachedObject->getMidPos();

        const Uint32 attached_x = mp_AttachedObject->getXPosition() + mRelcam.x;
        const Uint32 attached_y = mp_AttachedObject->getYPosition() + mRelcam.y;
        const Uint32 local_x = getXPosition();
        const Uint32 local_y = getYPosition();


        // Make the camera move and tell if it's scrolling through the mMoving variable
        if( attached_x > local_x )
        {
            moveRight( attached_x - local_x );
            mMoving |= true;
        }
        else if( attached_x < local_x )
        {
            moveLeft( local_x - attached_x );
            mMoving |= true;
        }

        if( attached_y > local_y )
        {
            moveDown( attached_y - local_y );
            mMoving |= true;
        }
        else if( attached_y < local_y )
        {
            moveUp( local_y - attached_y );
            mMoving |= true;
        }
    }

    const auto xPosStc = getXPosition()>>STC;
    const auto yPosStc = getYPosition()>>STC;

    // delta is how much we need to scroll in order to get the camera stalled
    auto scroll = mpMap->getMainScrollCoords();
    int delta_x = xPosStc-scroll.x;
    int delta_y = yPosStc-scroll.y;


    // The bounds are relatively scaled for bigger game resolutions
    const int scFacW = gamerect.w/320;
    const int scFacH = gamerect.h/200;

    st_camera_bounds CameraBounds = gVideoDriver.getCameraBounds();
    const int left = CameraBounds.left*scFacW;
    const int up = CameraBounds.up*scFacH;
    const int right = CameraBounds.right*scFacW;
    const int down = CameraBounds.down*scFacH;
    const int speed = CameraBounds.speed;

    //// left-right scrolling
    auto smoothScrollRight = [&](const int target_x)
    {
        do{
            delta_x = target_x-scroll.x;
            if(!mpMap->scrollRight())
            {
                break;
            }

            scroll = mpMap->getMainScrollCoords();
        }while(delta_x > right+speed);
    };

    // For cases the camera faces too much to the left (First level Keen 8 mod)
    int leftSideLimit, rightSideLimit;

    mpMap->fetchNearestVertBlockers(getXPosition(), leftSideLimit, rightSideLimit);

    if( (scroll.x<<STC) < leftSideLimit)
    {
        mpMap->scrollRight();
    }

    if(delta_x > right && scroll.x < maxscrollx)
    {
        smoothScrollRight(xPosStc);
    }
    else if(delta_x < left && scroll.x > 32)
    {
        do{
            delta_x = xPosStc-scroll.x;
            if(!mpMap->scrollLeft())
            {
                break;
            }

            scroll = mpMap->getMainScrollCoords();

        }while(delta_x < left-speed);
    }

    // up-down scrolling
    auto smoothScrollUp = [&](const int max_y)
    {
        do{
            delta_y = max_y-scroll.y;
            if(!mpMap->scrollUp())
            {
                break;
            }

            scroll = mpMap->getMainScrollCoords();

        }while(delta_y < up-speed);
    };

    // For cases the camera forces too much to the bottom
    if ( scroll.y > maxscrolly )
    {
        smoothScrollUp(maxscrolly);
    }

    if (delta_y > down && scroll.y < maxscrolly)
    {
        do{
            delta_y = (getYPosition()>>STC)-scroll.y;
            if(!mpMap->scrollDown())
            {
                break;
            }

            scroll = mpMap->getMainScrollCoords();

        }while(delta_y > down+speed);
    }
    else if ( delta_y < up && scroll.y > 32 )
    {
        smoothScrollUp((getYPosition()>>STC));
    }

}

void CCamera::reAdjust()
{
    SDL_Rect gameRes = gVideoDriver.getGameResolution().SDLRect();

    const int x = getXPosition();
    const int y = getYPosition();

    // Check for the nearest vertical edges
    int blockYup, blockYdown, blockXleft, blockXright;

    mpMap->fetchNearestVertBlockers(x, blockXleft, blockXright);
    mpMap->fetchNearestHorBlockers(y, blockYup, blockYdown);

    blockXleft >>= STC;
    blockXright >>= STC;
    blockYup >>= STC;
    blockYdown >>= STC;

    if( (blockYdown-blockYup) < gameRes.h ||
        (blockXright-blockXleft) < gameRes.w)
    {
        mpMap->calcVisibleArea();
        mpMap->refreshVisibleArea();
        return;
    }

    // This will always snap correctly to the edges
    auto scroll = mpMap->getMainScrollCoords();
    if((scroll.x>>TILE_S) <= (blockXleft>>TILE_S))
    {
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mpMap->scrollRight();
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mpMap->scrollLeft();
    }

    scroll = mpMap->getMainScrollCoords();
    if(scroll.x > blockXright - gameRes.w)
    {
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mpMap->scrollLeft();
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mpMap->scrollRight();
    }

    scroll = mpMap->getMainScrollCoords();
    if((scroll.y>>TILE_S) <= blockYup>>TILE_S)
    {
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mpMap->scrollDown();
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mpMap->scrollUp();
    }

    scroll = mpMap->getMainScrollCoords();
    if(scroll.y > blockYdown - gameRes.h)
    {
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mpMap->scrollUp();
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mpMap->scrollDown();
    }

    mpMap->calcVisibleArea();
    mpMap->refreshVisibleArea();
}


bool CCamera::outOfSight()
{
    bool outofsight = false;

    const int cam_width = ((gVideoDriver.getGameResolution().dim.x/2)<<STC);
    const int cam_height = ((gVideoDriver.getGameResolution().dim.y/2)<<STC);

    const int camX = getXPosition();
    const int camY = getYPosition();

    const int playerLeft = mp_AttachedObject->getXLeftPos();
    const int playerRight = mp_AttachedObject->getXRightPos();
    const int playerUp = mp_AttachedObject->getYUpPos();
    const int playerDown = mp_AttachedObject->getYDownPos();

    if( playerLeft > camX + cam_width )
        outofsight |= true;
    else if( playerRight < camX - cam_width )
        outofsight |= true;
    if( playerUp > camY + cam_height )
        outofsight |= true;
    else if( playerDown < camY - cam_height )
        outofsight |= true;

    return outofsight;
}

void CCamera::forbidLead(const int id)
{
    mDontUseThisLead[id] = true;
}

void CCamera::allowLead(const int id)
{
    mDontUseThisLead[id] = false;
}

