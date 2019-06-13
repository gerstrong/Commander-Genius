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

#include <graphics/cgttf.h>

CGUIText::CGUIText(const std::string& text,
         const GsRect<float> &rect) :
GsControl(rect)
{
    setText(text);
}

CGUIText::CGUIText(const std::string& text,
         const GsRect<float> &rect,
         const GsControl::Style style) :
GsControl(style, rect)
{
    setText(text);
}



void CGUIText::setText(const std::string& text)
{    
    if(!mTextVec.empty())
        mTextVec.clear();

	// Split up the text in lines
    mTextDim.dim.x = 0;

	// TODO: I think there is a more elegant way to achieve this!
	std::string buf = "";
	for( size_t i=0 ; i<text.size() ; i++ )
	{
		if( endofText(text.substr(i)) )
		{            
            if( mTextDim.dim.x < buf.size() )
                mTextDim.dim.x = static_cast<unsigned int>(buf.size());

            mTextVec.push_back(buf);
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
    mTextVec.push_back(buf);

    if( mTextDim.dim.x < buf.size() )
        mTextDim.dim.x = static_cast<unsigned int>(buf.size());

    mTextDim.dim.y = static_cast<unsigned int>(mTextVec.size());

    mTextChanged = (mText != text);
    mText = text;
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

void CGUIText::updateFontState(const GsRect<float> &displayRect)
{
    const GsColor textColor = { 0, 0, 0, 0 };

    const int reqFontSize = int(displayRect.dim.y*0.75f);

    if(mFontSize != reqFontSize || mTextChanged)
    {
        mFontSize = reqFontSize;

        mTrueTypeFont.openFromMem(gCgTtf, reqFontSize);

        const auto numTexLines = mTextVec.size();
        mTextSfcVec.resize(numTexLines);

        for(unsigned int idx = 0 ; idx<numTexLines ; idx++)
        {
            mTrueTypeFont.render(mTextSfcVec[idx],
                                 mTextVec[idx],
                                 textColor);
        }

        mTextChanged = false;
    }
}

void CGUIText::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);

    auto lRect = displayRect.SDLRect();

    if(mStyle == GsControl::NONE)
    {

#if defined(USE_SDL_TTF)

    updateFontState(displayRect);

    auto &blit = gVideoDriver.gameSfc();

    unsigned int totTextSfcH = 0;
    for(auto &textSfc : mTextSfcVec)
    {        
        if(textSfc.empty())
            break;

        const auto textW = textSfc.width();
        const auto textH = textSfc.height();
        GsVec2D<int> textSfcDim(textW, textH);

        GsRect<float> blitPos = displayRect;
        blitPos.pos = blitPos.pos + (blitPos.dim-textSfcDim)/2;
        blitPos.pos.y += totTextSfcH;

        textSfc.blitTo(blit, blitPos.SDLRect());
        totTextSfcH += textH;
    }    

#else
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
#endif

    }
    else
    {
        // Now lets draw the text of the list control
        auto &Font = gGraphics.getFont(mFontID);

        auto textIt = mTextVec.begin();
        for( size_t i=0 ; textIt != mTextVec.end() ; textIt++, i++ )
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
                Font.drawFontCentered(gVideoDriver.getBlitSurface(), theText,
                                      lRect.x, lRect.w, lRect.y+i*8, false);
                mScrollPosMax = 0;
            }
        }
    }
}

void CGUIText::processRender(const GsRect<float> &/*backRect*/,
                             const GsRect<float> &/*frontRect*/)
{
    // Transform this object to the display coordinates
    assert(0);
    /*
    auto objBackRect = backRect.transformed(mRect);
    auto objFrontRect = objBackRect.clipped(frontRect);

    auto &blit = gVideoDriver.gameSfc();

    auto srcRect = objBackRect.SDLRect();
    auto dstRect = objFrontRect.SDLRect();

    auto *renderer = &gVideoDriver.getRendererRef();

    srcRect.x = (srcRect.x < dstRect.x) ? dstRect.x-srcRect.x : 0;
    srcRect.y = (srcRect.y < dstRect.y) ? dstRect.y-srcRect.y : 0;

    srcRect.dim.x = (dstRect.dim.x < srcRect.dim.x) ? dstRect.dim.x : srcRect.dim.x;
    srcRect.h = (dstRect.h < srcRect.h) ? dstRect.h : srcRect.h;

    mTextSfc.blitTo(blit, srcRect, dstRect);
    //mTextSfc.blitTo(blit, dstRect);

    */
}

