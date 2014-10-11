/*
 * GsFont.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "GsFont.h"
//#include "GsPalette.h"
#include <base/utils/FindFile.h>
#include <base/video/CVideoDriver.h>
#include "GsFont.xpm"
#include "AlternateFont.xpm"
#include <string.h>
#include <cstdlib>



GsFont::GsFont()
{
	memset(&mWidthtable, 8, 256);
}





///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////


// We have the colors from 0-15, which are 16 colors in our 4-bit colorspace.
// Colour 16 represents transparency.
const unsigned int COLORKEY_4BIT = 16;


bool GsFont::CreateSurface(SDL_Color *Palette, Uint32 Flags,
							Uint16 width, Uint16 height)
{
	mFontSurface.reset(SDL_CreateRGBSurface(Flags, width,
			height, 8, 0, 0, 0, 0), &SDL_FreeSurface );
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetPaletteColors(mFontSurface->format->palette, Palette, 0, 255);
    SDL_SetColorKey(mFontSurface.get(), SDL_TRUE, COLORKEY_4BIT);
#else
    SDL_SetColors(mFontSurface.get(), Palette, 0, 255);
    SDL_SetColorKey(mFontSurface.get(), SDL_SRCCOLORKEY, COLORKEY_4BIT);
#endif

	if( mFontSurface )
	  return true;
	else
	  return false;
}





SDL_Surface *loadfromXPMData(const char **data, const SDL_PixelFormat *format, const Uint32 flags)
{
	int width, height, colors;

	// Read the dimensions and amount of colors
	sscanf((const char*)data[0], "%d %d %d", &width, &height, &colors);

	// Create the surface
	SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height,
			  	  	  	  	  	  	  	  	 format->BitsPerPixel,
			  	  	  	  	  	  	  	  	 format->Rmask, format->Gmask,
			  	  	  	  	  	  	  	  	 format->Bmask, 0 );

#if SDL_VERSION_ATLEAST(2, 0, 0)
    bool usesAlpha = false;
#else
    bool usesAlpha = (sfc->flags & SDL_SRCALPHA);
#endif
	const Uint32 colorkey = SDL_MapRGB(sfc->format, 0xFF, 0x00, 0xFF);

	if(!usesAlpha)
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetColorKey( sfc, SDL_TRUE|SDL_RLEACCEL, colorkey );
#else
		SDL_SetColorKey( sfc, SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey );
#endif

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
		Sint8 *pixel_data = const_cast<Sint8*>((Sint8*)data[colors+1+y]);

		for( int x = 0 ; x < width ; x++)
		{
			const Sint8 newPix = pixel_data[x];

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



bool GsFont::loadAlternateFont()
{
	// Has the Surface to the entire font been loaded?

	SDL_Surface *blit = gVideoDriver.getBlitSurface();
	mFontSurface.reset( loadfromXPMData( alternatefont_xpm, blit->format, blit->flags ), &SDL_FreeSurface );
	return true;
}



void GsFont::loadinternalFont()
{
	SDL_Surface *blit = gVideoDriver.getBlitSurface();

    mFontSurface.reset( loadfromXPMData( GsFont_xpm, blit->format, blit->flags ), &SDL_FreeSurface );

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode(mFontSurface.get(), SDL_BLENDMODE_BLEND);
#endif
}

// This sets the width of the characters so the text is printed nicely.
// This is by default 8 pixels in vorticons and it is normally only used
// in the galaxy engine.
void GsFont::setWidthToCharacter(Uint8 width, Uint16 letter)
{
	mWidthtable[letter] = width;
}



void GsFont::tintColor( const Uint32 fgColor )
{
    SDL_Surface *sfc = mFontSurface.get();
    Uint32 color = 0;
    Uint8 r, g, b, a;

    if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);

    // This makes the white pixel transparent
    Uint8 *pixel = (Uint8*)sfc->pixels;

	for( Uint16 y=0 ; y<sfc->h ; y++ )
	{
		for( Uint16 x=0 ; x<sfc->w ; x++ )
		{
			memcpy( &color, pixel, sfc->format->BytesPerPixel );

			SDL_GetRGBA( color, sfc->format, &r, &g, &b, &a );

			if( a>0 )
			{
			    memcpy( pixel, &fgColor, sfc->format->BytesPerPixel );
			}

			pixel += sfc->format->BytesPerPixel;
		}
	}
	if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
}


void GsFont::setupColor( const Uint32 fgColor )
{
	// Here comes the main part. We have to manipulate the Surface the way it gets
	// the given color
	SDL_Color color[16];
	memcpy( color, mFontSurface->format->palette->colors, 16*sizeof(SDL_Color) );

    SDL_PixelFormat *pPixelformat = gVideoDriver.getBlitSurface()->format;

    SDL_GetRGB(fgColor, pPixelformat, &color[15].r, &color[15].g, &color[15].b);

	// Change palette colors to the desired one
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetPaletteColors(mFontSurface->format->palette, color, 0, 255);
    SDL_SetColorKey( mFontSurface.get(), SDL_TRUE, 16);
#else
	SDL_SetColors( mFontSurface.get(), color, 0, 16);
#endif
}

Uint32 GsFont::getFGColor()
{
	// Here comes the main part. We have to manipulate the Surface the way it gets
	// the given color
	SDL_Color color[16];
	memcpy( color, mFontSurface->format->palette->colors, 16*sizeof(SDL_Color) );

    SDL_PixelFormat *pPixelformat = gVideoDriver.getBlitSurface()->format;

	// Change palette colors to the desired one
    return SDL_MapRGB(pPixelformat, color[15].r, color[15].g, color[15].b);
}

void GsFont::createTextSurface(GsSurface &sfc,
                               const std::string& text,
                               unsigned char r,
                               unsigned char g,
                               unsigned char b)
{
    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = getPixelTextWidth(text);
    rect.h = getPixelTextHeight()*calcNumLines(text);

    SDL_Surface *blit = gVideoDriver.getBlitSurface();
    SDL_PixelFormat *format = blit->format;

    sfc.create(SDL_SWSURFACE,
               rect.w, rect.h,
               format->BitsPerPixel,
               format->Rmask,
               format->Gmask,
               format->Bmask,
               0);

    sfc.setColorMask(0, 255, 255);
    sfc.fillRGB(0, 255, 255);

    const Uint32 oldColor = getFGColor();

    const Uint32 fgColor = sfc.mapRGB( r, g, b );

    setupColor( fgColor );

    drawFont( sfc.getSDLSurface(), text, 0, 0);

    setupColor( oldColor );
}


unsigned int GsFont::getPixelTextWidth( const std::string& text )
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





unsigned int GsFont::getPixelTextHeight()
{
	return mFontSurface->h/16;
}


Uint32 GsFont::getBGColour(const bool highlight)
{
    SDL_PixelFormat *format = gVideoDriver.getBlitSurface()->format;

	return getBGColour(format, highlight);
}




Uint32 GsFont::getBGColour(SDL_PixelFormat *format, const bool highlight)
{
	Uint8 r, g, b;

	getBGColour( &r, &g, &b, highlight );

	return SDL_MapRGB(format, r, g, b);
}





void  GsFont::getBGColour(Uint8 *r, Uint8 *g, Uint8 *b, const bool highlight)
{

    GsWeakSurface fontSfc(mFontSurface.get());

    const Uint32 color = fontSfc.getPixel(0, highlight ? 80 : 16 );

    fontSfc.getRGB(color, *r, *g, *b);
}





////////////////////////////
///// Drawing Routines /////
////////////////////////////
void GsFont::drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff)
{
	SDL_Rect scrrect, dstrect;

//#if SDL_VERSION_ATLEAST(2, 0, 0)

//#else
    scrrect.x = (mFontSurface->w/16)*(character%16);
	scrrect.y = (mFontSurface->h/16)*(character/16);
	scrrect.w = dstrect.w = (mWidthtable[character]);
	scrrect.h = dstrect.h = (mFontSurface->h/16);
	dstrect.x = xoff;	dstrect.y = yoff;
//#endif

	BlitSurface(mFontSurface.get(), &scrrect, dst, &dstrect);
}

void GsFont::drawFont(SDL_Surface* dst,
                      const std::string& text,
                      const Uint16 xoff,
                      const Uint16 yoff,
                      const bool highlight)
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

void GsFont::drawFontAlpha(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, const Uint8 alpha)
{
	unsigned int i,x=xoff,y=yoff;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod(mFontSurface.get(), alpha);
#else
    SDL_SetAlpha(mFontSurface.get(), SDL_SRCALPHA, alpha);
#endif

	if(text.size() != 0)
	{
		for(i=0;i<text.size();i++)
		{
			unsigned char c = text[i];

			if ( !endofText( text.substr(i) ) )
			{
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

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod(mFontSurface.get(), SDL_ALPHA_OPAQUE);
#else
    SDL_SetAlpha(mFontSurface.get(), SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
#endif
}


void GsFont::drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, bool highlight)
{
	drawFontCentered(dst, text, 0, width, yoff, highlight);
}

void GsFont::drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, const Uint8 alpha)
{
	drawFontCenteredAlpha(dst, text, 0, width, yoff, alpha);
}

void GsFont::drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, const Uint8 alpha)
{
	Uint16 xmidpos = 0;

	for( unsigned int i=0 ; i<text.size() ; i++)
		xmidpos += mWidthtable[ static_cast<unsigned int>(text[i]) ];

	xmidpos = (width-xmidpos)/2+x;

	drawFontAlpha(dst, text, xmidpos, yoff, alpha);
}

void GsFont::drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, bool highlight)
{
	Uint16 xmidpos = 0;

	for( unsigned int i=0 ; i<text.size() ; i++)
		xmidpos += mWidthtable[ static_cast<unsigned int>(text[i]) ];

	xmidpos = (width-xmidpos)/2+x;

	drawFont(dst, text, xmidpos, yoff, highlight);
}

void GsFont::drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, Uint16 height, const Uint8 alpha)
{
	Uint16 xmidpos = 0;
	Uint16 ymidpos = 0;

	for( unsigned int i=0 ; i<text.size() ; i++)
		xmidpos += mWidthtable[ static_cast<unsigned int>(text[i]) ];

	xmidpos = (width-xmidpos)/2+x;
	ymidpos = yoff + (height - 8)/2;

	drawFontAlpha(dst, text, xmidpos, ymidpos, alpha);
}

void GsFont::drawFontCentered(SDL_Surface* dst,
                              const std::string& text,
                              const Uint16 x,
                              const Uint16 width,
                              const Uint16 yoff,
                              const Uint16 height,
                              const bool highlight)
{
	Uint16 xmidpos = 0;
	Uint16 ymidpos = 0;

	Uint16 ylineoff = yoff;

	for( unsigned int i=0 ; i<text.size() ; i++)
	{
		xmidpos += mWidthtable[ static_cast<unsigned int>(text[i]) ];

		if ( endofText( text.substr(i) ) )
		{
		    xmidpos = (width-xmidpos)/2+x;
		    ymidpos = ylineoff + (height - 8)/2;
		    ylineoff += height;

		    drawFont(dst, text, xmidpos, ymidpos, highlight);
		}
	}

	xmidpos = (width-xmidpos)/2+x;
	ymidpos = ylineoff + (height - 8)/2;

	drawFont(dst, text, xmidpos, ymidpos, highlight);
}


void GsFont::drawMap(SDL_Surface* dst)
{
	BlitSurface(mFontSurface.get(), NULL, dst, NULL);
}
