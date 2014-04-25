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

int CCamera::mCamlead = 0;
bool CCamera::mCamLeadChange = false;
std::array<bool, 4> CCamera::mDontUseThisLead;


CCamera::CCamera(CMap *pmap, Uint32 x, Uint32 y, CSpriteObject *p_attacher) :
CSpriteObject(pmap, x, y, 0),
mp_AttachedObject(p_attacher)
{
	m_relcam.x = 0;
	m_relcam.y = 0;
	sprite = BLANKSPRITE;
	solid = false;
	m_attached = true;
	m_freeze = false;
	m_moving = true;

    const int camId = mp_AttachedObject->getSpriteVariantId();

    mDontUseThisLead[camId] = false;

    resetCamLead();
}

void CCamera::cycleCamlead()
{
    const int numPlayers = g_pBehaviorEngine->mPlayers;

    if( numPlayers == 1 ) // For one player this doesn't make sense to change
        return;

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
}

void CCamera::setPosition(const Vector2D<int>& newpos)
{

	int cam_x = newpos.x-((gVideoDriver.getGameResolution().w/2)<<STC);
	int cam_y = newpos.y-((gVideoDriver.getGameResolution().h/2)<<STC);

    const int minimumEdgeDist = (2<<CSF)+1;
    const int maxWidth = (mp_Map->m_width<<CSF)-1;
    const int maxHeight = (mp_Map->m_height<<CSF)-1;

    if(cam_x<minimumEdgeDist)
        cam_x = minimumEdgeDist;

    if(cam_y<minimumEdgeDist)
        cam_y = minimumEdgeDist;

    if(cam_x>maxWidth)
        cam_x = maxWidth;

    if(cam_y>maxHeight)
        cam_y = maxHeight;

	moveToForce(newpos);

    // Only the lead camera may change the scroll position
    if(mCamlead != mp_AttachedObject->getSpriteVariantId())
        return;

	mp_Map->gotoPos(cam_x>>STC, cam_y>>STC);

    mp_Map->mGamePlayPos = newpos;
	
	reAdjust();
}

void CCamera::process()
{
    // Cycle Cam Code
    if( gInput.getPressedCommand(mCamlead, IC_CAMLEAD) )
    {
        cycleCamlead();
    }

    const int camId = mp_AttachedObject->getSpriteVariantId();

    if(camId != mCamlead)
        return;

    if(mCamLeadChange)
    {
        mCamLeadChange = false;

        setPosition(m_Pos);
    }

    if(m_freeze)
		return;
	
	SDL_Rect gamerect = gVideoDriver.getGameResolution().SDLRect();
	const int maxscrollx = (mp_Map->m_width<<4) - gamerect.w - 32;
	const int maxscrolly = (mp_Map->m_height<<4) - gamerect.h - 32;
	

	if(!m_attached)
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
		if(mp_AttachedObject == NULL)
			return;

		m_moving = false;

        mp_Map->mGamePlayPos = mp_AttachedObject->getMidPos();

		const Uint32 attached_x = mp_AttachedObject->getXPosition() + m_relcam.x;
		const Uint32 attached_y = mp_AttachedObject->getYPosition() + m_relcam.y;
		const Uint32 local_x = getXPosition();
		const Uint32 local_y = getYPosition();


		// Make the camera move and tell if it's scrolling through the m_moving variable
		if( attached_x > local_x )
		{
			moveRight( attached_x - local_x );
			m_moving |= true;
		}
		else if( attached_x < local_x )
		{
			moveLeft( local_x - attached_x );
			m_moving |= true;
		}

		if( attached_y > local_y )
		{
			moveDown( attached_y - local_y );
			m_moving |= true;
		}
		else if( attached_y < local_y )
		{
			moveUp( local_y - attached_y );
			m_moving |= true;
		}
	}


    Uint16 &scroll_x = mp_Map->m_scrollx;
    Uint16 &scroll_y = mp_Map->m_scrolly;

    // delta is how much we need to scroll in order to get the camera stalled
    int delta_x = (getXPosition()>>STC)-scroll_x;
    int delta_y = (getYPosition()>>STC)-scroll_y;



    // The bounds are relatively scaled for bigger game resolutions
    const int scFacW = gamerect.w/320;
    const int scFacH = gamerect.h/200;

    st_camera_bounds CameraBounds = gVideoDriver.getCameraBounds();
    const int left = CameraBounds.left*scFacW;
    const int up = CameraBounds.up*scFacH;
    const int right = CameraBounds.right*scFacW;
    const int down = CameraBounds.down*scFacH;
	const int speed = CameraBounds.speed;

	// left-right scrolling
	if(delta_x > right && scroll_x < maxscrollx)
	{
		do{
			delta_x = (getXPosition()>>STC)-scroll_x;
			if(!mp_Map->scrollRight())
			    break;
		}while(delta_x > right+speed);
	}
    else if(delta_x < left && scroll_x > 32)
	{
		do{
			delta_x = (getXPosition()>>STC)-scroll_x;
			if(!mp_Map->scrollLeft())
			    break;
		}while(delta_x < left-speed);
	}

	// up-down scrolling
	if (delta_y > down && scroll_y < maxscrolly)
	{
		do{
			delta_y = (getYPosition()>>STC)-scroll_y;
			if(!mp_Map->scrollDown())
				break;
		}while(delta_y > down+speed);
	}
	else if ( delta_y < up && scroll_y > 32 )
	{
		do{
			delta_y = (getYPosition()>>STC)-scroll_y;
			if(!mp_Map->scrollUp())
				break;
		}while(delta_y < up-speed);
	}

}

void CCamera::reAdjust() 
{
    SDL_Rect gameRes = gVideoDriver.getGameResolution().SDLRect();
  
	Uint16 &scroll_x = mp_Map->m_scrollx;
	Uint16 &scroll_y = mp_Map->m_scrolly;
	const int x = getXPosition();
	const int y = getYPosition();

	// Check for the nearest vertical edges	
	int blockYup, blockYdown, blockXleft, blockXright;
	
	mp_Map->fetchNearestVertBlockers(x, blockXleft, blockXright);
	mp_Map->fetchNearestHorBlockers(y, blockYup, blockYdown);
	
    blockXleft >>= STC;
	blockXright >>= STC;
	blockYup >>= STC;
    blockYdown >>= STC;

    if( (blockYdown-blockYup) < gameRes.h ||
        (blockXright-blockXleft) < gameRes.w)
    {
        mp_Map->calcVisibleArea();
        mp_Map->refreshVisibleArea();
        return;
    }

	// This will always snap correctly to the edges
    if(scroll_x < blockXleft)
	{
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mp_Map->scrollRight();
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mp_Map->scrollLeft();
	}
    if(scroll_x > blockXright - gameRes.w)
	{
            for(int amt=0 ; amt<gameRes.w ; amt++ )
            mp_Map->scrollLeft();
        for(int amt=0 ; amt<gameRes.w ; amt++ )
            mp_Map->scrollRight();
	}	
	if(scroll_y < blockYup)
	{
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mp_Map->scrollDown();
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mp_Map->scrollUp();
	}
    if(scroll_y > blockYdown - gameRes.h)
	{
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mp_Map->scrollUp();
        for(int amt=0 ; amt<gameRes.h ; amt++ )
            mp_Map->scrollDown();
    }

    mp_Map->calcVisibleArea();
    mp_Map->refreshVisibleArea();
}


bool CCamera::outOfSight()
{
	bool outofsight = false;

	const int cam_width = ((gVideoDriver.getGameResolution().w/2)<<STC);
	const int cam_height = ((gVideoDriver.getGameResolution().h/2)<<STC);

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
