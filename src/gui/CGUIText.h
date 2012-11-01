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

#include "CGUIControl.h"
#include <string>

class CGUIText : public CGUIControl
{
public:
	CGUIText(const std::string& text);

	void setupButtonSurface();

	void setText(const std::string& text);

	void processLogic();
	void drawGalaxyStyle(SDL_Rect& lRect);
	void processRender(const CRect<float> &RectDispCoordFloat);

	// Defines in character units how tall and wide the entire text is.
	CRect<unsigned int> mTextDim;

private:
	std::list<std::string> mTextList;

	std::unique_ptr<SDL_Surface> mpTextDarkSfc;
	std::unique_ptr<SDL_Surface> mpTextLightSfc;
	std::unique_ptr<SDL_Surface> mpTextDisabledSfc;

};

#endif /* CGUITEXT_H_ */
