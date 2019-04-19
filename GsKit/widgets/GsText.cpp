/*
 * CGUIText.cpp
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>


#include "GsText.h"

CGUIText::CGUIText(const std::string& text)
{
	setText(text);
}


void CGUIText::setText(const std::string& text)
{
#if defined(USE_SDL_TTF)
    const SDL_Color textColor = { 0xff, 0, 0, 0 };

    mTrueTypeFont.open("lazy.ttf", 28);
    mTrueTypeFont.render(mTextSfc, text, textColor);
#else


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
        {
			buf += text[i];
        }
	}

	size_t pos = 0;
	if(!buf.empty())
	{
		while( (pos = buf.find('\n')) != std::string::npos )
			buf.erase(pos,1);
	}
	mTextList.push_back(buf);

	if( mTextDim.w<buf.size() )
		mTextDim.w=buf.size();

	mTextDim.h = mTextList.size();

#endif
}


void CGUIText::processLogic()
{
    // Horizontal scrolling.
    // If Max is zero, nothing need to be scrolled
    if(mScrollPosMax <= 0)
        return;

    // Check if scroll position touches the edges
    if(mScrollPos <= 0)
    {
        mScrollDir = ScrollDir::RIGHT;
        mScrollPos = 0;
    }
    else if(mScrollPos >= mScrollPosMax)
    {
        mScrollDir = ScrollDir::LEFT;
        mScrollPos = mScrollPosMax;
    }

    // Scroll into the direction
    if(mScrollDir == ScrollDir::LEFT)
    {
        mScrollPos -= mScrollVel;
    }
    else if(mScrollDir == ScrollDir::RIGHT)
    {
        mScrollPos += mScrollVel;
    }
}

void CGUIText::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();


#if defined(USE_SDL_TTF)
    auto *blit = gVideoDriver.getBlitSurface();

    BlitSurface(mTextSfc.getSDLSurface(), nullptr,
                blit, &lRect);

    BlitSurface(mTextSfc.getSDLSurface(), nullptr,
                blit, nullptr);
#endif


    /*auto *renderer = &gVideoDriver.getRendererRef();

    //Render to screen
    SDL_RenderCopyEx( renderer,
                      mTexture.getPtr(), nullptr,
                      &lRect,
                      0, nullptr, SDL_FLIP_NONE );*/

    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);

    std::list<std::string>::iterator textIt = mTextList.begin();
    for( size_t i=0 ; textIt != mTextList.end() ; textIt++, i++ )
    {
        auto &theText = *textIt;

        const int textWidth = Font.calcPixelTextWidth(theText);

        // The tolerance is the amount of pixels at least of difference to consider
        // for scrolling. We consider a tolerance so strange jittery are avoided for text
        // that nearly fits
        const int tol = 8;

        // The first text item decides wheter scrolling takes place
        if(textWidth > lRect.w + tol) // tolerance
        {
            const auto diff = textWidth - lRect.w;
            mScrollPosMax = diff;

            Font.drawFont(gVideoDriver.getBlitSurface(),
                          theText,
                          lRect.x-int(mScrollPos),
                          lRect.y+i*8,
                          false);
        }
        else
        {
            Font.drawFontCentered(gVideoDriver.getBlitSurface(), theText, lRect.x, lRect.w, lRect.y+i*8, false);
            mScrollPosMax = 0;
        }
    }
}
