#ifndef NUMBERCONTROL_H
#define NUMBERCONTROL_H

#include <widgets/GsNumberControl.h>
#include "KeenStyles.h"

class NumberControl : public CGUINumberControl
{
public:
    NumberControl(const std::string& text,
                  const int startValue,
                  const int endValue,
                  const int deltaValue,
                  const int value,
                  const bool slider,
                  const Style style);

    //void setupButtonSurface(const std::string &text = "") override;

    void processLogic() override;

    void drawNoStyle(SDL_Rect& lRect);

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    const Style mStyle;
};

#endif // NUMBERCONTROL_H
