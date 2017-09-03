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

class CGUIText : public CGUIControl
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

};

#endif /* CGUITEXT_H_ */
