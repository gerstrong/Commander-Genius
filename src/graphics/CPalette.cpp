/*
 * CPalette.cpp
 *
 *  Created on: 05.09.2009
 *      Author: gerstrong
 */

#include "CPalette.h"

CPalette::CPalette() {
	m_numcolors = 17;
	m_fxsurface = NULL;
	m_alpha=0;
}

CPalette::~CPalette() {
	// TODO Auto-generated destructor stub
}

void CPalette::fade(Uint8 alpha)
{
	m_alpha = alpha;
	SDL_SetAlpha( m_fxsurface, SDL_SRCALPHA, m_alpha);
}

void CPalette::setFXSurface(SDL_Surface *fxsurface)
{
	m_fxsurface = fxsurface;
}

void CPalette::setFadeColour(Uint32 colour)
{
	SDL_FillRect(m_fxsurface, NULL, colour);
}

void CPalette::setupColorPalettes()
{
	setPaletteColour( 0, 0x00, 0x00, 0x00);
	setPaletteColour( 1, 0x00, 0x00, 0xa8);
	setPaletteColour( 2, 0x00, 0xa8, 0x00);
	setPaletteColour( 3, 0x00, 0xa8, 0xa8);
	setPaletteColour( 4, 0xa8, 0x00, 0x00);
	setPaletteColour( 5, 0xa8, 0x00, 0xa8);
	setPaletteColour( 6, 0xa8, 0x54, 0x00);
	setPaletteColour( 7, 0xa8, 0xa8, 0xa8);
	setPaletteColour( 8, 0x54, 0x54, 0x54);
	setPaletteColour( 9, 0x54, 0x54, 0xfc);
	setPaletteColour(10, 0x54, 0xfc, 0x54);
	setPaletteColour(11, 0x54, 0xfc, 0xfc);
	setPaletteColour(12, 0xfc, 0x54, 0x54);
	setPaletteColour(13, 0xfc, 0x54, 0xfc);
	setPaletteColour(14, 0xfc, 0xfc, 0x54);
	setPaletteColour(15, 0xfc, 0xfc, 0xfc);
	setPaletteColour(16, 0x00, 0xff, 0xff);
}

void CPalette::setPaletteColour( Uint8 c, Uint8 r, Uint8 g, Uint8 b)
{
	m_Palette[c].r = r;
	m_Palette[c].g = g;
	m_Palette[c].b = b;
}

// returns the index of a color in the current palette with an RGB value
// identical to the one given, or -1.
int CPalette::getcolor(unsigned char r, unsigned char g, unsigned char b)
{
	for(int i=0;i<m_numcolors;i++)
	{
		if (m_Palette[i].r==r && m_Palette[i].g==g && m_Palette[i].b==b)
			return i;
	}
	return -1;
}

// adds a color onto the end of the palette and returns it's index.
// if the palette is full, returns -1.
char CPalette::addcolor(unsigned char r, unsigned char g, unsigned char b)
{
	if (m_numcolors >= 256) return -1;

	m_Palette[m_numcolors].r = r;
	m_Palette[m_numcolors].g = g;
	m_Palette[m_numcolors].b = b;

	m_numcolors++;
	return (m_numcolors-1);
}

// sets whether to use the "dark" (lights off) palette or not
void CPalette::setdark(int dark)
{

}

bool CPalette::in_progress(void)		// return whether or not a fade is in progress
{
	return false;
}

