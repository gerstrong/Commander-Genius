#include "GsScrollbar.h"

#include <base/video/CVideoDriver.h>
#include <base/PointDevice.h>
#include <graphics/GsGraphics.h>


GsScrollbar::GsScrollbar(const GsRect<float> &rect) :
GsWidgetsManager(rect)
{
    mpUpButton =
            add(new GsButton("U", GsRect<float>(0.0f, 0.0f, 1.0f, 0.1f)));
    mpDownButton =
            add(new GsButton("D", GsRect<float>(0.0f, 0.9f, 1.0f, 0.1f)));
}

void GsScrollbar::setScrollDownFn(const std::function <void ()> function)
{
    mpDownButton->setActivationEvent(function);
}

void GsScrollbar::setScrollUpFn(const std::function <void ()> function)
{
    mpUpButton->setActivationEvent(function);
}

void GsScrollbar::processLogic()
{
    GsWidgetsManager::processLogic();
}


void GsScrollbar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    blitsfc.fill(displayRect.SDLRect(), mBackgroundColor);

    auto &widgetsList = getWidgetList();

    for( auto &ctrl : widgetsList)
    {
        ctrl->processRender(displayRect);
    }
}

bool GsScrollbar::sendEvent(const InputCommand)
{
    return false;
}


void GsScrollbar::setBackgroundColor(const GsColor &color)
{
    mBackgroundColor = color;
}
