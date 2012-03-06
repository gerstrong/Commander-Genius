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

class CGUIControl
{

public:
	CGUIControl();

	virtual void processLogic() = 0;
	virtual void processRender(const CRect<float> &RectDispCoordFloat) = 0;


	void setRect( const CRect<float>& rect )
	{	mRect = rect;	}


	void enable(const bool value)
	{	mEnabled = value;	};


	void drawTwirl( const SDL_Rect& lRect );


	CRect<float> mRect;
	bool mEnabled;

protected:
	bool mHovered;
	bool mButtonDown;
	bool mButtonUp;

	static int mTwirliconID;

};


#endif /* CGUICONTROL_H_ */
