/*
 * CMap.h
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#ifndef CMAP_H_
#define CMAP_H_

#include <SDL/SDL.h>
#include <string>

#include "../graphics/CTilemap.h"

class CMap {
public:
	CMap( SDL_Surface *p_scrollsurface, CTilemap *p_Tilemap);
	virtual ~CMap();
	
	bool gotoPos( int x, int y );
	void scrollLeft(void);
	void scrollRight(void);
	void scrollUp(void);
	void scrollDown(void);
	
	void redrawAt(int mx, int my);
	void drawAll();
	void drawHstripe( unsigned int y, unsigned int mpy );
	void drawVstripe( unsigned int x, unsigned int mpx );
	void deAnimate(int x, int y);
	
	Uint16 at(Uint16 x, Uint16 y);
	Uint16 getObjectat(Uint16 x, Uint16 y);
	bool findObject(unsigned int obj, int *xout, int *yout);
	
	bool setTile(Uint16 x, Uint16 y, Uint16 t);
	bool setTile(Uint16 x, Uint16 y, Uint16 t, bool update);
	bool changeTile(Uint16 x, Uint16 y, Uint16 t);
	
	unsigned int getlevelat(unsigned int x, unsigned int y)	{
		return m_objectlayer[x>>4][y>>4];	}
	
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
private:
	
	Uint8 m_scrollpix;     	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapx;           	// map X location shown at scrollbuffer row 0
	Uint16 m_mapxstripepos;  	// X pixel position of next stripe row
	
	Uint8 m_scrollpixy;    	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapy;         	// map Y location shown at scrollbuffer column 0
	Uint16 m_mapystripepos;  	// Y pixel position of next stripe column
	
	SDL_Surface *mp_scrollsurface;
	CTilemap *mp_Tilemap;
};

#endif /* CMAP_H_ */
