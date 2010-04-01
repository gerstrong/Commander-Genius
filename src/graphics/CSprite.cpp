/*
 * CSprite.cpp
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#include "CSprite.h"
#include "CPalette.h"
#include "../sdl/CVideoDriver.h"

#define SAFE_DELETE(x) { if(x) SDL_FreeSurface(x); x = NULL; }

CSprite::CSprite() :
m_surface(NULL),
m_masksurface(NULL)
{
	m_xsize = m_ysize = 0;
	m_bboxX1 = m_bboxY1 = 0;
	m_bboxX2 = m_bboxY2 = 0;
}

/////////////////////////////
// Initialization Routines //
/////////////////////////////
bool CSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{
	m_surface = SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
	SDL_SetColors( m_surface, Palette, 0, 255);
	SDL_SetColorKey( m_surface, SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
	m_masksurface = SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
	SDL_SetColors( m_masksurface, Palette, 0, 255);
	SDL_SetColorKey( m_masksurface, SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
	
	return ( !m_surface && !m_masksurface );
}

bool CSprite::optimizeSurface()
{
	if(m_surface)
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormatAlpha(m_surface);
		SDL_FreeSurface(m_surface);
		m_surface = temp_surface;
		
        return true;
	}
	else
        return false;
}

void CSprite::applyTransparency()
{
	Uint8 *pixel;
	Uint8 *maskpx;
	Uint32 colour, mask;
	Uint8 r,g,b,a;
	
	if(!m_surface) return;
	
	if(SDL_MUSTLOCK(m_surface)) SDL_LockSurface(m_surface);
	if(SDL_MUSTLOCK(m_masksurface)) SDL_LockSurface(m_masksurface);
	
	pixel = (Uint8*)m_surface->pixels;
	maskpx = (Uint8*)m_masksurface->pixels;
	
	for( Uint8 y=0 ; y<m_ysize ; y++ )
	{
		for( Uint8 x=0 ; x<m_xsize ; x++ )
		{
			memcpy( &colour, pixel, m_surface->format->BytesPerPixel );
			memcpy( &mask, pixel, m_surface->format->BytesPerPixel );
			
			SDL_GetRGBA( colour, m_surface->format, &r, &g, &b, &a );
			
			if(*maskpx<16)
				a = (255*(*maskpx))/15;
			else a = 255;
			
			colour = SDL_MapRGBA( m_surface->format, r, g, b, a );
			
			memcpy( pixel, &colour, m_surface->format->BytesPerPixel );
			
			pixel += m_surface->format->BytesPerPixel;
			maskpx += m_masksurface->format->BytesPerPixel;
		}
	}
	if(SDL_MUSTLOCK(m_masksurface)) SDL_LockSurface(m_masksurface);
	if(SDL_MUSTLOCK(m_surface)) SDL_LockSurface(m_surface);
	
}

///
// Getters and Setters
///
void CSprite::setBouncingBoxCoordinates( Uint16 bboxx1, Uint16 bboxy1, Uint16 bboxx2, Uint16 bboxy2 )
{
	m_bboxX1 = bboxx1;
	m_bboxY1 = bboxy1;
	m_bboxX2 = bboxx2;
	m_bboxY2 = bboxy2;
}

void CSprite::copy( CSprite &Destination, SDL_Color *Palette )
{
	Destination.m_bboxX1 = m_bboxX1;
	Destination.m_bboxY1 = m_bboxY1;
	Destination.m_bboxX2 = m_bboxX2;
	Destination.m_bboxY2 = m_bboxY2;
	Destination.setSize(m_xsize, m_ysize);
	
	Destination.createSurface( m_surface->flags, Palette );
	
	SDL_FillRect(Destination.getSDLSurface(), NULL, COLORKEY);
	SDL_BlitSurface( m_surface, NULL, Destination.getSDLSurface(), NULL);
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
// NOTE: This only can be used, when the surface is at 8-bit colours
// and palettized. Don't use it, after it has been optimized
void CSprite::replaceSpriteColor( Uint16 find, Uint16 replace, Uint16 miny )
{
	Uint16 x,y;
	Uint8* pixel;
	
	if(SDL_MUSTLOCK(m_surface)) SDL_LockSurface(m_surface);
	pixel = (Uint8*) m_surface->pixels;
	for(y=miny;y<m_ysize;y++)
	{
		for(x=0;x<m_xsize;x++)
		{
			if (pixel[y*m_xsize + x] ==find)
				pixel[y*m_xsize + x] = replace;
		}
	}
	if(SDL_MUSTLOCK(m_surface)) SDL_UnlockSurface(m_surface);
}

///
// Drawing Routines
///
void CSprite::drawSprite( SDL_Surface *dst, Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;
	
	src_rect.x = 0;	src_rect.y = 0;
	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;
	
	SDL_BlitSurface( m_surface, &src_rect, dst, &dst_rect );
}

void CSprite::freeSurfaces()
{
	SAFE_DELETE(m_surface);
	SAFE_DELETE(m_masksurface);
}

CSprite::~CSprite() {
	freeSurfaces();
}

