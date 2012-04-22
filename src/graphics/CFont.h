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

class CFont
{
public:
	CFont();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags, Uint8 bpp=8, Uint16 width = 128, Uint16 height = 128);
	SDL_Surface *getSDLSurface() { return mFontSurface; }

	bool loadinternalFont();

	void setWidthToCharacter(Uint8 width, Uint16 letter);

	/**
	 * Allocates a colored surface with the given Text printed in.
	 */
	SDL_Surface* fetchColoredTextSfc(const std::string& text, const Uint32 fgColor );

	/*
	 * \brief This will retrieve the total width in pixels of the Font.
	 * 		  In Galaxy it's more difficult to calculate that since every character has it own with
	 * \param text Text which will be analyzed to get the full width
	 * \return the width in pixel units
	 */
	unsigned int getPixelTextWidth( const std::string& text );

	/*
	 * \brief This will help to get the right height of some Dialog Boxes
	 * \return height of the text height in pixel units.
	 */
	unsigned int getPixelTextHeight();


	/**
	 * \brief gets the background color of the font
	 */
	Uint32 getBGColour(const bool highlight = false);

	/**
	 * \brief Components-version
	 */
	void getBGColour(Uint8 *r, Uint8 *g, Uint8 *b, const bool highlight = false);

	void drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff);
	void drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, bool highlight = false);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, Uint16 height, bool highlight = false);
	void drawMap(SDL_Surface* dst);

	void destroySurface();
	~CFont();

private:
	SDL_Surface *mFontSurface;
	Uint8 mWidthtable[256];
};

#endif /* CFONT_H_ */
