#ifndef GSFRAME_H
#define GSFRAME_H

#include "GsControl.h"
#include "GsControlsManager.h"
#include <base/utils/Color.h>
#include <list>

#include <functional>

/**
 * @brief The GsFrame class A frame is container holding different widgets.
 *        The coordinates are all defined in relation to the Framesize
 */

class GsFrame : public GsControlsManager
{

public:
    GsFrame(const GsRect<float> &rect);

    virtual void processLogic() override;

    virtual void processRender(const GsRect<float> &rectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &backRect,
                               const GsRect<float> &frontRect) override;

    void setBackgroundColor(const GsColor &color);

    void setActivationEvent(const std::function <void ()>& f)
    {
        mFunction = f;
    }


protected:

    GsColor mBackgroundColor;

private:

    std::function <void ()> mFunction;
};

#endif // GSFRAME_H
