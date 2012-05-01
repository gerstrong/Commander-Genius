/*
 * CGUINumberControl.h
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#ifndef CGUINUMBERCONTROL_H_
#define CGUINUMBERCONTROL_H_

#include <string>
#include "CGUIControl.h"
#include "engine/CEvent.h"
#include "SmartPointer.h"


class CGUINumberControl: public CGUIControl
{
public:

	CGUINumberControl(	const std::string& text,
			const int startValue,
			const int endValue,
			const int deltaValue,
			const int value );


	void increment();
	void decrement();

	const int getSelection();
	void setSelection( const int value );

	void setupButtonSurface();

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawGalaxyStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

private:
	std::string	mText;
	const int mStartValue;
	const int mEndValue;
	const int mDeltaValue;
	int mValue;
	bool mIncSel;
	bool mDecSel;
	static int mTwirliconID;

	SmartPointer<SDL_Surface> mpTextDarkSfc;
	SmartPointer<SDL_Surface> mpTextLightSfc;
	SmartPointer<SDL_Surface> mpTextDisabledSfc;


	void (CGUINumberControl::*drawButton)(SDL_Rect&);

};

#endif /* CGUINUMBERCONTROL_H_ */
