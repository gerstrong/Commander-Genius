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
#include "../sdl/CVideoDriver.h"


// TODO: We need to add documentation. I'll do that very soon!

CFont::CFont() :
m_FontSurface(NULL),
m_ColouredSurface(NULL),
m_fgcolour(0x0),
m_bgcolour(0x0),
m_monochrome(false)
{
	//m_widthtable.assign(256,8);
	memset(&m_widthtable, 8, 256);
}

bool CFont::CreateSurface(SDL_Color *Palette, Uint32 Flags, Uint8 bpp, Uint16 width, Uint16 height)
{
	if(m_FontSurface) SDL_FreeSurface(m_FontSurface);
	if(m_ColouredSurface) SDL_FreeSurface(m_ColouredSurface);
	m_FontSurface = SDL_CreateRGBSurface(Flags, width, height, bpp, 0, 0, 0, 0);

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
	if(!m_monochrome)
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
	}
	return true;
}

///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////

bool CFont::loadHiColourFont( const std::string& filename )
{
	if(!IsFileAvailable(filename))
		return false;

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

// This sets the width of the characters so the text is printed nicely.
// This is by default 8 pixels in vorticons and it is normally only used
// in the galaxy engine.
void CFont::setWidthToCharacter(Uint8 width, Uint16 letter)
{
	m_widthtable[letter] = width;
}

/**
 * \brief	This function defines, that we have a monochrome fontmap
 * 			Vorticon engine uses a 8x8 tilemap for printing the fonts.
 * 			As it has other characters, it can never be monochrome
 * 			The Galaxy engine uses monochrome because it derives the other
 * 			colours by tricking around with palette in our case...
 * \param	value	set true if the fontmap is monochrome, else false
 */
void CFont::setMonochrome(bool value)
{	m_monochrome = value;	}

void CFont::setBGColour(SDL_PixelFormat* p_pixelformat, Uint32 bgcolour, bool force)
{
	if(m_monochrome)
	{
		SDL_Color *p_Color = m_FontSurface->format->palette->colors;

		if( m_bgcolour != bgcolour || force ) // For the Background
			SDL_GetRGB(bgcolour, p_pixelformat, &p_Color[0].r, &p_Color[0].g, &p_Color[0].b);
	}
}

// Sets the colour used for printing the text
void CFont::setFGColour(SDL_PixelFormat* p_pixelformat, Uint32 fgcolour, bool force)
{
	if(m_monochrome)
	{
		SDL_Color *p_Color = m_FontSurface->format->palette->colors;

		if( m_fgcolour != fgcolour || force )	// For the Foreground
		{
			SDL_GetRGB(fgcolour, p_pixelformat, &p_Color[15].r, &p_Color[15].g, &p_Color[15].b);
			SDL_SetColors( m_FontSurface, p_Color, 0, 16);
			m_fgcolour = fgcolour;
		}
	}
	else
	{
		if( m_fgcolour != fgcolour || force )
		{
			m_fgcolour = fgcolour;
			SDL_FillRect(m_ColouredSurface, NULL, m_fgcolour);
			SDL_BlitSurface(m_FontSurface, NULL, m_ColouredSurface, NULL);
		}
	}
}

//
Uint32 CFont::getColour()
{
	return m_fgcolour;
}

/**
 * \brief Retrieves the background colour of the font map
 * \param highlighted The fontmap has two background colours.
 * 		  			  One is highlighted (default gray)
 * 					  The other is not (default white)
 * 					  Those colours normally differ in mods.
 */

Uint32 CFont::getBGColour(bool highlighted)
{
	const int height = highlighted ? 10 : 2;

	SDL_LockSurface(m_ColouredSurface);

	Uint8* pixel = (Uint8*) m_ColouredSurface->pixels + height*8*m_ColouredSurface->pitch;
	Uint32 pixel32;

	memcpy(&pixel32, pixel, m_ColouredSurface->format->BytesPerPixel);

	Uint8 r,g,b;
	SDL_GetRGB(pixel32, m_ColouredSurface->format, &r, &g, &b);

	SDL_UnlockSurface(m_ColouredSurface);

	return SDL_MapRGB(m_ColouredSurface->format, r, g, b);
}

void CFont::getBGColour(bool highlighted, Uint8 *r, Uint8 *g, Uint8 *b)
{
	Uint32 colour = getBGColour(false);
	SDL_GetRGB(colour, m_ColouredSurface->format, r, g, b);
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////
void CFont::drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff)
{
	SDL_Rect scrrect, dstrect;
	
	scrrect.x = (m_ColouredSurface->w/16)*(character%16);
	scrrect.y = (m_ColouredSurface->h/16)*(character/16);
	scrrect.w = dstrect.w = (m_widthtable[character]);
	scrrect.h = dstrect.h = (m_ColouredSurface->h/16);
	dstrect.x = xoff;	dstrect.y = yoff;
	
	if(m_monochrome)
		SDL_BlitSurface(m_FontSurface, &scrrect, dst, &dstrect);
	else
		SDL_BlitSurface(m_ColouredSurface, &scrrect, dst, &dstrect);
}

void CFont::drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, bool highlight)
{
	unsigned int i,x=xoff,y=yoff;

	if(text.size() != 0)
	{
		for(i=0;i<text.size();i++)
		{
			unsigned char c = text[i];

			if ( !endofText( text.substr(i) ) )
			{
				if(highlight) c |= 128;

				drawCharacter(dst, c, x, y);

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

void CFont::drawMap(SDL_Surface* dst)
{
	SDL_BlitSurface(m_FontSurface,NULL,dst,NULL);
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

