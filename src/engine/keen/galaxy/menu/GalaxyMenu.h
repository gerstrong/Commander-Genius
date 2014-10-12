#ifndef __GALAXYMENU_H__
#define __GALAXYMENU_H__

#include <widgets/GsBaseMenu.h>

#include "widgets/Button.h"

#include <widgets/GsMenuController.h>

class GalaxyMenu : public CBaseMenu
{
public:
    GalaxyMenu( const GsRect<float>& rect ):
        CBaseMenu( rect, new GalaxyButton( "x", new CloseMenuEvent()), CGUIDialog::FXState(0) )
    {
        GsRect<float> localRect(0.30f, 0.282f, 0.5f, 0.5f);
        mpMenuDialog->setRect(localRect);
        mpMenuDialog->addControl( mpReturnButton,
                                  GsRect<float>(
                                      -0.1f, 0.1f, 0.03f/localRect.w, 0.03f/localRect.h) );
        mpReturnButton->setDown(true);

        createGalaxyBackground();
    }

    void createGalaxyBackground();

    void render();

private:
    GsSurface mBackground;

};

#endif /* __GALAXYMENU_H__ */
