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
}


void ComboSelection::cycleOption()
{
    CGUIComboSelection::cycleOption();

    setupButtonSurface(*mOLCurrent);
}


void ComboSelection::setupButtonSurface(const std::string &optionText)
{
    GsFont &Font = gGraphics.getFont(mFontID);

    const std::string showText = "  " + mText + ": " + optionText;
    Font.createTextSurface(mTextDarkSfc, showText, 38, 134, 38 );
    Font.createTextSurface(mTextLightSfc, showText, 84, 234, 84 );
    Font.createTextSurface(mTextDisabledSfc, showText, 123, 150, 123 );
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

    GsWeakSurface sfc(gVideoDriver.getBlitSurface());

    if(!mEnabled)
    {
        mTextDisabledSfc.blitTo(sfc, lRect);
    }
    else
    {
        if(mHovered)
        {
            mTextLightSfc.blitTo(sfc, lRect);
        }
        else // Button is not hovered
        {
            mTextDarkSfc.blitTo(sfc, lRect);
        }
    }

    drawBlinker(lRect);
}

}
