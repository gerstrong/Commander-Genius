#include "GalaxyMenu.h"
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


GalaxyMenu::GalaxyMenu( const GsRect<float>& rect ):
    GameMenu( rect,GsControl::Style::GALAXY)
{
    GsRect<float> localRect(0.30f, 0.282f, 0.5f, 0.5f);
    mpMenuDialog->setRect(localRect);
/*        mpMenuDialog->addControl( mpReturnButton,
                              GsRect<float>(
                                  -0.1f, 0.1f,
                                  0.03f/localRect.dim.x,
                                  0.03f/localRect.dim.y) );
                                  */
    mpMenuDialog->addControl( mpReturnButton );

    mpReturnButton->setHovered(true);

    initGalaxyBackground();
}

void GalaxyMenu::initGalaxyBackground()
{
    GsBitmap backgroundBmp( *gGraphics.getBitmapFromStr(0, "KEENSWATCH") );

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    backgroundBmp.scaleTo(gameRes);

    GsRect<Uint16> bmpRect(backgroundBmp.width(), backgroundBmp.height());

    GsWeakSurface swatchSfc(backgroundBmp.getSDLSurface());

    mBackground.create( 0, bmpRect.dim.x, bmpRect.dim.y, RES_BPP, 0, 0, 0, 0);
    swatchSfc.blitTo(mBackground);

    // Besides the Background Bitmap we need to draw two lines
    SDL_Surface *backSfc = mBackground.getSDLSurface();

    Uint32 color = SDL_MapRGB( backSfc->format, 84, 234, 84 );
    SDL_Rect scoreRect;
    scoreRect.w = 15*gameRes.dim.x/32;
    scoreRect.h = gameRes.dim.y/200;
    scoreRect.x = gameRes.dim.x/4;

    if(gameRes.dim.y > 200) // This will display the line score correctly.
        scoreRect.y = 55*gameRes.dim.y/202;
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
