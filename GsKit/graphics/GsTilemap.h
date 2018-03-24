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

    bool CreateSurface(SDL_Color *sdlPalette, Uint32 Flags,
            Uint16 numtiles, Uint16 pbasesize, Uint16 column);

    bool loadHiresTile( const std::string& filename,
                        const std::string& path );

	bool optimizeSurface();

	SDL_Surface *getSDLSurface();

	int getDimension();

	int EmptyBackgroundTile() { return m_EmptyBackgroundTile; }

    void drawTile(SDL_Surface *dst,
                  const int x,
                  const int y,
                  const Uint16 t);

    void drawTile(GsWeakSurface &dst,
                  const int x,
                  const int y,
                  const Uint16 t);


    void drawTileBlended(SDL_Surface *dst,
                         const int x, const int y,
                         const Uint16 t, const Uint8 alpha);

    void applyGalaxyHiColourMask();

private:

    GsSurface mTileSurface;
    GsSurface mTileSurfaceAlpha;

    int m_EmptyBackgroundTile = 143;
    Uint16 m_numtiles = 0;
    Uint16 m_pbasesize = 0;
    Uint16 m_column = 0;
};
#endif /* GsTilemap_H_ */
