#ifndef GAMEBUTTON_H_INCLUDED
#define GAMEBUTTON_H_INCLUDED

#include <widgets/GsButton.h>

class GameButton : public GsButton
{
public:
    GameButton(const std::string& text,
               CEvent *ev,
               const Style style);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat);   

    void drawVorticonStyle(SDL_Rect& lRect);
};

#endif /* GAMEBUTTON_H_INCLUDED */
