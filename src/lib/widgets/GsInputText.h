/*
 * CGUIInputText.h
 *
 *  Created on: 25.03.2012
 *      Author: gerstrong
 */

#ifndef CGUIINPUTTEXT_H_
#define CGUIINPUTTEXT_H_

#include "GsControl.h"
#include <string>

//#include "engine/CEvent.h"

class CGUIInputText : public CGUIControl
{
public:

	CGUIInputText(const std::string& text,
				  const Style	style);

	bool sendEvent(const InputCommands command);

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);

	void drawGalaxyStyle(SDL_Rect& lRect);


	void processRender(const GsRect<float> &RectDispCoordFloat);

	void setText(const std::string& text)
	{	mText = text;	}

	std::string getText() const
	{	return mText;	}

	bool Typing() const { return mTyping; }
	void setTypeMode( const bool value );

private:

	// Returns the string that has to be displayed on the gui
	// It should say empty if it's so or a character when ticking
	// while user is typing.
	std::string getInputString();


	std::string mText;
	bool mTyping;
	int mTypeTick;
	bool mTick;

	std::unique_ptr<SDL_Surface> mpTextSfc;

	void (CGUIInputText::*drawButton)(SDL_Rect&);

};

#endif /* CGUIINPUTTEXT_H_ */
