/*
 * CStatusScreen.cpp
 *
 *  Created on: 05.03.2011
 *      Author: gerstrong
 */

#include "CStatusScreenGalaxy.h"
#include "graphics/GsGraphics.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>

#include "engine/core/CBehaviorEngine.h"
#include "fileio/KeenFiles.h"


CStatusScreenGalaxy::CStatusScreenGalaxy(const stItemGalaxy& Item) :
m_showstatus(false),
m_Item(Item)
{}

std::string CStatusScreenGalaxy::getDifficultyText()
{
    auto diff = gBehaviorEngine.mDifficulty;

    std::string difftext;

    if(diff == EASY)
        difftext = "Easy";
    else if(diff == NORMAL)
        difftext = "Normal";
    else if(diff == HARD)
        difftext = "Hard";
    else if(diff == EXPERT)
        difftext = "Expert";
    else if(diff == NINJA)
        difftext = "Ninja";
    else if(diff == ELITE)
        difftext = "Elite";
    else
        difftext = "???";

    return difftext;
}

void CStatusScreenGalaxy::drawBase(SDL_Rect &EditRect)
{
    SDL_Rect DestRect = {0, 0, 320, 200};

    mStatusSurface.createRGBSurface(DestRect);
    //mStatusSurface.makeBlitCompatible();

    /// Draw the required bitmaps and backgrounds for Statusscreen
	// Draw the support Bitmap and see where the gray rectangle starts...
	// Prepare the drawrect for positions
	SDL_Rect Dest;

	// Create upper stomp support
	GsBitmap &SupportBmp = gGraphics.getMaskedBitmap(2);
	SDL_Rect SupportRect;

    SupportRect.w = SupportBmp.width();
    SupportRect.h = SupportBmp.height();
	Dest.x = (DestRect.w-SupportRect.w)/2;	Dest.y = 0; 

    SupportBmp.draw(Dest.x, Dest.y, mStatusSurface);

	// Draw the gray surface
	SDL_Rect BackRect;
	BackRect.w = 192; // Standard sizes in Keen 4
	BackRect.h = 152;
	BackRect.x = (DestRect.w-BackRect.w)/2;
	BackRect.y = SupportRect.h;

    mStatusSurface.fillRGB(BackRect, 0xAA, 0xAA, 0xAA);

	// Draw the cables Bitmap
	GsBitmap &Cables_Bitmap = gGraphics.getMaskedBitmap(1);
	SDL_Rect CableRect;
	CableRect.w = Cables_Bitmap.getSDLSurface()->w;
	CableRect.h = Cables_Bitmap.getSDLSurface()->h;
	Dest.x = BackRect.x - CableRect.w;	Dest.y = 0;

    Cables_Bitmap.draw(Dest.x, Dest.y, mStatusSurface);

	// Now draw the borders
	GsTilemap &Tilemap = gGraphics.getTileMap(2);

	// Upper Left corner
    Tilemap.drawTile(mStatusSurface, BackRect.x, BackRect.y, 54);

	// Upper Part
	for(int c=1 ; c<(BackRect.w/8)-1 ; c++)
        Tilemap.drawTile(mStatusSurface, BackRect.x+c*8, BackRect.y, 55);

	// Upper Right Part
    Tilemap.drawTile(mStatusSurface, BackRect.x+BackRect.w-8, BackRect.y, 56);

	// Left Part
	for(int c=1 ; c<(BackRect.h/8)-1 ; c++)
        Tilemap.drawTile(mStatusSurface, BackRect.x, BackRect.y+c*8, 57);

	// Right Part
	for(int c=1 ; c<(BackRect.h/8)-1 ; c++)
        Tilemap.drawTile(mStatusSurface, BackRect.x+BackRect.w-8, BackRect.y+c*8, 59);

	// Lower Left Part
    Tilemap.drawTile(mStatusSurface, BackRect.x, BackRect.y+BackRect.h-8, 60);

	// Lower Part
	for(int c=1 ; c<(BackRect.w/8)-1 ; c++)
        Tilemap.drawTile(mStatusSurface, BackRect.x+c*8, BackRect.y+BackRect.h-8, 61);

	// Lower Right Part
    Tilemap.drawTile(mStatusSurface, BackRect.x+BackRect.w-8, BackRect.y+BackRect.h-8, 62);

	EditRect.x = BackRect.x+16;
	EditRect.y = BackRect.y+12;
	EditRect.w = BackRect.w-32;
	EditRect.h = BackRect.h-32;
}

