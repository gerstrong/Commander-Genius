/*
 * CTilemap.cpp
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#include "fileio/CTileLoader.h"
#include "sdl/CVideoDriver.h"
#include "FindFile.h"
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
/////    Getters       /////
////////////////////////////


int CTilemap::getDimension()
{
	return (1<<m_pbasesize);
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////

#ifdef DEBUG_COLLISION

#define CSF    9
#define TILE_S			4
#define STC (CSF-TILE_S)

void FillSlopeRect(SDL_Surface *dst, const SDL_Rect dst_rect, Uint32 color, char blocked)
{
	/**
	 * So far only used in Galaxy. This is the code for sloped tiles downside
	 * This is performed when Keen walks into a sloped tile
	 *
	 * 0	Fall through		1	Flat
	 * 2	Top -> Middle		3	Middle -> bottom
	 * 4	Top -> bottom		5	Middle -> top
	 * 6	Bottom -> middle	7	Bottom -> top
	 * 8	Unused			9	Deadly, can't land on in God mode
	 */

	int yb1, yb2;

	if(!blocked)
		return;

	if( blocked == 2 )
		yb1 = 0,	yb2 = 256;
	else if( blocked == 3 )
		yb1 = 256,	yb2 = 512;
	else if( blocked == 4 )
		yb1 = 0,	yb2 = 512;
	else if( blocked == 5 )
		yb1 = 256,	yb2 = 0;
	else if( blocked == 6 )
		yb1 = 512,	yb2 = 256;
	else if( blocked == 7 )
		yb1 = 512,	yb2 = 0;
	else
		yb1 = 0, yb2 = 0;

	SDL_Rect sloperect = dst_rect;
	sloperect.w = 1;

	if( blocked > 1 )
	{
		for( int c = 0 ; c<512 ; c++ )
		{
			sloperect.x = dst_rect.x + (c>>STC);
			sloperect.y = dst_rect.y + ((yb1+c*(yb2-yb1)/512)>>STC);
			sloperect.h = dst_rect.h - sloperect.y;
			SDL_FillRect( dst, &sloperect, 0xFFFFFFFF);
		}
	}

	SDL_Rect line1_rect = dst_rect;
	line1_rect.h = 1;
	SDL_FillRect( dst, &line1_rect, 0xFF545454);
	SDL_Rect line2_rect = dst_rect;
	line2_rect.w = 1;
	SDL_FillRect( dst, &line2_rect, 0xFF545454);
}
#endif

void CTilemap::drawTile(SDL_Surface *dst, Uint16 x, Uint16 y, Uint16 t)
{
	SDL_Rect src_rect, dst_rect;
	src_rect.x = (t%m_column)<<m_pbasesize;
	src_rect.y = (t/m_column)<<m_pbasesize;
	const int size = 1<<m_pbasesize;
	src_rect.w = src_rect.h = dst_rect.w = dst_rect.h = size;

/*	const int max_width = g_pVideoDriver->getGameResolution().w;
	const int max_height = g_pVideoDriver->getGameResolution().h;*/

	/*if( x + size > max_width )
		src_rect.w = max_width-x;

	if( y + size > max_height )
		src_rect.h = max_height-y;*/


	dst_rect.x = x;		dst_rect.y = y;
	
	SDL_BlitSurface(m_Tilesurface, &src_rect, dst, &dst_rect);

#ifdef DEBUG_COLLISION

	//std::vector<CTileProperties> &TileProp = g_pBehaviorEngine->getTileProperties(1);
	//FillSlopeRect(dst, dst_rect, 0xFFFFFFFF, TileProp[t].bup);
#endif

}

CTilemap::~CTilemap() 
{
	if(m_Tilesurface) 
	    SDL_FreeSurface(m_Tilesurface);
}
