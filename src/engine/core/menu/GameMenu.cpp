#include "GameMenu.h"
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


GameMenu::GameMenu( const GsRect<float>& rect,
                    const Style &style ) :
CBaseMenu( rect, CGUIDialog::FXKind(0) ),
mStyle(style)
{
    if(style == Style::GALAXY)
    {
        GsRect<float> dlgRect(0.25f, 0.282f, 0.45f, 0.5f);
        mpMenuDialog->setRect(dlgRect);

        mpReturnButton =
            mpMenuDialog->add( new GameButton( "close",
                                               new CloseMenuEvent(),
                                               style) );

        mpReturnButton->setRect(GsRect<float>(0.01f, 0.01f,
                                              0.15f/dlgRect.dim.x,
                                              0.05f/dlgRect.dim.y));

    }
    else if(style == Style::VORTICON)
    {
        GsRect<float> dlgRect(0.15f, 0.282f, 0.70f, 0.6f);
        mpMenuDialog->setRect(dlgRect);


        GsRect<float> buttonRect(-0.10f, -0.15f,
                                 0.15f/dlgRect.dim.x,
                                 0.08f/dlgRect.dim.y);

        mpReturnButton =
            mpMenuDialog->add( new GameButton( "close",
                                               buttonRect,
                                               new CloseMenuEvent(),
                                               Style::VORTICON) );
        mpReturnButton->enableBackground(true);
        mpReturnButton->enableBorder(true);
    }
    else
    {
        const auto localRect = mpMenuDialog->getRect();

        mpReturnButton =
            mpMenuDialog->add(
                    new GsButton( "x",
                                  GsRect<float>(-0.05f, -0.05f,
                                                0.06f/localRect.dim.x,
                                                0.06f/localRect.dim.y),
                                                new CloseMenuEvent()) );
    }


    mpReturnButton->setHovered(true);

    if(style == Style::GALAXY)
    {
        initBackground = &GameMenu::initGalaxyBackground;
    }
    else if(style == Style::VORTICON)
    {
        initBackground = &GameMenu::initVorticonBackground;
    }
    else
    {
        initBackground = &GameMenu::initBackgroundNoStyle;
    }

    (this->*initBackground)();
}

void GameMenu::setProperty( const Property newProperty )
{
    mpReturnButton->setText( newProperty == CLOSEABLE ? "close" : "back" );

    auto rect = mpReturnButton->getRect();
    rect.dim.x = 0.2f;
    mpReturnButton->setRect(rect);
}

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
    if(mStyle == Style::GALAXY)
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
        if(mStyle == Style::GALAXY)
        {
            mBackground.blitTo(blit);
        }
        else if(mStyle == Style::VORTICON)
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
