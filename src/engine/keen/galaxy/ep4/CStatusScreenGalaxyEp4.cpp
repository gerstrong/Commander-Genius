/*
 * CStatusScreenGalaxyEp4.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CStatusScreenGalaxyEp4.h"
#include "graphics/GsGraphics.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/utils/StringUtils.h>
#include "fileio/KeenFiles.h"

CStatusScreenGalaxyEp4::CStatusScreenGalaxyEp4(const stItemGalaxy& Item) :
CStatusScreenGalaxy(Item)
{}


void CStatusScreenGalaxyEp4::GenerateStatus()
{
	SDL_Rect EditRect;
	drawBase(EditRect);

	GsFont &Font = gGraphics.getFont(0);
	Font.setupColor(0x555555);

	Font.drawFontCentered(mpStatusSurface.get(), "LOCATION", EditRect.x, EditRect.w, EditRect.y, false);

	// Temporary Rect for drawing some stuff like background for scores and so...
	SDL_Rect TempRect;

	// Location Rect
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+10;
	TempRect.w = EditRect.w;
	TempRect.h = 20;

	Font.setupColor(0x0);
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFFFFFFFF);
    Font.drawFontCentered(mpStatusSurface.get(), m_Item.mLevelName, TempRect.x, TempRect.w, TempRect.y+6, false);
	Font.setupColor(0x444444);


	/// SCORE and EXTRA Rect
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+32;
	TempRect.w = EditRect.w/2; TempRect.h = 10;
	Font.drawFont(mpStatusSurface.get(), "SCORE            EXTRA", TempRect.x+16, TempRect.y);
	TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
	TempRect.y = EditRect.y+42;

	// Score Box
	TempRect.w = 8*8;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_points), 8), TempRect.x, TempRect.y+2, mpStatusSurface.get());

	// Extra Box
	TempRect.x = EditRect.x+96;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifeAt), 8), TempRect.x, TempRect.y+2, mpStatusSurface.get());

    byte *ptr = gKeenFiles.exeFile.getRawData();
			
	std::string rescLine = "RESCUED           LEVEL";
	std::string dropsLine;
	std::string swLine;
	
	char rescBuf[9];
	char dropsBuf[9];
	char swBuf[9];
	
	memcpy(rescBuf, ptr+0x2F49C, 9 );
	memcpy(dropsBuf, ptr+0x2F4CB, 9 );
	memcpy(swBuf, ptr+0x2F4D1, 9 );
	
	rescLine.replace ( 0, 7, (const char*)rescBuf );
	dropsLine = dropsBuf;
	swLine = swBuf;
	
	/// RESCUED and LEVEL Rects
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+56;
	TempRect.w = EditRect.w/2; TempRect.h = 10;
	Font.drawFont(mpStatusSurface.get(), rescLine, TempRect.x+8, TempRect.y);
	TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
	TempRect.y = EditRect.y+66;

	// Rescued Box
	TempRect.w = 8*8;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	for( int count=0 ; count<m_Item.m_special.ep4.elders ; count++ )
		gGraphics.drawDigit(40, TempRect.x+8*count, TempRect.y+1, mpStatusSurface.get());

	// Level Box
	TempRect.x = EditRect.x+96;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFFFFFFFF);
	Font.setupColor(0x0);

    const std::string difftext = getDifficultyText();

	Font.drawFontCentered(mpStatusSurface.get(), difftext, TempRect.x, TempRect.w, TempRect.y+1, false);
	Font.setupColor(0x333333);

	// Keys Box
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+80;
	Font.drawFont(mpStatusSurface.get(), "KEYS", TempRect.x, TempRect.y);
	TempRect.w = 8*4; TempRect.h = 10;
	TempRect.x = TempRect.x+8*5;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	if(m_Item.m_gem.red)
		gGraphics.drawDigit(36, TempRect.x, TempRect.y+1, mpStatusSurface.get());
	if(m_Item.m_gem.yellow)
		gGraphics.drawDigit(37, TempRect.x+8, TempRect.y+1, mpStatusSurface.get());
	if(m_Item.m_gem.blue)
		gGraphics.drawDigit(38, TempRect.x+16, TempRect.y+1, mpStatusSurface.get());
	if(m_Item.m_gem.green)
		gGraphics.drawDigit(39, TempRect.x+24, TempRect.y+1, mpStatusSurface.get());

	// Ammo Box
	TempRect.x = EditRect.x+96;
	TempRect.y = EditRect.y+80;
	Font.drawFont(mpStatusSurface.get(), "AMMO", TempRect.x, TempRect.y);
	TempRect.w = 8*3; TempRect.h = 10;
	TempRect.x = TempRect.x+8*5;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_bullets), 3), TempRect.x, TempRect.y+1, mpStatusSurface.get());

	// Keens Box
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+96;
	Font.drawFont(mpStatusSurface.get(), "KEENS", TempRect.x, TempRect.y);
	TempRect.w = 8*2; TempRect.h = 10;
	TempRect.x = TempRect.x+8*5+8;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifes), 2), TempRect.x, TempRect.y+1, mpStatusSurface.get());

	// Drops Box
	TempRect.x = EditRect.x+96;
	TempRect.y = EditRect.y+96;
	Font.drawFont(mpStatusSurface.get(), dropsLine, TempRect.x, TempRect.y);
	TempRect.w = 8*2; TempRect.h = 10;
	TempRect.x = TempRect.x+8*5+8;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFF000000);
	gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_drops), 2), TempRect.x, TempRect.y+1, mpStatusSurface.get());

	// Swim Suit Box
	TempRect.x = EditRect.x;
	TempRect.y = EditRect.y+114;
	TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
	SDL_FillRect(mpStatusSurface.get(), &TempRect, 0xFFFFFFFF);
	Font.setupColor(0x0);
	Font.drawFontCentered(mpStatusSurface.get(), m_Item.m_special.ep4.swimsuit ? swLine : "???", TempRect.x, TempRect.w, TempRect.y+1, false);

	// Press a Key Sign
	GsTilemap &Tilemap = gGraphics.getTileMap(2);
	TempRect.x = EditRect.x+(EditRect.w/2);
	TempRect.y = EditRect.y+110;
	for( int c=0 ; c<10 ; c++ )
		Tilemap.drawTile(mpStatusSurface.get(), TempRect.x+c*8, TempRect.y, 72+c);
	TempRect.y += 8;
	for( int c=0 ; c<10 ; c++ )
		Tilemap.drawTile(mpStatusSurface.get(), TempRect.x+c*8, TempRect.y, 82+c);

}
