/*
 * CEGAGraphicsVort.h
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#ifndef CEGAGRAPHICSVORT_H_
#define CEGAGRAPHICSVORT_H_

#include "../CEGAGraphics.h"
#include "CEGALatch.h"
#include "CEGASprit.h"

namespace vorticon
{


class CEGAGraphicsVort : public CEGAGraphics
{
public:
	CEGAGraphicsVort(short episode, const std::string& path);

	bool loadData( int version, unsigned char *p_exedata );
	short getNumTiles();
	int getNumSprites();

	virtual ~CEGAGraphicsVort();

private:
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

}

#endif /* CEGAGRAPHICSVORT_H_ */
