/*
 * CCamera.h
 *
 *  Created on: 09.03.2010
 *      Author: pizza2004 and gerstrong
 *
 *  This camera class is used for the scrolling during the "gameplay".
 *  The old values of that used that class are now a structure part of CVideoDriver
 *  This class will be another object that will following the player
 *  It has the feature to attach to any object in the game scenario and follow it.
 *  It also can detached and moved manually. That might be used for
 *  the active Level viewer feature.
 */


#include "CCamera.h"
#include "../CLogFile.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"

CCamera::CCamera(CMap *pmap, Uint32 x, Uint32 y):
CObject(pmap, x, y, OBJ_NONE),
mp_AttachedObject(NULL)
{
	g_pLogFile->ftextOut("Starting the camera system...<br>");
	sprite = 138;
	solid = false;
}

/**
 * This function will attach the camera to the object and scroll
 * automatically with it
 */
void CCamera::attachToObject(CObject &attacher)
{	mp_AttachedObject = &attacher;	}

/**
 * This function will dettach the camera from a object.
 * If this happens, the camera can be scrolled manually.
 */
bool CCamera::detach()
{
	if(!mp_AttachedObject)
		return false;
	else
	{
		mp_AttachedObject = NULL;
		return true;
	}
}

/**
 * This is main cycle which will be called every Logic loop
 * Here it moves automatically, performs the smooth scrolling
 * and gets the controls to move when it's not attached to an object
 */
void CCamera::process()
{
	if(mp_AttachedObject == NULL)
	{	// This means, that there is no attached object. Let the camera scroll freely!

		if(g_pInput->getHoldedKey(KA))
			moveLeft(100);
		else if(g_pInput->getHoldedKey(KD))
			moveRight(100);
		if(g_pInput->getHoldedKey(KW))
			moveUp(100);
		else if(g_pInput->getHoldedKey(KS))
			moveDown(100);

		/*if(g_pInput->getHoldedCommand(IC_LEFT))
			moveLeft(10);
		else if(g_pInput->getHoldedCommand(IC_RIGHT))
			moveRight(10);
		if(g_pInput->getHoldedCommand(IC_UP))
			moveUp(10);
		else if(g_pInput->getHoldedCommand(IC_DOWN))
			moveDown(10);*/
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

/**
 * Move the camera left also checking the bounds
 */
/*void CCamera::moveLeft()
{
	mp_Map->scrollLeft();
}*/

/**
 * Move the camera right also checking the bounds
 */
/*void CCamera::moveRight()
{
	mp_Map->scrollRight();
}*/

/**
 * Move the camera up also checking the bounds
 */
/*void CCamera::moveUp()
{
	mp_Map->scrollUp();
}*/

/**
 * Move the camera down also checking the bounds
 */
/*void CCamera::moveDown()
{
	mp_Map->scrollDown();
}*/

CCamera::~CCamera()
{
	// TODO Auto-generated destructor stub
}
