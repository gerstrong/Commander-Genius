#include "GsScrollingFrame.h"

#include <base/video/CVideoDriver.h>

GsScrollingFrame::GsScrollingFrame(const GsRect<float> &rect,
                                   const GsRect<float> &virtualRect) :
GsFrame(rect),
mVirtualRect(virtualRect)
{

}
void GsScrollingFrame::processLogic()
{
    GsFrame::processLogic();

    processPointingState();
}

void GsScrollingFrame::processRender(const GsRect<float> &srcRectFloat,
                                     const GsRect<float> &dstRectFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    GsRect<float> displaySrcRect, displayDstRect;
    displayDstRect = displaySrcRect = getRect();

    displaySrcRect.transform(srcRectFloat);
    displayDstRect.transform(dstRectFloat);

    auto lRect = displayDstRect.SDLRect();

    if(mBgEnabled)
    {
        blitsfc.fill(lRect, mBackgroundColor);
    }

    auto backRect = displaySrcRect;
    auto frontRect = displayDstRect;

    backRect.pos.x += mScrollX * frontRect.dim.x;
    backRect.pos.y += mScrollY * frontRect.dim.y;


    auto &wList = getWidgetList();

    for(auto &widget : wList)
    {
        widget->processRender(backRect, frontRect);
    }

    if(mSelected)
    {
        GsColor color(0x0, 0x0, 0xFF);
        const auto colorInt = blitsfc.mapColorAlpha(color);
        blitsfc.drawFrameRect(lRect, 1, colorInt);
    }
}

void GsScrollingFrame::processRender(const GsRect<float> &rectDispCoordFloat)
{
    processRender(rectDispCoordFloat, rectDispCoordFloat);

    /*
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = getRect();

    displayRect.transform(rectDispCoordFloat);

    auto dim = rectDispCoordFloat.dim;

    displayRect.pos.x +=
            mScrollX * dim.x;
    displayRect.pos.y +=
            mScrollY * dim.y;

    for(auto &widget : mWidgetList)
    {
        widget->processRender(displayRect);
    }
*/
}


void GsScrollingFrame::setPosX(const float x)
{
    mScrollX = x;
}

void GsScrollingFrame::setPosY(const float y)
{
    mScrollY = y;
}

void GsScrollingFrame::moveX(const float dx)
{
    mScrollX += dx;
}

void GsScrollingFrame::moveY(const float dy)
{
    mScrollY += dy;
}
