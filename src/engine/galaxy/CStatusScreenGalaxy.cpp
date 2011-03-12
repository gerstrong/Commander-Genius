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

void CStatusScreenGalaxy::generateStatusEP4()
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

	// Draw the required Bitmaps
	SDL_Rect Dest;
	Dest.x = 100;
	Dest.y = 100;

	CBitmap &Cables_Bitmap = g_pGfxEngine->getMaskedBitmap(1);
	SDL_Rect CableRect = Cables_Bitmap.getRect();
	temp = SDL_ConvertSurface( Cables_Bitmap.getSDLSurface(), mp_StatusSurface->format, flags );
	BlitSurfaceMerge( temp, &CableRect, mp_StatusSurface, &Dest );
}

void CStatusScreenGalaxy::draw()
{
	SDL_BlitSurface( mp_StatusSurface, NULL, g_pVideoDriver->getBlitSurface(), NULL );
}

CStatusScreenGalaxy::~CStatusScreenGalaxy() {
	if(mp_StatusSurface)
		SDL_FreeSurface(mp_StatusSurface);
}
