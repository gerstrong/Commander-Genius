/*
 * CGUINumberControl.h
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#ifndef __VORTICONNUMBERCONTROL_H_
#define __VORTICONNUMBERCONTROL_H_

#include <lib/widgets/GsNumberControl.h>

#include <string>
#include <graphics/GsSurface.h>


namespace vorticon
{

class NumberControl: public CGUINumberControl
{
public:

    NumberControl(	const std::string& text,
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

    virtual void setupButtonSurface() {}

    void processLogic();

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const GsRect<float> &RectDispCoordFloat);

protected:
    bool mIncSel;
    bool mDecSel;

    GsSurface mTextDarkSfc;
    GsSurface mTextLightSfc;
    GsSurface mTextLightSfcR;
    GsSurface mTextLightSfcL;
    GsSurface mTextDisabledSfc;

    std::string	mText;
    int mValue;

private:

	// draws the slider to the corresponding value
	std::string sliderStr();

	const int mStartValue;
	const int mEndValue;
	const int mDeltaValue;

	static int mTwirliconID;
	const bool mSlider;    
};

}

#endif /* __VORTICONNUMBERCONTROL_H_ */
