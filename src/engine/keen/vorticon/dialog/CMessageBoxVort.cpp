/*
 * CMessageBoxVort.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#include <memory>
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <widgets/GsBitmap.h>


#include "CMessageBoxVort.h"
#include "graphics/GsGraphics.h"
#include "sdl/extensions.h"


CMessageBoxVort::CMessageBoxVort(const std::string& Text, bool lower, bool keymsg, bool leftbound) :
CMessageBox(Text, lower, keymsg, leftbound, CGUIDialog::EXPAND)
{
   initVorticonBackground();   
}


void CMessageBoxVort::initVorticonBackground()
{
//    return;
    const SDL_Rect lRect = gVideoDriver.toBlitRect(mRect);

    mBackgroundSfc.create(0, lRect.w, lRect.h, RES_BPP, 0, 0, 0, 0);
    mBackgroundSfc.fillRGB(230, 230, 230);


    GsRect<float> rect = getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mBackgroundSfc.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(1);

    // Now draw the borders
    SDL_Surface *backSfc = mBackgroundSfc.getSDLSurface();
    drawBorderRect(backSfc, sdlRect);

    // Draw the character so the classical vorticon menu is drawn

    // Start with the blank space (normally it's white. Might be different in some mods)
    for( int x=8 ; x<sdlRect.w-8 ; x+=8 )
    {
        for( int y=8 ; y<sdlRect.h-8 ; y+=8 )
        {
            Font.drawCharacter( backSfc, 32, x, y );
        }
    }

    processRendering(mBackgroundSfc.getSDLSurface());
}


void CMessageBoxVort::render()
{
    CMessageBox::render();
    //GsRect<float> rect = getRect();
    //const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);

    //GsWeakSurface blit(gVideoDriver.getBlitSurface());

    //mBackgroundSfc.blitTo(blit, sdlRect);
    //CGUIDialog::processRendering();
}



// This function is used in your Ship need those parts.
// This special Messagebox can hold up to 4 images
void CMessageBoxVort::addTileAt(Uint16 tile, Uint16 x, Uint16 y)
{
	GsTilemap &tilemap = gGraphics.getTileMap(1);

	const int tileDim = tilemap.getDimension();

	// now we'll create a new surface on which the Tile will be printed.
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = rect.h = tileDim;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    std::shared_ptr<SDL_Surface> bmpSfc( CG_CreateRGBSurface( rect ), &SDL_FreeSurface );
    bmpSfc.reset(gVideoDriver.convertThroughBlitSfc(bmpSfc.get()), &SDL_FreeSurface);

#else
    std::shared_ptr<SDL_Surface> bmpSfc( CG_CreateRGBSurface(rect), &SDL_FreeSurface );
#endif

    SDL_FillRect(bmpSfc.get(), NULL, 0xFF00FFFF);
	tilemap.drawTile(bmpSfc.get(), 0, 0, tile);

	rect.x = x;	rect.y = y;

	GsRect<float> fRect( x, y, 16.0f, 16.0f);

	GsRect<float> gameRect = gVideoDriver.getGameResolution();
    fRect.transformInverse(gameRect);

    GsRect<float> scaleRect = mRect;

    scaleRect.x = 0;
    scaleRect.y = 0;

    fRect.transformInverse(scaleRect);

    std::shared_ptr<GsBitmap> pBitmap(new GsBitmap(bmpSfc));
    addControl( new CGUIBitmap(pBitmap), fRect );
}
