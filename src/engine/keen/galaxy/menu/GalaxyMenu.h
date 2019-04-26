#ifndef GALAXYMENU_H
#define GALAXYMENU_H

#include "engine/core/menu/GameMenu.h"

//#include "widgets/Button.h"
#include "engine/core/menu/widgets/Button.h"

#include <widgets/GsMenuController.h>

class GalaxyMenu : public GameMenu
{
public:

    GalaxyMenu( const GsRect<float>& rect );

    void initGalaxyBackground();

    void render();

private:
    GsSurface mBackground;

};

#endif /* GALAXYMENU_H */
