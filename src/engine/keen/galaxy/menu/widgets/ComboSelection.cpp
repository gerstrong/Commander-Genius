#include "ComboSelection.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

namespace galaxy
{

ComboSelection::ComboSelection( const std::string& text,
                                const std::list<std::string>& optionsList ) :
CGUIComboSelection(text, optionsList)
{
    mFontID = 1;
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

}
