/*
 * CSprite.h
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#ifndef CSPRITE_H_
#define CSPRITE_H_

#include <SDL.h>
#include <string>
#include <vector>

class CSprite {
public:
	CSprite();

	bool createSurface(Uint32 flags, SDL_Color *Palette);
	bool optimizeSurface();
	void readMask(SDL_Surface *displaysurface);
	void readBBox(SDL_Surface *displaysurface);
	void applyTransparency();
	void applyTranslucency(Uint8 value);
	void copy( CSprite &Destination, SDL_Color *Palette );
	void replaceSpriteColor( Uint16 find, Uint16 replace, Uint16 miny );

	bool loadHQSprite( const std::string& filename );
	
	void setSize(Uint8 w, Uint8 h) { m_xsize = w; m_ysize = h; }
	void setOffset(Uint16 x, Uint16 y) { m_xoffset = x; m_yoffset = y; }
	void setBouncingBoxCoordinates( Uint16 bboxx1, Uint16 bboxy1, Uint16 bboxx2, Uint16 bboxy2 );
	SDL_Surface *getSDLSurface() { return m_surface; }
	SDL_Surface *getSDLMaskSurface() { return m_masksurface; }

	void drawSprite( SDL_Surface *dst, const Uint16 x, const Uint16 y, const Uint8 alpha=255 );
	void drawBlinkingSprite( SDL_Surface *dst, Uint16 x, Uint16 y );

	Uint16 getXOffset() { return m_xoffset; }
	Uint16 getYOffset() { return m_yoffset; }
	Uint8 getWidth() { return m_xsize; };
	Uint8 getHeight() { return m_ysize; };
	void setWidth(Uint8 w) { m_xsize=w; };
	void setHeight(Uint8 h) { m_ysize=h; };

	void freeSurfaces();

	virtual ~CSprite();

	// bounding box for hit detection
	Uint32 m_bboxX1, m_bboxY1;
	Uint32 m_bboxX2, m_bboxY2;

private:
	SDL_Surface* m_surface;
	SDL_Surface* m_masksurface;

	Uint8 m_xsize, m_ysize;
	Uint16 m_xoffset, m_yoffset;
	Uint8 m_alpha;
};

#endif /* CSPRITE_H_ */
