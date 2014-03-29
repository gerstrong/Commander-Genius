#ifndef GSSCROLLBAR_H
#define GSSCROLLBAR_H

#include "GsControl.h"

class GsScrollbar : public CGUIControl
{
public:

    void processLogic();

    void drawScrollBar(const SDL_Rect &lRect);

    void processRender(const GsRect<float> &RectDispCoordFloat);

    bool sendEvent(const InputCommands command);

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

    GsScrollbar(CGUIControl *parent) :
        mScrollPos(0),
        mMaxScrollAmt(0),
        mLastToShow(0),
        mpParent(parent) {}


    int mScrollPos;
    int mMaxScrollAmt;
    int mLastToShow;

private:
    CGUIControl *mpParent;

};

#endif // GSSCROLLBAR_H
