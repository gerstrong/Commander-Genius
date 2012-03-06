/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef CGUIBUTTON_H_
#define CGUIBUTTON_H_

#include "CGUIControl.h"
#include <string>
#include "engine/CEvent.h"
#include "SmartPointer.h"

class CGUIButton : public CGUIControl
{
public:

	enum Style
	{
		NONE,
		VORTICON
	};

	CGUIButton(const std::string& text,
			const SmartPointer<CEvent> ev,
			const Style	style = NONE);

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

	void setText(const std::string& text)
	{	mText = text;	}


private:
	std::string mText;
	SmartPointer<CEvent> mEvent;

	void (CGUIButton::*drawButton)(SDL_Rect&);

};

#endif /* CGUIBUTTON_H_ */
