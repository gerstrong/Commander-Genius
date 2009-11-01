/*
 * CTilemap.h
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

#ifndef CTILEMAP_H_
#define CTILEMAP_H_

// animation rate of animated tiles
#define ANIM_TILE_TIME        8
#define MAX_ANIMTILES  200

// for each entry in the animtileinuse array that is nonzero, that
// location on the display is an animated tile which is currently registered
// in animtiles[]. Used in map_draw_hstripe and map_draw_vstripe.
// When drawing a new stripe over one that previously contained an animated
// tile, this lets it know it needs to unregister the animated tile that
// used to be there. the nonzero value corresponds to the associated entry
// in animtiles[]. the x,y pixel position is the index in here * 16.
#define ATILEINUSE_SIZEX               33
#define ATILEINUSE_SIZEY               33

#define MAX_TILES    800

#include <SDL/SDL.h>
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
	
	void drawTile(SDL_Surface *dst, Uint16 x, Uint16 y, Uint16 t);
	
	void animateAllTiles(SDL_Surface *dst);
	void unregisterAnimtiles(int tile);
	void registerAnimation(Uint32 x, Uint32 y, int c);
	void deAnimateAt(Uint16 px, Uint16 py);
	
	stTile *mp_tiles;
	
	int m_numtiles;
	
private:
	SDL_Surface *m_Tilesurface;
	
	// (map) stripe attribute structures, for animated tiles
	// slot 0 is not used. data starts at slot 1. see description
	// of AnimTileInUse in map structure to see why.
	struct {
		bool slotinuse;        // if 0, this entry should not be drawn
		int x;                // x pixel position in scrollbuf[] where tile is
		int y;                // y pixel position in scrollbuf[]
		int baseframe;        // base frame, i.e. the first frame of animation
		int offset;           // offset from base frame
	} m_animtiles[MAX_ANIMTILES+1];
	
	unsigned int m_AnimTileInUse[ATILEINUSE_SIZEX][ATILEINUSE_SIZEY];
	int m_animtiletimer, m_curanimtileframe;
};
#endif /* CTILEMAP_H_ */
