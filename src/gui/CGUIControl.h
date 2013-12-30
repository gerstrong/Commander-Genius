/*
 * CGUIControl.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  Base Class for any Type of Control
 */

#ifndef CGUICONTROL_H_
#define CGUICONTROL_H_

#include "utils/Geometry.h"
#include "sdl/input/InputEvents.h"

class CGUIControl
{
public:
	enum Style
	{
		UNSET,
		NONE,
		VORTICON,
		GALAXY,
		GALAXY_BORDERED
	};


	CGUIControl();
	virtual ~CGUIControl() {}

	virtual void processLogic() = 0;
    virtual void processRender(const GsRect<float> &RectDispCoordFloat) = 0;

    virtual void updateGraphics() {}

    /*void processInputState(const GsInputState &inputState)
    {
        const bool hasPoint = mAbsRect.hasPoint(inputState.mPos);
        const bool bDown = (inputState.mActionButton>0);

        mReleased = false;

        if(!bDown && mPressed)
        {
            mPressed = false;

            if(hasPoint)
            {
                mReleased = true;
            }
        }

        if(!bDown || mPressed)
        {
            mHovered = hasPoint;
        }

        if(mHovered && bDown)
        {
            mPressed = true;
        }
    }*/


    void setRect( const GsRect<float>& rect )
	{	mRect = rect;	}


	void enable( const bool value )
    {	mEnabled = value;	}

	void setHovered( const bool value )
	{	mHovered = value;	}

	bool getHovered()
	{	return mHovered;	}


    bool Up()
    {	return mReleased;	}


    void setDown( const bool value )
    {	mHovered = value;	}

	virtual bool sendEvent(const InputCommands command) { return false; }


	void drawTwirl( const SDL_Rect& lRect );

	void drawBlinker( const SDL_Rect& lRect );


    // The relative rect describes the rect which is normally tied to its parent.
    //GsRect<float> mRelRect;

    // In contrast the absolute rect is position and size to the root element, which is normally
    // the surface where the control will be drawn
    //GsRect<Uint16> mAbsRect;
    GsRect<Uint16> mRect;
	
    	bool mEnabled;

protected:
    
	int mFontID;
	bool mHovered;
    bool mPressed;
    bool mReleased;

	static int mTwirliconID;

};


#endif /* CGUICONTROL_H_ */
