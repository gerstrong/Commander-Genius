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

#include "engine/core/CSpriteObject.h"

#include <array>

#ifndef CCAMERA_H_
#define CCAMERA_H_

class CCamera : public CSpriteObject
{
public:
    CCamera(CMap *pmap, Uint32 x, Uint32 y, CSpriteObject *p_attacher);

    /**
     * @brief cycleCamlead passes the camera lead only enables the cam which has the lead
     */
    void cycleCamlead();

	/**
	 * \brief Whenever the object where the camera has to follow up is created it needs to be attached
	 *        in order to follow it.
	 * \return p_attacher Object Pointer like the player to which the camera will follow.
	 */
	void attachObject(CSpriteObject *p_attacher);

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
    void process();


	/**
	 * \brief
	 * This function tells if the attached object is out of the camera view
	 *
	 * \return true if the Attached object is outside the camera, else false
	 */

	/**
	 * in case the map is loaded or the player warped to another point,
	 * this function will adjust the camera to the proper edges in the level
	 */
	void reAdjust();

    /**
     * @brief shakeVertView This will make the camera shake a bit.
     * @param time      time in milliseconds for how long the camera shakes
     * @param vAmount   amount of vertical placement which provides that effect
     */
    void shakeVertView( const unsigned int time, const unsigned int vAmount );

	bool outOfSight();

    void forbidLead(const int id);

    static int getLead()
    {
        return mCamlead;
    }

    static void resetCamLead()
    {
        mCamlead = 0;
        mCamLeadChange = false;
    }

	bool m_attached;
	bool m_freeze;
	bool m_moving;

	/**
	 *	Just relative structure to the camera position. This is used when the player tries to look up or down
	 */
	struct
	{
		int x, y;
	}m_relcam;

private:

	CSpriteObject *mp_AttachedObject;

    static int mCamlead;
    static bool mCamLeadChange;
    static std::array<bool, 4> mDontUseThisLead;
};

#endif /* CCAMERA_H_ */
