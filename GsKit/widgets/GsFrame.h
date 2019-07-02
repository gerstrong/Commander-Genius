#ifndef GSFRAME_H
#define GSFRAME_H

#include "GsControl.h"
#include "GsWidgetsManager.h"
#include <base/utils/Color.h>
#include <list>

#include <functional>

/**
 * @brief The GsFrame class represents a WidgetManager
 *        with an activation function and an optional background colour
 *        The coordinates are all defined in relation to the Framesize
 */

class GsFrame : public GsWidgetsManager
{

public:
    GsFrame(const GsRect<float> &rect);

    virtual void processLogic() override;

    virtual void processRender(const GsRect<float> &rectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &backRect,
                               const GsRect<float> &frontRect) override;

    void setBackgroundColor(const GsColor &color);

    void setActivationEvent(const std::function <void ()>& f)
    {        mFunction = f;    }

    void enableBackground(const bool value)
    {        mBgEnabled = value;    }

protected:

    GsColor mBackgroundColor;
    bool mBgEnabled = true;

private:

    std::function <void ()> mFunction;
};

#endif // GSFRAME_H
