#ifndef __GALAXY_BORDEREDBUTTON_H__
#define __GALAXY_BORDEREDBUTTON_H__

#include "Button.h"

namespace galaxy
{

class BorderedButton : public GalaxyButton
{
public:
    BorderedButton(const std::string& text, CEvent *ev);

    void processRender(const GsRect<float> &RectDispCoordFloat);
};

}

#endif // __GALAXY_BORDEREDBUTTON_H__
