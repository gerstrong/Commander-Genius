/*
 * CPalette.h
 *
 *  Created on: 05.09.2009
 *      Author: gerstrong
 *  This one manages the Palette and fade effects
 */

#ifndef CPALETTE_H_
#define CPALETTE_H_

#include <SDL/SDL.h>

class CPalette {
public:
	CPalette();
	virtual ~CPalette();

	void fade(Uint8 alpha);

	void setupColorPalettes();
	void setPaletteColour( Uint8 c, Uint8 r, Uint8 g, Uint8 b);
	void setFXSurface(SDL_Surface *fxsurface);
	void setFadeColour(Uint32 colour);
	void setdark(int dark);

	char addcolor(unsigned char r, unsigned char g, unsigned char b);
	int getcolor(unsigned char r, unsigned char g, unsigned char b);

	bool in_progress(void);


	SDL_Color m_Palette[256];
	Uint8 m_alpha;
	Uint8 m_numcolors;
	SDL_Surface *m_fxsurface;
};

#endif /* CPALETTE_H_ */
