/*
 * GsButton.cpp
 *
 *  Created on: 26.11.2011
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <widgets/GsMenuController.h>
#include <base/PointDevice.h>

#include "GsButton.h"


const int BLEND_SPEED = 16;


GsButton::GsButton(const std::string& text,
            CEvent *ev,
            const Style style,
            const float red,
            const float green,
            const float blue) :
mText(text),
mLightRatio(128),
mEvent(ev),
mRed(red),
mGreen(green),
mBlue(blue)
{}


bool GsButton::sendEvent(const InputCommands command)
{
    if(command == IC_STATUS || command == IC_JUMP)
    {
        gEventManager.add(mEvent);
        return true;
    }
    return false;
}


void GsButton::updateGraphics()
{
    if( auto menuEv = std::dynamic_pointer_cast<OpenMenuEvent>(mEvent)  )
    {
        menuEv->mMenuDialogPointer->updateGraphics();
    }
}


void GsButton::processLogic()
{
    processPointingState();

    if(mEnabled)
    {
        // For some nice special effects
        if(mHovered || mSelected)
        {
            int maxBlend = 224;

            if(mHovered && mSelected)
            {
                maxBlend = 255;
            }

            if(mLightRatio+BLEND_SPEED < maxBlend)
               mLightRatio += BLEND_SPEED;
            else
               mLightRatio = maxBlend;
        }
        else // Button is not hovered
        {
            if(mLightRatio-BLEND_SPEED > 0)
               mLightRatio -= BLEND_SPEED;
            else
               mLightRatio = 0;
        }

        // If button was pushed and gets released, trigger the assigned event.
        if(mReleased)
        {
            gEventManager.add(mEvent);
        }
    }
}

void GsButton::drawNoStyle(SDL_Rect& lRect)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int lComp;

    // Set as default a grey border
    Uint32 borderColor = blitsfc.mapRGBA( 0xBB, 0xBB, 0xBB, 0xFF);

    if(mEnabled)
    {
        if( mPressed || mSelected )
        {
            // Try to highlight the border color a bit more by determing which one dominates the most
            auto red   = Uint8(mRed*127.0f);
            auto green = Uint8(mGreen*127.0f);
            auto blue  = Uint8(mBlue*127.0f);

            if(red > green && red > blue)
                red <<= 1;
            else if(green > red  && green > blue)
                green <<= 1;
            else if(blue > green && blue > red )
                blue <<= 1;


            // If want to highlight the button set the color
            borderColor = blitsfc.mapRGBA( red, green, blue, 0xFF);

            lComp = 0xFF - (mLightRatio*(0xFF-0xCF)/255);
        }
        else
        {
            lComp = 0xFF - (mLightRatio*(0xFF-0xDF)/255);
        }
    }
    else
    {
        lComp = 0xFF;
    }


    auto lcompf = float(lComp);

    auto redC   = Uint8(lcompf*mRed);
    auto greenC = Uint8(lcompf*mGreen);
    auto blueC  = Uint8(lcompf*mBlue);

    const auto fillColor = blitsfc.mapRGBA( redC, greenC, blueC, 0xFF);

    GsRect<Uint16> rect(lRect);

    blitsfc.drawRect( rect, 2, borderColor, fillColor );


    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);

    if(mEnabled) // If the button is enabled use the normal text, otherwise the highlighted color
        Font.drawFontCentered( blitsfc.getSDLSurface(), mText, lRect.x, lRect.w, lRect.y, lRect.h, false );
    else
        Font.drawFontCentered( blitsfc.getSDLSurface(), mText, lRect.x, lRect.w, lRect.y, lRect.h, true );
}

void GsButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    auto displayRect = mRect;

    displayRect.transform(RectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

    drawNoStyle(lRect);
}
