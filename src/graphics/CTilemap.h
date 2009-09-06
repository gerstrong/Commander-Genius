/*
 * CTilemap.h
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#ifndef CTILEMAP_H_
#define CTILEMAP_H_

#include <SDL/SDL.h>

class CTilemap {
public:
	CTilemap();
	virtual ~CTilemap();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags);
	bool optimizeSurface();
	SDL_Surface *getSDLSurface();

	void drawTile(SDL_Surface *dst, Uint16 x, Uint16 y, Uint16 t);

private:
	SDL_Surface *m_Tilesurface;
};
#endif /* CTILEMAP_H_ */
