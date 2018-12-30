#include "GameMenu.h"
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

void GameMenu::createGalaxyBackground()
{
    GsBitmap backgroundBmp( *gGraphics.getBitmapFromStr(0, "KEENSWATCH") );

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    backgroundBmp.scaleTo(gameRes);

    GsRect<Uint16> bmpRect(backgroundBmp.width(), backgroundBmp.height());

    GsWeakSurface swatchSfc(backgroundBmp.getSDLSurface());

    mBackground.create( 0, bmpRect.w, bmpRect.h, RES_BPP, 0, 0, 0, 0);
    swatchSfc.blitTo(mBackground);
   

    // Besides the Background Bitmap we need to draw two lines
    SDL_Rect scoreRect;
    scoreRect.w = (15*gameRes.w)/32;
    scoreRect.h = gameRes.h/200;
    scoreRect.x = gameRes.w/4;
    scoreRect.y = 55;

    if(gameRes.h > 200) // This will display the line score correctly.
    {
        scoreRect.y = (scoreRect.y*gameRes.h)/202;
    }

    mBackground.fillRGB(scoreRect, 84, 234, 84);
}

void GameMenu::initVorticonBackground()
{
    GsRect<float> rect = mpMenuDialog->getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mBackground.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(1);

    SDL_Surface *backSfc = mBackground.getSDLSurface();


    // Draw the characters for the classical vorticon menu

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

void GameMenu::initBackgroundNoStyle()
{
    GsRect<float> rect = mpMenuDialog->getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mBackground.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

    // Now lets draw the text of the list control
    //GsFont &Font = gGraphics.getFont(0);

    GsColor backColor(0xFF, 0xFF, 0xFF);
    GsColor borderColor(0x7F, 0x7F, 0x7F);

    mBackground.fillRGBA(backColor);
    mBackground.fillRGBA(GsRect<Uint16>(0, 0, sdlRect.w, 1), borderColor);
    mBackground.fillRGBA(GsRect<Uint16>(0, 0, 1, sdlRect.h), borderColor);
    mBackground.fillRGBA(GsRect<Uint16>(0, sdlRect.h-1, sdlRect.w, 1 ), borderColor);
    mBackground.fillRGBA(GsRect<Uint16>(sdlRect.w-1, 0, 1, sdlRect.h), borderColor);
}

// Processes the stuff that the menus have in common
void
GameMenu::ponder(const float deltaT)
{
    CBaseMenu::ponder(deltaT);
}


void GameMenu::setMenuLabel(const std::string &label)
{
    if(mStyle == GsControl::Style::GALAXY)
    {
        CBaseMenu::setMenuLabel(label);
    }
}

void GameMenu::render()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    if(!mBackground.empty())
    {
        if(mStyle == GsControl::Style::GALAXY)
        {
            mBackground.blitTo(blit);
        }
        else if(mStyle == GsControl::Style::VORTICON)
        {
            GsRect<float> rect = mpMenuDialog->getRect();
            const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);

            mBackground.blitTo(blit, sdlRect);
        }
        else
        {
            GsRect<float> rect = mpMenuDialog->getRect();
            const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);

            mBackground.blitTo(blit, sdlRect);
        }

    }

    CBaseMenu::render();
}
