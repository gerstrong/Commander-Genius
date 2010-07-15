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
		if(g_pInput->getHoldedCommand(IC_LEFT))
			moveLeft();
		else if(g_pInput->getHoldedCommand(IC_RIGHT))
			moveRight();
		if(g_pInput->getHoldedCommand(IC_UP))
			moveUp();
		else if(g_pInput->getHoldedCommand(IC_DOWN))
			moveDown();
	}
}

/**
 * Move the camera left also checking the bounds
 */
void CCamera::moveLeft()
{
	mp_Map->scrollLeft();
}

/**
 * Move the camera right also checking the bounds
 */
void CCamera::moveRight()
{
	mp_Map->scrollRight();
}

/**
 * Move the camera up also checking the bounds
 */
void CCamera::moveUp()
{
	mp_Map->scrollUp();
}

/**
 * Move the camera down also checking the bounds
 */
void CCamera::moveDown()
{
	mp_Map->scrollDown();
}

CCamera::~CCamera()
{
	// TODO Auto-generated destructor stub
}
