/*
 * CFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CFont.h"
#include "CPalette.h"
#include "../FindFile.h"
#include <string.h>

// TODO: That Font class need some improvements...
// let's make the fontmap smaller but give the ability, to have more than one
// and switch between them.

CFont::CFont() :
m_FontSurface(NULL),
m_ColouredSurface(NULL),
m_colour(0x0)
{
	m_widthtable.assign(256,8);
}

bool CFont::CreateSurface(SDL_Color *Palette, Uint32 Flags, Uint8 bpp)
{
	if(m_FontSurface) SDL_FreeSurface(m_FontSurface);
	if(m_ColouredSurface) SDL_FreeSurface(m_ColouredSurface);
	m_FontSurface = SDL_CreateRGBSurface(Flags, 128, 128, bpp, 0, 0, 0, 0);

	if(bpp == 8)
	{
		SDL_SetColors(m_FontSurface, Palette, 0, 255);
		SDL_SetColorKey(m_FontSurface, SDL_SRCCOLORKEY, COLORKEY);
	}

	m_ColouredSurface = SDL_DisplayFormat(m_FontSurface);

	return (m_FontSurface != NULL && m_ColouredSurface != NULL );
}

bool CFont::optimizeSurface()
{
	if(m_FontSurface)
	{
		SDL_Surface *temp_surface;
		temp_surface = SDL_DisplayFormat(m_FontSurface);
		SDL_FreeSurface(m_FontSurface);
		m_FontSurface = temp_surface;
		SDL_SetColorKey(m_FontSurface, SDL_SRCCOLORKEY, 0x0);

		temp_surface = SDL_DisplayFormat(m_ColouredSurface);
		SDL_FreeSurface(m_ColouredSurface);
		m_ColouredSurface = temp_surface;
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
		SDL_Surface *temp_surface = SDL_LoadBMP(GetFullFileName(filename).c_str());
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
//	//SDL_Rect srcrect, fmrect;
//
//	// Copy the first 5 tiles
//	//srcrect.x = fmrect.x = 0;
//	//srcrect.y = 16; 	fmrect.y = 136;
//	//srcrect.w = fmrect.w = 128;
//	//srcrect.h = fmrect.h = 8*6;
//	//SDL_BlitSurface(m_FontSurface, &srcrect, m_FontSurface, &fmrect);
//
//	// And this code makes the letter create blue edges
//	SDL_LockSurface(m_FontSurface);
//
//	int bpp = m_FontSurface->format->BytesPerPixel;
//	Uint8 *pixel = (Uint8*) m_FontSurface->pixels /*+ 136*128*bpp*/;
//
//	for(Uint8 y=0 ; y<8*6 ; y++)
//	{
//		for(Uint8 x=0 ; x<128 ; x++)
//		{
//			Uint8 red, blue, green;
//			Uint32 color;
//
//			memcpy(&color, pixel,bpp);
//			SDL_GetRGB(color, m_FontSurface->format, &red, &green, &blue);
//			if(red < 0x0F && blue < 0x0F && green < 0x0F )
//			{
//				red = 0x00;
//				green = 0x00;
//				blue = 0xFF;
//				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
//				memcpy(pixel, &color, bpp);
//			}
//			pixel+=bpp;
//		}
//	}
//	SDL_UnlockSurface(m_FontSurface);
}

// Used for scrolling text in the Credits section
void CFont::generateInverseFonts()
{
//	SDL_Rect srcrect, fmrect;
//
//	// Copy the first 5 tiles
//	srcrect.x = fmrect.x = 0;
//	srcrect.y = 16; fmrect.y = 184;
//	srcrect.w = fmrect.w = 128;
//	srcrect.h = fmrect.h = 8*6;
//	SDL_BlitSurface(m_FontSurface, &srcrect, m_FontSurface, &fmrect);
//
//	// And this code makes the letter create blue edges
//	SDL_LockSurface(m_FontSurface);
//
//	int bpp = m_FontSurface->format->BytesPerPixel;
//	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + fmrect.y*128*bpp;
//
//	for(Uint8 y=0 ; y<8*6 ; y++)
//	{
//		for(Uint8 x=0 ; x<128 ; x++)
//		{
//			Uint8 red, blue, green;
//			Uint32 color;
//
//			memcpy(&color, pixel,bpp);
//			SDL_GetRGB(color, m_FontSurface->format, &red, &green, &blue);
//			if(red < 0x0F && blue < 0x0F && green < 0x0F )
//			{
//				red = 0x00;
//				green = 0x80;
//				blue = 0xFF;
//				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
//				memcpy(pixel, &color, bpp);
//			}
//			else
//			{
//				red = 0x00;
//				green = 0x00;
//				blue = 0x00;
//				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
//				memcpy(pixel, &color, bpp);
//			}
//			pixel+=bpp;
//		}
//	}
//	SDL_UnlockSurface(m_FontSurface);
}

