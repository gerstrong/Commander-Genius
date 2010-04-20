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

#include <SDL.h>
#include <string>
#include <vector>

class CFont {
public:
	CFont();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags, Uint8 bpp=8, Uint16 width = 128, Uint16 height = 128);
	bool optimizeSurface();
	SDL_Surface *getSDLSurface() { return m_FontSurface; }
	SDL_Surface *getSDLColouredSurface() { return m_ColouredSurface; }

	bool loadHiColourFont( const std::string& filename );

	void setWidthToCharacter(Uint8 width, Uint16 letter);
	void setColour(Uint32 colour, bool force=false);
	Uint32 getColour();

	void drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff, Uint32 colour = 0x0);
	void drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, bool highlight = false, Uint32 colour=0x0 );
	void drawMap(SDL_Surface* dst);

	void destroySurface();
	virtual ~CFont();

private:
	SDL_Surface *m_FontSurface, *m_ColouredSurface;
	Uint32 m_colour;
	std::vector<Uint8> m_widthtable;
	bool m_monochrome;
};

#endif /* CFONT_H_ */
