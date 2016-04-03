#ifndef GSPROGRESSBAR_H
#define GSPROGRESSBAR_H

#include "GsControl.h"

class GsProgressBar : public CGUIControl
{
public:
    GsProgressBar();


    void processLogic();

    void processRender(const GsRect<float> &RectDispCoordFloat);

};

#endif // GSPROGRESSBAR_H
