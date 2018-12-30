#ifndef VORTICONBUTTON_H
#define VORTICONBUTTON_H

#include <widgets/GsButton.h>

namespace vorticon
{

class Button : public GsButton
{
public:
    Button(const std::string& text,
                 CEvent *ev);

    void processRender(const GsRect<float> &RectDispCoordFloat);

private:

    void setupButtonSurface(const std::string &);

    void drawVorticonStyle(SDL_Rect& lRect);

};

}

#endif /* VORTICONBUTTON_H */
