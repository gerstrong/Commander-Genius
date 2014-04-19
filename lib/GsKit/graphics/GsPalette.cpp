/*
 * GsPalette.cpp
 *
 *  Created on: 05.09.2009
 *      Author: gerstrong
 */

#include "GsPalette.h"
#include <base/video/CVideoDriver.h>

#include <cstring>
#include <cstdio>


///
// Initialization
///
GsPalette::GsPalette()
{
	m_numcolors = 17;
	m_alpha = 0;
	m_fadespeed = 1;
	m_dark = false;
	m_darkness = FADE_DARKNESS;
	m_fade_in_progess = false;
}

/**
 * This function checks whether the palette is patched or not.
 * It has to look for a certain binary code, to know that...
 */
bool GsPalette::patchedPalette(byte *p_exedata, int m_episode)
{
	// go to the offset where we check the patched code
	if(m_episode == 1)
		p_exedata += 0x441C;
	else if(m_episode == 2)
		p_exedata += 0x157D;
	else if(m_episode == 3)
		p_exedata += 0x153B;

	const byte palette_patch_array[] =
	{ 0xB8, 0x0D, 0x00, 0xCD,
      0x10, 0xB8, 0x12, 0x10, 0xB9, 0x08, 0x00, 0xBB,
      0x00, 0x00, 0x8C, 0xDA, 0x8E, 0xC2, 0xBA, 0x04,
      0x00, 0xCD, 0x10, 0xBB, 0x18, 0x00, 0xBA, 0x1C,
      0x00, 0xCD, 0x10, 0xC3 };

	return (memcmp( palette_patch_array, p_exedata, 32 ) == 0);
}

/**
 * Reads the patched code and sets it up...
 */
void GsPalette::setupPatchedPalette(byte *p_exedata, int m_episode)
{
	if(m_episode == 1)
		p_exedata += 0x13054;
	else if(m_episode == 2)
		p_exedata += 0x17784;
	else if(m_episode == 3)
		p_exedata += 0x19824;

	for( size_t i=0 ; i<16 ; i++ )
	{
		Uint8 r, g, b;
		r = 4*(*p_exedata);
		p_exedata++;
		g = 4*(*p_exedata);
		p_exedata++;
		b = 4*(*p_exedata);
		p_exedata++;
		setPaletteColour( i, r, g, b);
	}
	setPaletteColour(16, 0x00, 0xff, 0xff);
}

void GsPalette::setupColorPalettes(byte *p_exedata, int m_episode)
{
	// TODO: Try to add patching code for palette patches of mods here!

	// TODO: Here it will check, if the exe was patched with that code.
	if(patchedPalette(p_exedata, m_episode))
	{
		setupPatchedPalette(p_exedata, m_episode);
	}
	else
	{	// Default Palette
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
}

///
// Getters and Setters
///

void GsPalette::setPaletteColour( Uint8 c, Uint8 r, Uint8 g, Uint8 b)
{
    m_Palette[c].r = r;
    m_Palette[c].g = g;
    m_Palette[c].b = b;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    m_Palette[c].a = 255;
#endif

}

// sets whether to use the "dark" (lights off) or not
void GsPalette::setdark(bool dark)
{
	m_dark = dark;

    if(dark)
        gVideoDriver.setLightIntensity(0.1f);
    else
        gVideoDriver.setLightIntensity(1.0f);

	fadeto( m_dark ? m_darkness : 0, FADE_SPEED_SLOW);
}

void GsPalette::setdarkness(Uint8 darkness)
{
	m_darkness = darkness;
}


// returns the index of a color in the current palette with an RGB value
// identical to the one given, or -1.
int GsPalette::getcolor(unsigned char r, unsigned char g, unsigned char b)
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
bool GsPalette::in_progress(void)		// return whether or not a fade is in progress
{
	return m_fade_in_progess;
}

void GsPalette::fadeto(Uint8 alpha, Uint8 fadespeed)
{
	if(m_alpha != alpha)
	{
		m_alpha = alpha;
		m_fade_in_progess = true;
		m_fadespeed = fadespeed;
	}
}

void GsPalette::applyFade()
{
    /*SDL_Surface *overlay = gVideoDriver.getOverlaySurface();

    Uint8 current_alpha = getPerSurfaceAlpha(overlay);


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

#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod( overlay, current_alpha);
#else
        SDL_SetAlpha( overlay, SDL_SRCALPHA, current_alpha);
#endif
	}
    else*/
	{
		m_fade_in_progess = false;
	}
}



