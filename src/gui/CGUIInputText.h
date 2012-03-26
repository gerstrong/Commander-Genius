/*
 * CGUIInputText.h
 *
 *  Created on: 25.03.2012
 *      Author: gerstrong
 */

#ifndef CGUIINPUTTEXT_H_
#define CGUIINPUTTEXT_H_

#include "CGUIControl.h"
#include <string>
#include "engine/CEvent.h"
#include "SmartPointer.h"

class CGUIInputText : public CGUIControl
{
public:

	enum Style
	{
		NONE,
		VORTICON
	};

	CGUIInputText(const std::string& text,
				  const Style	style);

	void sendEvent(const InputCommands command);

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

	void setText(const std::string& text)
	{	mText = text;	}

	std::string getText()
	{	return mText;	}

private:
	std::string mText;
	bool mTyping;

	void (CGUIInputText::*drawButton)(SDL_Rect&);

};

#endif /* CGUIINPUTTEXT_H_ */
