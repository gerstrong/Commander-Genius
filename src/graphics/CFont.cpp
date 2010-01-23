/*
 * CFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CFont.h"
#include "CPalette.h"
#include <string.h>

CFont::CFont() :
m_FontSurface(NULL)
{}

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

///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////

bool CFont::loadHiColourFont( const std::string& filename )
{
	if(m_FontSurface)
	{
		SDL_Surface *temp_surface = SDL_LoadBMP(filename.c_str());
		if(temp_surface)
		{
			SDL_Surface *displaysurface = SDL_ConvertSurface(temp_surface, m_FontSurface->format, m_FontSurface->flags);
			SDL_BlitSurface(displaysurface, NULL, m_FontSurface, NULL);
			SDL_FreeSurface(displaysurface);
			SDL_FreeSurface(temp_surface);
			return true;
		}
	}
	return false;
}

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
	
	int bpp = m_FontSurface->format->BytesPerPixel;
	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + 136*128*bpp;

	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			Uint8 red, blue, green;
			Uint32 color;

			memcpy(&color, pixel,bpp);
			SDL_GetRGB(color, m_FontSurface->format, &red, &green, &blue);
			if(red < 0x0F && blue < 0x0F && green < 0x0F )
			{
				red = 0x00;
				green = 0x00;
				blue = 0xFF;
				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
				memcpy(pixel, &color, bpp);
			}
			pixel+=bpp;
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
	
	int bpp = m_FontSurface->format->BytesPerPixel;
	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + fmrect.y*128*bpp;

	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			Uint8 red, blue, green;
			Uint32 color;

			memcpy(&color, pixel,bpp);
			SDL_GetRGB(color, m_FontSurface->format, &red, &green, &blue);
			if(red < 0x0F && blue < 0x0F && green < 0x0F )
			{
				red = 0x00;
				green = 0x80;
				blue = 0xFF;
				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
				memcpy(pixel, &color, bpp);
			}
			else
			{
				red = 0x00;
				green = 0x00;
				blue = 0x00;
				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
				memcpy(pixel, &color, bpp);
			}
			pixel+=bpp;
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

	int bpp = m_FontSurface->format->BytesPerPixel;
	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + fmrect.y*128*bpp;

	for(Uint8 y=0 ; y<8*6 ; y++)
	{
		for(Uint8 x=0 ; x<128 ; x++)
		{
			Uint8 red, blue, green;
			Uint32 color = 0;

			memcpy(&color, pixel,bpp);
			SDL_GetRGB(color, m_FontSurface->format, &red, &green, &blue);
			if(red < 0x0F && blue < 0x0F && green < 0x0F )
			{
				red = 0x80;
				green = 0x80;
				blue = 0x80;
				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
				memcpy(pixel, &color, bpp);
			}
			pixel+=bpp;
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
	src += (8*10 + 7*128)*m_FontSurface->format->BytesPerPixel;
	dst += (128*128 + 5*8)*m_FontSurface->format->BytesPerPixel;
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8*m_FontSurface->format->BytesPerPixel);
		src -= 8*16*m_FontSurface->format->BytesPerPixel;
		dst += 8*16*m_FontSurface->format->BytesPerPixel;
	}
	src = dst = (Uint8*) m_FontSurface->pixels;
	
	// for twirl 7 (LB down left)
	src += (8*9 + 7*128)*m_FontSurface->format->BytesPerPixel;
	dst += (128*128 + 6*8)*m_FontSurface->format->BytesPerPixel;
	for(Uint16 i=0 ; i<8 ; i++ ) // Draw the inverse
	{
		memcpy(dst,src,8*m_FontSurface->format->BytesPerPixel);
		src -= 8*16*m_FontSurface->format->BytesPerPixel;
		dst += 8*16*m_FontSurface->format->BytesPerPixel;
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
	
	if(text.size() != 0)
	{
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
}

///
// Destruction Routines
///
void CFont::DestroySurface(){
	if(m_FontSurface) SDL_FreeSurface(m_FontSurface);
	m_FontSurface = NULL;
}

CFont::~CFont() {
}

