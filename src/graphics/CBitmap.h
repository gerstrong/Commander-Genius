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

class CBitmap
{
public:
	CBitmap();

	bool createSurface(Uint32 flags, SDL_Color *Palette);
	bool optimizeSurface();
	bool loadHQBitmap( const std::string& filename );

	SDL_Surface *getSDLSurface();
	void setDimensions(const Uint16 w, const Uint16 h);
	void setName(const std::string &name);
	void setColorPalette(SDL_Color *Palette);
	Uint16 getWidth() { return m_ImageRect.w; }
	Uint16 getHeight() { return m_ImageRect.h; }
	SDL_Rect getRect() {	return m_ImageRect;	}
	std::string getName() { return m_name; }

	void draw(SDL_Surface *dst, Uint16 x, Uint16 y);
	void _draw(SDL_Surface *dst, Uint16 x, Uint16 y);

	void destroySurface();

private:
	SDL_Rect m_ImageRect;
	std::string m_name;
	SDL_Surface *m_BitmapSurface;
};

#endif /* CBITMAP_H_ */
