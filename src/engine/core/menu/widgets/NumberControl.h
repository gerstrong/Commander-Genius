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

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processLogic() override;

    const Style mStyle;

private:

    GsColor mColorNormal;
    GsColor mColorHovered;
    GsColor mColorPressed;
    GsColor mColorReleased;
    GsColor mColorSelected;
};

#endif // NUMBERCONTROL_H
