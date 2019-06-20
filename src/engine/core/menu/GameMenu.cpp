#include "GameMenu.h"
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

void GameMenu::initGalaxyBackground()
{
    GsBitmap backgroundBmp( *gGraphics.getBitmapFromStr(0, "KEENSWATCH") );

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    backgroundBmp.scaleTo(gameRes);

    GsRect<Uint16> bmpRect(backgroundBmp.width(), backgroundBmp.height());

    GsWeakSurface swatchSfc(backgroundBmp.getSDLSurface());

    mBackground.create( 0, bmpRect.dim.x, bmpRect.dim.y, RES_BPP, 0, 0, 0, 0);
    swatchSfc.blitTo(mBackground);
   

    // Besides the Background Bitmap we need to draw two lines
    SDL_Rect scoreRect;
    scoreRect.w = (15*gameRes.dim.x)/32;
    scoreRect.h = gameRes.dim.y/200;
    scoreRect.x = gameRes.dim.x/4;
    scoreRect.y = 55;

    if(gameRes.dim.y > 200) // This will display the line score correctly.
    {
        scoreRect.y = (scoreRect.y*gameRes.dim.y)/202;
    }

    mBackground.fillRGB(scoreRect, 84, 234, 84);

    GsRect<float> rect = mpMenuDialog->getRect();
    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);

    // check if resolution still match to background. If not update it.
    const auto bgRectW = sdlRect.w;
    const auto bgRectH = sdlRect.h;

    mCachedBgRect.dim.x = bgRectW;
    mCachedBgRect.dim.y = bgRectH;
}

void GameMenu::initVorticonBackground()
{
    GsRect<float> rect = mpMenuDialog->getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mCachedBgRect.dim.x = sdlRect.w;
    mCachedBgRect.dim.y = sdlRect.h;
    mBackground.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFontLegacy(1);

    SDL_Surface *backSfc = mBackground.getSDLSurface();


    // Draw the characters for the classical vorticon menu

    // Start with the blank space (normally it's white. Might be different in some mods)
    for( int x=8 ; x<sdlRect.w-8 ; x+=8 )
    {
        for( int y=8 ; y<sdlRect.h-8 ; y+=8 )
        {
            Font.drawCharacter( backSfc, 32,
                                Uint16(x), Uint16(y) );
        }
    }

    // Now draw the borders
    mpMenuDialog->drawBorderRect(backSfc, sdlRect);
}

void GameMenu::initBackgroundNoStyle()
{
    GsRect<float> rect = mpMenuDialog->getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mCachedBgRect.dim.x = sdlRect.w;
    mCachedBgRect.dim.y = sdlRect.h;
    mBackground.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

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
    GsRect<float> rect = mpMenuDialog->getRect();
    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);

    // check if resolution still match to background. If not update it.
    const auto bgRectW = sdlRect.w;
    const auto bgRectH = sdlRect.h;

    if(bgRectW != mCachedBgRect.dim.x ||
       bgRectH != mCachedBgRect.dim.y)
    {
        (this->*initBackground)();
        return;
    }

    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    if(!mBackground.empty())
    {        
        if(mStyle == GsControl::Style::GALAXY)
        {
            mBackground.blitTo(blit);
        }
        else if(mStyle == GsControl::Style::VORTICON)
        {
            mBackground.blitTo(blit, sdlRect);
        }
        else
        {
            mBackground.blitTo(blit, sdlRect);
        }

    }

    CBaseMenu::render();
}
