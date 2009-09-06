/*
 * CSprite.cpp
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#include "CSprite.h"
#include "CPalette.h"

CSprite::CSprite() {
	m_xsize = m_ysize = 0;
	m_bboxX1 = m_bboxY1 = 0;
	m_bboxX2 = m_bboxY2 = 0;
	m_surface = NULL;
}

CSprite::~CSprite() {
	if(m_surface) SDL_FreeSurface(m_surface);	// TODO Auto-generated destructor stub
}

/////////////////////////////
// Initialization Routines //
/////////////////////////////
bool CSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{
	m_surface = SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
	SDL_SetColors( m_surface, Palette, 0, 255);
	SDL_SetColorKey( m_surface, SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
	return ( m_surface != NULL );
}

bool CSprite::optimizeSurface()
{
	if(m_surface)
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormat(m_surface);
		SDL_FreeSurface(m_surface);
		m_surface = temp_surface;
	}
	return true;
}

///
// Getters and Setters
///
void CSprite::setSize(Uint8 w, Uint8 h)
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

void CSprite::copy( CSprite *Destination, SDL_Color *Palette )
{
	Destination->m_bboxX1 = m_bboxX1;
	Destination->m_bboxY1 = m_bboxY1;
	Destination->m_bboxX2 = m_bboxX2;
	Destination->m_bboxY2 = m_bboxY2;
	Destination->setSize(m_xsize, m_ysize);

	Destination->createSurface( m_surface->flags, Palette );

	SDL_FillRect(Destination->getSDLSurface(), NULL, COLORKEY);
	SDL_BlitSurface( m_surface, NULL, Destination->getSDLSurface(), NULL);
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
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
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;

	SDL_BlitSurface( m_surface, &src_rect, dst, &dst_rect );
}

void CSprite::eraseSprite( SDL_Surface *dst,  Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	SDL_FillRect(dst, &dst_rect, dst->format->colorkey);
}
