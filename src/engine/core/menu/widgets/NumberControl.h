#ifndef __NUMBERCONTROL_H__
#define __NUMBERCONTROL_H__

#include <widgets/GsNumberControl.h>


class NumberControl : public CGUINumberControl
{
public:
    NumberControl(const std::string& text,
                  const int startValue,
                  const int endValue,
                  const int deltaValue,
                  const int value,
                  const bool slider,
                  const GsControl::Style style);

    void setupButtonSurface();

    void processLogic();

    void drawNoStyle(SDL_Rect& lRect);

    void processRender(const GsRect<float> &RectDispCoordFloat);
};

#endif // __NUMBERCONTROL_H__
