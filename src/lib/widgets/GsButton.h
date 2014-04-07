/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef GsButton_H_
#define GsButton_H_

#include "GsControl.h"
#include <string>
#include <memory>
#include <map>
//#include <engine/CEvent.h>
//#include <common/CBehaviorEngine.h>
#include <graphics/GsSurface.h>


class GsButton : public CGUIControl
{
public:

    GsButton(const std::string& text,
			CEvent *ev,
			const Style style=UNSET);

	bool sendEvent(const InputCommands command);

    void updateGraphics();

    virtual void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);

	void drawGalaxyBorderedStyle(SDL_Rect& lRect);

	void drawGalaxyStyle(SDL_Rect& lRect);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat);

    void setText(const std::string& text)
	{
		mText = text;
		setupButtonSurface();
    }

	std::string getText()
	{	return mText;	}

    virtual void setupButtonSurface() {}

protected:

    std::string mText;
    GsSurface mTextDarkSfc;
    GsSurface mTextLightSfc;
    GsSurface mTextDisabledSfc;

    int mLightRatio; // This will blend between selected and unselected item.

private:        

	std::shared_ptr<CEvent> mEvent;
};

#endif /* GsButton_H_ */
