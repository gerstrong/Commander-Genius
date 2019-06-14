#include "ComboSelection.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


ComboSelection::ComboSelection(const std::string& text,
                                const std::list<std::string>& optionsList,
                               const Style &style) :
CGUIComboSelection(text, GsRect<float>(0.0f,0.0f,1.0f,1.0f),
                   optionsList,
                   style)
{
    if(mStyle == Style::GALAXY)
    {
        setTextColor(GsColor(0x26, 0x86, 0x26));
    }

    if(mStyle == Style::NONE)
    {
        mFontID = 0;
    }
    else
    {
        mFontID = 1;
    }
}


void ComboSelection::cycleOption()
{
    CGUIComboSelection::cycleOption();
}

void ComboSelection::setSelection( const std::string& selectionText )
{
    for( auto it = mOptionsList.begin() ;
              it != mOptionsList.end() ; it++)
    {
        if( *it == selectionText )
        {
            //setupButtonSurface(selectionText);
            CGUIComboSelection::setSelection(selectionText);
            mOLCurrent = it;
            return;
        }
    }

    CGUIComboSelection::setSelection(selectionText);
}

void ComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();
    
    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );    

    CGUIComboSelection::processRender(RectDispCoordFloat);
    
    /*if(mStyle == Style::GALAXY)
    {        
        if(!mEnabled)
        {
            mTextDisabledSfc.blitTo(blitsfc, lRect);
        }
        else
        {
#ifndef DISABLE_HOVER
            drawEnabledButton(blitsfc, lRect, mHovered);
#else
            drawEnabledButton(blitsfc, lRect, false);
#endif
        }
        
        drawBlinker(lRect);
    }
    else */if(mStyle == Style::VORTICON)
    {                
        // Now lets draw the text of the list control
        /*auto &Font = gGraphics.getFont(mFontID);

        Font.drawFont( blitsfc.getSDLSurface(), mText, lRect.x+24, lRect.y, false );
        Font.drawFont( blitsfc.getSDLSurface(), ":", lRect.x+24+mText.size()*8, lRect.y, false );
        const std::string text = (*mOLCurrent);
        Font.drawFont( blitsfc.getSDLSurface(), text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );*/
        
        drawTwirl(lRect);      
    }
    
}

