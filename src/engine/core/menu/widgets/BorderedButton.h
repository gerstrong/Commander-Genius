#ifndef __GALAXY_BORDEREDBUTTON_H__
#define __GALAXY_BORDEREDBUTTON_H__

#include "Button.h"

class BorderedButton : public GameButton
{
public:
    BorderedButton(const std::string& text, CEvent *ev,
                   const GsControl::Style style);

    void processRender(const GsRect<float> &RectDispCoordFloat);
};


#endif // __GALAXY_BORDEREDBUTTON_H__
