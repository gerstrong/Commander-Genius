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

	void attachObject(CObject *p_attacher);
	void process();
	
	bool m_attached;
	bool m_freeze;
	bool m_moving;

private:
	CObject *mp_AttachedObject;
};

#endif /* CCAMERA_H_ */
