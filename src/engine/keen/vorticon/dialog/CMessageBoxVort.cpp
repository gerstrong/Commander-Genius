/*
 * CMessageBoxVort.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#include <memory>
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <widgets/GsBitmapBox.h>


#include "CMessageBoxVort.h"
#include "graphics/GsGraphics.h"



CMessageBoxVort::CMessageBoxVort(const std::string& Text,
                                 const bool lower,
                                 const bool keymsg,
                                 const bool vorticonBorders) :
CMessageBox(Text, lower, keymsg, vorticonBorders, CGUIDialog::FXKind::EXPAND)
{
   initVorticonBackground();   
   mpTextCtrl->setFontId(0);
}


void CMessageBoxVort::initVorticonBackground()
{
    GsRect<float> rect = getRect();

    const SDL_Rect sdlRect = gVideoDriver.toBlitRect(rect);
    mBackgroundSfc.create(0, sdlRect.w, sdlRect.h, RES_BPP, 0, 0, 0, 0);

    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFontLegacy(1);

    // Now draw the borders
    SDL_Surface *backSfc = mBackgroundSfc.getSDLSurface();
    drawBorderRect(backSfc, sdlRect);

    // Start with the blank spaces for background (normally it's white. Might be different in some mods)
    for( int x=8 ; x<sdlRect.w-8 ; x+=8 )
    {
        for( int y=8 ; y<sdlRect.h-8 ; y+=8 )
        {
            Font.drawCharacter( backSfc, 32,
                                x, y );
        }
    }

    processRendering(mBackgroundSfc.getSDLSurface());
}


// This function is used in "Your ship need those parts".
// This special Messagebox can hold up to 4 images
void CMessageBoxVort::addTileAt(const Uint16 tile, const Uint16 x, const Uint16 y)
{
	GsTilemap &tilemap = gGraphics.getTileMap(1);

	const int tileDim = tilemap.getDimension();

    auto &vidDrv = gVideoDriver;

	// now we'll create a new surface on which the Tile will be printed.
    SDL_Rect tileRect;
    tileRect.x = tileRect.y = 0;
    tileRect.w = tileRect.h = tileDim;


    GsSurface bmpSfcTile;
    bmpSfcTile.createRGBSurface(tileRect);
    bmpSfcTile.makeBlitCompatible();

    bmpSfcTile.fillRGBA(0xFF, 0x00, 0xFF, 0xFF);

    tilemap.drawTile(bmpSfcTile.getSDLSurface(), 0, 0, tile);

    tileRect.x = x+40;	tileRect.y = y+24;

    GsRect<float> fRect(tileRect);

    GsRect<float> scaleRect = getRect();

    GsRect<float> gameRect = vidDrv.getGameResolution();

    fRect.transformInverse(gameRect);

    fRect.dim.x /= scaleRect.dim.x;
    fRect.dim.y /= scaleRect.dim.y;
    fRect.pos.x += (scaleRect.pos.x*scaleRect.dim.x);
    fRect.pos.y += (scaleRect.pos.y*scaleRect.dim.y);


    std::shared_ptr<GsBitmap> bmpShared(new GsBitmap(bmpSfcTile));
    add( new GsBitmapBox(bmpShared,fRect) );
}
