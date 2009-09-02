/*
 * CSprite.h
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#ifndef CSPRITE_H_
#define CSPRITE_H_

#include <SDL/SDL.h>

#define COLORKEY 255

class CSprite {
public:
	CSprite();
	virtual ~CSprite();

	bool createSurface(Uint32 flags, SDL_Color *Palette);
	void setSize(Uint8 w, Uint16 h);
	void setBouncingBoxCoordinates( Uint16 bboxx1, Uint16 bboxy1, Uint16 bboxx2, Uint16 bboxy2 );
	SDL_Surface *getSDLSurface();

	void drawSprite( SDL_Surface *dst, Uint16 x, Uint16 y );
	void eraseSprite( SDL_Surface *dst,  Uint16 x, Uint16 y );

	Uint8 m_xsize, m_ysize;

private:
	// bounding box for hit detection
	Uint32 m_bboxX1, m_bboxY1;
	Uint32 m_bboxX2, m_bboxY2;

	SDL_Surface* m_surface;
};

#endif /* CSPRITE_H_ */
