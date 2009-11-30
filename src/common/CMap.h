/*
 * CMap.h
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#ifndef CMAP_H_
#define CMAP_H_

#include <SDL.h>
#include <string>

#include "../graphics/CTilemap.h"

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

class CMap {
public:
	CMap( SDL_Surface *p_scrollsurface, CTilemap *p_Tilemap);

	void setSDLSurface( SDL_Surface *p_scrollsurface ) { mp_scrollsurface=p_scrollsurface; }

	bool gotoPos( int x, int y );
	void resetScrolls();
	void scrollLeft(void);
	void scrollRight(void);
	void scrollUp(void);
	void scrollDown(void);

	void redrawAt(int mx, int my);
	void drawAll();
	void drawHstripe( unsigned int y, unsigned int mpy );
	void drawVstripe( unsigned int x, unsigned int mpx );

	Uint16 at(Uint16 x, Uint16 y);
	Uint16 getObjectat(Uint16 x, Uint16 y);
	bool findObject(unsigned int obj, int *xout, int *yout);
	bool findTile(unsigned int tile, int *xout, int *yout);

	bool setTile(Uint16 x, Uint16 y, Uint16 t);
	bool setTile(Uint16 x, Uint16 y, Uint16 t, bool update);
	bool changeTile(Uint16 x, Uint16 y, Uint16 t);

	// Animation methods
	void deAnimate(int x, int y);
	void animateAllTiles();
	void unregisterAnimtiles(int tile);
	void registerAnimation(Uint32 x, Uint32 y, int c);

	unsigned int getlevelat(unsigned int x, unsigned int y)	{
		return m_objectlayer[x>>4][y>>4];	}

	virtual ~CMap();


	Sint16 m_scrollx_buf;		// Amount to move surface in X.
	Sint16 m_scrolly_buf; 		// Amount to move surface in Y.

	Uint16 m_scrollx;      	// total amount of X scroll
	Uint16 m_scrolly;    		// amount the scroll buffer is scrolled(y)

	Uint16 *mp_data;       // the map data
	// in-game, contains monsters and special object tags like for switches
	// on world map contains level numbers and flags for things like teleporters
	Uint16 m_objectlayer[256][256];
	// TODO: Still need changes, because it shouldn't limit up to 256x256
	Uint32 m_width, m_height;            // size of the map
	bool m_worldmap;             // if 1, this is the world map

	unsigned int m_maxscrollx, m_maxscrolly;
	std::string m_gamepath;

	bool m_animation_enabled;

	stTile *mp_tiles;

private:

	Uint8 m_scrollpix;     	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapx;           	// map X location shown at scrollbuffer row 0
	Uint16 m_mapxstripepos;  	// X pixel position of next stripe row

	Uint8 m_scrollpixy;    	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapy;         	// map Y location shown at scrollbuffer column 0
	Uint16 m_mapystripepos;  	// Y pixel position of next stripe column

	SDL_Surface *mp_scrollsurface;
	CTilemap *mp_Tilemap;

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

#endif /* CMAP_H_ */
