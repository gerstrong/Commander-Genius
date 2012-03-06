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

	enum Style
	{
		NONE,
		VORTICON
	};

	CGUINumberControl(	const std::string& text,
			const int startValue,
			const int endValue,
			const int deltaValue,
			const int value,
			const Style	style = NONE);


	void increment();
	void decrement();

	const int getSelection();
	void setSelection( const int value );

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

private:
	std::string	mText;
	bool mHovered;
	bool mButtonDown;
	bool mButtonUp;
	const int mStartValue;
	const int mEndValue;
	const int mDeltaValue;
	int mValue;
	bool mIncSel;
	bool mDecSel;
	static int mTwirliconID;

	void (CGUINumberControl::*drawButton)(SDL_Rect&);

};

#endif /* CGUINUMBERCONTROL_H_ */
