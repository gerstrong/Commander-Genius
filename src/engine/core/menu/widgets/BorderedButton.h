#ifndef GALAXY_BORDEREDBUTTON_H
#define GALAXY_BORDEREDBUTTON_H

#include "Button.h"

class BorderedButton : public GameButton
{
public:
    BorderedButton(const std::string& text,
                   CEvent *ev,
                   const GsControl::Style style);

    BorderedButton(const std::string& text,
                   const GsRect<float> &rect,
                   CEvent *ev,
                   const GsControl::Style style);

    void processRender(const GsRect<float> &RectDispCoordFloat) override;
};


#endif // __GALAXY_BORDEREDBUTTON_H
