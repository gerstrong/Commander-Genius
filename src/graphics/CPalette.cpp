/*
 * CPalette.cpp
 *
 *  Created on: 05.09.2009
 *      Author: gerstrong
 */

#include "CPalette.h"

///
// Initialization
///
CPalette::CPalette() {
	m_numcolors = 17;
	m_fxsurface = NULL;
	m_alpha = 0;
	m_fadespeed = 1;
	m_dark = false;
	m_darkness = FADE_DARKNESS;
}

CPalette::~CPalette() {
	// TODO Auto-generated destructor stub
}

void CPalette::setupColorPalettes(byte *p_exedata, int m_episode)
{
	// TODO: Try to add patching code for palette patches of mods here!

	// TODO: Here it will check, if the exe was patched with that code.

	// If it has that patch code read the colours and apply it.
	// Remember one thing, the patch code totally depends on the episode, so this will be hard...

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

// adds a color onto the end of the palette and returns it's index.
// if the palette is full, returns -1.
char CPalette::addcolor(unsigned char r, unsigned char g, unsigned char b)
{
	if (m_numcolors >= 255) return -1;
	
	m_Palette[m_numcolors].r = r;
	m_Palette[m_numcolors].g = g;
	m_Palette[m_numcolors].b = b;
	
	m_numcolors++;
	return (m_numcolors-1);
}

///
// Getters and Setters
///

void CPalette::setPaletteColour( Uint8 c, Uint8 r, Uint8 g, Uint8 b)
{
	m_Palette[c].r = r;
	m_Palette[c].g = g;
	m_Palette[c].b = b;
}

// sets whether to use the "dark" (lights off) or not
void CPalette::setdark(bool dark)
{
	m_dark = dark;
	setFadeColour(0);
	fadeto( m_dark ? m_darkness : 0, FADE_SPEED_SLOW);
}

void CPalette::setdarkness(Uint8 darkness)
{
	m_darkness = darkness;
}

void CPalette::setFXSurface(SDL_Surface *fxsurface)
{
	m_fxsurface = fxsurface;
	SDL_SetAlpha( m_fxsurface, SDL_SRCALPHA, m_alpha);
}

void CPalette::setFadeColour(Uint32 colour)
{
	SDL_FillRect(m_fxsurface, NULL, colour);
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

///
// Fading Routines
///
bool CPalette::in_progress(void)		// return whether or not a fade is in progress
{
	return m_fade_in_progess;
}

void CPalette::fadeto(Uint8 alpha, Uint8 fadespeed)
{
	m_alpha = alpha;
	m_fade_in_progess = true;
	m_fadespeed = fadespeed;
}

void CPalette::applyFade()
{
	Uint8 current_alpha = m_fxsurface->format->alpha;
	
	if( m_alpha!=current_alpha )
	{
		m_fade_in_progess = true;
		if( m_alpha>current_alpha )
		{
			if(current_alpha+m_fadespeed>m_alpha)
				current_alpha = m_alpha;
			else
				current_alpha += m_fadespeed;
		}
		else if( m_alpha<current_alpha )
		{
			if(current_alpha-m_fadespeed<m_alpha)
				current_alpha = m_alpha;
			else
				current_alpha -= m_fadespeed;
		}
		
		SDL_SetAlpha( m_fxsurface, SDL_SRCALPHA, current_alpha);
	}
	else
	{
		m_fade_in_progess = false;
	}
}



