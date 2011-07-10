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

#include "common/CObject.h"

#ifndef CCAMERA_H_
#define CCAMERA_H_

class CCamera : public CObject
{
public:
	CCamera(CMap *pmap, Uint32 x, Uint32 y, CObject *p_attacher=NULL);

	/**
	 * \brief Whenever the object where the camera has to follow up is created it needs to be attached
	 *        in order to follow it.
	 * \return p_attacher Object Pointer like the player to which the camera will follow.
	 */
	void attachObject(CObject *p_attacher);

	/**
	 * \brief This will put the camera to the given coordinate
	 * Normally the camera moves the player and it's not necessary to move it here normally
	 * but in some cases, when the player is teleported (or going a door) and appears at another place
	 * the camera might not be able to follow him (reason can be scroll-blocking). This
	 * function can resolve such issues
	 * \param newpos New Position for the camera as math vector
	 */
	void setPosition(const VectorD2<int>& newpos);

	/**
	 * \brief
	 * This is main cycle which will be called every Logic loop
	 * Here it moves automatically, performs the smooth scrolling
	 * and gets the controls to move when it's not attached to an object
	 *
	 * \param force If on the map are scrollblockers, they will be ignored when this is enabled
	 */
	void process(const bool force = false);

	/**
	 * \brief
	 * This function tells if the attached object is out of the camera view
	 *
	 * \return true if the Attached object is outside the camera, else false
	 */

	bool outOfSight();
	
	bool m_attached;
	bool m_freeze;
	bool m_moving;
	bool m_abouttomove;

	/**
	 *	Just relative structure to the camera position. This is used when the player tries to look up or down
	 */
	struct
	{
		int x, y;
	}m_relcam;

private:
	CObject *mp_AttachedObject;
};

#endif /* CCAMERA_H_ */
