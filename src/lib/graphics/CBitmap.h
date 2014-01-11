/*
 * CBitmap.h
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#ifndef CBITMAP_H_
#define CBITMAP_H_

#include <SDL.h>
#include <vector>
#include <string>
#include <memory>

#include "utils/Geometry.h"

class CBitmap
{
public:

	CBitmap();

	CBitmap(const std::shared_ptr<SDL_Surface> &bmpSfc);

	explicit CBitmap(const CBitmap &bitmap);

	bool createSurface(Uint32 flags, SDL_Rect rect, SDL_Color *Palette);
	bool optimizeSurface();
	bool loadHQBitmap( const std::string& filename );

    void exchangeColor( const Uint8 oldR, const Uint8 oldG, const Uint8 oldB,
                        const Uint8 newR, const Uint8 newG, const Uint8 newB);
    void exchangeColor( const Uint32 oldColor, const Uint32 newColor );
    void makeBlackTransparent();

	SDL_Surface *getSDLSurface() const
	{
		return mpBitmapSurface.get();
	}

	Uint16 getWidth() const { return mpBitmapSurface->w; }
	Uint16 getHeight() const { return mpBitmapSurface->h; }

    void setColorKey(const Uint8 r, const Uint8 g, const Uint8 b);
    bool scaleTo(const GsRect<Uint16> &gameRes);

	std::string getName() const { return mName; }
	void setName(const std::string &name) { mName = name; }

    void draw(const int x, const int y);
    void _draw(const int x, const int y, SDL_Surface *dst) const;

private:
	std::string mName;
	std::shared_ptr<SDL_Surface> mpBitmapSurface;
};

#endif /* CBITMAP_H_ */
