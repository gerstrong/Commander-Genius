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
        enableHighlightBg(false);

        mNormalTextColor = GsColor(0x26, 0x86, 0x26);
        mHoverBgColor = GsColor(0x66, 0x66, 0x66);
        mHoverTextColor = GsColor(0x66, 0xC6, 0x66);

        /*setTextColorHovered(mHoverTextColor);
        setTextColorPressed(GsColor(0x66, 0xF6, 0x66));
        setTextColorSelected(GsColor(0xA6, 0xC6, 0x66));
        */


        mColorNormal   = GsColor(0x26, 0x86, 0x26);
        mColorHovered  = GsColor(0x66, 0xC6, 0x66);
        mColorPressed  = GsColor(0x66, 0xF6, 0x66);
        mColorReleased = GsColor(0x46, 0xF6, 0x56);
        mColorSelected = GsColor(0xA6, 0xC6, 0x66);

        setTextColor(mColorNormal);

        mpFeatureName->setFontId(1);
        mpFeatureValue->setFontId(1);
    }

    if(mStyle == Style::VORTICON)
    {
        enableTwirl(true);
        enableHighlightBg(false);
    }
}

/*
void ComboSelection::spawnSubWidgets()
{
    mpFeatureName =
            addWidget(new CGUIText(mFeatureText,
                                GsRect<float>(0.0f, 0.0f, 0.5f, 1.0f)));

    mpFeatureValue =
            addWidget(new GsButton("?",
                                GsRect<float>(0.5f, 0.0f, 0.5f, 1.0f),
                                [&]{this->cycleOption();}));
}
*/

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

void ComboSelection::processLogic()
{
    setTextColor(mColorNormal);

    if(mHovered)
    {
        setTextColor(mColorHovered);
    }

    CGUIComboSelection::processLogic();
}
