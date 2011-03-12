/*
 * CStatusScreen.cpp
 *
 *  Created on: 05.03.2011
 *      Author: gerstrong
 */

#include "CStatusScreenGalaxy.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/video/colourconvert.h"
#include "common/CBehaviorEngine.h"

CStatusScreenGalaxy::CStatusScreenGalaxy() :
m_showstatus(false),
mp_StatusSurface(NULL)
{
	const int episode = g_pBehaviorEngine->getEpisode();

	// TODO Auto-generated constructor stub
	if(episode == 4)
		generateStatusEP4();
	//else if(episode == 5)
	//else if(episode == 6)
}

void CStatusScreenGalaxy::drawBase(SDL_Rect &EditRect)
{
	// Create a surface for that
	SDL_Surface *temp;
	const Uint32 flags = 0;
	// For some reason the Alpha mask doesn't work, if blitsurface == screensurface. Not sure if every system is affected of that.
	// Maybe I write a function for the proper masks...
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		const Uint32 rmask = 0xFF000000;
		const Uint32 gmask = 0x00FF0000;
		const Uint32 bmask = 0x0000FF00;
    	const Uint32 amask = 0x000000FF;
    #else
    	const Uint32 rmask = 0x000000FF;
    	const Uint32 gmask = 0x0000FF00;
    	const Uint32 bmask = 0x00FF0000;
    	const Uint32 amask = 0xFF000000;
    #endif

   	const SDL_Rect& DestRect = g_pVideoDriver->getGameResolution();
	mp_StatusSurface = SDL_CreateRGBSurface( flags, DestRect.w, DestRect.h, 32, rmask, gmask, bmask, amask);

	/// Draw the required bitmaps and backgrounds
	// Draw the support Bitmap and see where the gray rectangle starts...
	// Prepare the drawrect for positions
	SDL_Rect Dest;

	// Create upper stomp support
	CBitmap &SupportBmp = g_pGfxEngine->getMaskedBitmap(2);
	SDL_Rect SupportRect = SupportBmp.getRect();
	Dest.x = (DestRect.w-SupportRect.w)/2;	Dest.y = 0;
	temp = SDL_ConvertSurface( SupportBmp.getSDLSurface(), mp_StatusSurface->format, flags );
	BlitSurfaceMerge( temp, &SupportRect, mp_StatusSurface, &Dest );
	SDL_FreeSurface(temp);

	// Draw the gray surface
	SDL_Rect BackRect;
	BackRect.w = 192; // Standard sizes in Keen 4
	BackRect.h = 152;
	BackRect.x = (DestRect.w-BackRect.w)/2;
	BackRect.y = SupportRect.h;
	SDL_FillRect( mp_StatusSurface, &BackRect, 0xFFAAAAAA); //gray

	// Draw the cables Bitmap
	CBitmap &Cables_Bitmap = g_pGfxEngine->getMaskedBitmap(1);
	SDL_Rect CableRect = Cables_Bitmap.getRect();
	Dest.x = BackRect.x - CableRect.w;	Dest.y = 0;
	temp = SDL_ConvertSurface( Cables_Bitmap.getSDLSurface(), mp_StatusSurface->format, flags );
	BlitSurfaceMerge( temp, &CableRect, mp_StatusSurface, &Dest );
	SDL_FreeSurface(temp);

	// Now draw the borders
	CTilemap &Tilemap = g_pGfxEngine->getTileMap(2);

	// Upper Left corner
	Tilemap.drawTile(mp_StatusSurface, BackRect.x, BackRect.y, 54);

	// Upper Part
	for(int c=1 ; c<(BackRect.w/8)-1 ; c++)
		Tilemap.drawTile(mp_StatusSurface, BackRect.x+c*8, BackRect.y, 55);

	// Upper Right Part
	Tilemap.drawTile(mp_StatusSurface, BackRect.x+BackRect.w-8, BackRect.y, 56);

	// Left Part
	for(int c=1 ; c<(BackRect.h/8)-1 ; c++)
		Tilemap.drawTile(mp_StatusSurface, BackRect.x, BackRect.y+c*8, 57);

	// Right Part
	for(int c=1 ; c<(BackRect.h/8)-1 ; c++)
		Tilemap.drawTile(mp_StatusSurface, BackRect.x+BackRect.w-8, BackRect.y+c*8, 59);

	// Lower Left Part
	Tilemap.drawTile(mp_StatusSurface, BackRect.x, BackRect.y+BackRect.h-8, 60);

	// Lower Part
	for(int c=1 ; c<(BackRect.w/8)-1 ; c++)
		Tilemap.drawTile(mp_StatusSurface, BackRect.x+c*8, BackRect.y+BackRect.h-8, 61);

	// Lower Right Part
	Tilemap.drawTile(mp_StatusSurface, BackRect.x+BackRect.w-8, BackRect.y+BackRect.h-8, 62);

	EditRect.x = BackRect.x+8;
	EditRect.y = BackRect.y+8;
	EditRect.w = BackRect.w-16;
	EditRect.h = BackRect.h-16;
}

void CStatusScreenGalaxy::generateStatusEP4()
{
	SDL_Rect EditRect;
	drawBase(EditRect);

	SDL_PixelFormat *pixelformat = mp_StatusSurface->format;

	CFont &Font = g_pGfxEngine->getFont(0);
	Font.setBGColour(pixelformat, 0xAAAAAA);
	Font.setFGColour(pixelformat, 0x555555);

	Font.drawFontCentered(mp_StatusSurface, "LOCATION", EditRect.x+4, EditRect.w, EditRect.y+4, false);

	// Temporary Rect for drawing some stuff like background for scores and so...
	SDL_Rect TempRect;

	// Location Rect
	TempRect.x=EditRect.x+8;
	TempRect.y=EditRect.y+16;
	TempRect.w=EditRect.w-16;
	TempRect.h=20;

	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFFFFFFFF);

	// Score Rect
	TempRect.x = EditRect.x+8;
	TempRect.y = EditRect.y+40;
	TempRect.w = EditRect.w/2; TempRect.h = 10;
	Font.drawFontCentered(mp_StatusSurface, "SCORE            EXTRA", TempRect.x, EditRect.w, TempRect.y, false);
	TempRect.w = (EditRect.w/2)-16; TempRect.h = 10;
	TempRect.y = EditRect.y+50;
	SDL_FillRect(mp_StatusSurface, &TempRect, 0xFF000000);

}

void CStatusScreenGalaxy::draw()
{
	SDL_BlitSurface( mp_StatusSurface, NULL, g_pVideoDriver->getBlitSurface(), NULL );
}

CStatusScreenGalaxy::~CStatusScreenGalaxy() {
	if(mp_StatusSurface)
		SDL_FreeSurface(mp_StatusSurface);
}
