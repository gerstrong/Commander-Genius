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

	SDL_Surface *getSDLSurface() const
	{
		return mpBitmapSurface.get();
	}

	Uint16 getWidth() const { return mpBitmapSurface->w; }
	Uint16 getHeight() const { return mpBitmapSurface->h; }

    bool scaleTo(const CRect<Uint16> gameRes);

	std::string getName() const { return mName; }
	void setName(const std::string &name) { mName = name; }

	void draw(Uint16 x, Uint16 y);
	void _draw(SDL_Surface *dst, Uint16 x, Uint16 y) const;

private:
	std::string mName;
	std::shared_ptr<SDL_Surface> mpBitmapSurface;
};

#endif /* CBITMAP_H_ */
