/*
 * CEGAGraphics.h
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 *
 *  This Class carries the head of graphics
 *  which are generally read from EGAHEAD.CK?
 *  It also has uses two other clases, which
 *  have the individual task to read its stuff
 *  (EGASPRIT and EGALATCH)
 */

#ifndef CEGAGRAPHICS_H_
#define CEGAGRAPHICS_H_

#include <string>
#include "CEGALatch.h"
#include "CEGASprit.h"

class CEGAGraphics {
public:
	CEGAGraphics(short episode, const std::string& path);
	virtual ~CEGAGraphics();

	bool loadData();

	int getNumSprites();

private:
	short m_episode;
	std::string m_path;

	// Part of the EGAHEAD Data Structure
	// Section 1:
	   long LatchPlaneSize;              	//Size of one plane of latch data
	   long SpritePlaneSize;              	//Size of one plane of sprite data
	   long BitmapTableStart;				   		//Start locations of the EGAHEAD entries for unmasked graphics (Excluding font and tiles.)
	   long SpriteStart;					//Where in the EGAHEAD the entries for masked graphics (Sprites) start

	   short FontTiles;						//Number of 8x8 tiles. Meant for fonts
	   long FontLocation;					//Font start location (relative to plane data, but normally 0)
	   long ScreenTiles;                   	//Number of Screengraphics 32x32 tiles (always 0, because they were removed)
	   long ScreenLocation;                 //Offset of 32x32 tiles (relative to plane data)
	   short Num16Tiles;                   	//Number of 16x16 tiles
	   long Tiles16Location;                //Offset of 16x16 tiles (relative to plane data)
	   short NumBitmaps;                   	//Number of bitmaps in table
	   long BitmapLocation;                 //Offset of bitmaps (relative to plane data)
	   short NumSprites;                   	//Number of sprites
	   long SpriteLocation;                 //Offset of sprites (relative to plane data)
	   short compressed;                    // LZ compressed data

	 // Section 2 is in CEGALatch and section 3 is in EGASprit where also are the to be used graphics
	 // Here are which will be created, when object of class initialized
	   CEGALatch *m_Latch;
	   CEGASprit *m_Sprit;

	SDL_Surface *m_FontSurface;		// 8x8 Tiles
	SDL_Surface *m_BigTileSurface;	// May be 32x32. Never used in the original games
	SDL_Surface *m_TileSurface;		// 16x16 Tiles
	SDL_Surface *m_BitmapsSurface;	// Bitmaps of the games, like title screen
};

#endif /* CEGAGRAPHICS_H_ */
