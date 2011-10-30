/*
 * CSprite.cpp
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#include "CSprite.h"
#include "CPalette.h"
#include "../FindFile.h"
#include <string.h>
#include "../sdl/CVideoDriver.h"

#define SAFE_DELETE(x) { if(x) SDL_FreeSurface(x); x = NULL; }

CSprite::CSprite() :
m_surface(NULL),
m_masksurface(NULL),
m_alpha(255)
{
	m_xsize = m_ysize = 0;
	m_bboxX1 = m_bboxY1 = 0;
	m_bboxX2 = m_bboxY2 = 0;
	m_xoffset = m_yoffset = 0;
}

/////////////////////////////
// Initialization Routines //
/////////////////////////////

bool CSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{
	if(m_surface) SDL_FreeSurface(m_surface);
	m_surface = SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
	SDL_SetColors( m_surface, Palette, 0, 255);
	SDL_SetColorKey( m_surface, SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
	if(m_masksurface) SDL_FreeSurface(m_masksurface);
	m_masksurface = SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
	SDL_SetColors( m_masksurface, Palette, 0, 255);
	SDL_SetColorKey( m_masksurface, SDL_SRCCOLORKEY, COLORKEY ); // color key.
	
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

bool CSprite::loadHQSprite( const std::string& filename )
{
	if(m_surface)
	{
		SDL_Surface *temp_surface = SDL_LoadBMP(GetFullFileName(filename).c_str());
		if(temp_surface)
		{
			SDL_Surface *displaysurface = SDL_ConvertSurface(temp_surface, m_surface->format, m_surface->flags);
			readMask(displaysurface);
			readBBox(displaysurface);
			SDL_BlitSurface(displaysurface, NULL, m_surface, NULL);
			SDL_FreeSurface(displaysurface);
			SDL_FreeSurface(temp_surface);
			return true;
		}
	}
	return false;
}

/**
 * \brief Reads the mask of a created modkeen style bitmap und converts that mask to 8-bit
 * 		  so it can be applied to the others. This is for HQ Sprites, the other have in internal algorithm
 */
void CSprite::readMask(SDL_Surface *displaysurface)
{
	Uint8 *maskpx, *pixel;
	Uint32 color = 0;
	Uint8 mask = 0;
	Uint8 r,g,b,a;
	Uint16 h,w;

	h = displaysurface->h;
	w = (displaysurface->w)/3;

	if(SDL_MUSTLOCK(displaysurface)) SDL_LockSurface(displaysurface);
	if(SDL_MUSTLOCK(m_masksurface)) SDL_LockSurface(m_masksurface);

	maskpx = (Uint8*)m_masksurface->pixels;
	pixel = (Uint8*)displaysurface->pixels + (displaysurface->w/3)*displaysurface->format->BytesPerPixel;

	for( Uint8 y=0 ; y<h ; y++ )
	{
		for( Uint8 x=0 ; x<w ; x++ )
		{
			memcpy( &color, pixel, displaysurface->format->BytesPerPixel );

			SDL_GetRGBA( color, displaysurface->format, &r, &g, &b, &a );

			Uint32 mask32 = (r+g+b)/(3*16);
			mask = 15-mask32;
			//mask = 15;

			memcpy( maskpx, &mask, 1 );

			pixel += m_surface->format->BytesPerPixel;
			maskpx += m_masksurface->format->BytesPerPixel;
		}
		pixel += 2*w*m_surface->format->BytesPerPixel;
	}
	if(SDL_MUSTLOCK(m_masksurface)) SDL_LockSurface(m_masksurface);
	if(SDL_MUSTLOCK(displaysurface)) SDL_LockSurface(displaysurface);
}

/**
 * \brief Reads the bounding box of a created modkeen style bitmap and assigns new coordinates
 */
void CSprite::readBBox(SDL_Surface *displaysurface)
{
	// TODO: That code need to be implemented
}

