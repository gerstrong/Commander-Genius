#ifndef GsWidget_H_
#define GsWidget_H_

#include <base/interface/Geometry.h>
#include <memory>

class GsWidget
{

public:

    GsWidget(const GsRect<float> &rect);

    GsWidget(const GsRect<float> &rect,
             GsWidget *parent);


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

    virtual void processPointingStateRel(const GsRect<float> &) {}

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) = 0;

    virtual void processRender(const GsRect<float> &,
                               const GsRect<float> &) {}

    virtual void updateGraphics() {}

    void setParent(GsWidget *parent)
    {
        mpParent = parent;
    }

    GsWidget *getParent()
    {
        return mpParent;
    }

    std::string getTag()
    {
        return mTag;
    }

    const void setTag(const std::string &tag)
    {
        mTag = tag;
    }

private:

    GsWidget *mpParent = nullptr;

    // This relative rect describes the rect which is normally tied to its parent.
    GsRect<float> mRect;

    unsigned int mId;
    std::string mTag;
    static unsigned int mNumWidgets;
};

#endif // GsWidget_H_
