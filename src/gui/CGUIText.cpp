/*
 * CGUIText.cpp
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 */

#include "CGUIText.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include "StringUtils.h"

CGUIText::CGUIText(const std::string& text)
{
	setText(text);
}


void CGUIText::setText(const std::string& text)
{
	if(!mTextList.empty())
		mTextList.clear();

	// Split up the text in lines
	mTextDim.w = 0;
	// TODO: I think there is a more elegant way to achieve this!
	std::string buf = "";
	for( size_t i=0 ; i<text.size() ; i++ )
	{
		if( endofText(text.substr(i)) )
		{
			if( mTextDim.w<buf.size() )
				mTextDim.w=buf.size();

			mTextList.push_back(buf);
			buf.clear();
		}
		else
			buf += text[i];
	}

	size_t pos = 0;
	if(!buf.empty())
		while( (pos = buf.find('\n')) != std::string::npos )
			buf.erase(pos,1);
	mTextList.push_back(buf);

	if( mTextDim.w<buf.size() )
		mTextDim.w=buf.size();

	mTextDim.h = mTextList.size();
}


void CGUIText::processLogic()
{

}

void CGUIText::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	std::list<std::string>::iterator text = mTextList.begin();
	for( size_t i=0 ; text != mTextList.end() ; text++, i++ )
	{
		Font.drawFontCentered(g_pVideoDriver->getBlitSurface(), *text, lRect.x, lRect.w, lRect.y+i*8, false);
	}


}
