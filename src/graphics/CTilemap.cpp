/*
 * CTilemap.cpp
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#include "../fileio/CTileLoader.h"
#include "CTilemap.h"
#include "CPalette.h"
#include <stdlib.h>

CTilemap::CTilemap(stTile *pTileProperties, int numtiles) :
m_Tilesurface(NULL),
mp_tiles(pTileProperties),
m_numtiles(numtiles),
m_EmptyBackgroundTile(143)
{ }

bool CTilemap::CreateSurface(SDL_Color *Palette, Uint32 Flags)
{
	m_Tilesurface = SDL_CreateRGBSurface(Flags, 13*16, 800*16, 8, 0, 0, 0, 0);
	SDL_SetColors(m_Tilesurface, Palette, 0, 255);
	SDL_SetColorKey(m_Tilesurface, SDL_SRCCOLORKEY, COLORKEY);
	
	return ( m_Tilesurface != NULL );
}

bool CTilemap::loadHiresTile( const std::string& filename )
{
	if(m_Tilesurface)
	{
		SDL_Surface *temp_surface = SDL_LoadBMP(filename.c_str());
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
	src_rect.x = 16*(t%13);		src_rect.y = 16*(t/13);
	src_rect.w = src_rect.h = dst_rect.w = dst_rect.h = 16;
	dst_rect.x = x;		dst_rect.y = y;
	
	SDL_BlitSurface(m_Tilesurface, &src_rect, dst, &dst_rect);
}

CTilemap::~CTilemap() {
	if(mp_tiles) delete [] mp_tiles;
	if(m_Tilesurface) SDL_FreeSurface(m_Tilesurface);
}