void CSprite::applyTransparency()
{
	Uint8 *pixel;
	Uint8 *maskpx;
	Uint32 colour, mask;
	Uint8 r,g,b,a;
	
	if(!m_surface) return;

	if(m_surface->format->BitsPerPixel == 8) // In case we did not call SDL_Displayformat
	{
		SDL_BlitSurface(m_masksurface, NULL, m_surface, NULL);
		return;
	}
	
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

void CSprite::applyTranslucency(Uint8 value)
{
	Uint8 *pixel;
	Uint32 colour = 0;
	Uint8 r,g,b,a;

	r = g = b = a = 0;

	if(m_alpha == value)
		return;

	if(!m_surface) return;

	if(SDL_MUSTLOCK(m_surface)) SDL_LockSurface(m_surface);

	pixel = (Uint8*)m_surface->pixels;

	for( Uint8 y=0 ; y<m_ysize ; y++ )
	{
		for( Uint8 x=0 ; x<m_xsize ; x++ )
		{
			memcpy( &colour, pixel, m_surface->format->BytesPerPixel );

			SDL_GetRGBA( colour, m_surface->format, &r, &g, &b, &a );

			if(a!=0) a = value;

			colour = SDL_MapRGBA( m_surface->format, r, g, b, a );

			memcpy( pixel, &colour, m_surface->format->BytesPerPixel );

			pixel += m_surface->format->BytesPerPixel;
		}
	}

	if(SDL_MUSTLOCK(m_surface)) SDL_LockSurface(m_surface);

	m_alpha = value;
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

void blitMaskedSprite(SDL_Surface *dst, SDL_Surface *src, Uint32 color)
{
	Uint16 x,y;
	Uint32 *srcpixel, *dstpixel;

	if(SDL_MUSTLOCK(dst)) SDL_LockSurface(dst);
	if(SDL_MUSTLOCK(src)) SDL_LockSurface(src);
	dstpixel = (Uint32*) dst->pixels;
	srcpixel = (Uint32*) src->pixels;
	for(y=0;y<dst->h;y++)
	{
		for(x=0;x<dst->w;x++)
		{
			Uint8 r,g,b,a;

			SDL_GetRGBA(*srcpixel,src->format, &r, &g, &b, &a);
			SDL_GetRGB(color, src->format, &r, &g, &b);
			*dstpixel = SDL_MapRGBA(dst->format, r, g, b, a);

			dstpixel++;
			srcpixel++;
		}
	}
	if(SDL_MUSTLOCK(src)) SDL_UnlockSurface(src);
	if(SDL_MUSTLOCK(dst)) SDL_UnlockSurface(dst);
}


///
// Drawing Routines
///
/**
 * \brief The function that blits the sprite to dst
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CSprite::drawSprite( const Uint16 x, const Uint16 y, const Uint8 alpha )
{
	g_pVideoDriver->mDrawTasks.add( new DrawSpriteTask( this, x, y, alpha ) );
}

/**
 * \brief The function that blits the sprite to dst (Low-Level)
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CSprite::_drawSprite( SDL_Surface *dst, const Uint16 x, const Uint16 y, const Uint8 alpha )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;
	
	applyTranslucency(alpha);
	SDL_BlitSurface( m_surface, &src_rect, dst, &dst_rect );
}

/**
 * \brief The function that blits the sprite to dst
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CSprite::drawBlinkingSprite( Uint16 x, Uint16 y )
{
	g_pVideoDriver->mDrawTasks.add( new DrawBlinkingSpriteTask( this, x, y ) );
}


/**
 * \brief The function that blits the sprite to dst (lowlevel)
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CSprite::_drawBlinkingSprite( SDL_Surface *dst, Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;

	SDL_Surface *blanksfc =	SDL_DisplayFormatAlpha(m_surface);
	blitMaskedSprite(blanksfc, m_surface, 0xFFFFFF);
	SDL_BlitSurface( blanksfc, &src_rect, dst, &dst_rect );
	SDL_FreeSurface(blanksfc);
}


void CSprite::freeSurfaces()
{
	if(m_surface) SDL_FreeSurface(m_surface);
	if(m_masksurface) SDL_FreeSurface(m_masksurface);
	m_masksurface = NULL;
	m_surface = NULL;
}

CSprite::~CSprite() {
	freeSurfaces();
}

