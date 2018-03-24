/*
 * CGUIBitmap.cpp
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 *  Control which displays bitmaps.
 */

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsBitmapBox.h"


CGUIBitmap::CGUIBitmap(std::shared_ptr<GsBitmap> &bmpPtr)
{
    setBitmapPtr(bmpPtr);
}

void CGUIBitmap::setBitmapPtr(std::shared_ptr<GsBitmap> &bmpPtr)
{
    if(bmpPtr)
    {
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

CGUIBitmap::CGUIBitmap(const std::string &text) :
mTextID(text)
{
    updateGraphics();
}


void CGUIBitmap::updateGraphics()
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
        *mpBitmap = *gGraphics.getBitmapFromStr(mTextID);
    }

    sizeForScreen.w /= 320;
    sizeForScreen.h /= 200;
    sizeForScreen.w *= mpBitmap->width();
    sizeForScreen.h *= mpBitmap->height();

    mpBitmap->scaleTo(sizeForScreen);
}


void CGUIBitmap::processLogic()
{

}



void CGUIBitmap::processRender(const GsRect<float> &RectDispCoordFloat)
{    
    if(!mScaledBitmapPtr)
    {
        return;
    }

    if(mScaledBitmapPtr->empty())
    {
        return;
    }

	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
    GsRect<Uint16> lRect = displayRect.SDLRect();

    if( mScaledBitmapPtr->width()  != lRect.w ||
        mScaledBitmapPtr->height() != lRect.h )
    {
        // copy the bitmap
        mScaledBitmapPtr.reset(new GsBitmap);
        *mScaledBitmapPtr = *mpBitmap;
        lRect.x = 0;    lRect.y = 0;
        mScaledBitmapPtr->scaleTo(lRect);
    }
    else
    {
        mScaledBitmapPtr->draw( lRect.x, lRect.y );
    }
}

