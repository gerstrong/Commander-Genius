#include "GsScrollbar.h"

#include <base/video/CVideoDriver.h>
#include <base/PointDevice.h>
#include <graphics/GsGraphics.h>


GsScrollbar::GsScrollbar(const GsRect<float> &rect) :
GsWidgetsManager(rect)
{
    mpUpButton =
            add(new GsButton("", GsRect<float>(0.0f, 0.0f, 1.0f, 0.1f)));
    mpDownButton =
            add(new GsButton("", GsRect<float>(0.0f, 0.9f, 1.0f, 0.1f)));
}

void GsScrollbar::setScrollDownFn(const std::function <void ()> function)
{
    mpDownButton->setActivationEvent(function);
}

void GsScrollbar::setScrollUpFn(const std::function <void ()> function)
{
    mpUpButton->setActivationEvent(function);
}

void GsScrollbar::updateState(const float posY,
                              const float minY,
                              const float maxY)
{
    mpUpButton->enable( (posY >= maxY) ? false : true);
    mpDownButton->enable( (posY <= minY) ? false : true);

    mPosRel = -posY/(maxY-minY);

    mPosRel = std::min(mPosRel, 1.0f);
    mPosRel = std::max(mPosRel, 0.0f);
}

void GsScrollbar::processLogic()
{
    GsWidgetsManager::processLogic();
}

void GsScrollbar::processPointingStateRel(const GsRect<float> &rect)
{
    GsWidgetsManager::processPointingStateRel(rect);

    if(isReleased())
    {
        const auto absRect = rect.transformed(getRect());

        GsPointingState &pointingState = gPointDevice.mPointingState;

        const bool hasPoint = absRect.HasPoint(pointingState.mPos);                

        if(hasPoint)
        {
            const auto y1 = absRect.pos.y;
            const auto mouseY = y1 + pointingState.mPos.y;
            const auto scrollPosY = y1 + mPosRel;

            if(scrollPosY > mouseY)
            {
                mpUpButton->activateFunction();
            }

            if(scrollPosY < mouseY)
            {
                mpDownButton->activateFunction();
            }
        }
    }

}

void GsScrollbar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    blitsfc.fill(displayRect.SDLRect(), mBackgroundColor);

    // Small position indicator
    {
        auto posIndRect = displayRect;
        const auto bUpHeight = displayRect.dim.x;
        auto sliderSize = bUpHeight*0.8f;
        posIndRect.dim.y = posIndRect.dim.x = sliderSize;

        posIndRect.pos.x = displayRect.pos.x + (0.1f * sliderSize);
        posIndRect.pos.y = displayRect.pos.y + bUpHeight +
                 (mPosRel * (displayRect.dim.y-(3.0f*bUpHeight)));

        blitsfc.fill(posIndRect.SDLRect(), GsColor(0xD0, 0xD0, 0xD0));
    }

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
