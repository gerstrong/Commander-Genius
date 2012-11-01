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
#include "sdl/extensions.h"

CStatusScreenGalaxy::CStatusScreenGalaxy(const stItemGalaxy& Item, const std::string &LevelName) :
m_showstatus(false),
m_Item(Item),
m_LevelName(LevelName)
{}

void CStatusScreenGalaxy::drawBase(SDL_Rect &EditRect)
{
	// Create a surface for that
	SmartPointer<SDL_Surface> temp;

   	const SDL_Rect DestRect = g_pVideoDriver->getGameResolution().SDLRect();
   	mpStatusSurface.reset(CG_CreateRGBSurface(DestRect), &SDL_FreeSurface);


	/// Draw the required bitmaps and backgrounds
	// Draw the support Bitmap and see where the gray rectangle starts...
	// Prepare the drawrect for positions
	SDL_Rect Dest;

	// Create upper stomp support
	CBitmap &SupportBmp = g_pGfxEngine->getMaskedBitmap(2);
	SDL_Rect SupportRect;

	SupportRect.w = SupportBmp.getSDLSurface()->w;
	SupportRect.h = SupportBmp.getSDLSurface()->h;
	Dest.x = (DestRect.w-SupportRect.w)/2;	Dest.y = 0;
	temp = SDL_DisplayFormat( SupportBmp.getSDLSurface() );
	SDL_BlitSurface( temp.get(), NULL, mpStatusSurface.get(), &Dest );

	// Draw the gray surface
	SDL_Rect BackRect;
	BackRect.w = 192; // Standard sizes in Keen 4
	BackRect.h = 152;
	BackRect.x = (DestRect.w-BackRect.w)/2;
	BackRect.y = SupportRect.h;
	SDL_FillRect( mpStatusSurface.get(), &BackRect, 0xFFAAAAAA); //gray

	// Draw the cables Bitmap
	CBitmap &Cables_Bitmap = g_pGfxEngine->getMaskedBitmap(1);
	SDL_Rect CableRect;
	CableRect.w = Cables_Bitmap.getSDLSurface()->w;
	CableRect.h = Cables_Bitmap.getSDLSurface()->h;
	Dest.x = BackRect.x - CableRect.w;	Dest.y = 0;
	temp = SDL_DisplayFormat( Cables_Bitmap.getSDLSurface() );
	SDL_BlitSurface( temp.get(), NULL, mpStatusSurface.get(), &Dest );

	// Now draw the borders
	CTilemap &Tilemap = g_pGfxEngine->getTileMap(2);

	// Upper Left corner
	Tilemap.drawTile(mpStatusSurface.get(), BackRect.x, BackRect.y, 54);

	// Upper Part
	for(int c=1 ; c<(BackRect.w/8)-1 ; c++)
		Tilemap.drawTile(mpStatusSurface.get(), BackRect.x+c*8, BackRect.y, 55);

	// Upper Right Part
	Tilemap.drawTile(mpStatusSurface.get(), BackRect.x+BackRect.w-8, BackRect.y, 56);

	// Left Part
	for(int c=1 ; c<(BackRect.h/8)-1 ; c++)
		Tilemap.drawTile(mpStatusSurface.get(), BackRect.x, BackRect.y+c*8, 57);

	// Right Part
	for(int c=1 ; c<(BackRect.h/8)-1 ; c++)
		Tilemap.drawTile(mpStatusSurface.get(), BackRect.x+BackRect.w-8, BackRect.y+c*8, 59);

	// Lower Left Part
	Tilemap.drawTile(mpStatusSurface.get(), BackRect.x, BackRect.y+BackRect.h-8, 60);

	// Lower Part
	for(int c=1 ; c<(BackRect.w/8)-1 ; c++)
		Tilemap.drawTile(mpStatusSurface.get(), BackRect.x+c*8, BackRect.y+BackRect.h-8, 61);

	// Lower Right Part
	Tilemap.drawTile(mpStatusSurface.get(), BackRect.x+BackRect.w-8, BackRect.y+BackRect.h-8, 62);

	EditRect.x = BackRect.x+16;
	EditRect.y = BackRect.y+12;
	EditRect.w = BackRect.w-32;
	EditRect.h = BackRect.h-32;
}

void CStatusScreenGalaxy::draw()
{
	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mpStatusSurface, NULL, NULL ) );
}
