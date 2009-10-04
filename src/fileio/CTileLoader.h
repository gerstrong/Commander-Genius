/*
 * CTileLoader.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CTILELOADER_H_
#define CTILELOADER_H_

#define MAX_TILES    800

// Tile information planes
enum TileEmurations{
	ANIMATION,
	BEHAVIOR,
	BUP,
	BRIGHT,
	BDOWN,
	BLEFT
};

extern int numtiles;
extern int **TileProperty;

typedef struct
{
 int masktile;        // if nonzero, specifies a mask for this tile
 int chgtile;         // tile to change to when level completed (for wm)
					 // or tile to change to when picked up (in-level)
 unsigned int animOffset;   // starting offset from the base frame
} stTile;

class CTileLoader {
public:
	CTileLoader(int episode, int version, unsigned char *data);
	virtual ~CTileLoader();

	bool load();

private:
	int m_episode;
	int m_version;
	long m_offset;
	unsigned char *m_data;

	void assignChangeTileAttribute(stTile *tile);
	bool setProperOffset();
	bool canbePickedup(int tile);
	bool isaDoor(int tile);
};

#endif /* CTILELOADER_H_ */
