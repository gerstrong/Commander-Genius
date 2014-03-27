#ifndef GSSCROLLBAR_H
#define GSSCROLLBAR_H

#include "GsControl.h"

class GsScrollbar : public CGUIControl
{
public:

    void processLogic();

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

    GsScrollbar() :
        mScrollPos(0),
        mMaxScrollAmt(0),
        mLastToShow(0) {}


    int mScrollPos;
    int mMaxScrollAmt;
    int mLastToShow;

};

#endif // GSSCROLLBAR_H
