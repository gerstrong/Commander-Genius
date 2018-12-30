/*
 * CEGAGraphicsVort.h
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#ifndef CEGAGRAPHICSVORT_H_
#define CEGAGRAPHICSVORT_H_

#include "CEGALatch.h"
#include "CEGASprit.h"
#include "engine/core/CResourceLoader.h"

namespace vorticon
{

class CEGAGraphicsVort
{
public:

    CEGAGraphicsVort(CResourceLoaderBackground &loader) :
        mLoader(loader) {}


    bool loadData( int version,
                   unsigned char *p_exedata);
	short getNumTiles();
	int getNumSprites();

	virtual ~CEGAGraphicsVort();

private:
	// Part of the EGAHEAD Data Structure
	// Section 1:
	long LatchPlaneSize = 0;              	//Size of one plane of latch data
	long SpritePlaneSize = 0;              	//Size of one plane of sprite data
	long BitmapTableStart = 0;				   		//Start locations of the EGAHEAD entries for unmasked graphics (Excluding font and tiles.)
	long SpriteStart = 0;					//Where in the EGAHEAD the entries for masked graphics (Sprites) start

	short FontTiles = 0;						//Number of 8x8 tiles. Meant for fonts
	long FontLocation = 0;					//Font start location (relative to plane data, but normally 0)
	long ScreenTiles = 0;                   	//Number of Screengraphics 32x32 tiles (always 0, because they were removed)
	long ScreenLocation = 0;                 //Offset of 32x32 tiles (relative to plane data)
	short Num16Tiles = 0;                   	//Number of 16x16 tiles
	long Tiles16Location = 0;                //Offset of 16x16 tiles (relative to plane data)
	short NumBitmaps = 0;                   	//Number of bitmaps in table
	long BitmapLocation = 0;                 //Offset of bitmaps (relative to plane data)
	short NumSprites = 0;                   	//Number of sprites
	long SpriteLocation = 0;                 //Offset of sprites (relative to plane data)
	short compressed = 0;                    // LZ compressed data

	// Section 2 is in CEGALatch and section 3 is in EGASprit where also are the to be used graphics
	// Here are which will be created, when object of class initialized
	CEGALatch *m_Latch = nullptr;
	CEGASprit *m_Sprit = nullptr;

	SDL_Surface *m_FontSurface = nullptr;		// 8x8 Tiles
	SDL_Surface *m_BigTileSurface = nullptr;	// May be 32x32. Never used in the original games
	SDL_Surface *m_TileSurface = nullptr;		// 16x16 Tiles
	SDL_Surface *m_BitmapsSurface = nullptr;	// Bitmaps of the games, like title screen

    CResourceLoaderBackground &mLoader;
};

}

#endif /* CEGAGRAPHICSVORT_H_ */
