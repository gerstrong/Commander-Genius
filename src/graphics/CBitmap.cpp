/*
 * CBitmap.cpp
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#include "CBitmap.h"
#include "CPalette.h"
#include "FindFile.h"
#include "sdl/CVideoDriver.h"

CBitmap::CBitmap()
{}


CBitmap::CBitmap(const SmartPointer<SDL_Surface> &bmpSfc)
{
	mpBitmapSurface = bmpSfc;
}

CBitmap::CBitmap(const CBitmap &bitmap) :
mName(bitmap.getName())
{
	SDL_Surface *sfc = bitmap.getSDLSurface();
	if( sfc != NULL )
		mpBitmapSurface = SDL_DisplayFormat( sfc );
}

///
// Creation Routines
///
bool CBitmap::createSurface(Uint32 flags, SDL_Rect rect, SDL_Color *Palette)
{
	mpBitmapSurface = SDL_CreateRGBSurface(flags, rect.w, rect.h, 8, 0, 0, 0, 0);
	SDL_SetColors(mpBitmapSurface.get(), Palette, 0, 255);
	SDL_SetColorKey(mpBitmapSurface.get(), SDL_SRCCOLORKEY, COLORKEY);
	return (!mpBitmapSurface.empty());
}

bool CBitmap::optimizeSurface()
{
	if(!mpBitmapSurface.empty())
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormat(mpBitmapSurface.get());
		mpBitmapSurface = temp_surface;
		return true;
	}
	else
		return false;
}

bool CBitmap::loadHQBitmap( const std::string& filename )
{
	if(!IsFileAvailable(filename))
		return false;

	if(!mpBitmapSurface.empty())
	{
		SDL_Surface *tempSurface = SDL_LoadBMP(GetFullFileName(filename).c_str());
		if(tempSurface)
		{
			SDL_Surface *displaysurface = SDL_ConvertSurface(tempSurface, mpBitmapSurface->format, mpBitmapSurface->flags);
			SDL_BlitSurface(displaysurface, NULL, mpBitmapSurface.get(), NULL);
			SDL_FreeSurface(displaysurface);
			SDL_FreeSurface(tempSurface);
			return true;
		}
	}
	return false;
}


/**
 * \brief The function that blits the sprite to dst
 * \param x		 		X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CBitmap::draw(Uint16 x, Uint16 y)
{
	g_pVideoDriver->mDrawTasks.add( new DrawBitmapTask( this, x, y ) );
}

///
// Drawing Routines
///
void CBitmap::_draw(SDL_Surface *dst, Uint16 x, Uint16 y) const
{
	SDL_Rect dst_rect;
	dst_rect.x = x;	dst_rect.y = y;
	
	SDL_BlitSurface(mpBitmapSurface.get(), NULL, dst, &dst_rect);
}
