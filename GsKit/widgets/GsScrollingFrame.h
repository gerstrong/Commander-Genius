#ifndef GSSCROLLINGFRAME_H
#define GSSCROLLINGFRAME_H

#include "GsFrame.h"


class GsScrollingFrame : public GsFrame
{

public:

    GsScrollingFrame(const GsRect<float> &rect,
                     const GsRect<float> &virtualRect);


    virtual void processLogic() override;

    virtual void processPointingStateRel(const GsRect<float> &rect) override;

    virtual void processRender(const GsRect<float> &rectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &srcRectFloat,
                               const GsRect<float> &dstRectFloat) override;


    void setPosX(const float x);
    void setPosY(const float y);

    void moveX(const float dx);
    void moveY(const float dy);

    float getScrollX() {return mScrollX;}
    float getScrollY() {return mScrollY;}

    void setBackgroundColor(const GsColor &color) override;

private:

    GsRect<float> mVirtualRect;
    float mScrollX = 0.0f;
    float mScrollY = 0.0f;

};

#endif // GSSCROLLINGFRAME_H
