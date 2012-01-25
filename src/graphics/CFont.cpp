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
#include "CGFont.xpm"
#include "StringUtils.h"

// TODO: We need to add documentation. I'll do that very soon!

CFont::CFont() :
mFontSurface(NULL)
{
	memset(&mWidthtable, 8, 256);
}


///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////

bool CFont::CreateSurface(SDL_Color *Palette, Uint32 Flags,
							Uint8 bpp, Uint16 width, Uint16 height)
{
	mFontSurface = SDL_CreateRGBSurface(Flags, width,
			height, bpp, 0, 0, 0, 0);
	SDL_SetColors(mFontSurface, Palette, 0, 255);
	SDL_SetColorKey(mFontSurface, SDL_SRCCOLORKEY, COLORKEY);

	return ( mFontSurface != NULL );
}

SDL_Surface *loadfromXPMData(const char **data, const SDL_PixelFormat *format, const Uint32 flags)
{
	int width, height, colors;

	// Read the dimensions and amount of colors
	sscanf(data[0], "%d %d %d", &width, &height, &colors);

	// Create the surface
	SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height,
			  	  	  	  	  	  	  	  	 format->BitsPerPixel,
			  	  	  	  	  	  	  	  	 format->Rmask, format->Gmask,
			  	  	  	  	  	  	  	  	 format->Bmask, format->Amask );

	// read the data and pass it to the surface
	SDL_LockSurface(sfc);

	std::string textbuf;

	Uint32 *pixel = static_cast<Uint32*>(sfc->pixels);
	for( int y = 0 ; y < height ; y++)
	{
		char *pixel_data = const_cast<char*>(data[colors+1+y]);

		for( int x = 0 ; x < width ; x++)
		{
			char new_pix = pixel_data[x];

			// Now get the new pixel
			for( int c = 0 ; c < colors ; c++)
			{
				// Found the entry, get the color
				if( new_pix == *(data[1+c]) )
				{
					textbuf = data[1+c]+4;
					break;
				}
			}

			if( textbuf == "None" )
			{
				// no color, make this one transparent
				*pixel = 0;
			}
			else
			{
				// Get the hexstring.
				textbuf = textbuf.substr(1);

				// And convert it
				*pixel = (from_string<Uint32>(textbuf)) | 0xFF000000;
			}

			pixel++;
		}
	}

	SDL_UnlockSurface(sfc);

	return sfc;
}


bool CFont::loadinternalFont()
{
	// Has the Surface to the entire font been loaded?
	if(mFontSurface)
		SDL_FreeSurface(mFontSurface);

	SDL_Surface *blit = g_pVideoDriver->getBlitSurface();
	mFontSurface = loadfromXPMData(CGFont_xpm, blit->format, blit->flags);
	return true;
}

// This sets the width of the characters so the text is printed nicely.
// This is by default 8 pixels in vorticons and it is normally only used
// in the galaxy engine.
void CFont::setWidthToCharacter(Uint8 width, Uint16 letter)
{
	mWidthtable[letter] = width;
}



unsigned int CFont::getPixelTextWidth( const std::string& text )
{
	unsigned int c = 0, width = 0;
	for( ; c<text.size() ; c++)
	{
		const int e = text[c];
		width += (mWidthtable[e]+1);
	}
	return width;
}

unsigned int CFont::getPixelTextHeight()
{
	return mFontSurface->h/16;
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////
void CFont::drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff)
{
	SDL_Rect scrrect, dstrect;

	scrrect.x = (mFontSurface->w/16)*(character%16);
	scrrect.y = (mFontSurface->h/16)*(character/16);
	scrrect.w = dstrect.w = (mWidthtable[character]);
	scrrect.h = dstrect.h = (mFontSurface->h/16);
	dstrect.x = xoff;	dstrect.y = yoff;

	SDL_BlitSurface(mFontSurface, &scrrect, dst, &dstrect);
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

				x+=mWidthtable[c];
			}
			else
			{
				x=xoff;
				y+=8;
			}
		}
	}
}

void CFont::drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, bool highlight)
{
	drawFontCentered(dst, text, 0, width, yoff, highlight);
}

void CFont::drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, bool highlight)
{
	Uint16 xmidpos = 0;

	for( unsigned int i=0 ; i<text.size() ; i++)
		xmidpos += mWidthtable[text[i]];

	xmidpos = (width-xmidpos)/2+x;

	drawFont(dst, text, xmidpos, yoff, highlight);
}

void CFont::drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, Uint16 height, bool highlight)
{
	Uint16 xmidpos = 0;
	Uint16 ymidpos = 0;

	for( unsigned int i=0 ; i<text.size() ; i++)
		xmidpos += mWidthtable[text[i]];

	xmidpos = (width-xmidpos)/2+x;
	ymidpos = yoff + (height - 8)/2;

	drawFont(dst, text, xmidpos, ymidpos, highlight);
}


void CFont::drawMap(SDL_Surface* dst)
{
	SDL_BlitSurface(mFontSurface,NULL,dst,NULL);
}

///
// Destruction Routines
///
void CFont::destroySurface()
{
	if(mFontSurface) SDL_FreeSurface(mFontSurface);
		mFontSurface = NULL;
}

CFont::~CFont()
{
	destroySurface();
}

