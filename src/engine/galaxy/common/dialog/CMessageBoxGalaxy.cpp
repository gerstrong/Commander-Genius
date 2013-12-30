/*
 * CMessageBoxGalaxy.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 *
 *  This class basically handles the message boxes seen 
 *  within the galaxy games. This box is shown whenever there is a Dialog between
 *  Keen and one of the oracles.
 */

#include "CMessageBoxGalaxy.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "graphics/CGfxEngine.h"
#include "sdl/extensions.h"
#include "StringUtils.h"
#include <memory>

const int FONT_ID = 0;

CMessageBoxGalaxy::CMessageBoxGalaxy(const std::string& Text) :
mMustClose(false),
mText(Text)
{
	CFont &Font = g_pGfxEngine->getFont(FONT_ID);

	mTextHeight = Font.getPixelTextHeight()*calcNumLines(mText);
    GsRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();

	// Create a surface for that
	mMBRect.w = Font.getPixelTextWidth(mText)+16;
	mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+1)+16;
    mMBRect.x = (gameRes.w-mMBRect.w)/2;
    mMBRect.y = (gameRes.h-mMBRect.h)/2;
}

void CMessageBoxGalaxy::init()
{    
    mpMBSurface.reset(CG_CreateRGBSurface( mMBRect ), &SDL_FreeSurface);
    mpMBSurface.reset(g_pVideoDriver->convertThroughBlitSfc( mpMBSurface.get() ), &SDL_FreeSurface);

	initGalaxyFrame();

	SDL_Rect rect = mMBRect;
	rect.x = 8;
	rect.y = 8;
	rect.w -= 16;
	rect.h -= 16;

	initText(rect);
}

void CMessageBoxGalaxy::initGalaxyFrame()
{
	SDL_Surface *dst = mpMBSurface.get();

	// first draw a blank rect
	SDL_Rect rect;
	rect.x = 4;
	rect.y = 4;
	rect.w = mMBRect.w-16;
	rect.h = mMBRect.h-16;
	SDL_FillRect(dst, &rect, 0xFFFFFFFF);

	// Draw the borders
	rect = mMBRect;
	rect.x += 8;
	rect.y += 8;
	rect.w -= 16;
	rect.h -= 16;

	CTilemap &Tilemap = g_pGfxEngine->getTileMap(3);

	/// Now draw the borders
	// Upper Left corner
	Tilemap.drawTile(dst, 0, 0, 0);

	// Upper border
	for(int x=8 ; x<rect.w ; x+=8)
		Tilemap.drawTile(dst, x, 0, 1);

	// Upper Right corner
	Tilemap.drawTile(dst, rect.w, 0, 2);

	// Left border
	for(int y=8 ; y<rect.h ; y+=8)
		Tilemap.drawTile(dst, 0, y, 3);

	// Right border
	for(int y=8 ; y<rect.h ; y+=8)
		Tilemap.drawTile(dst, rect.w, y, 5);

	// Lower Left corner
	Tilemap.drawTile(dst, 0, rect.h, 6);

	// Lower border
	for(int x=8 ; x<rect.w ; x+=8)
		Tilemap.drawTile(dst, x, rect.h, 7);

	// Lower Right corner
	Tilemap.drawTile(dst, rect.w, rect.h, 8);
}

void CMessageBoxGalaxy::initText(const SDL_Rect &rect)
{
	CFont &Font = g_pGfxEngine->getFont(FONT_ID);

	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;

	std::unique_ptr<SDL_Surface,SDL_Surface_Deleter> pTextSfc(Font.fetchColoredTextSfc( mText, SDL_MapRGB( format, 0, 0, 0 ) ));
	SDL_BlitSurface(pTextSfc.get(), NULL, mpMBSurface.get(), const_cast<SDL_Rect*>(&rect));
}

void CMessageBoxGalaxy::ponder()
{
    // Look, if somebody pressed a button, and close this dialog!
    if( g_pInput->getPressedAnyCommand() )
    {
        mMustClose = true;
        g_pInput->flushCommands();
        return;
    }
}

void CMessageBoxGalaxy::render()
{
    // Just render the MessageBox
    SDL_BlitSurface(mpMBSurface.get(), nullptr, g_pVideoDriver->getBlitSurface(), &mMBRect);
}
