/*
 * CTilemap.cpp
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#include "../fileio/CTileLoader.h"
#include "../FindFile.h"
#include "CTilemap.h"
#include "CPalette.h"
#include <stdlib.h>

CTilemap::CTilemap() :
m_Tilesurface(NULL),
m_EmptyBackgroundTile(143),
m_numtiles(0),
m_pbasesize(0),
m_column(0)
{ }

bool CTilemap::CreateSurface(SDL_Color *Palette, Uint32 Flags,
				Uint16 numtiles, Uint16 pbasesize, Uint16 column)
{
	m_numtiles = numtiles;
	m_pbasesize = pbasesize;
	m_column = column;
	m_Tilesurface = SDL_CreateRGBSurface(Flags, m_column<<m_pbasesize,
										(m_numtiles/m_column)<<m_pbasesize, 8, 0, 0, 0, 0);
	SDL_SetColors(m_Tilesurface, Palette, 0, 255);
	SDL_SetColorKey(m_Tilesurface, SDL_SRCCOLORKEY, COLORKEY);
	
	return ( m_Tilesurface != NULL );
}

bool CTilemap::loadHiresTile( const std::string& filename )
{
	if(!IsFileAvailable(filename))
		return false;

	if(m_Tilesurface)
	{
		SDL_Surface *temp_surface = SDL_LoadBMP(GetFullFileName(filename).c_str());
		if(temp_surface)
		{
			SDL_FreeSurface(m_Tilesurface);
			m_Tilesurface = temp_surface;
			return true;
		}
	}
	
	return false;
}

bool CTilemap::optimizeSurface()
{
	if(m_Tilesurface)
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormatAlpha(m_Tilesurface);
		SDL_FreeSurface(m_Tilesurface);
		m_Tilesurface = temp_surface;
		return true;
	}
	else
		return false;
}

///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////
SDL_Surface *CTilemap::getSDLSurface()
{
	return m_Tilesurface;
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////

void CTilemap::drawTile(SDL_Surface *dst, Uint16 x, Uint16 y, Uint16 t)
{
	SDL_Rect src_rect, dst_rect;
	src_rect.x = (t%m_column)<<m_pbasesize;
	src_rect.y = (t/m_column)<<m_pbasesize;
	src_rect.w = src_rect.h = dst_rect.w = dst_rect.h = 1<<m_pbasesize;
	dst_rect.x = x;		dst_rect.y = y;
	
	SDL_BlitSurface(m_Tilesurface, &src_rect, dst, &dst_rect);
}

CTilemap::~CTilemap() {
	if(m_Tilesurface) SDL_FreeSurface(m_Tilesurface);
}
