/*
 * CFont.h
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 *
 *  This class is the has the graphics for fonts of any type.
 *  They can be read out of EgaGraphicstile, or a special image file.
 *  The surface has not like the orignal keen game 256 tiles.
 *  It can carry up to 512 tiles, so 256 are free for personal stuff.
 *  One thing which is used, is the special twirl for all angles.
 *  That missed in the original game and made the graphics less smooth
 */

#ifndef CFONT_H_
#define CFONT_H_

#include <SDL/SDL.h>

class CFont {
public:
	CFont();
	virtual ~CFont();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags);
	SDL_Surface *getSDLSurface();

	void setColorPalette(SDL_Color *Palette);

	void generateGlowFonts();
	void generateSpecialTwirls();

	void drawTwirl(SDL_Surface* dst, int twirlframe, Uint16 x, Uint16 y);

	void drawCharacter(SDL_Surface* dst, Uint16 character);

private:

	SDL_Surface *FontSurface;

};

#endif /* CFONT_H_ */
