/*
 * CMessageBoxVort.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#include "CMessageBoxVort.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include "graphics/GsGraphics.h"
#include "sdl/extensions.h"
#include "gui/CGUIBitmap.h"
#include <memory>

CMessageBoxVort::CMessageBoxVort(const std::string& Text, bool lower, bool keymsg, bool leftbound) :
CMessageBox(Text, lower, keymsg, leftbound, CGUIDialog::EXPAND)
{}

// This function is used in your Ship need those parts.
// This special Messagebox can hold up to 4 images
void CMessageBoxVort::addTileAt(Uint16 tile, Uint16 x, Uint16 y)
{
	CTilemap &tilemap = gGraphics.getTileMap(1);

	const int tileDim = tilemap.getDimension();

	// now we'll create a new surface on which the Tile will be printed.
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = rect.h = tileDim;
	std::shared_ptr<SDL_Surface> bmpSfc( CG_CreateRGBSurface(rect), &SDL_FreeSurface );

	SDL_FillRect(bmpSfc.get(), NULL, 0xFFFFFFFF);
	tilemap.drawTile(bmpSfc.get(), 0, 0, tile);

	rect.x = x;	rect.y = y;

	GsRect<float> fRect( x, y, 16.0f, 16.0f);

	GsRect<float> gameRect = gVideoDriver.getGameResolution();
	fRect.transformInverse(gameRect);
	fRect.transform(mRect);

	std::unique_ptr<CBitmap> pBitmap(new CBitmap(bmpSfc));

	addControl( new CGUIBitmap(move(pBitmap)), fRect );
}
