#ifndef GsWidget_H_
#define GsWidget_H_

#include <base/utils/Geometry.h>

class GsWidget
{

public:

    GsWidget(const GsRect<float> &rect);

    virtual ~GsWidget();

    unsigned int getId() const
    {
        return mId;
    }

    void setRect( const GsRect<float>& rect )
    {
        mRect = rect;
    }

    GsRect<float> getRect() const
    {
        return mRect;
    }

    void setPosition(const float x, const float y)
    {
        mRect.pos.x = x;
        mRect.pos.y = y;
    }


    virtual void processLogic() = 0;

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) = 0;

    virtual void processRender(const GsRect<float> &,
                               const GsRect<float> &) {}

    virtual void updateGraphics() {}



private:

    // The relative rect describes the rect which is normally tied to its parent.
    GsRect<float> mRect;

    static unsigned int mNumWidgets;
    unsigned int mId;
};

#endif // GsWidget_H_
