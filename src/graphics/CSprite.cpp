/*
 * CSprite.cpp
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#include "CSprite.h"

CSprite::CSprite() {
	m_xsize = m_ysize = 0;
	m_bboxX1 = m_bboxY1 = 0;
	m_bboxX2 = m_bboxY2 = 0;
	m_surface = NULL;
}

CSprite::~CSprite() {
	if(m_surface) SDL_FreeSurface(m_surface);	// TODO Auto-generated destructor stub
	m_surface = NULL;
}

///
// Initialization Routines
///
bool CSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{
	//if(m_surface) SDL_FreeSurface(m_surface);
	m_surface = SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
	SDL_SetColors( m_surface, Palette, 0, 255);
	SDL_SetColorKey( m_surface, SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
	SDL_SetPalette( m_surface, flags, Palette, 0, COLORKEY );
	return ( m_surface != NULL );
}

void CSprite::setSize(Uint8 w, Uint16 h)
{
	m_xsize = w; m_ysize = h;
}

void CSprite::setBouncingBoxCoordinates( Uint16 bboxx1, Uint16 bboxy1, Uint16 bboxx2, Uint16 bboxy2 )
{
	m_bboxX1 = bboxx1;
	m_bboxY1 = bboxy1;
	m_bboxX2 = bboxx2;
	m_bboxY2 = bboxy2;
}

///
// Getters and Setters
///
SDL_Surface *CSprite::getSDLSurface()
{
	return m_surface;
}

///
// Drawing Routines
///
void CSprite::drawSprite( SDL_Surface *dst, Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	SDL_BlitSurface( m_surface, NULL, dst, &dst_rect );
}

void CSprite::eraseSprite( SDL_Surface *dst,  Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	SDL_FillRect(dst, &dst_rect, dst->format->colorkey);
}
