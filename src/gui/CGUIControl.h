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
	virtual void processRender(const CRect<float> &RectDispCoordFloat) = 0;


	void setRect( const CRect<float>& rect )
	{	mRect = rect;	}


	void enable( const bool value )
	{	mEnabled = value;	};

	void setHovered( const bool value )
	{	mHovered = value;	}

	bool getHovered()
	{	return mHovered;	}


	bool Up()
	{	return mButtonUp;	}


	void setDown( const bool value )
	{	mHovered = value;	}

	virtual bool sendEvent(const InputCommands command) { return false; }


	void drawTwirl( const SDL_Rect& lRect );

	void drawBlinker( const SDL_Rect& lRect );


	CRect<float> mRect;
	
    	bool mEnabled;

protected:
    
	int mFontID;
	bool mHovered;
	bool mButtonDown;
	bool mButtonUp;

	static int mTwirliconID;

};


#endif /* CGUICONTROL_H_ */
