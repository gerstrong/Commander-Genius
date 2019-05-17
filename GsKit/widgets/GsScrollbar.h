#ifndef GSSCROLLBAR_H
#define GSSCROLLBAR_H

#include "GsControl.h"

class GsScrollbar : public GsControl
{
public:

    void processLogic();

    void drawScrollBar(const SDL_Rect &lRect);

    void processRender(const GsRect<float> &RectDispCoordFloat);

    bool sendEvent(const InputCommand);

    void scrollUp()
    {
        if(mScrollPos > 0)
            mScrollPos--;
    }

    void scrollDown()
    {
        if(mScrollPos < mMaxScrollAmt)
            mScrollPos++;
    }

    int scrollPos()
    {
        return mScrollPos;
    }

    int lastToShow()
    {
        return mLastToShow;
    }

    GsScrollbar(GsControl *parent) :
        mpParent(parent),
        mSliderHeight(0.0f),
        mArrowHeight(0.0f) {}


    int mScrollPos = 0;
    int mMaxScrollAmt = 0;
    int mLastToShow = 0;

private:
    GsControl *mpParent = nullptr;

    float mSliderHeight;
    float mArrowHeight;
};

#endif // GSSCROLLBAR_H
