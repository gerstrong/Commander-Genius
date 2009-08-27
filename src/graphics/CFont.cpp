/*
 * CFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CFont.h"
#include <string.h>

#define COLORKEY 255

CFont::CFont() {
	FontSurface = NULL;
}

CFont::~CFont() {
	if(FontSurface) delete FontSurface;
}

bool CFont::CreateSurface(SDL_Color *Palette, Uint32 Flags)
{
	FontSurface = SDL_CreateRGBSurface(Flags, 128, 256, 8, 0, 0, 0, 0);
	SDL_SetColors(FontSurface, Palette, 0, 255);
	SDL_SetColorKey(FontSurface, SDL_SRCCOLORKEY, COLORKEY);

	return (FontSurface != NULL);
}

SDL_Surface *CFont::getSDLSurface()
{
	return FontSurface;
}

///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////

void CFont::setColorPalette(SDL_Color *Palette)
{
	SDL_SetColors(FontSurface, Palette, 0, 255);
}

void CFont::generateGlowFonts()
{
	SDL_Rect srcrect, fmrect;

	// Copy the first 5 tiles
	srcrect.x = fmrect.x = 0;
	srcrect.y = 16;
	srcrect.w = fmrect.w = 128;
	srcrect.h = fmrect.h = 8*6;
	fmrect.y = 136;
	SDL_BlitSurface(FontSurface, &srcrect, FontSurface, &fmrect);

	// And this code makes the letter create blue edges
	SDL_LockSurface(FontSurface);
	Uint8 *pixel = (Uint8*) FontSurface->pixels + 136*128;
	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			if( *pixel == 15 && ( *(pixel+1) == 16 || *(pixel-1) == 16 ||
								  *(pixel+128) == 16 || (*(pixel-128) == 16 && y>0)) )
				memset(pixel,9,1);
			pixel++;
		}
	}
	SDL_UnlockSurface(FontSurface);
}

void CFont::generateSpecialTwirls()
{
	// The positions from the fonts are from 9 to 14
	// You see, just six direction
	// What to do is to copy the first five twirls to the new row of the font tile
	// Copy the some inverted twirls and then put the last one at the end.
	// By that way the font tile will get smoother
	SDL_Rect twrect, fmrect;

	// Copy the first 5 tiles
	twrect.x=9*8;
	twrect.y = fmrect.x = 0;
	twrect.w=5*8;
	twrect.h=8;
	fmrect.y=128;
	fmrect.w=5*8;
	fmrect.h=8;
	SDL_BlitSurface(FontSurface, &twrect, FontSurface, &fmrect);

	// now the complex stuff for the extra two tiles
	// Draw tile 9 and 10 inverted
	SDL_LockSurface(FontSurface);

	Uint8* src;
	Uint8* dst;
	src = dst = (Uint8*) FontSurface->pixels;

	// for twirl 6 (LB down)
	//src += (8*10 + 8*128);
	src += (8*10 + 7*128);
	dst += (128*128 + 5*8);
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8);
		src -= 8*16;
		dst += 8*16;
	}
	src = dst = (Uint8*) FontSurface->pixels;

	// for twirl 7 (LB down left)
	src += (8*9 + 7*128);
	dst += (128*128 + 6*8);
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8);
		src -= 8*16;
		dst += 8*16;
	}
	SDL_UnlockSurface(FontSurface);

	// Now copy the last twirl (8) taking the original 6th one
	twrect.x=14*8;
	twrect.y=0;
	twrect.w = fmrect.w = twrect.h = fmrect.h = 8 ;
	fmrect.x=7*8;
	fmrect.y=128;
	SDL_BlitSurface(FontSurface, &twrect, FontSurface, &fmrect);
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////

void CFont::drawTwirl(SDL_Surface* dst, int twirlframe, Uint16 x, Uint16 y)
{
	SDL_Rect twrect;
	SDL_Rect fmrect;

	twrect.x = twirlframe*8;
	twrect.y = 128;
	fmrect.x = x;
	fmrect.y = y;
	fmrect.w = fmrect.h = twrect.w = twrect.h = 8;

	SDL_BlitSurface(FontSurface, &twrect, dst, &fmrect);
	//SDL_BlitSurface(FontSurface, NULL, dst, NULL);
}

void CFont::drawCharacter(SDL_Surface* dst, Uint16 character)
{}