void CStatusScreenGalaxy::scaleToResolution()
{
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    int scaleFac = gameres.h/200;
    GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());

    if(scaleFac < 0) scaleFac = 1;

    auto biggerRect = mStatusSurface.getSDLSurface()->clip_rect;
    biggerRect.w *= scaleFac;
    biggerRect.h *= scaleFac;

    mStatusSfcTransformed.createRGBSurface(biggerRect);

    // NOTE: Colorkey confusion in legacy version. This fixes the problem
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
    if(scaleFac == 1)
    {
        mStatusSfcTransformed.applyDisplayFormat();
        mStatusSfcTransformed.fillRGB(0x0, 0xFE, 0xFF);
        mStatusSfcTransformed.setColorKey(0x0, 0xFE, 0xFF);
    }
#endif

    mStatusSurface.blitScaledTo(mStatusSfcTransformed);

    // Modern makes the hud a small bit semi transparent
    const auto &optModern = gBehaviorEngine.mOptions[GameOption::MODERN];
    if (optModern.value)
    {
        mStatusSfcTransformed.setAlpha(244);
    }
}

void CStatusScreenGalaxy::GenerateStatusEp4()
{
    SDL_Rect EditRect;
    drawBase(EditRect);

    GsFont &Font = gGraphics.getFont(0);
    Font.setupColor(0x555555);

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), "LOCATION",
                          EditRect.x, EditRect.w, EditRect.y, false);


    // drawing Rect for stuff like background for scores and items
    SDL_Rect TempRect;

    // Location Rect
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+10;
    TempRect.w = EditRect.w;
    TempRect.h = 20;

    Font.setupColor(0x0);

    mStatusSurface.fillRGBA(TempRect, 0xFF, 0xFF, 0xFF, 0xFF);
    Font.drawFontCentered(mStatusSurface.getSDLSurface(),
                          m_Item.mLevelName,
                          TempRect.x,
                          TempRect.w,
                          TempRect.y+6,
                          false);
    Font.setupColor(mStatusSurface.mapRGB(0x44, 0x44, 0x44));


    /// SCORE and EXTRA Rect
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+32;
    TempRect.w = EditRect.w/2; TempRect.h = 10;
    Font.drawFont(mStatusSurface, "SCORE            EXTRA", TempRect.x+16,
                  TempRect.y, false);
    TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
    TempRect.y = EditRect.y+42;

    // Score Box
    TempRect.w = 8*8;
    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_points), 8),
                         TempRect.x, TempRect.y+2,
                         mStatusSurface.getSDLSurface());

    // Extra Box
    TempRect.x = EditRect.x+96;
    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifeAt), 8),
                         TempRect.x, TempRect.y+2,
                         mStatusSurface.getSDLSurface());

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
    Font.drawFont(mStatusSurface, rescLine, TempRect.x+8, TempRect.y, false);
    TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
    TempRect.y = EditRect.y+66;

    // Rescued Box
    TempRect.w = 8*8;
    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);
    for( int count=0 ; count<m_Item.m_special.elders ; count++ )
    {
        gGraphics.drawDigit(40,
                            TempRect.x+8*count,
                            TempRect.y+1,
                            mStatusSurface.getSDLSurface());
    }

    // Level Box
    TempRect.x = EditRect.x+96;
    mStatusSurface.fillRGBA(TempRect, 0xFF, 0xFF, 0xFF, 0xFF);
    Font.setupColor(0x0);

    const std::string difftext = getDifficultyText();

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), difftext,
                          TempRect.x, TempRect.w, TempRect.y+1, false);
    Font.setupColor(0x333333);

    // Keys Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+80;
    Font.drawFont(mStatusSurface, "KEYS", TempRect.x, TempRect.y, false);
    TempRect.w = 8*4; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5;


    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);

    if(m_Item.m_gem.red)
    {
        gGraphics.drawDigit(36, TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());
    }
    if(m_Item.m_gem.yellow)
    {
        gGraphics.drawDigit(37, TempRect.x+8, TempRect.y+1, mStatusSurface.getSDLSurface());
    }
    if(m_Item.m_gem.blue)
    {
        gGraphics.drawDigit(38, TempRect.x+16, TempRect.y+1, mStatusSurface.getSDLSurface());
    }
    if(m_Item.m_gem.green)
    {
        gGraphics.drawDigit(39, TempRect.x+24, TempRect.y+1, mStatusSurface.getSDLSurface());
    }

    // Ammo Box
    TempRect.x = EditRect.x+96;
    TempRect.y = EditRect.y+80;
    Font.drawFont(mStatusSurface, "AMMO", TempRect.x, TempRect.y, false);
    TempRect.w = 8*3; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5;

    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_bullets), 3),
                         TempRect.x, TempRect.y+1,
                         mStatusSurface.getSDLSurface());

    // Keens Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+96;
    Font.drawFont(mStatusSurface, "KEENS", TempRect.x, TempRect.y, false);
    TempRect.w = 8*2; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5+8;
    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifes), 2),
                         TempRect.x,
                         TempRect.y+1,
                         mStatusSurface.getSDLSurface());

    // Drops Box
    TempRect.x = EditRect.x+96;
    TempRect.y = EditRect.y+96;
    Font.drawFont(mStatusSurface, dropsLine, TempRect.x, TempRect.y, false);
    TempRect.w = 8*2; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5+8;

    mStatusSurface.fillRGBA(TempRect, 0x0, 0x0, 0x0, 0xFF);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_drops), 2),
                         TempRect.x,
                         TempRect.y+1,
                         mStatusSurface.getSDLSurface());

    // Swim Suit Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+114;
    TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;

    mStatusSurface.fillRGBA(TempRect, 0xFF, 0xFF, 0xFF, 0xFF);
    Font.setupColor(0x0);
    Font.drawFontCentered(mStatusSurface.getSDLSurface(),
                          m_Item.m_special.swimsuit ? swLine : "???",
                          TempRect.x,
                          TempRect.w,
                          TempRect.y+1, false);

    // Press a Key Sign
    GsTilemap &Tilemap = gGraphics.getTileMap(2);
    TempRect.x = EditRect.x+(EditRect.w/2);
    TempRect.y = EditRect.y+110;
    for( int c=0 ; c<10 ; c++ )
    {
        Tilemap.drawTile(mStatusSurface, TempRect.x+c*8, TempRect.y, 72+c);
    }
    TempRect.y += 8;
    for( int c=0 ; c<10 ; c++ )
    {
        Tilemap.drawTile(mStatusSurface, TempRect.x+c*8, TempRect.y, 82+c);
    }
}

