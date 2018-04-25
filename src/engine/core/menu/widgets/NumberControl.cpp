
#include "NumberControl.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>
#include <graphics/GsGraphics.h>


const int SLIDER_WIDTH = 16;

NumberControl::NumberControl(const std::string &text,
                             const int startValue,
                             const int endValue,
                             const int deltaValue,
                             const int value,
                             const bool slider,
                             const Style style) :
CGUINumberControl(text, startValue, endValue,
                  deltaValue, value, 1,
                  slider, style)
{   
    if(mStyle == Style::VORTICON)
    {
        mFontID = 1;    
    }
    else if(mStyle == Style::GALAXY)    
    {
        setupButtonSurface();
    }
    else
    {
        mFontID = 0;
    }
}


void NumberControl::setupButtonSurface()
{        
    GsFont &Font = gGraphics.getFont(mFontID);

    const std::string showText = "  " + mText + ": " + itoa(mValue);
    GsButton::setupButtonSurface(showText);

    const std::string showTextL = "  " + mText + ":<" + itoa(mValue);
    const std::string showTextR = "  " + mText + ": " + itoa(mValue) + ">";

    Font.createTextSurface(mTextLightSfcR, showTextR, 84, 234, 84 );
    Font.createTextSurface(mTextLightSfcL, showTextL, 84, 234, 84 );
}


void NumberControl::processLogic()
{
    CGUINumberControl::processLogic();

    if(mMustRedraw)
    {
        mMustRedraw = false;
        setupButtonSurface();
    }
}

void NumberControl::drawNoStyle(SDL_Rect& lRect)
{
    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    std::string text = mText + ":";

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

/*
    if(mSlider)
    {
        text += sliderStr();
    }
    else*/
    {
        text += (mDecSel) ? "\025" : " ";
        text += itoa(mValue);
        if(mIncSel)
            text += static_cast<char>(17);
        else
            text += " ";
    }

    Font.drawFont( blitsfc, text, lRect.x+40, lRect.y, false );

    drawTwirl(lRect);
}

void NumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();
    
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());
    
    if(mStyle == Style::GALAXY)
    {
        
        if(!mEnabled)
        {
            mTextDisabledSfc.blitTo(blitsfc, lRect);
        }
        else
        {
            drawEnabledButton(blitsfc, lRect, mHovered);
            
            if(mHovered)
            {
                if(mDecSel)
                    mTextLightSfcL.blitTo(blitsfc, lRect);
                else if(mIncSel)
                    mTextLightSfcR.blitTo(blitsfc, lRect);
                else
                    mTextLightSfc.blitTo(blitsfc, lRect);
            }
        }
        
        drawBlinker(lRect);
    }
    else if(mStyle == Style::VORTICON)
    {
        // Now lets draw the text of the list control
        GsFont &Font = gGraphics.getFont(mFontID);
        
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
        Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );
        
        if(mSlider)
        {
            gGraphics.getFont(2).drawFont( blitsfc, sliderStr(), lRect.x+16+(mText.size()+2)*8, lRect.y, false );
        }
        else
        {
            std::string text = (mDecSel) ? "\025" : " ";
            text += itoa(mValue);
            if(mIncSel)
                text += static_cast<char>(17);
            else
                text += " ";
            
            Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );
        }
        
        drawTwirl(lRect);
        
    }
    else
    {
        drawNoStyle(lRect);
    }
}

