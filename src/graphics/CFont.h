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

	bool loadinternalFont();
	bool loadHiColourFont( const std::string& filename );

	void setWidthToCharacter(Uint8 width, Uint16 letter);
	void setMonochrome(bool value);
	void setFGColour(SDL_PixelFormat* p_pixelformat, Uint32 fgcolour, bool force=false);
	void setBGColour(SDL_PixelFormat* p_pixelformat, Uint32 bgcolour, bool force=false);
	Uint32 getColour();

	/**
	 * \brief Retrieves the background colour of the font map
	 * \param highlighted The fontmap has two background colours.
	 * 		  			  One is highlighted (default gray)
	 * 					  The other is not (default white)
	 * 					  Those colours normally differ in mods.
	 */
	Uint32 getBGColour(bool highlighted);

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


	void getBGColour(bool highlighted, Uint8 *r, Uint8 *g, Uint8 *b);

	void drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff);
	void drawFont(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, bool highlight = false);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawMap(SDL_Surface* dst);

	void destroySurface();
	virtual ~CFont();

private:
	SDL_Surface *m_FontSurface, *m_ColouredSurface;
	Uint32 m_fgcolour, m_bgcolour;
	//std::vector<Uint8> m_widthtable;
	Uint8 m_widthtable[256];
	bool m_monochrome;
};

#endif /* CFONT_H_ */
