#ifndef GSFRAME_H
#define GSFRAME_H

#include "GsControl.h"
#include <base/utils/Color.h>
#include <list>

/**
 * @brief The GsFrame class A frame is container holding different widgets.
 *        The coordinates are all defined in relation to the Framesize
 */

class GsFrame : public GsControl
{

public:
    GsFrame();

    void processLogic() override;

    void processRender(const GsRect<float> &rectDispCoordFloat) override;



    void setBackgroundColor(const GsColor &color);

private:
    std::list<GsControl*> mControlList;

    GsColor mBackgroundColor;
};

#endif // GSFRAME_H
