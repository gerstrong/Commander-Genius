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
	CTileProperties();

	int chgtile;         // tile to change to when level completed (for wm)
	// or tile to change to when picked up (in-level)
	//unsigned int animOffset;   // starting offset from the base frame
	int nextTile;

	// Tile Properties start here!
	unsigned char animationtime;
	char behaviour;
	char bup,bright,bdown,bleft;
	char slippery;
private:
};

#endif /* CTILEPROPERTIES_H_ */
