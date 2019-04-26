#include "GsFrame.h"

#include <base/video/CVideoDriver.h>

GsFrame::GsFrame(const GsRect<float> &rect) :
GsControlsManager(rect)
{

}


void GsFrame::processLogic()
{    
    processPointingState();

    processBlendEffects();

    if(mEnabled)
    {
        // If button was pushed and gets released, trigger the assigned event.
        if(mReleased)
        {
            if(mFunction)
            {
                mFunction();
            }
        }
    }

    for(auto &obj : mControlList)
    {
        obj->processLogic();
    }

}

void GsFrame::processRender(const GsRect<float> &rectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = mRect;

    displayRect.transform(rectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

    blitsfc.fill(lRect, mBackgroundColor);

    for(auto &obj : mControlList)
    {
        obj->processRender(rectDispCoordFloat);
    }

}

void GsFrame::processRender(const GsRect<float> &backRect,
                            const GsRect<float> &frontRect)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform this object to the display coordinates
    auto objBackRect = backRect.transformed(mRect);
    auto objFrontRect = objBackRect.clipped(frontRect);

    blitsfc.fill(objFrontRect.SDLRect(), mBackgroundColor);

    for(auto &obj : mControlList)
    {
        obj->processRender(objBackRect,
                           objFrontRect);
    }
}



void GsFrame::setBackgroundColor(const GsColor &color)
{
    mBackgroundColor = color;
}
