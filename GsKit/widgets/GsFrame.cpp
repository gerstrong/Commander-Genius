#include "GsFrame.h"

#include <base/video/CVideoDriver.h>

GsFrame::GsFrame()
{

}


void GsFrame::processLogic()
{
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

void GsFrame::setBackgroundColor(const GsColor &color)
{
    mBackgroundColor = color;
}
