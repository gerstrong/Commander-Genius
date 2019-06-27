/*
 * GsBitmapBox.cpp
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 *  Control which displays bitmaps.
 */

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsBitmapBox.h"


GsBitmapBox::GsBitmapBox(const GsRect<float> &rect) :
    GsControl(rect)
{}


GsBitmapBox::GsBitmapBox(std::shared_ptr<GsBitmap> &bmpPtr)
{
    setBitmapPtr(bmpPtr);
}

void GsBitmapBox::setBitmapPtr(std::shared_ptr<GsBitmap> &bmpPtr)
{
    if(bmpPtr)
    {
        if(bmpPtr == mpBitmap)
            return;

        mpBitmap = bmpPtr;

        // copy the bitmap
        mScaledBitmapPtr.reset(new GsBitmap);
        *mScaledBitmapPtr = *bmpPtr;
    }
    else
    {
        mScaledBitmapPtr = nullptr;
        mpBitmap = nullptr;
    }
}

GsBitmapBox::GsBitmapBox(const std::string &text) :
mTextID(text)
{
    updateGraphics();
}


void GsBitmapBox::updateGraphics()
{
    GsRect<Uint16> sizeForScreen = gVideoDriver.getGameResolution();

    // If mTextID is not empty, the bitmap is part of the gGraphics database.
    // Otherwise it is an external and original does not exist.
    // In that case we just will try to scale it somehow...
    if(!mTextID.empty())
    {
        mpBitmap.reset(new GsBitmap);

        // Copy the bitmap to the smart pointer
        mScaledBitmapPtr.reset(new GsBitmap);
        *mpBitmap = *gGraphics.getBitmapFromStr(0, mTextID);
    }

    sizeForScreen.dim.x /= 320;
    sizeForScreen.dim.y /= 200;
    sizeForScreen.dim.x *= mpBitmap->width();
    sizeForScreen.dim.y *= mpBitmap->height();

    mpBitmap->scaleTo(sizeForScreen);
}


void GsBitmapBox::processLogic()
{

}



void GsBitmapBox::processRender(const GsRect<float> &RectDispCoordFloat)
{    
    // Check for loaded and valid bitmap object data
    if(!mScaledBitmapPtr)
        return;

    if(mScaledBitmapPtr->empty())
        return;

    // Transform to the display coordinates
    auto displayRect = getRect();

    displayRect.transform(RectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

	// Transform to the display coordinates
    /*GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    GsRect<Uint16> lRect = displayRect.SDLRect();*/

    if( mScaledBitmapPtr->width()  != lRect.w ||
        mScaledBitmapPtr->height() != lRect.h )
    {
        // copy the bitmap
        mScaledBitmapPtr.reset(new GsBitmap);

        // Copy original into buffer
        *mScaledBitmapPtr = *mpBitmap;

        lRect.x = 0;    lRect.y = 0;
        mScaledBitmapPtr->scaleTo(lRect);
    }
    else
    {
        mScaledBitmapPtr->draw( lRect.x, lRect.y );
    }
}