void CStatusScreenGalaxy::GenerateStatusEp5()
{
    SDL_Rect EditRect;
    drawBase(EditRect);

    GsFont &Font = gGraphics.getFont(0);
    Font.setupColor(0x555555);

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), "LOCATION",
                          EditRect.x, EditRect.w, EditRect.y, false);

    // Temporary Rect for drawing some stuff like background for scores and so...
    SDL_Rect TempRect;

    // Location Rect
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+10;
    TempRect.w = EditRect.w;
    TempRect.h = 20;

    Font.setupColor(0x0);

    mStatusSurface.fillRGBA(TempRect, 0xFF, 0xFF, 0xFF, 0xFF);

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), m_Item.mLevelName,
                          TempRect.x, TempRect.w, TempRect.y+6, false);
    Font.setupColor(0x444444);


    /// SCORE and EXTRA Rect
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+32;
    TempRect.w = EditRect.w/2; TempRect.h = 10;
    Font.drawFont(mStatusSurface.getSDLSurface(), "SCORE            EXTRA", TempRect.x+16, TempRect.y, false);
    TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
    TempRect.y = EditRect.y+42;

    // Score Box
    TempRect.w = 8*8;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_points), 8), TempRect.x, TempRect.y+2, mStatusSurface.getSDLSurface());

    // Extra Box
    TempRect.x = EditRect.x+96;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifeAt), 8), TempRect.x, TempRect.y+2, mStatusSurface.getSDLSurface());

    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+56;
    Font.drawFont(mStatusSurface.getSDLSurface(), "KEYCARD", TempRect.x, TempRect.y+4, false);

    // Small Keycard Box
    TempRect.y = EditRect.y+59;
    TempRect.x = EditRect.x+54;
    TempRect.w = 10;
    TempRect.h = 10;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    if(m_Item.m_keycards)
    {
        gGraphics.drawDigit(40, TempRect.x+1, TempRect.y+1, mStatusSurface.getSDLSurface());
    }

    // LEVEL Rects
    TempRect.w = EditRect.w/2;
    TempRect.x = EditRect.x+24;
    TempRect.y = EditRect.y+56;
    Font.drawFont(mStatusSurface.getSDLSurface(), "                  LEVEL", TempRect.x, TempRect.y, false);
    TempRect.h = 11;
    TempRect.y = EditRect.y+66;


    // Level Box
    TempRect.x = EditRect.x+96;
    TempRect.w = 64;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFFFFFFFF);
    Font.setupColor(0x0);

    const std::string difftext = getDifficultyText();

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), difftext, TempRect.x, TempRect.w, TempRect.y+1, false);
    Font.setupColor(0x333333);

    // Keys Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+80;
    Font.drawFont(mStatusSurface.getSDLSurface(), "KEYS", TempRect.x, TempRect.y, false);
    TempRect.w = 8*4; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    if(m_Item.m_gem.red)
        gGraphics.drawDigit(36, TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());
    if(m_Item.m_gem.yellow)
        gGraphics.drawDigit(37, TempRect.x+8, TempRect.y+1, mStatusSurface.getSDLSurface());
    if(m_Item.m_gem.blue)
        gGraphics.drawDigit(38, TempRect.x+16, TempRect.y+1, mStatusSurface.getSDLSurface());
    if(m_Item.m_gem.green)
        gGraphics.drawDigit(39, TempRect.x+24, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Ammo Box
    TempRect.x = EditRect.x+96;
    TempRect.y = EditRect.y+80;
    Font.drawFont(mStatusSurface.getSDLSurface(), "AMMO", TempRect.x, TempRect.y, false);
    TempRect.w = 8*3; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_bullets), 3), TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Keens Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+96;
    Font.drawFont(mStatusSurface.getSDLSurface(), "KEENS", TempRect.x, TempRect.y, false);
    TempRect.w = 8*2; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5+8;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifes), 2), TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Drops Box
    TempRect.x = EditRect.x+96;
    TempRect.y = EditRect.y+96;
    Font.drawFont(mStatusSurface.getSDLSurface(), "VITALIN", TempRect.x, TempRect.y, false);
    TempRect.w = 8*2; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5+8;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_drops), 2), TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Press a Key Sign
    GsTilemap &Tilemap = gGraphics.getTileMap(2);
    TempRect.x = EditRect.x+(EditRect.w-80)/2;
    TempRect.y = EditRect.y+110;
    for( int c=0 ; c<10 ; c++ )
    {
        Tilemap.drawTile(mStatusSurface.getSDLSurface(), TempRect.x+c*8, TempRect.y, 72+c);
        Tilemap.drawTile(mStatusSurface.getSDLSurface(), TempRect.x+c*8, TempRect.y+8, 82+c);
    }
}