// Those are light grey and normally used in the main menu indicating disabled items
void CFont::generateDisabledFonts()
{
//	SDL_Rect srcrect, fmrect;
//
//	// Copy the first 5 tiles
//	srcrect.x = fmrect.x = 0;
//	srcrect.y = 16; fmrect.y = 232;
//	srcrect.w = fmrect.w = 128;
//	srcrect.h = fmrect.h = 8*6;
//	SDL_BlitSurface(m_FontSurface, &srcrect, m_FontSurface, &fmrect);
//
//	// And this code makes the letter create blue edges
//	SDL_LockSurface(m_FontSurface);
//
//	int bpp = m_FontSurface->format->BytesPerPixel;
//	Uint8 *pixel = (Uint8*) m_FontSurface->pixels + fmrect.y*128*bpp;
//
//	for(Uint8 y=0 ; y<8*6 ; y++)
//	{
//		for(Uint8 x=0 ; x<128 ; x++)
//		{
//			Uint8 red, blue, green;
//			Uint32 color = 0;
//
//			memcpy(&color, pixel,bpp);
//			SDL_GetRGB(color, m_FontSurface->format, &red, &green, &blue);
//			if(red < 0x0F && blue < 0x0F && green < 0x0F )
//			{
//				red = 0x80;
//				green = 0x80;
//				blue = 0x80;
//				color = SDL_MapRGB(m_FontSurface->format, red, green, blue);
//				memcpy(pixel, &color, bpp);
//			}
//			pixel+=bpp;
//		}
//	}
//	SDL_UnlockSurface(m_FontSurface);
}

// This sets the width of the characters so the text is printed nicely.
// This is by default 8 pixels in vorticons and it is normally only used
// in the galaxy engine.
void CFont::setWidthToCharacter(Uint8 width, Uint16 letter)
{
	m_widthtable.at(letter) = width;
}

// Sets the colour used for printing the text
void CFont::setColour(Uint32 colour, bool force)
{
	if( m_colour != colour || force )
	{
		m_colour = colour;
		SDL_FillRect(m_ColouredSurface, NULL, colour);
		SDL_BlitSurface(m_FontSurface, NULL, m_ColouredSurface, NULL);
	}
}

//
Uint32 CFont::getColour()
{
	return m_colour;
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////
void CFont::drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff, Uint32 colour)
{
	SDL_Rect scrrect, dstrect;
	
	scrrect.x = 8*(character%16);	scrrect.y = 8*(character/16);
	scrrect.w = dstrect.w = 8;	scrrect.h = dstrect.h = 8;
	dstrect.x = xoff;	dstrect.y = yoff;
	
	SDL_BlitSurface(m_ColouredSurface, &scrrect, dst, &dstrect);
}

void CFont::drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, bool highlight, Uint32 colour)
{
	unsigned int i,x=xoff,y=yoff;

	if(text.size() != 0)
	{
		for(i=0;i<text.size();i++)
		{
			unsigned char c = text[i];
			if (c!=13)
			{
				if(highlight) c |= 128;

				drawCharacter(dst, c, x, y, colour);

				x+=m_widthtable[c];
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
void CFont::destroySurface(){
	if(m_FontSurface) SDL_FreeSurface(m_FontSurface);
	if(m_ColouredSurface) SDL_FreeSurface(m_ColouredSurface);
	m_ColouredSurface = m_FontSurface = NULL;
}

CFont::~CFont() {
}

