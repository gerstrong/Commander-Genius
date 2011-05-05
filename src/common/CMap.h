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
#include <vector>
#include <list>

#include "graphics/CTilemap.h"
#include "fileio/TypeDefinitions.h"
#include "CPlane.h"

// animation rate of animated tiles
#define ANIM_TILE_TIME      256

#define CSF    9
#define TILE_S			4
#define STC (CSF-TILE_S)


class CMap {
public:
	CMap();

	void setTileMap( CTilemap &Tilemap );

	void setLevel(Uint16 Level);
	Uint16 getLevel();

	void setLevelName( const std::string& name );
	std::string getLevelName();

	bool createEmptyDataPlane(size_t plane, Uint32 width, Uint32 height);

	bool gotoPos( int x, int y );
	void resetScrolls();
	// If force is enabled it will ignore scroll blockers
	bool scrollLeft(const bool force=false);
	bool scrollRight(const bool force=false);
	bool scrollUp(const bool force=false);
	bool scrollDown(const bool force=false);

	void redrawAt(int mx, int my);
	void drawAll();
	void drawHstripe( unsigned int y, unsigned int mpy );
	void drawVstripe( unsigned int x, unsigned int mpx );
	void drawForegroundTiles();

	Uint16 at(Uint16 x, Uint16 y, Uint16 t=1);
	Uint16 getObjectat(Uint16 x, Uint16 y);
	/*
	 * \brief
	 * This will check in horizontal direction if there is a scroll blocker.
	 * As far known, this is only used in the Galaxy engine
	 * \param y The y coordinate (CSFed) of the row where to look for that blocker
	 */
	bool findScrollHorizontalScrollBlocker(const int y);
	bool findObject(unsigned int obj, int *xout, int *yout);
	bool findTile(unsigned int tile, int *xout, int *yout, int plane=1);

	bool setTile(Uint16 x, Uint16 y, Uint16 t, Uint16 plane=1);
	bool setTile(Uint16 x, Uint16 y, Uint16 t, bool update, Uint16 plane=1);
	bool changeTile(Uint16 x, Uint16 y, Uint16 t);
	void changeTileArrayY(Uint16 x, Uint16 y, Uint16 h, Uint16 tile);

	// Animation methods
	Uint8 getAnimtiletimer();
	void drawAnimatedTile(SDL_Surface *dst, Uint16 mx, Uint16 my, Uint16 tile);
	void animateAllTiles();

	unsigned int getlevelat(unsigned int x, unsigned int y)	{
		return m_objectlayer[x>>4][y>>4];	}

	Uint16 getPlaneDataAt(int plane, unsigned int x, unsigned int y){
		return m_Plane[plane].getMapDataAt(x>>CSF, y>>CSF);
	}

	word *getData(Uint8 PlaneNum);
	word *getForegroundData();
	word *getBackgroundData();

	Sint16 m_scrollx_buf;		// Amount to move surface in X.
	Sint16 m_scrolly_buf; 		// Amount to move surface in Y.

	Uint16 m_scrollx;      		// Amount of how much is scrolled on the map relative to (0,0) in X
	Uint16 m_scrolly;    		// Amount of how much is scrolled on the map relative to (0,0) in Y

	// in-game, contains monsters and special object tags like for switches
	// on world map contains level numbers and flags for things like teleporters
	Uint16 m_objectlayer[256][256];
	// TODO: Still need changes, because it shouldn't limit up to 256x256
	Uint32 m_width, m_height;            // size of the map
	bool m_worldmap;             // if 1, this is the world map

	unsigned int m_maxscrollx, m_maxscrolly;
	std::string m_gamepath;

	bool m_animation_enabled;
	bool m_Dark;
	Uint8 m_Difficulty;
	bool m_PlatExtending;

private:

	Uint8 m_scrollpix;     	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapx;           	// map X location shown at scrollbuffer row 0
	Uint16 m_mapxstripepos;  	// X pixel position of next stripe row

	Uint8 m_scrollpixy;    	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapy;         	// map Y location shown at scrollbuffer column 0
	Uint16 m_mapystripepos;  	// Y pixel position of next stripe column

	std::vector<CTilemap> &m_Tilemaps;

	Uint8 m_animtiletimer;

	CPlane m_Plane[3];
	bool m_Background; // This variable holds, whether we have a background to draw or not.
	Uint16 m_Level;
	std::string m_LevelName;
};

#endif /* CMAP_H_ */