void CStatusScreenGalaxy::GenerateStatusEp6()
{
    SDL_Rect EditRect;
    drawBase(EditRect);

    GsFont &Font = gGraphics.getFont(0);
    Font.setupColor(0x555555);

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), "LOCATION", EditRect.x, EditRect.w, EditRect.y, false);

    // Temporary Rect for drawing some stuff like background for scores and so...
    SDL_Rect TempRect;

    // Location Rect
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+10;
    TempRect.w = EditRect.w;
    TempRect.h = 20;

    Font.setupColor(0x0);
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFFFFFFFF);
    Font.drawFontCentered(mStatusSurface.getSDLSurface(), m_Item.mLevelName,
                          TempRect.x, TempRect.w, TempRect.y+6, false);
    Font.setupColor(0x444444);


    /// SCORE and EXTRA Rect
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+32;
    TempRect.w = EditRect.w/2; TempRect.h = 10;
    Font.drawFont(mStatusSurface.getSDLSurface(), "SCORE            EXTRA", TempRect.x+16, TempRect.y, false);
    TempRect.w = (EditRect.w/2)-16; TempRect.h = 11;
    TempRect.y = EditRect.y+42;

    // Score Box
    TempRect.w = 8*8;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_points), 8), TempRect.x, TempRect.y+2, mStatusSurface.getSDLSurface());

    // Extra Box
    TempRect.x = EditRect.x+96;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifeAt), 8), TempRect.x, TempRect.y+2, mStatusSurface.getSDLSurface());

    // Items Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+66;
    Font.drawFont(mStatusSurface.getSDLSurface(), "ITEMS", TempRect.x, TempRect.y+1, false);

    TempRect.x = EditRect.x+44;
    TempRect.w = 30;
    TempRect.h = 10;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);

    auto &spItem = m_Item.m_special;

    const int sandwichTile = (spItem.sandwich > 0) ? 2 : 1;
    const int hookTile = (spItem.hook > 0) ? 4 : 3;
    const int keycardTile = (spItem.rocketKeycard > 0) ? 6 : 5;

    gGraphics.drawDigit(sandwichTile, TempRect.x+1, TempRect.y+1, mStatusSurface.getSDLSurface());
    gGraphics.drawDigit(hookTile, TempRect.x+11, TempRect.y+1, mStatusSurface.getSDLSurface());
    gGraphics.drawDigit(keycardTile, TempRect.x+21, TempRect.y+1, mStatusSurface.getSDLSurface());

    // LEVEL Rects
    TempRect.w = EditRect.w/2;
    TempRect.x = EditRect.x+24;
    TempRect.y = EditRect.y+56;
    Font.drawFont(mStatusSurface.getSDLSurface(), "                  LEVEL", TempRect.x, TempRect.y);
    TempRect.h = 11;
    TempRect.y = EditRect.y+66;


    // Level Box
    TempRect.x = EditRect.x+96;
    TempRect.w = 64;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFFFFFFFF);
    Font.setupColor(0x0);

    const std::string difftext = getDifficultyText();

    Font.drawFontCentered(mStatusSurface.getSDLSurface(), difftext, TempRect.x, TempRect.w, TempRect.y+1, false);
    Font.setupColor(0x333333);

    // Keys Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+80;
    Font.drawFont(mStatusSurface.getSDLSurface(), "KEYS", TempRect.x, TempRect.y);
    TempRect.w = 8*4; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    if(m_Item.m_gem.red)
        gGraphics.drawDigit(36, TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());
    if(m_Item.m_gem.yellow)
        gGraphics.drawDigit(37, TempRect.x+8, TempRect.y+1, mStatusSurface.getSDLSurface());
    if(m_Item.m_gem.blue)
        gGraphics.drawDigit(38, TempRect.x+16, TempRect.y+1, mStatusSurface.getSDLSurface());
    if(m_Item.m_gem.green)
        gGraphics.drawDigit(39, TempRect.x+24, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Ammo Box
    TempRect.x = EditRect.x+96;
    TempRect.y = EditRect.y+80;
    Font.drawFont(mStatusSurface.getSDLSurface(), "AMMO", TempRect.x, TempRect.y);
    TempRect.w = 8*3; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_bullets), 3), TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Keens Box
    TempRect.x = EditRect.x;
    TempRect.y = EditRect.y+96;
    Font.drawFont(mStatusSurface.getSDLSurface(), "KEENS", TempRect.x, TempRect.y);
    TempRect.w = 8*2; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5+8;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_lifes), 2), TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Drops Box
    TempRect.x = EditRect.x+96;
    TempRect.y = EditRect.y+96;
    Font.drawFont(mStatusSurface.getSDLSurface(), "VIVAS", TempRect.x, TempRect.y);
    TempRect.w = 8*2; TempRect.h = 10;
    TempRect.x = TempRect.x+8*5+8;
    SDL_FillRect(mStatusSurface.getSDLSurface(), &TempRect, 0xFF000000);
    gGraphics.drawDigits(getRightAlignedString(itoa(m_Item.m_drops), 2), TempRect.x, TempRect.y+1, mStatusSurface.getSDLSurface());

    // Press a Key Sign
    GsTilemap &Tilemap = gGraphics.getTileMap(2);
    TempRect.x = EditRect.x+(EditRect.w-80)/2;
    TempRect.y = EditRect.y+110;
    for( int c=0 ; c<10 ; c++ )
    {
        Tilemap.drawTile(mStatusSurface.getSDLSurface(), TempRect.x+c*8, TempRect.y, 72+c);
        Tilemap.drawTile(mStatusSurface.getSDLSurface(), TempRect.x+c*8, TempRect.y+8, 82+c);
    }
}

void CStatusScreenGalaxy::draw()
{
    auto weakBlit = GsWeakSurface(gVideoDriver.getBlitSurface());
    mStatusSfcTransformed.blitTo(weakBlit);
}
