/*
 * GsControl.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  Base Class for any Type of Control
 *  Control is a widget that allows more interaction with keyboard
 *  mouse, touch, gestures and joystick
 */

#ifndef GsControl_H_
#define GsControl_H_

#include <base/InputEvents.h>

#include "GsWidget.h"

class GsControl : public GsWidget
{
public:

    GsControl(const GsRect<float> &rect);

    GsControl();
    
    virtual ~GsControl();	

    void processBlendEffects();

    void processPointingState() {}

    void processPointingState(const GsRect<float> &rect);

    virtual void processPointingStateRel(const GsRect<float> &rect);

    void enable( const bool value )
    {	mEnabled = value;	}

    bool isEnabled()
    {   return mEnabled;    }


    void select( const bool value )
    { mSelected = value; }

    bool isSelected()
    { return mSelected; }

    bool isPressed() const
    { return mPressed; }

    bool Up()
    {	return mReleased;	}

    void setHovered( const bool value )
    {
        mHovered = value;
    }

    virtual bool sendEvent(const InputCommand) { return false; }

	void drawTwirl( const SDL_Rect& lRect );

	void drawBlinker( const SDL_Rect& lRect );

    void enableBlinker(const bool value)
    {
        mDrawBlinker = value;
    }

    void enableTwirl(const bool value)
    {
        mDrawTwirl = value;
    }


    void setPressed(const bool value)
    {
        mPressed = value;
    }

    void setReleased(const bool value)
    {
        mReleased = value;
    }


protected:


    bool mEnabled = true;

    // For mouse/touch we have those
	bool mHovered  = false;
    bool mPressed  = false;
    bool mReleased = false;

    // This is needed for gamepad/keyboard input
    bool mSelected = false;



	static int mTwirliconID;

    int mLightRatio = 128; // This blends between selected and unselected item.

    bool mDrawBlinker = false;
    bool mDrawTwirl = false;

    /**
     * @brief mHoverTriggers    This is for touch friendly devices were there is no device
     */
#ifdef ANDROID
    static const bool mHoverTriggers = true;
#else
    static const bool mHoverTriggers = false;
#endif

};


#endif /* GsControl_H_ */
