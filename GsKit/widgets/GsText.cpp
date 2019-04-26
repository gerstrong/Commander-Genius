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


CGUIText::CGUIText(const std::string& text,
         const GsRect<float> &rect) :
GsControl(rect)
{
    setText(text);
}


void CGUIText::setText(const std::string& text)
{    
    const GsColor textColor = { 0, 0, 0, 0 };

#if defined(USE_SDL_TTF)
    mTrueTypeFont.open("Oswald-Regular.ttf", 28);
    mTrueTypeFont.render(mTextSfc, text, textColor);
#else


	if(!mTextList.empty())
		mTextList.clear();

	// Split up the text in lines
    mTextDim.dim.x = 0;

	// TODO: I think there is a more elegant way to achieve this!
	std::string buf = "";
	for( size_t i=0 ; i<text.size() ; i++ )
	{
		if( endofText(text.substr(i)) )
		{            
            if( mTextDim.dim.x<buf.size() )
                mTextDim.dim.x=buf.size();

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

    if( mTextDim.dim.x<buf.size() )
        mTextDim.dim.x=buf.size();

    mTextDim.dim.y = mTextList.size();

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

#if defined(USE_SDL_TTF)
	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    auto &blit = gVideoDriver.gameSfc();

    mTextSfc.blitTo(blit, lRect);

#endif
}

void CGUIText::processRender(const GsRect<float> &backRect,
                             const GsRect<float> &frontRect)
{
    // Transform this object to the display coordinates
    auto objBackRect = backRect.transformed(mRect);
    auto objFrontRect = objBackRect.clipped(frontRect);

    auto &blit = gVideoDriver.gameSfc();

    auto srcRect = objBackRect.SDLRect();
    auto dstRect = objFrontRect.SDLRect();

    /*auto *renderer = &gVideoDriver.getRendererRef();

    srcRect.x = (srcRect.x < dstRect.x) ? dstRect.x-srcRect.x : 0;
    srcRect.y = (srcRect.y < dstRect.y) ? dstRect.y-srcRect.y : 0;

    srcRect.dim.x = (dstRect.dim.x < srcRect.dim.x) ? dstRect.dim.x : srcRect.dim.x;
    srcRect.h = (dstRect.h < srcRect.h) ? dstRect.h : srcRect.h;

    mTextSfc.blitTo(blit, srcRect, dstRect);
    //mTextSfc.blitTo(blit, dstRect);

    */
}

