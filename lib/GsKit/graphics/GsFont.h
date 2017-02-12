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

    /**
     * @brief GsFont    8x8 pixels is the default area used for one character.
     *                  On modern this might be very small
     */
    GsFont()
    {
        setFillWidthTable(8);
    }

    /**
     * @brief setFillWidthTable Sets the width to all the characters
     * @param width desired width to set
     */
    void setFillWidthTable(const int width)
    {
        mWidthtable.fill(width);
    }

    /**
     * @brief GsFont    Copy constructor
     * @param that      element to copy
     */
    GsFont(const GsFont& that)
    {
        operator=(that);
    }


    /**
     * @brief operator =    Basic assignment operator
     * @param that      element to copy
     * @return Reference to the newly set GsFont instance
     */
    GsFont& operator=(const GsFont& that)
    {
        if(that.mFontSurface)
        {
            mFontSurface.createCopy(that.mFontSurface);
        }

        mWidthtable = that.mWidthtable;

        return *this;
    }

    /**
     * @brief CreateSurface Creates a basic font surface
     * @param Palette
     * @param Flags
     * @param width
     * @param height
     * @return
     */
	bool CreateSurface(SDL_Color *Palette, Uint32 Flags, Uint16 width = 128, Uint16 height = 128);

    SDL_Surface *SDLSurfacePtr() const { return mFontSurface.getSDLSurface(); }

    /**
     * @brief loadAlternateFont This loads the internal font found in Alternatefont.xpm
     * @return  true if it was sucessfully loaded, otherwise false
     */
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

    /**
     * @brief setWidthToCharacter   This sets the width of the characters so the text is printed nicely.
     * @param width                 This is by default 8 pixels in vorticons and it is normally only used
     * @param letter
     */
    void setWidthToCharacter(const Uint8 width, const Uint16 letter)
    {
        mWidthtable[letter] = width;
    }

    /**
     * @brief getWidthofChar    Getter for graphical width of the character
     * @param letter            Letter of which the width is to be retruned
     */
    Uint16 getWidthofChar(const Uint16 letter) const
    {
        return mWidthtable[letter];
    }

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

    /**
     * @brief This will retrieve the total width in pixels of the Font.
	 * 		  In Galaxy it's more difficult to calculate that since every character has it own with
     * @param text Text which will be analyzed to get the full width
     * @return the width in pixel units
	 */
    unsigned int calcPixelTextWidth( const std::string& text );

    /**
     * @brief This will help to get the right height of some Dialog Boxes
     * @return height of the text height in pixel units.
	 */
    unsigned int getPixelTextHeight()
    {
        const auto height = mFontSurface.getSDLSurface()->h;
        return height / 16;
    }

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

    void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, bool highlight = false)
    {
        drawFontCentered(dst, text, 0, width, yoff, highlight);
    }


	void drawFontCentered(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, bool highlight = false);
	void drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 x, Uint16 width, Uint16 yoff, Uint16 height, const Uint8 alpha);

    void drawFontCenteredAlpha(SDL_Surface* dst, const std::string& text, Uint16 width, Uint16 yoff, const Uint8 alpha)
    {
        drawFontCenteredAlpha(dst, text, 0, width, yoff, alpha);
    }


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
    //GsSurface mFontSurface2x;   /** FontSurface but scaled up using Scale2X filter  */

    std::array<Uint8, 256> mWidthtable;    
};

#endif /* GsFont_H_ */
