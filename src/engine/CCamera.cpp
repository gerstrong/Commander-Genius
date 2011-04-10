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
#include "sdl/CInput.h"
#include "spritedefines.h"

CCamera::CCamera(CMap *pmap, Uint32 x, Uint32 y, CObject *p_attacher) :
CObject(pmap, x, y, OBJ_NONE),
mp_AttachedObject(p_attacher)
{
	sprite = BLANKSPRITE;
	solid = false;
	m_attached = true;
	m_freeze = false;
	m_moving = true;
}

void CCamera::attachObject(CObject *p_attacher)
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

		// Make the camera move and tell if it's scrolling through the m_moving variable
		if(mp_AttachedObject->getXPosition() > getXPosition())
		{
			moveRight(mp_AttachedObject->getXPosition() - getXPosition());
			m_moving |= true;

		}
		else if(mp_AttachedObject->getXPosition() < getXPosition())
		{
			moveLeft(getXPosition() - mp_AttachedObject->getXPosition());
			m_moving |= true;
		}

		if(mp_AttachedObject->getYPosition() > getYPosition())
		{
			moveDown(mp_AttachedObject->getYPosition() - getYPosition());
			m_moving |= true;
		}
		else if(mp_AttachedObject->getYPosition() < getYPosition())
		{
			moveUp(getYPosition() - mp_AttachedObject->getYPosition());
			m_moving |= true;
		}
	}

	int px, py, left, up, right, down, speed;

	Uint16& scroll_x = mp_Map->m_scrollx;
	Uint16& scroll_y = mp_Map->m_scrolly;

	px = (getXPosition()>>STC)-scroll_x;
	py = (getYPosition()>>STC)-scroll_y;

	st_camera_bounds CameraBounds = g_pVideoDriver->getCameraBounds();
	left = CameraBounds.left;
	up = CameraBounds.up;
	right = CameraBounds.right;
	down = CameraBounds.down;
	speed = CameraBounds.speed;

	// left-right scrolling
	if(px > right && scroll_x < mp_Map->m_maxscrollx)
	{
		do{
			px = (getXPosition()>>STC)-scroll_x;
			mp_Map->scrollRight();
		}while(px > right+speed && scroll_x < mp_Map->m_maxscrollx);
	}
	else if(px < left && scroll_x > 32)
	{
		do{
			px = (getXPosition()>>STC)-scroll_x;
			mp_Map->scrollLeft();
		}while(px < left-speed && scroll_x > 32);
	}

	// up-down scrolling
	if (py > down && scroll_y < mp_Map->m_maxscrolly)
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			if(!mp_Map->scrollDown())
				break;
		}while(py > down+speed && scroll_y < mp_Map->m_maxscrolly);
	}
	else if ( py < up && scroll_y > 32 )
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			if(!mp_Map->scrollUp())
				break;
		}while(py < up-speed && scroll_y > 32);
	}

	// This will always snap correctly to the edge
	while(scroll_x < 32)
		mp_Map->scrollRight(true);
	while(scroll_x > mp_Map->m_maxscrollx)
		mp_Map->scrollLeft(true);
	while(scroll_y < 32)
		mp_Map->scrollDown(true);
	while(scroll_y > mp_Map->m_maxscrolly)
		mp_Map->scrollUp(true);
}
