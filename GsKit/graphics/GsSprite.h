/*
 * GsSprite.h
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#ifndef GsSprite_H_
#define GsSprite_H_

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>

#include "graphics/GsSurface.h"

class GsBitmap;

class GsSprite
{
public:
    GsSprite();


    GsSprite(const GsSprite& original);
    GsSprite operator=(const GsSprite& original);    

    /**
     * @brief copy  Copy the whole sprite including the surface
     * @param original the source
     */
    void copy(const GsSprite& original);

    /**
     * @brief copyTilted  Copy
     * @param original
     */
    void copyTilted(const GsSprite& original);

	bool createSurface(Uint32 flags, SDL_Color *Palette);

    GsBitmap exportBitmap();

    /**
     * @brief optimizeSurface
     * @return
     */
	bool optimizeSurface();

    /**
     * @brief empty If there is no surface allocated,
     *        the sprite will be considered empty
     * @return
     */
    bool empty();


	void readMask(SDL_Surface *displaysurface);
	void readBBox(SDL_Surface *displaysurface);
	void applyTransparency();
	void applyTranslucency(Uint8 value);
	void copy( GsSprite &Destination, SDL_Color *Palette );
    void replaceSpriteColor( const Uint16 find, const Uint16 replace, const Uint16 miny );
    void exchangeSpriteColor(const Uint16 find1, const Uint16 find2, const Uint16 miny );

	void generateSprite( const int points );
	bool loadHQSprite( const std::string& filename );
	
	void setSize(Uint8 w, Uint8 h) { m_xsize = w; m_ysize = h; }
	void setOffset(Sint16 x, Sint16 y) { m_xoffset = x; m_yoffset = y; }
	void setBoundingBoxCoordinates( Sint32 bboxx1, Sint32 bboxy1, Sint32 bboxx2, Sint32 bboxy2 );

    GsSurface & Surface()  { return mSurface; }
    GsSurface & MaskSurface()   { return mMaskSurface; }

    const GsSurface & Surface() const { return mSurface; }
    const GsSurface & MaskSurface() const { return mMaskSurface; }

    void drawSprite(const int x, const int y, const int w, const int h, const Uint8 alpha=255 );
    void drawSprite(SDL_Surface *dst, const int x, const int y , const int w, const int h);
    void drawBlinkingSprite(const int x, const int y );
    void _drawBlinkingSprite(SDL_Surface *dst, const int x, const int y );

    Sint16 getXOffset() { return m_xoffset; }
    Sint16 getYOffset() { return m_yoffset; }

    Uint8 getWidth() { return m_xsize; }    
    Uint8 getHeight() { return m_ysize; }

    void setWidth(Uint8 w) { m_xsize=w; }
    void setHeight(Uint8 h) { m_ysize=h; }

	std::string getName() const { return mName; }
	void setName(const std::string &name) { mName = name; }

    Uint8 getAlpha() const
    { return m_alpha; }

    void readSize(Uint8 &xsize, Uint8 &ysize) const
    { xsize = m_xsize; ysize = m_ysize; }

    void readBBox(Sint32 &bboxX1, Sint32 &bboxY1,
                  Sint32 &bboxX2, Sint32 &bboxY2) const
    {
        bboxX1 = m_bboxX1; bboxY1 = m_bboxY1;
        bboxX2 = m_bboxY2; bboxY2 = m_bboxY2;
    }

    void readOffsets(Sint16 &xoffset, Sint16 &yoffset) const
    {
        xoffset = m_xoffset ; yoffset = m_yoffset;
    }

	// bounding box for hit detection
	Sint32 m_bboxX1, m_bboxY1;
	Sint32 m_bboxX2, m_bboxY2;

private:


    GsSurface mSurface;
    GsSurface mMaskSurface;

	std::string mName;
    Uint8 m_xsize = 0;
    Uint8 m_ysize = 0;
	Sint16 m_xoffset, m_yoffset;
    Uint8 m_alpha = 255;
};

#endif /* GsSprite_H_ */
