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

/**
 * This is main cycle which will be called every Logic loop
 * Here it moves automatically, performs the smooth scrolling
 * and gets the controls to move when it's not attached to an object
 */
void CCamera::process()
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
			mp_Map->scrollDown();
		}while(py > down+speed && scroll_y < mp_Map->m_maxscrolly);
	}
	else if ( py < up && scroll_y > 32  )
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			mp_Map->scrollUp();
		}while(py < up-speed && scroll_y > 32);
	}

	// This will always snap correctly to the edge
	while(scroll_x < 32)
		mp_Map->scrollRight();
	while(scroll_x > mp_Map->m_maxscrollx)
		mp_Map->scrollLeft();
	while(scroll_y < 32)
		mp_Map->scrollDown();
	while(scroll_y > mp_Map->m_maxscrolly)
		mp_Map->scrollUp();
}
