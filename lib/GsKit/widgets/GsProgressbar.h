#ifndef GSPROGRESSBAR_H
#define GSPROGRESSBAR_H

#include "GsControl.h"

class GsProgressBar : public CGUIControl
{
public:

    GsProgressBar(const int &progress) :
        mProgress(progress) {}



    void processLogic();

    void processRender(const GsRect<float> &RectDispCoordFloat);

private:

    const int &mProgress;

};

#endif // GSPROGRESSBAR_H
