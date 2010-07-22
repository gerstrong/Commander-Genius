/*
 * CPalette.h
 *
 *  Created on: 05.09.2009
 *      Author: gerstrong
 *  This one manages the Palette and fade effects
 */

#ifndef CPALETTE_H_
#define CPALETTE_H_

#define COLORKEY 16 // Used only for the 8-bit tile. Colour number COLORKEY in the Palette

// Standard Fade speeds
#define FADE_SPEED_FAST 40
#define FADE_SPEED_SLOW 8
#define FADE_SPEED_VERY_SLOW 4

// Standard Darkness for light switches
#define FADE_DARKNESS_HARD 216
#define FADE_DARKNESS 200
#define FADE_DARKNESS_EASY 166

#include <SDL.h>
#include "../fileio/TypeDefinitions.h"

class CPalette {
public:
	CPalette();
	virtual ~CPalette();

	void fadeto(Uint8 alpha, Uint8 fadespeed);
	void applyFade();

	void setupColorPalettes(byte *p_exedata, int m_episode);
	void setPaletteColour( Uint8 c, Uint8 r, Uint8 g, Uint8 b);
	void setFXSurface(SDL_Surface *fxsurface);
	void setFadeColour(Uint32 colour);
	void setdark(bool dark);
	void setdarkness(Uint8 darkness);

	char addcolor(unsigned char r, unsigned char g, unsigned char b);
	int getcolor(unsigned char r, unsigned char g, unsigned char b);

	bool in_progress(void);


	SDL_Color m_Palette[256];
	Uint8 m_alpha;
	Uint8 m_numcolors;
	Uint8 m_fadespeed;
	Uint8 m_darkness;
	SDL_Surface *m_fxsurface;
	bool m_fade_in_progess;
	bool m_dark;
};

#endif /* CPALETTE_H_ */
