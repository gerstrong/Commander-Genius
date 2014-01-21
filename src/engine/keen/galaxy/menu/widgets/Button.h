#ifndef __GALAXYBUTTON_H__
#define __GALAXYBUTTON_H__

#include <widgets/GsButton.h>

class GalaxyButton : public GsButton
{
public:
    GalaxyButton(const std::string& text,
                 CEvent *ev,
                 const bool bordered = false);

    void processLogic();

    void drawEnabledButton(GsWeakSurface &blitsfc, const SDL_Rect &lRect);

    void processRender(const GsRect<float> &RectDispCoordFloat);

private:

    void setupButtonSurface();

    void drawGalaxyStyle(SDL_Rect& lRect);
    void drawGalaxyBorderedStyle(SDL_Rect& lRect);

    const bool mBordered;
    int mLightRatio; // This will blend between selected and unselected item.
};

#endif /* __GALAXYBUTTON_H__ */
