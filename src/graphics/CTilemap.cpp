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

CTilemap::CTilemap(stTile *pTileProperties, int numtiles) {
	m_Tilesurface = NULL;
	memset( m_AnimTileInUse, 0, sizeof(m_AnimTileInUse));
	memset( m_animtiles, 0, sizeof(m_animtiles));
	m_animtiletimer = m_curanimtileframe = 0;
	mp_tiles = pTileProperties;
	m_numtiles = numtiles;
	m_animation_enabled = true;
}

CTilemap::~CTilemap() {
	if(mp_tiles) delete [] mp_tiles;
}

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

//////////////////////////////
///// Animation Routines /////
//////////////////////////////
void CTilemap::animateAllTiles(SDL_Surface *dst)
{
	/* animate animated tiles */
	if (m_animtiletimer>ANIM_TILE_TIME && m_animation_enabled)
	{
		/* advance to next frame */
		m_curanimtileframe = (m_curanimtileframe+1)&7;
		
		/* re-draw all animated tiles */
		for(int i=1;i<MAX_ANIMTILES-1;i++)
		{
			if ( m_animtiles[i].slotinuse )
			{
				drawTile( dst, m_animtiles[i].x, m_animtiles[i].y,
						 m_animtiles[i].baseframe+
						 ((m_animtiles[i].offset+m_curanimtileframe)%
						  mp_tiles[m_animtiles[i].baseframe].animation));
			}
		}
		m_animtiletimer = 0;
	}
	else m_animtiletimer++;
}

// unregisters all animated tiles with baseframe tile
void CTilemap::unregisterAnimtiles(int tile)
{
	int i;
	for(i=0;i<MAX_ANIMTILES-1;i++)
	{
        if (m_animtiles[i].baseframe == tile)
        {
			m_animtiles[i].slotinuse = 0;
        }
	}
}

// register the tiles which has to be animated
void CTilemap::registerAnimation(Uint32 x, Uint32 y, int c)
{
	// we just drew over an animated tile which we must unregisterp
    if (m_AnimTileInUse[x>>4][y>>4])
    {
		m_animtiles[m_AnimTileInUse[x>>4][y>>4]].slotinuse = 0;
		m_AnimTileInUse[x>>4][y>>4] = 0;
    }
	
    // we just drew an animated tile which we will now register
    if ( mp_tiles[c].animation > 1 )
    {
		for(int i=1 ; i<MAX_ANIMTILES-1 ; i++)
		{
			if (!m_animtiles[i].slotinuse)
			{  // we found an unused slot
				m_animtiles[i].x = x;
				m_animtiles[i].y = y;
				m_animtiles[i].baseframe = c - mp_tiles[c].animOffset;
				m_animtiles[i].offset = mp_tiles[c].animOffset;
				m_animtiles[i].slotinuse = 1;
				m_AnimTileInUse[x>>4][y>>4] = i;
				break;
			}
		}
    }
}

void CTilemap::deAnimateAt(Uint16 px, Uint16 py)
{
    // find it!
    for(int i=1;i<MAX_ANIMTILES-1;i++)
    {
		if (m_animtiles[i].x == px && m_animtiles[i].y == py)
		{
			m_animtiles[i].slotinuse = 0;
			m_animtiles[i].offset = 0;
			m_AnimTileInUse[px>>4][py>>4] = 0;
			return;
		}
    }
}

