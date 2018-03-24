#ifndef GSMINITEXTREADER_H
#define GSMINITEXTREADER_H

#include "GsControl.h"

class GsMiniTextViewer : public GsControl
{
public:
    GsMiniTextViewer();

    void processLogic();

    void processRender(const GsRect<float> &RectDispCoordFloat);
};

#endif // GSMINITEXTREADER_H
