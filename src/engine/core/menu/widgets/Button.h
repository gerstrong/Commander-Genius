#ifndef GAMEBUTTON_H_INCLUDED
#define GAMEBUTTON_H_INCLUDED

#include <widgets/GsButton.h>
#include "KeenStyles.h"

class GameButton : public GsButton
{
public:
    GameButton(const std::string& text,
               CEvent *ev,
               const Style style);

    GameButton(const std::string& text,
               const GsRect<float> &rect,
               CEvent *ev,
               const Style style);

    virtual void processLogic() override;

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) override;

    const Style mStyle;

private:

    GsColor mColorNormal;
    GsColor mColorHovered;
    GsColor mColorPressed;
    GsColor mColorReleased;
    GsColor mColorSelected;

};

#endif /* GAMEBUTTON_H_INCLUDED */
