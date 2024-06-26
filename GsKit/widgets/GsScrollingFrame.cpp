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

void GsScrollingFrame::processPointingStateRel(const GsRect<float> &rect)
{
    GsControl::processPointingStateRel(rect);

    if(this->isReleased())
    {
        this->select(true);
    }

    auto absRect = rect.transformed(getRect());

    absRect.pos.y += mScrollY * absRect.dim.y;
    absRect.pos.x += mScrollX * absRect.dim.x;

    auto &controlsList = getControlsList();
    for(auto &widget : controlsList)
    {
        widget->processPointingStateRel(absRect);
    }
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
        GsColor color(0x80, 0x80, 0x80);
        const auto colorInt = blitsfc.mapColorAlpha(color);
        blitsfc.drawFrameRect(lRect, 1, colorInt);
    }
}

void GsScrollingFrame::processRender(const GsRect<float> &rectDispCoordFloat)
{
    processRender(rectDispCoordFloat, rectDispCoordFloat);
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


void GsScrollingFrame::setBackgroundColor(const GsColor &color)
{
    mBackgroundColor = color;
}
