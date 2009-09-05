/*
 * CBitmap.cpp
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#include "CBitmap.h"

CBitmap::CBitmap() {
	m_BitmapSurface = NULL;
	m_name = "";
}

CBitmap::~CBitmap() {
	if(m_BitmapSurface) SDL_FreeSurface(m_BitmapSurface);
	m_BitmapSurface = NULL;
}

///
// Creation Routines
///
bool CBitmap::createSurface(Uint32 flags, SDL_Color *Palette)
{
	if(m_BitmapSurface) SDL_FreeSurface(m_BitmapSurface);

	m_BitmapSurface = SDL_CreateRGBSurface(flags, m_width, m_height, 8, 0, 0, 0, 0);
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

///
// Getters and Setters
///
SDL_Surface *CBitmap::getSDLSurface()
{
	return m_BitmapSurface;
}

void CBitmap::setDimensions(Uint16 w, Uint16 h)
{
	m_width = w;	m_height = h;
}

void CBitmap::setName(const std::string &name)
{
	m_name = name;
}

void CBitmap::setColorPalette(SDL_Color *Palette)
{
	SDL_SetColors(m_BitmapSurface, Palette, 0, 255);
}

///
// Drawing Routines
///
void CBitmap::draw(SDL_Surface *dst, Uint16 x, Uint16 y)
{
	SDL_Rect dst_rect;
	dst_rect.x = x;	dst_rect.y = y;
	dst_rect.w = m_width;
	dst_rect.h = m_height;

	SDL_BlitSurface(m_BitmapSurface, NULL, dst, &dst_rect);
}
