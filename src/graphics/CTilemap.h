/*
 * CTilemap.h
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#ifndef CTILEMAP_H_
#define CTILEMAP_H_

#define MAX_TILES    800

#include <SDL.h>
#include <string>

class CTilemap {
public:
	CTilemap();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags);
	bool loadHiresTile( const std::string& filename );
	bool optimizeSurface();
	SDL_Surface *getSDLSurface();

	int EmptyBackgroundTile() { return m_EmptyBackgroundTile; }

	void drawTile(SDL_Surface *dst, Uint16 x, Uint16 y, Uint16 t);

	virtual ~CTilemap();

private:
	SDL_Surface *m_Tilesurface;
	int m_EmptyBackgroundTile;
};
#endif /* CTILEMAP_H_ */
