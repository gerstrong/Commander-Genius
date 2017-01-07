/*
 * GsBitmap.h
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#ifndef GsBitmap_H_
#define GsBitmap_H_

#include <SDL.h>
#include <vector>
#include <string>
#include <memory>

#include <base/utils/Geometry.h>
#include <graphics/GsSurface.h>

class GsBitmap
{
public:

	GsBitmap();

    // Will create a copy of the bitmap
    GsBitmap(const GsWeakSurface &sfc);

	GsBitmap(const std::shared_ptr<SDL_Surface> &bmpSfc);

	explicit GsBitmap(const GsBitmap &bitmap);

	bool createSurface(Uint32 flags, SDL_Rect rect, SDL_Color *Palette);
	bool optimizeSurface();
	bool loadHQBitmap( const std::string& filename );

    void exchangeColor( const Uint8 oldR, const Uint8 oldG, const Uint8 oldB,
                        const Uint8 newR, const Uint8 newG, const Uint8 newB);
    void exchangeColor( const Uint32 oldColor, const Uint32 newColor );


    void setPerSurfaceAlpha(const Uint8 alpha);

    bool empty() const
    {
        if(!mpBitmapSurface)
            return true;
        else
            return false;
    }

    void clear()
    {
        if(mpBitmapSurface)
            mpBitmapSurface = nullptr;
    }

    SDL_Surface *getSDLSurface() const
	{
		return mpBitmapSurface.get();
	}

    Uint16 width() const { return mpBitmapSurface->w; }
    Uint16 height() const { return mpBitmapSurface->h; }

    void setColorKey(const Uint8 r, const Uint8 g, const Uint8 b);
    bool scaleTo(const GsRect<Uint16> &destRes);

	std::string getName() const { return mName; }
	void setName(const std::string &name) { mName = name; }

    /**
     * \brief The function that blits the sprite to the main blitSurface
     * \param x		 		X-Coordinate, indicating the position on dst
     * \param y				Y-Coordinate, indicating the position on dst
     */
    void draw(const int x, const int y) const;

    void _draw(const int x, const int y, SDL_Surface *dst) const;

private:
	std::string mName;
	std::shared_ptr<SDL_Surface> mpBitmapSurface;
};

#endif /* GsBitmap_H_ */
