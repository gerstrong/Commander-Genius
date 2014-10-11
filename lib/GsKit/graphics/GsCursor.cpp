/*
 * GsCursor.cpp
 *
 *  Created on: 11.04.2010
 *      Author: gerstrong
 *
 *  This class is used for the surface which hold some icons
 *  for the cursor. By now it only draws the complete twirl,
 *  but more elements are planned.
 */

#include "GsCursor.h"

#include <base/video/CVideoDriver.h>
#include <cstring>
#include <cstdio>

GsCursor::GsCursor(SDL_Surface *p_screen)
{
//#if SDL_VERSION_ATLEAST(2, 0, 0)

//#else
    mp_Surface = gVideoDriver.convertThroughBlitSfc(p_screen);
	SDL_FillRect(mp_Surface ,NULL, 0x0);
//#endif
}

void GsCursor::generateTwirls(GsFont &Font)
{
	// The positions from the fonts are from 9 to 14
	// You see, just six direction
	// What to do is to copy the first five twirls to the new row of the font tile
	// Copy the some inverted twirls and then put the last one at the end.
	// By that way the font tile will get smoother
	SDL_Rect twrect, fmrect;
	//SDL_Surface *srcsfc = Font.getSDLColouredSurface();
	SDL_Surface *srcsfc = Font.getSDLSurface();

	// Copy the first 5 tiles
	twrect.x=9*8;
	twrect.y = fmrect.x = 0;
	twrect.w = fmrect.w = 5*8;
	twrect.h = fmrect.h = 8;	fmrect.y=0;
    BlitSurface(srcsfc, &twrect, mp_Surface, &fmrect);

	// now the complex stuff for the extra two tiles
	// Draw tile 9 and 10 inverted
	SDL_LockSurface(mp_Surface);

	Uint8 *src, *dst;
	src = dst = (Uint8*) mp_Surface->pixels;

	// for twirl 6 (LB down)
	src += (1*8 + 7*128)*mp_Surface->format->BytesPerPixel;
	dst += (5*8)*mp_Surface->format->BytesPerPixel;
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8*mp_Surface->format->BytesPerPixel);
		src -= 8*16*mp_Surface->format->BytesPerPixel;
		dst += 8*16*mp_Surface->format->BytesPerPixel;
	}
	src = dst = (Uint8*) mp_Surface->pixels;

	// for twirl 7 (LB down left)
	src += (0*8 + 7*128)*mp_Surface->format->BytesPerPixel;
	dst += (6*8)*mp_Surface->format->BytesPerPixel;
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8*mp_Surface->format->BytesPerPixel);
		src -= 8*16*mp_Surface->format->BytesPerPixel;
		dst += 8*16*mp_Surface->format->BytesPerPixel;
	}
	SDL_UnlockSurface(mp_Surface);

	// Now copy the last twirl (8) taking the original 6th one
	twrect.x=14*8;	twrect.y=0;
	twrect.w = fmrect.w = twrect.h = fmrect.h = 8;
	fmrect.x = 7*8;	fmrect.y = 0;
    BlitSurface(srcsfc, &twrect, mp_Surface, &fmrect);
}

void GsCursor::draw(SDL_Surface* dst, Uint8 character, Uint16 x, Uint16 y)
{
	SDL_Rect src_rect, dst_rect;

	src_rect.x = character*8;
	src_rect.y = 0;
	dst_rect.x = x;
	dst_rect.y = y;
	dst_rect.w = src_rect.w = 8;
	dst_rect.h = src_rect.h = 8;

    BlitSurface(mp_Surface, &src_rect, dst, &dst_rect);
}

GsCursor::~GsCursor()
{
	SDL_FreeSurface(mp_Surface);
}
