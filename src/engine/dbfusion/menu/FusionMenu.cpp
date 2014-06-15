#include "FusionMenu.h"


void FusionMenu::initFusionBackground()
{
    GsWeakSurface blit( gVideoDriver.getBlitSurface() );

    mBackground.create( 0, blit.width(), blit.height(), RES_BPP, 0, 0, 0, 0);

    mBackground.fillRGBA(250, 0, 0, 128);
}

void FusionMenu::render()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    mBackground.blitTo(blit);

    CBaseMenu::render();
}
