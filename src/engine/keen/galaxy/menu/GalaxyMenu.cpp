#include "GalaxyMenu.h"
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

void GalaxyMenu::createGalaxyBackground()
{
    GsBitmap backgroundBmp( *gGraphics.getBitmapFromStr("KEENSWATCH") );

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    backgroundBmp.scaleTo(gameRes);

    GsRect<Uint16> bmpRect(backgroundBmp.getWidth(), backgroundBmp.getHeight());

    GsWeakSurface swatchSfc(backgroundBmp.getSDLSurface());

    mBackground.create( 0, bmpRect.w, bmpRect.h, RES_BPP, 0, 0, 0, 0);
    swatchSfc.blitTo(mBackground);

    // Besides the Background Bitmap we need to draw two lines
    SDL_Surface *backSfc = mBackground.getSDLSurface();

    Uint32 color = SDL_MapRGB( backSfc->format, 84, 234, 84 );
    SDL_Rect scoreRect;
    scoreRect.w = 15*gameRes.w/32;
    scoreRect.h = gameRes.h/200;
    scoreRect.x = gameRes.w/4;

    if(gameRes.h > 200) // This will display the line score correctly.
        scoreRect.y = 55*gameRes.h/202;
    else
        scoreRect.y = 55;

    SDL_FillRect(backSfc, &scoreRect, color);
}

void GalaxyMenu::render()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    mBackground.blitTo(blit);

    CBaseMenu::render();
}
