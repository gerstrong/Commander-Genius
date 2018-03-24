/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef CGUITEXT_H_
#define CGUITEXT_H_

#include <string>

#include "GsControl.h"

class CGUIText : public GsControl
{
public:
	CGUIText(const std::string& text);

    void setupButtonSurface(const std::string& text);

	virtual void setText(const std::string& text);

	virtual void processLogic();
    //void drawGalaxyStyle(SDL_Rect& lRect);
    virtual void processRender(const GsRect<float> &RectDispCoordFloat);

	// Defines in character units how tall and wide the entire text is.
    GsRect<unsigned int> mTextDim;

protected:
	std::list<std::string> mTextList;
	
private:
	std::unique_ptr<SDL_Surface> mpTextDarkSfc;
	std::unique_ptr<SDL_Surface> mpTextLightSfc;
	std::unique_ptr<SDL_Surface> mpTextDisabledSfc;


    /// Automatic horizontal scrolling in case the text is too big
    float mScrollPosMax = 0;
    float mScrollPos = 0;
    float mScrollVel = 0.3;

    enum class ScrollDir
    {
        NONE,
        LEFT,
        RIGHT
    } mScrollDir = ScrollDir::NONE;

};

#endif /* CGUITEXT_H_ */
