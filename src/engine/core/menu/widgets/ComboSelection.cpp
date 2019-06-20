#include "ComboSelection.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


ComboSelection::ComboSelection(const std::string& text,
                               const std::list<std::string>& optionsList,
                               const Style &style) :
CGUIComboSelection(text,
                   GsRect<float>(0.0f,0.0f,1.0f,1.0f),
                   optionsList),
mStyle(style)
{
    if(mStyle == Style::GALAXY)
    {        
        enableButtonBorders(false);
        enableWidgetsCenteringH(false);
        enableBlinker(true);

        mNormalTextColor = GsColor(0x26, 0x86, 0x26);
        mHoverBgColor = GsColor(0x66, 0x66, 0x66);
        mHoverTextColor = GsColor(0x66, 0xC6, 0x66);
        setTextColor(mNormalTextColor);
        setTextColorHovered(mHoverTextColor);
        setTextColorPressed(GsColor(0x66, 0xF6, 0x66));
        setTextColorSelected(GsColor(0xA6, 0xC6, 0x66));
    }

    if(mStyle == Style::VORTICON)
    {
        enableTwirl(true);
    }

    if(mStyle == Style::NONE)
    {
        //mFontID = 0;
    }
    else
    {
        //mFontID = 1;
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
    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();
    
    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );    

    auto controlsRect = RectDispCoordFloat;

    if(mDrawBlinker)
    {
        drawBlinker(lRect);
        controlsRect.pos.x += 11;
        controlsRect.dim.x -= 11;
    }

    if(mDrawTwirl)
    {
        controlsRect.pos.x += 24;
        controlsRect.dim.x -= 24;
        drawTwirl(lRect);
    }

    CGUIComboSelection::processRender(controlsRect);    
}

