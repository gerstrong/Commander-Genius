/*
 * CGUINumberControl.h
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#ifndef CGUINUMBERCONTROL_H_
#define CGUINUMBERCONTROL_H_

#include "GsControl.h"

#include <string>
#include <graphics/GsSurface.h>

//#include "engine/CEvent.h"


class CGUINumberControl: public CGUIControl
{
public:

	CGUINumberControl(	const std::string& text,
			const int startValue,
			const int endValue,
			const int deltaValue,
			const int value,
			const bool slider = true);


	void increment();
	void decrement();

	bool sendEvent(const InputCommands command);

	const int getSelection();
	void setSelection( const int value );

	void setupButtonSurface();

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawGalaxyStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const GsRect<float> &RectDispCoordFloat);

private:

	// draws the slider to the corresponding value
	std::string sliderStr();

	std::string	mText;
	const int mStartValue;
	const int mEndValue;
	const int mDeltaValue;
	int mValue;
	bool mIncSel;
	bool mDecSel;
	static int mTwirliconID;
	const bool mSlider;


    GsSurface mTextDarkSfc;
    GsSurface mTextLightSfc;
    GsSurface mTextLightSfcR;
    GsSurface mTextLightSfcL;
    GsSurface mTextDisabledSfc;

	void (CGUINumberControl::*drawButton)(SDL_Rect&);

};

#endif /* CGUINUMBERCONTROL_H_ */
