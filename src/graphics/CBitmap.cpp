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
{
	m_BitmapSurface = NULL;
	m_name = "";
}

CBitmap::CBitmap(const CBitmap &bitmap) :
m_ImageRect(bitmap.getRect()),
m_name(bitmap.getName()),
m_BitmapSurface(NULL)
{
	//SDL_Surface *sfc = const_cast<SDL_Surface*>(bitmap.getSDLSurface());
	SDL_Surface *sfc = bitmap.getSDLSurface();
	if( sfc != NULL )
		m_BitmapSurface = SDL_DisplayFormat( sfc );
}

///
// Creation Routines
///
bool CBitmap::createSurface(Uint32 flags, SDL_Color *Palette)
{
	if(m_BitmapSurface) SDL_FreeSurface(m_BitmapSurface);
	
	m_BitmapSurface = SDL_CreateRGBSurface(flags, m_ImageRect.w, m_ImageRect.h, 8, 0, 0, 0, 0);
	SDL_SetColors(m_BitmapSurface, Palette, 0, 255);
	SDL_SetColorKey(m_BitmapSurface, SDL_SRCCOLORKEY, COLORKEY);
	return (m_BitmapSurface != NULL);
}

bool CBitmap::optimizeSurface()
{
	if(m_BitmapSurface)
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormat(m_BitmapSurface);
		SDL_FreeSurface(m_BitmapSurface);
		m_BitmapSurface = temp_surface;
		return true;
	}
	else
		return false;
}

bool CBitmap::loadHQBitmap( const std::string& filename )
{
	if(!IsFileAvailable(filename))
		return false;

	if(m_BitmapSurface)
	{
		SDL_Surface *temp_surface = SDL_LoadBMP(GetFullFileName(filename).c_str());
		if(temp_surface)
		{
			SDL_Surface *displaysurface = SDL_ConvertSurface(temp_surface, m_BitmapSurface->format, m_BitmapSurface->flags);
			SDL_BlitSurface(displaysurface, NULL, m_BitmapSurface, NULL);
			SDL_FreeSurface(displaysurface);
			SDL_FreeSurface(temp_surface);
			return true;
		}
	}
	return false;
}

///
// Getters and Setters
///
/*SDL_Surface *CBitmap::getSDLSurface()
{
	return m_BitmapSurface;
}*/

void CBitmap::setDimensions(const Uint16 w, const Uint16 h)
{
	m_ImageRect.w = w;
	m_ImageRect.h = h;
}

void CBitmap::setName(const std::string &name)
{
	m_name = name;
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
void CBitmap::_draw(SDL_Surface *dst, Uint16 x, Uint16 y)
{
	SDL_Rect dst_rect;
	dst_rect.x = x;	dst_rect.y = y;
	dst_rect.w = m_ImageRect.w;
	dst_rect.h = m_ImageRect.h;
	
	if( dst_rect.w>0 && dst_rect.h>0 )
		SDL_BlitSurface(m_BitmapSurface, NULL, dst, &dst_rect);
}

///
// Freeing Routines
///

void CBitmap::destroySurface() {
	if(m_BitmapSurface) SDL_FreeSurface(m_BitmapSurface);
	m_BitmapSurface = NULL;
}
