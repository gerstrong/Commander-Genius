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
#include "CLogFile.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "engine/spritedefines.h"

CCamera::CCamera(CMap *pmap, Uint32 x, Uint32 y, CSpriteObject *p_attacher) :
CSpriteObject(pmap, x, y),
mp_AttachedObject(p_attacher)
{
	m_relcam.x = 0;
	m_relcam.y = 0;
	sprite = BLANKSPRITE;
	solid = false;
	m_attached = true;
	m_freeze = false;
	m_moving = true;
}

void CCamera::attachObject(CSpriteObject *p_attacher)
{
	mp_AttachedObject = p_attacher;
}

void CCamera::setPosition(const VectorD2<int>& newpos)
{
	int cam_x = newpos.x-((g_pVideoDriver->getGameResolution().w/2)<<STC);
	int cam_y = newpos.y-((g_pVideoDriver->getGameResolution().h/2)<<STC);

	if(cam_x<0)
		cam_x = 0;

	if(cam_y<0)
		cam_y = 0;

	moveToForce(newpos);
	mp_Map->gotoPos(cam_x>>STC, cam_y>>STC);
}

void CCamera::process(const bool force)
{
	if(m_freeze)
		return;

	if(!m_attached)
	{	// This means, that there is no attached object. Let the camera scroll freely!
		size_t movespeed = 100;

		if(g_pInput->getHoldedCommand(IC_LEFT))
			moveLeft(movespeed);
		else if(g_pInput->getHoldedCommand(IC_RIGHT))
			moveRight(movespeed);
		if(g_pInput->getHoldedCommand(IC_UP))
			moveUp(movespeed);
		else if(g_pInput->getHoldedCommand(IC_DOWN))
			moveDown(movespeed);
	}
	else
	{
		if(mp_AttachedObject == NULL)
			return;

		m_moving = false;

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

	st_camera_bounds CameraBounds = g_pVideoDriver->getCameraBounds();
	const int left = CameraBounds.left;
	const int up = CameraBounds.up;
	const int right = CameraBounds.right;
	const int down = CameraBounds.down;
	const int speed = CameraBounds.speed;

	// left-right scrolling
	if(delta_x > right && scroll_x < mp_Map->m_maxscrollx)
	{
		do{
			delta_x = (getXPosition()>>STC)-scroll_x;
			if(!mp_Map->scrollRight())
			    break;
		}while(delta_x > right+speed /*&& scroll_x < mp_Map->m_maxscrollx*/);
	}
	else if(delta_x < left && scroll_x > 32)
	{
		do{
			delta_x = (getXPosition()>>STC)-scroll_x;
			if(!mp_Map->scrollLeft())
			    break;
		}while(delta_x < left-speed /*&& scroll_x > 32*/);
	}

	// up-down scrolling
	if (delta_y > down && scroll_y < mp_Map->m_maxscrolly)
	{
		do{
			delta_y = (getYPosition()>>STC)-scroll_y;
			if(!mp_Map->scrollDown())
				break;
		}while(delta_y > down+speed /*&& scroll_y < mp_Map->m_maxscrolly*/);
	}
	else if ( delta_y < up && scroll_y > 32 )
	{
		do{
			delta_y = (getYPosition()>>STC)-scroll_y;
			if(!mp_Map->scrollUp())
				break;
		}while(delta_y < up-speed /*&& scroll_y > 32*/);
	}

	// This will always snap correctly to the edges
	while(scroll_x < 32)
		mp_Map->scrollRight(true);
	while(scroll_x > mp_Map->m_maxscrollx)
		mp_Map->scrollLeft(true);
	while(scroll_y < 32)
		mp_Map->scrollDown(true);
	while(scroll_y > mp_Map->m_maxscrolly)
		mp_Map->scrollUp(true);
}


bool CCamera::outOfSight()
{
	bool outofsight = false;

	const int cam_width = ((g_pVideoDriver->getGameResolution().w/2)<<(CSF-STC));
	const int cam_height = ((g_pVideoDriver->getGameResolution().h/2)<<(CSF-STC));

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
