/*
 * CGUIBitmap.cpp
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsBitmap.h"

CGUIBitmap::CGUIBitmap(std::shared_ptr<GsBitmap> &pBitmap) :
mpBitmap(pBitmap),
mScaledBitmap(*mpBitmap)
{}

void CGUIBitmap::setBitmapPtr(std::shared_ptr<GsBitmap> &pBitmap)
{
    mpBitmap = pBitmap;

    if(mpBitmap)
        mScaledBitmap = *mpBitmap;
    else
        mScaledBitmap.clear();
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
        mpBitmap.reset(new GsBitmap( *gGraphics.getBitmapFromStr(mTextID) ));
    }

    sizeForScreen.w /= 320;
    sizeForScreen.h /= 200;
    sizeForScreen.w *= mpBitmap->getWidth();
    sizeForScreen.h *= mpBitmap->getHeight();

    mpBitmap->scaleTo(sizeForScreen);
}


void CGUIBitmap::processLogic()
{

}



void CGUIBitmap::processRender(const GsRect<float> &RectDispCoordFloat)
{    
    if(mScaledBitmap.empty())
        return;

	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
    GsRect<Uint16> lRect = displayRect.SDLRect();

    if( mScaledBitmap.getWidth() != lRect.w ||
        mScaledBitmap.getHeight() != lRect.h )
    {
        mScaledBitmap = GsBitmap( *mpBitmap.get() );
        lRect.x = 0;    lRect.y = 0;
        mScaledBitmap.scaleTo(lRect);
    }
    else
    {
        mScaledBitmap.draw( lRect.x, lRect.y );
    }
}

