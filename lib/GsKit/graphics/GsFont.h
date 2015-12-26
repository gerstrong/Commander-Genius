/*
 * GsFont.h
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

#ifndef GsFont_H_
#define GsFont_H_

#include <graphics/GsSurface.h>

#include <string>
#include <vector>
#include <array>
#include <memory>

class GsFont
{
public:
	GsFont();


    void setFillWidthTable(const int width);

    GsFont(const GsFont& that)
    {
        operator=(that);
    }


    GsFont& operator=(const GsFont& that)
    {
        if(that.mFontSurface)
        {
            mFontSurface.createCopy(that.mFontSurface);
        }

        mWidthtable = that.mWidthtable;

        return *this;
    }

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags, Uint16 width = 128, Uint16 height = 128);

    SDL_Surface *SDLSurfacePtr() const { return mFontSurface.getSDLSurface(); }

	bool loadAlternateFont();

    /**
     * @brief loadinternalFont  Load embeeded Font map into the surface
     * @param pixmap    Static array of pixel information as XPM
     */
    void loadinternalFont(const char *pixmap[]);

    /**
     * @brief loadinternalFont
     * @param size  use 1 for normal sized and 2 for doubled, which would load an alternate pixmap into the surface
     */
    void loadinternalFont(const int size);

	void setWidthToCharacter(Uint8 width, Uint16 letter);
	
	void tintColor( const Uint32 fgColor );

	void setupColor( const Uint32 fgColor );

	Uint32 getFGColor();

    /**
     * Allocates a colored surface with the given Text printed in.
     */
    void createTextSurface(GsSurface &sfc,
                                   const std::string& text,
                                   unsigned char r,
                                   unsigned char g,
                                   unsigned char b);

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
	 * \brief gets the background color of the font
	 */
	Uint32 getBGColour(SDL_PixelFormat *format, const bool highlight = false);

	/**
	 * \brief Components-version
	 */
	void getBGColour(Uint8 *r, Uint8 *g, Uint8 *b, const bool highlight = false);

	void drawCharacter(SDL_Surface* dst, Uint16 character, Uint16 xoff, Uint16 yoff);
    void drawFont(SDL_Surface* dst, const std::string& text, const Uint16 xoff, const Uint16 yoff, const bool highlight = false);
    void drawFont(GsWeakSurface &dst, const std::string& text, const Uint16 xoff, const Uint16 yoff, const bool highlight = false)
    {
        drawFont(dst.getSDLSurface(), text, xoff, yoff, highlight);
    }

    void drawFontAlpha(SDL_Surface* dst, const std::string& text, Uint16 xoff, Uint16 yoff, const Uint8 alpha);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, Uint16 height, const Uint8 alpha);
	void drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, const Uint8 alpha);
	void drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, const Uint8 alpha);
    void drawFontCentered(SDL_Surface* dst,
                          const std::string& text,
                          const Uint16 x,
                          const Uint16 width,
                          const Uint16 yoff,
                          const Uint16 height,
                          const bool highlight = false);

    void drawFontCentered(GsWeakSurface &dst,
                          const std::string& text,
                          const GsRect<Uint16> &rect,
                          const bool highlight = false)
    {
        drawFontCentered(dst.getSDLSurface(),
                         text, rect.x, rect.w, rect.y, rect.h,
                         highlight);
    }


private:

    GsSurface mFontSurface;

    std::array<Uint8, 256> mWidthtable;    
};

#endif /* GsFont_H_ */
