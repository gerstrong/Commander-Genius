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

typedef struct
{
	int masktile;        // if nonzero, specifies a mask for this tile
	int chgtile;         // tile to change to when level completed (for wm)
	// or tile to change to when picked up (in-level)
	unsigned int animOffset;   // starting offset from the base frame

	// Tile Properties start here!
	char animation;
	char behaviour;
	char bup,bright,bdown,bleft;
} stTile;

class CTilemap {
public:
	CTilemap(stTile *pTileProperties, int numtiles);
	virtual ~CTilemap();

	bool CreateSurface(SDL_Color *Palette, Uint32 Flags);
	bool loadHiresTile( const std::string& filename );
	bool optimizeSurface();
	SDL_Surface *getSDLSurface();

	int EmptyBackgroundTile() { return m_EmptyBackgroundTile; }

	void drawTile(SDL_Surface *dst, Uint16 x, Uint16 y, Uint16 t);

	stTile *mp_tiles;

	int m_numtiles;

private:
	SDL_Surface *m_Tilesurface;
	int m_EmptyBackgroundTile;
};
#endif /* CTILEMAP_H_ */
