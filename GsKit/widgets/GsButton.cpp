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
#include <base/GsTTFSystem.h>

#include "GsButton.h"

GsButton::GsButton(const std::string& text,
                   const GsRect<float> &rect,
            CEvent *ev,
            const int fontId,
            const float red,
            const float green,
            const float blue) :
GsControl(rect),
mText(text),
mTextWidget(text,
            GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            fontId),
mEvent(ev),
mRed(red),
mGreen(green),
mBlue(blue)
{}

GsButton::GsButton(const std::string& text,
         const GsRect<float> &rect,
         CEvent *ev,
         const int fontId,
         const GsColor &color) :
GsButton(text, rect, ev, fontId,
         float(color.r)/float(0xFF),
         float(color.g)/float(0xFF),
         float(color.b)/float(0xFF) )
{}

GsButton::GsButton(const std::string& text,
            CEvent *ev,
            const int fontId,
            const float red,
            const float green,
            const float blue) :
GsControl(),
mText(text),
mTextWidget(text,
            GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            fontId),
mEvent(ev),
mRed(red),
mGreen(green),
mBlue(blue)
{}

GsButton::GsButton(const std::string& text,
                   const std::function <void ()>& f,
                   const int fontId,
                   const float red,
                   const float green,
                   const float blue) :
GsControl(),
mText(text),
mTextWidget(text,
            GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            fontId),
mRed(red),
mGreen(green),
mBlue(blue),
mFunction(f)
{}

GsButton::GsButton(const std::string& text,
         const GsRect<float> &rect,
         const std::function <void ()>& f,
         const int fontId,
         const float red,
         const float green,
         const float blue) :
    GsControl(rect),
    mText(text),
    mTextWidget(text,
                GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
                fontId),
    mRed(red),
    mGreen(green),
    mBlue(blue),
    mFunction(f)
{}


GsButton::GsButton(const std::string& text,
         const GsRect<float> &rect,
         const std::function <void ()>& f,
         const int fontId,
         const GsColor &color):
GsButton(text,rect,f,fontId,
         float(color.r)/float(0xFF),
         float(color.g)/float(0xFF),
         float(color.b)/float(0xFF))
{}



bool GsButton::sendEvent(const InputCommand command)
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
    processBlendEffects();

    if(mEnabled)
    {
        // If button was pushed and gets released, trigger the assigned event.
        if( mReleased )
        {
            if(mFunction)
            {
                mFunction();
            }
            else
            {
                gEventManager.add(mEvent);
            }
        }
    }   

    mTextWidget.processLogic();
}

void GsButton::drawNoStyle(const SDL_Rect& lRect)
{
    if(lRect.h == 0 || lRect.w == 0)
        return;

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int lComp = 0xFF;

    // Set as default a grey border
    Uint32 borderColor = blitsfc.mapRGBA( 0xBB, 0xBB, 0xBB, 0xFF);

    if(mEnabled)
    {
        auto lightRatio = mLightRatio;

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

            if(mPressed)
            {
                red <<= 1;
                blue >>= 1;
                lightRatio <<= 1;
            }

            // If want to highlight the button set the color
            borderColor = blitsfc.mapRGBA( red, green, blue, 0xFF);

            lComp = 0xFF - (lightRatio*(0xFF-0xCF)/255);
        }
        else
        {
            lComp = 0xFF - (lightRatio*(0xFF-0xDF)/255);
        }
    }


    auto lcompf = float(lComp);

    auto redC   = Uint8(lcompf*mRed);
    auto greenC = Uint8(lcompf*mGreen);
    auto blueC  = Uint8(lcompf*mBlue);

    if(!mEnabled)
    {
        redC = 0x8F;
        greenC = 0x8F;
        blueC = 0x8F;
    }

    const auto fillColor = blitsfc.mapRGBA( redC, greenC, blueC, 0xFF);

    GsRect<Uint16> rect(lRect);

    if(mShowBorders)
    {
        blitsfc.drawRect( rect, 2, borderColor, fillColor );
    }    
    else if(mUseBackground)
    {
        blitsfc.drawRect( rect, fillColor );
    }


    /*
    if(!gTTFDriver.isActive())
    {
        // Now lets draw the text of the list control
        auto &Font = gGraphics.getFontLegacy( Uint8(mFontID) );

        if(mEnabled) // If the button is enabled use the normal text, otherwise the highlighted color
        {
            Font.drawFontCentered( blitsfc.getSDLSurface(), mText,
                                   lRect.x, lRect.w, lRect.y, lRect.h, false );
        }
        else
        {
            Font.drawFontCentered( blitsfc.getSDLSurface(), mText,
                                   lRect.x, lRect.w, lRect.y, lRect.h, true );
        }
    }
    */
}

void GsButton::setupButtonSurface(const std::string &text)
{
}

void GsButton::drawEnabledButton(GsWeakSurface &blitsfc,
                                 const SDL_Rect& lRect,
                                 const bool alternate)
{
    //mTextLightSfc.setAlpha(mLightRatio);
    //mTextDarkSfc.setAlpha(255-mLightRatio);

    /*
    if(mLightRatio > 0)
    {
        if(alternate)
        {
            mTextRedSfc.blitTo(blitsfc, lRect);
        }
        else
        {
            mTextLightSfc.blitTo(blitsfc, lRect);
        }
    }

    if(mLightRatio < 255)
    {
        mTextDarkSfc.blitTo(blitsfc, lRect);
    }
    */
}


void GsButton::processRender(const GsRect<float> &RectDispCoordFloat)
{       
    // Transform to the display coordinates
    auto displayRect = getRect();

    displayRect.transform(RectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

    drawNoStyle(lRect);

    mTextWidget.processRender(displayRect);
}

void GsButton::processRender(const GsRect<float> &backRect,
                             const GsRect<float> &frontRect)
{    
    // Transform this object display coordinates
    auto objBackRect = backRect.transformed(getRect());
    auto objFrontRect = objBackRect.clipped(frontRect);

    drawNoStyle( objFrontRect.SDLRect() );

    mTextWidget.processRender(objBackRect,
                              objFrontRect);
}
