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
    CTileProperties() :
    chgtile(0),
    nextTile(0),
    animationtime(0),
    behaviour(0),
    bup(0),
    bright(0),
    bdown(0),
    bleft(0),
    slippery(0) {}

	int chgtile;         // tile to change to when level completed (for wm)
	// or tile to change to when picked up (in-level)
	int nextTile;

	// Tile Properties start here!
	unsigned char animationtime;
	signed char behaviour;
	signed char bup,bright,bdown,bleft;
	signed char slippery;
private:
};

#endif /* CTILEPROPERTIES_H_ */
