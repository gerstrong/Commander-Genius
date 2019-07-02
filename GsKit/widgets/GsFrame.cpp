#include "GsFrame.h"

#include <base/video/CVideoDriver.h>

GsFrame::GsFrame(const GsRect<float> &rect) :
GsControlsManager(rect)
{

}


void GsFrame::processLogic()
{    
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

    auto &wList = getWidgetList();

    for(auto &obj : wList)
    {
        obj->processLogic();
    }

}

void GsFrame::processRender(const GsRect<float> &rectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = getRect();

    displayRect.transform(rectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

    if(mBgEnabled)
    {
        blitsfc.fill(lRect, mBackgroundColor);
    }

    auto &wList = getWidgetList();

    for(auto &obj : wList)
    {
        obj->processRender(rectDispCoordFloat);
    }

}

void GsFrame::processRender(const GsRect<float> &backRect,
                            const GsRect<float> &frontRect)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform this object to the display coordinates
    auto localRect = getRect();
    auto objBackRect = backRect.transformed(localRect);
    auto objFrontRect = objBackRect.clipped(frontRect);

    if(mBgEnabled)
    {
        blitsfc.fill(objFrontRect.SDLRect(), mBackgroundColor);
    }

    auto &wList = getWidgetList();
    for(auto &obj : wList)
    {
       obj->processRender(objBackRect, objFrontRect);
    }
}



void GsFrame::setBackgroundColor(const GsColor &color)
{
    mBackgroundColor = color;
}
