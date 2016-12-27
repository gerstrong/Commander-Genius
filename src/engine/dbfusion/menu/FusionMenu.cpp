#include "FusionMenu.h"


void FusionMenu::initFusionBackground()
{    
    GsWeakSurface blit( gVideoDriver.getBlitSurface() );
    mBackground.createCopy(blit);

    GsSurface layer;

    /*SDL_PixelFormat blitFormat = blit.getSDLSurface()->format;
    mBackground.create(SDL_SWSURFACE,
                        blit.width(),
                        blit.height(),
                        RES_BPP,
                        blitFormat->Rmask,
                        blitFormat->Gmask,
                        blitFormat->Bmask,
                        0);*/

    layer.create( 0, blit.width(), blit.height(), RES_BPP, 0, 0, 0, 0);
    layer.fillRGB(50, 70, 50);
    layer.setAlpha(220);

    layer.blitTo(mBackground);
}

void FusionMenu::render()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    mBackground.blitTo(blit);

    CBaseMenu::render();
}
