/*
 * CFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CFont.h"
#include "CPalette.h"
#include "FindFile.h"
#include "sdl/CVideoDriver.h"
#include "CGFont.xpm"
#include "alternatefont.xpm"
#include "StringUtils.h"
#include "sdl/extensions.h"
#include <string.h>
#include <cstdlib>



CFont::CFont()
{
	memset(&mWidthtable, 8, 256);
}





///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////





bool CFont::CreateSurface(SDL_Color *Palette, Uint32 Flags,
							Uint16 width, Uint16 height)
{
	mFontSurface = SDL_CreateRGBSurface(Flags, width,
			height, 8, 0, 0, 0, 0);
	SDL_SetColors(mFontSurface.get(), Palette, 0, 255);
	SDL_SetColorKey(mFontSurface.get(), SDL_SRCCOLORKEY, COLORKEY);

	return ( !mFontSurface.empty() );
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

	bool usesAlpha = (sfc->flags & SDL_SRCALPHA);
	const Uint32 colorkey = SDL_MapRGB(sfc->format, 0xFF, 0x00, 0xFF);

	if(!usesAlpha)
		SDL_SetColorKey( sfc, SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey );

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
			{
				if(usesAlpha)
					color = SDL_MapRGBA(sfc->format, 0, 0, 0, 0);
				else
					color = colorkey;
			}
			else
			{
				color = colorMap[newPix];
				if(usesAlpha)
					color = SDL_MapRGBA(sfc->format,
										(color>>16) & 0xFF,
										(color>>8)  & 0xFF,
										(color>>0)  & 0xFF,
										0xFF);
				else
					color = SDL_MapRGB(sfc->format,
										(color>>16) & 0xFF,
										(color>>8)  & 0xFF,
										(color>>0)  & 0xFF);

			}

			*pixel = color;
			pixel++;
		}
	}

	SDL_UnlockSurface(sfc);

	return sfc;
}



bool CFont::loadAlternateFont()
{
	// Has the Surface to the entire font been loaded?

	SDL_Surface *blit = g_pVideoDriver->getBlitSurface();
	mFontSurface = loadfromXPMData( alternatefont_xpm, blit->format, blit->flags );
	return true;
}



bool CFont::loadinternalFont()
{
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


void CFont::setupColor( const Uint32 fgColor )
{
	// Here comes the main part. We have to manipulate the Surface the way it gets
	// the given color
	SDL_Color color[16];
	memcpy( color, mFontSurface->format->palette->colors, 16*sizeof(SDL_Color) );
	SDL_PixelFormat *pPixelformat = SDL_GetVideoSurface()->format;

	// Change palette colors to the desired one
	SDL_GetRGB(fgColor, pPixelformat, &color[15].r, &color[15].g, &color[15].b);
	SDL_SetColors( mFontSurface.get(), color, 0, 16);
}

Uint32 CFont::getFGColor()
{
	// Here comes the main part. We have to manipulate the Surface the way it gets
	// the given color
	SDL_Color color[16];
	memcpy( color, mFontSurface->format->palette->colors, 16*sizeof(SDL_Color) );
	SDL_PixelFormat *pPixelformat = SDL_GetVideoSurface()->format;

	// Change palette colors to the desired one
	return SDL_MapRGB(pPixelformat, color[15].r, color[15].g, color[15].b);
}


SDL_Surface* CFont::fetchColoredTextSfc(const std::string& text, const Uint32 fgColor )
{
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = getPixelTextWidth(text);
	rect.h = getPixelTextHeight()*calcNumLines(text);

	SDL_Surface *pColoredTextSurface = CG_CreateRGBSurface(rect);

	const Uint32 oldColor = getFGColor();

	setupColor( fgColor );

	drawFont( pColoredTextSurface, text, 0, 0);

	// Adapt the newly created surface to the running screen.
	SDL_Surface *temp = SDL_DisplayFormatAlpha(pColoredTextSurface);
	SDL_FreeSurface(pColoredTextSurface);
	pColoredTextSurface = temp;

	setupColor( oldColor );

	return pColoredTextSurface;
}




unsigned int CFont::getPixelTextWidth( const std::string& text )
{
	unsigned int c = 0, width = 0, len = 0;
	for( ; c<text.size() ; c++)
	{
		if ( endofText( text.substr(c) ) )
		{
			if(len > width)
				width = len;
			len = 0;
		}
		else
		{
			const int e = text[c];
			len += (mWidthtable[e]+1);
		}
	}

	if(len > width)
		width = len;

	return width;
}





unsigned int CFont::getPixelTextHeight()
{
	return mFontSurface->h/16;
}


Uint32 CFont::getBGColour(const bool highlight)
{
	SDL_PixelFormat *format = SDL_GetVideoSurface()->format;

	return getBGColour(format, highlight);
}




Uint32 CFont::getBGColour(SDL_PixelFormat *format, const bool highlight)
{
	Uint8 r, g, b;

	getBGColour( &r, &g, &b, highlight );

	return SDL_MapRGB(format, r, g, b);
}





void  CFont::getBGColour(Uint8 *r, Uint8 *g, Uint8 *b, const bool highlight)
{
	SDL_LockSurface(mFontSurface.get());

	const Uint32 color = getPixel(mFontSurface.get(), 0, highlight ? 80 : 16 );

	SDL_UnlockSurface(mFontSurface.get());

	SDL_GetRGB( color, mFontSurface->format, r, g, b);
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

	SDL_BlitSurface(mFontSurface.get(), &scrrect, dst, &dstrect);
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
		xmidpos += mWidthtable[ static_cast<unsigned int>(text[i]) ];

	xmidpos = (width-xmidpos)/2+x;

	drawFont(dst, text, xmidpos, yoff, highlight);
}

void CFont::drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, Uint16 height, bool highlight)
{
	Uint16 xmidpos = 0;
	Uint16 ymidpos = 0;

	for( unsigned int i=0 ; i<text.size() ; i++)
		xmidpos += mWidthtable[ static_cast<unsigned int>(text[i]) ];

	xmidpos = (width-xmidpos)/2+x;
	ymidpos = yoff + (height - 8)/2;

	drawFont(dst, text, xmidpos, ymidpos, highlight);
}


void CFont::drawMap(SDL_Surface* dst)
{
	SDL_BlitSurface(mFontSurface.get(), NULL, dst, NULL);
}

///
// Destruction Routines
///
void CFont::destroySurface()
{
	mFontSurface = NULL;
}

CFont::~CFont()
{
	destroySurface();
}

