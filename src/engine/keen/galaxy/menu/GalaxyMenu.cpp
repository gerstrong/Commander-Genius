#include "GalaxyMenu.h"
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


GalaxyMenu::GalaxyMenu( const GsRect<float>& rect ):
    GameMenu( rect,Style::GALAXY)
{
    GsRect<float> localRect(0.30f, 0.282f, 0.5f, 0.5f);
    mpMenuDialog->setRect(localRect);

    mpMenuDialog->add( mpReturnButton );

    mpReturnButton->setHovered(true);

    initGalaxyBackground();
}

void GalaxyMenu::render()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    mBackground.blitTo(blit);

    CBaseMenu::render();
}
