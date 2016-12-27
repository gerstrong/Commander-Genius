/*
 * GsTilemap.h
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#ifndef GsTilemap_H_
#define GsTilemap_H_

#define MAX_TILES    800

#include <SDL.h>
#include <string>

#include <graphics/GsSurface.h>

class GsTilemap
{
public:
	GsTilemap();

    bool CreateSurface(SDL_Color *sdlPalette, Uint32 Flags,
            Uint16 numtiles, Uint16 pbasesize, Uint16 column);
	bool loadHiresTile( const std::string& filename, const std::string& path );
	bool optimizeSurface();
	SDL_Surface *getSDLSurface();

	int getDimension();

	int EmptyBackgroundTile() { return m_EmptyBackgroundTile; }

    void drawTile(SDL_Surface *dst, int x, int y, Uint16 t);

    void drawTileBlended(SDL_Surface *dst, int x, int y, Uint16 t, Uint8 amount);

    void applyGalaxyHiColourMask();

private:

    GsSurface mTileSurface;

	int m_EmptyBackgroundTile;
	Uint16 m_numtiles;
	Uint16 m_pbasesize;
	Uint16 m_column;
};
#endif /* GsTilemap_H_ */
