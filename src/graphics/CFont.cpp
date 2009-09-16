/*
 * CFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CFont.h"
#include "CPalette.h"
#include <string.h>

CFont::CFont() {
	m_FontSurface = NULL;
}

CFont::~CFont() {
	if(m_FontSurface) delete m_FontSurface;
}

bool CFont::CreateSurface(SDL_Color *Palette, Uint32 Flags)
{
	if(m_FontSurface) SDL_FreeSurface(m_FontSurface);
	m_FontSurface = SDL_CreateRGBSurface(Flags, 128, 512, 8, 0, 0, 0, 0);
	SDL_SetColors(m_FontSurface, Palette, 0, 255);
	SDL_SetColorKey(m_FontSurface, SDL_SRCCOLORKEY, COLORKEY);
	return (m_FontSurface != NULL);
}

bool CFont::optimizeSurface()
{
	if(m_FontSurface)
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormat(m_FontSurface);
		SDL_FreeSurface(m_FontSurface);
		m_FontSurface = temp_surface;
	}
	return true;
}

SDL_Surface *CFont::getSDLSurface()
{
	return m_FontSurface;
}

///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////

// Used for the selected text
void CFont::generateGlowFonts()
{
	SDL_Rect srcrect, fmrect;

	// Copy the first 5 tiles
	srcrect.x = fmrect.x = 0;
	srcrect.y = 16; 	fmrect.y = 136;
	srcrect.w = fmrect.w = 128;
	srcrect.h = fmrect.h = 8*6;
	SDL_BlitSurface(m_FontSurface, &srcrect, m_FontSurface, &fmrect);

	// And this code makes the letter create blue edges
	SDL_LockSurface(m_FontSurface);

	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + 136*128;
	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			if( *pixel != 15 ) memset(pixel,1,1);
			pixel++;
		}
	}
	SDL_UnlockSurface(m_FontSurface);
}

// Used for scrolling text in the Credits section
void CFont::generateInverseFonts()
{
	SDL_Rect srcrect, fmrect;

	// Copy the first 5 tiles
	srcrect.x = fmrect.x = 0;
	srcrect.y = 16; fmrect.y = 184;
	srcrect.w = fmrect.w = 128;
	srcrect.h = fmrect.h = 8*6;
	SDL_BlitSurface(m_FontSurface, &srcrect, m_FontSurface, &fmrect);

	// And this code makes the letter create blue edges
	SDL_LockSurface(m_FontSurface);

	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + fmrect.y*128;
	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			if( *pixel == 15 ) memset(pixel,0,1);
			else memset(pixel,11,1);
			pixel++;
		}
	}
	SDL_UnlockSurface(m_FontSurface);
}

// Those are light grey and normally used in the main menu indicating disabled items
void CFont::generateDisabledFonts()
{
	SDL_Rect srcrect, fmrect;

	// Copy the first 5 tiles
	srcrect.x = fmrect.x = 0;
	srcrect.y = 16; fmrect.y = 232;
	srcrect.w = fmrect.w = 128;
	srcrect.h = fmrect.h = 8*6;
	SDL_BlitSurface(m_FontSurface, &srcrect, m_FontSurface, &fmrect);

	// And this code makes the letter create blue edges
	SDL_LockSurface(m_FontSurface);

	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + fmrect.y*128;
	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			if( *pixel == 15 ) memset(pixel,15,1);
			else memset(pixel,7,1);
			pixel++;
		}
	}
	SDL_UnlockSurface(m_FontSurface);
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
	twrect.w = fmrect.w = 5*8;
	twrect.h = fmrect.h = 8;	fmrect.y=128;
	SDL_BlitSurface(m_FontSurface, &twrect, m_FontSurface, &fmrect);

	// now the complex stuff for the extra two tiles
	// Draw tile 9 and 10 inverted
	SDL_LockSurface(m_FontSurface);

	Uint8 *src, *dst;
	src = dst = (Uint8*) m_FontSurface->pixels;

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
	src = dst = (Uint8*) m_FontSurface->pixels;

	// for twirl 7 (LB down left)
	src += (8*9 + 7*128);
	dst += (128*128 + 6*8);
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8);
		src -= 8*16;
		dst += 8*16;
	}
	SDL_UnlockSurface(m_FontSurface);

	// Now copy the last twirl (8) taking the original 6th one
	twrect.x=14*8;	twrect.y=0;
	twrect.w = fmrect.w = twrect.h = fmrect.h = 8 ;
	fmrect.x=7*8;	fmrect.y=128;
	SDL_BlitSurface(m_FontSurface, &twrect, m_FontSurface, &fmrect);
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////
void CFont::drawTwirl(SDL_Surface* dst, int twirlframe, Uint16 x, Uint16 y)
{
	SDL_Rect twrect, fmrect;

	twrect.x = twirlframe*8;	twrect.y = 128;
	fmrect.x = x;	fmrect.y = y;
	fmrect.w = fmrect.h = twrect.w = twrect.h = 8;

	SDL_BlitSurface(m_FontSurface, &twrect, dst, &fmrect);
}

void CFont::drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff)
{
	SDL_Rect scrrect, dstrect;

	scrrect.x = 8*(character%16);	scrrect.y = 8*(character/16);
	scrrect.w = dstrect.w = 8;	scrrect.h = dstrect.h = 8;
	dstrect.x = xoff;	dstrect.y = yoff;

	SDL_BlitSurface(m_FontSurface, &scrrect, dst, &dstrect);
}

void CFont::drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, Uint8 lettertype)
{
	unsigned int i,x=xoff,y=yoff;

	for(i=0;i<text.size();i++)
	{
	    unsigned char c = text[i];
	    if (c!=13)
	    {
	    	if(lettertype != LETTER_TYPE_NORMAL) c |= 128;
	    	if(lettertype == LETTER_TYPE_BLUE)
	    		drawCharacter(dst, ((Uint16) c) + 7*16, (int)x, (int)y);
	    	else if(lettertype == LETTER_TYPE_INVERSE)
	    		drawCharacter(dst, ((Uint16) c) + 13*16, (int)x, (int)y);
	    	else if(lettertype == LETTER_TYPE_DISABLED)
	    		drawCharacter(dst, ((Uint16) c) + 19*16, (int)x, (int)y);
	    	else
	    		drawCharacter(dst, c, (int)x, (int)y);

	    	x+=8;
	    }
	    else
	    {
	       x=xoff;
	       y+=8;
	    }
	}
}
