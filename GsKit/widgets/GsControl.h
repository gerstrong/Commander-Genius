/*
 * GsControl.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  Base Class for any Type of Control
 */

#ifndef GsControl_H_
#define GsControl_H_

#include <base/InputEvents.h>
#include <base/utils/Geometry.h>


class GsControl
{
public:
	enum Style
	{
		NONE,
		VORTICON,
        GALAXY
	};


    GsControl();
    
    GsControl(const Style style);
    
    virtual ~GsControl();

	virtual void processLogic() = 0;

    void processBlendEffects();

    void processPointingState()
    {
        processPointingState(mRect);
    }

    void processPointingState(const GsRect<float> &rect);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) = 0;

    virtual void updateGraphics() {}


    void setRect( const GsRect<float>& rect )
	{	mRect = rect;	}


    void enable( const bool value )
    {	mEnabled = value;	}

    bool isEnabled() const
    {   return mEnabled;    }


    void select( const bool value )
    { mSelected = value; }

    bool isSelected() const
    { return mSelected; }

    bool isPressed() const
    { return mPressed; }


    bool Up() const
    {	return mReleased;	}


    #ifndef DISABLE_HOVER
    void setDown( const bool value )
    {
        mHovered = value;
    }
    #else
    void setDown( const bool ) {}
    #endif

    virtual bool sendEvent(const InputCommand) { return false; }


	void drawTwirl( const SDL_Rect& lRect );

	void drawBlinker( const SDL_Rect& lRect );

    // The relative rect describes the rect which is normally tied to its parent.
    GsRect<float> mRect;
	

protected:

    bool mEnabled = true;
    
	int mFontID = 0;

    // For mouse/touch we have those
#ifndef DISABLE_HOVER
	bool mHovered  = false;
#endif
    bool mPressed  = false;
    bool mReleased = false;

    // This is needed for gamepad/keyboard input
    bool mSelected = false;

	static int mTwirliconID;

    int mLightRatio = 128; // This blends between selected and unselected item.

    Style mStyle = Style::NONE;

    /**
     * @brief mHoverTriggers    This is for touch friendly devices were there is no device
     */
#ifndef DISABLE_HOVER
#ifdef ANDROID
    static const bool mHoverTriggers = true;
#else
    static const bool mHoverTriggers = false;
#endif
#endif
};


#endif /* GsControl_H_ */
