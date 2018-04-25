#include "ComboSelection.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


ComboSelection::ComboSelection(const std::string& text,
                                const std::list<std::string>& optionsList, 
                               const Style &style) :
CGUIComboSelection(text, optionsList, style)
{
    if(mStyle == Style::NONE)
    {
        mFontID = 0;
    }
    else
    {
        mFontID = 1;
    }
    setupButtonSurface(mText);
}


void ComboSelection::cycleOption()
{
    CGUIComboSelection::cycleOption();

    setupButtonSurface(*mOLCurrent);
}


void ComboSelection::setupButtonSurface(const std::string &optionText)
{
    const std::string showText = "  " + mText + ": " + optionText;

    GsButton::setupButtonSurface(showText);
}

void ComboSelection::setSelection( const std::string& selectionText )
{
    std::list<std::string>::const_iterator ptr = mOLCurrent;

    do
    {
        if( *mOLCurrent == selectionText )
        {
            setupButtonSurface(selectionText);
            return;
        }

        cycleOption();

    } while( ptr != mOLCurrent );

    CGUIComboSelection::setSelection(selectionText);
}

void ComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();
    
    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );    
    
    if(mStyle == Style::GALAXY)
    {        
        if(!mEnabled)
        {
            mTextDisabledSfc.blitTo(blitsfc, lRect);
        }
        else
        {
            drawEnabledButton(blitsfc, lRect, mHovered);
        }
        
        drawBlinker(lRect);
    }
    else if(mStyle == Style::VORTICON)
    {
        if(!mEnabled)
            return;
        
        // Now lets draw the text of the list control
        GsFont &Font = gGraphics.getFont(mFontID);
        
        Font.drawFont( blitsfc.getSDLSurface(), mText, lRect.x+24, lRect.y, false );
        Font.drawFont( blitsfc.getSDLSurface(), ":", lRect.x+24+mText.size()*8, lRect.y, false );
        const std::string text = (*mOLCurrent);
        Font.drawFont( blitsfc.getSDLSurface(), text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );
        
        drawTwirl(lRect);      
    }
    else
    {
        if(!mEnabled)
            return;

        // Now lets draw the text of the list control
        GsFont &Font = gGraphics.getFont(mFontID);

        std::string text = mText + ": " + (*mOLCurrent);
        Font.drawFont( blitsfc.getSDLSurface(), text, lRect.x+40, lRect.y, false );

        drawTwirl(lRect);
    }
    
}

