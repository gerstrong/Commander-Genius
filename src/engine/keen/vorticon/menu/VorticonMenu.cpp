#include <graphics/GsGraphics.h>

#include "VorticonMenu.h"

void VorticonMenu::initVorticonBackground()
{
    GsRect<float> rect = mpMenuDialog->getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mBackground.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(1);

    SDL_Surface *backSfc = mBackground.getSDLSurface();


    // Draw the character so the classical vorticon menu is drawn

    // Start with the blank space (normally it's white. Might be different in some mods)
    for( int x=8 ; x<sdlRect.w-8 ; x+=8 )
    {
        for( int y=8 ; y<sdlRect.h-8 ; y+=8 )
        {
            Font.drawCharacter( backSfc, 32, x, y );
        }
    }

    // Now draw the borders
    mpMenuDialog->drawBorderRect(backSfc, sdlRect);
}

void VorticonMenu::render()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    GsRect<float> rect = mpMenuDialog->getRect();
    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);

    mBackground.blitTo(blit, sdlRect);

    CBaseMenu::render();
}
