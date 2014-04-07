/*
 * GsPalette.h
 *
 *  Created on: 05.09.2009
 *      Author: gerstrong
 *  This one manages the Palette and fade effects
 */

#ifndef GsPalette_H_
#define GsPalette_H_

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
#include "base/TypeDefinitions.h"

class GsPalette {
public:
	GsPalette();

	void fadeto(Uint8 alpha, Uint8 fadespeed);
	void applyFade();

	bool patchedPalette(byte *p_exedata, int m_episode);
	void setupPatchedPalette(byte *p_exedata, int m_episode);
	void setupColorPalettes(byte *p_exedata, int m_episode);
	void setPaletteColour( Uint8 c, Uint8 r, Uint8 g, Uint8 b);
    void setdark(const bool dark);
	void setdarkness(Uint8 darkness);

	int getcolor(unsigned char r, unsigned char g, unsigned char b);

	bool in_progress(void);


	SDL_Color m_Palette[256];
	Uint8 m_alpha;
	Uint8 m_numcolors;
	Uint8 m_fadespeed;
	Uint8 m_darkness;
	bool m_fade_in_progess;
	bool m_dark;
};

#endif /* GsPalette_H_ */
