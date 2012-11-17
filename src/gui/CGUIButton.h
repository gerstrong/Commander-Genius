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
#include "engine/CEvent.h"
#include "common/CBehaviorEngine.h"
#include <string>
#include <memory>
#include <map>


class CGUIButton : public CGUIControl
{
public:

	CGUIButton(const std::string& text,
			CEvent *ev,
			const Style style=UNSET);

	bool sendEvent(const InputCommands command);

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);

	void drawGalaxyBorderedStyle(SDL_Rect& lRect);

	void drawGalaxyStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

	void setText(const std::string& text)
	{
		mText = text;
		setupButtonSurface();
	}

	std::string getText()
	{	return mText;	}

	void setAutoActivation()
	{	mAutoActivation = true;	}

private:

	void setupButtonSurface();

	std::string mText;
	std::shared_ptr<CEvent> mEvent;
	std::unique_ptr<SDL_Surface> mpTextDarkSfc;
	std::unique_ptr<SDL_Surface> mpTextLightSfc;
	std::unique_ptr<SDL_Surface> mpTextDisabledSfc;
	std::map< Style, EngineType > mMapping;

	void (CGUIButton::*drawButton)(SDL_Rect&);

	// Auto activation means that as soon it's enabled and process the event will be triggered immediately
	bool mAutoActivation;
};

#endif /* CGUIBUTTON_H_ */
