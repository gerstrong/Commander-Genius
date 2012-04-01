/*
 * CFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CFont.h"
#include "CPalette.h"
#include "../FindFile.h"
#include "../sdl/CVideoDriver.h"
#include "CGFont.xpm"
#include "StringUtils.h"
#include "sdl/extensions.h"
#include <string.h>
#include <cstdlib>



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

	// Read the data and pass it to the surface
	SDL_LockSurface(sfc);

	// Now get the colors which has that XPM File
	std::map<char,Uint32> colorMap;
	char charCode, dummy;
	char colorCode[7];
	Uint32 color;
	std::stringstream ss;
	memset(colorCode, 0, 7*sizeof(char) );
	for( int c=0; c<colors ; c++ )
	{
		sscanf(data[c+1], "%c\t%c #%s", &charCode, &dummy, colorCode);
		color = strtol( colorCode, NULL, 16 );
		colorMap[charCode] = color;
	}


	std::string textbuf;

	Uint32 *pixel = static_cast<Uint32*>(sfc->pixels);
	for( int y = 0 ; y < height ; y++)
	{
		char *pixel_data = const_cast<char*>(data[colors+1+y]);

		for( int x = 0 ; x < width ; x++)
		{
			const char newPix = pixel_data[x];

			if( newPix == ' ' )
				color = 0;
			else
				color = colorMap[newPix] | 0xFF000000;

			*pixel = color;
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
	mFontSurface = loadfromXPMData( CGFont_xpm, blit->format, blit->flags );
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





Uint32 CFont::getBGColour(const bool highlight)
{
	SDL_LockSurface(mFontSurface);

	const Uint32 color = getPixel(mFontSurface, 0, highlight ? 80 : 16 );

	SDL_UnlockSurface(mFontSurface);

	return color;
}





void  CFont::getBGColour(Uint8 *r, Uint8 *g, Uint8 *b, const bool highlight)
{
	SDL_GetRGB( getBGColour(highlight), mFontSurface->format, r, g, b);
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

