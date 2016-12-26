/*
 * CTileProperties.h
 *
 *  Created on: 10.06.2010
 *      Author: gerstrong
 *
 *  This new class defines the Properties of one tile
 *  For the Galaxy Engine it will be extended and replaces the
 *  old st_tiles structure
 */

#ifndef CTILEPROPERTIES_H_
#define CTILEPROPERTIES_H_

class CTileProperties
{
public:

    int chgtile = 0;         // tile to change to when level completed (for wm)
                             // or tile to change to when picked up (in-level)
    int nextTile = 0;
    int prevTile = 0;

	// Tile Properties start here!
    unsigned char animationTime = 0;
    signed char behaviour = 0;
    signed char bup = 0, bright = 0, bdown = 0, bleft = 0;
    signed char slippery = 0;

};

#endif /* CTILEPROPERTIES_H_ */
