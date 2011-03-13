/*
 * CStatusScreenGalaxyEp4.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CStatusScreenGalaxyEp4.h"
#include "graphics/CGfxEngine.h"
#include "StringUtils.h"

CStatusScreenGalaxyEp4::CStatusScreenGalaxyEp4(const stItemGalaxy& Item) :
CStatusScreenGalaxy(Item)
{}


void CStatusScreenGalaxyEp4::GenerateStatus()
{
	SDL_Rect EditRect;
	drawBase(EditRect);

	SDL_PixelFormat *pixelformat = mp_StatusSurface->format;

	CFont &Font = g_pGfxEngine->getFont(0);
	Font.setBGColour(pixelformat, 0xAAAAAA);
	Font.setFGColour(pixelformat, 0x555555);

	Font.drawFontCentered(mp_StatusSurface, "LOCATION", EditRect.x, EditRect.w, EditRect.y, false);

	// Temporary Rect for drawing some stuff like background for scores and so...
	SDL_Rect TempRect;

	// Location Rect
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+10;
	TempRect.w = EditRect.w;
	TempRect.h = 20;

	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFFFFFFFF);

	/// SCORE and EXTRA Rect
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+32;
	TempRect.w = EditRect.w/2; TempRect.h = 10;
	Font.drawFont(mp_StatusSurface, "SCORE            EXTRA", TempRect.x+16, TempRect.y);
	TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
	TempRect.y = EditRect.y+42;

	// Score Box
	TempRect.w = 8*8;
	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFF000000);
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(m_Item.m_points), 8), TempRect.x, TempRect.y+2, mp_StatusSurface);

	// Extra Box
	TempRect.x = EditRect.x+96;
	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFF000000);
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(m_Item.m_lifeAt), 8), TempRect.x, TempRect.y+2, mp_StatusSurface);

	/// RESCUED and LEVEL Rects
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+56;
	TempRect.w = EditRect.w/2; TempRect.h = 10;
	Font.drawFont(mp_StatusSurface, "RESCUED           LEVEL", TempRect.x+8, TempRect.y);
	TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
	TempRect.y = EditRect.y+66;

	// Rescued Box
	TempRect.w = 8*8;
	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFF000000);

	// Level Box
	TempRect.x = EditRect.x+96;
	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFFFFFFFF);

	// Keys Box

	// Ammo Box

	// Keens Box

	// Drops Box

	// Swim Suit Box

	// Press a Key Sign
}
