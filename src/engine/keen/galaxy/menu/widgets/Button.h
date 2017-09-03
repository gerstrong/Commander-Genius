#ifndef __GALAXYBUTTON_H__
#define __GALAXYBUTTON_H__

#include <widgets/GsButton.h>

class GalaxyButton : public GsButton
{
public:
    GalaxyButton(const std::string& text,
                 CEvent *ev);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat);   
};

#endif /* __GALAXYBUTTON_H__ */
