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

	explicit CBitmap(const CBitmap &bitmap);

	bool createSurface(Uint32 flags, SDL_Color *Palette);
	bool optimizeSurface();
	bool loadHQBitmap( const std::string& filename );

	SDL_Surface *getSDLSurface() const
	{
		return m_BitmapSurface;
	}

	void setSDLSurface(SDL_Surface *surface)
	{	m_BitmapSurface = surface;	}

	void setDimensions(const Uint16 w, const Uint16 h);
	void setName(const std::string &name);
	void setColorPalette(SDL_Color *Palette);
	Uint16 getWidth() { return m_ImageRect.w; }
	Uint16 getHeight() { return m_ImageRect.h; }

	SDL_Rect getRect() const {	return m_ImageRect;	}
	void setRect(SDL_Rect &rect) { m_ImageRect = rect; }

	std::string getName() const { return m_name; }
	void setName(std::string &name) { m_name = name; }

	void draw(Uint16 x, Uint16 y);
	void _draw(SDL_Surface *dst, Uint16 x, Uint16 y);

	void destroySurface();

private:
	SDL_Rect m_ImageRect;
	std::string m_name;
	SDL_Surface *m_BitmapSurface;
};

#endif /* CBITMAP_H_ */
