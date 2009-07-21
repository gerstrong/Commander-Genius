/*
 * CLatch.h
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#ifndef CLATCH_H_
#define CLATCH_H_


// First declare the structure
typedef struct stEgaHead {
   long LatchPlaneSize;                //Size of one plane of latch data
   long SpritePlaneSize;               //Size of one plane of sprite data
   long OffBitmapTable;                //Offset in EGAHEAD to bitmap table
   long OffSpriteTable;                //Offset in EGAHEAD to sprite table
   short Num8Tiles;                    //Number of 8x8 tiles
   long Off8Tiles;                     //Offset of 8x8 tiles (relative to plane data)
   short Num32Tiles;                   //Number of 32x32 tiles (always 0)
   long Off32Tiles;                    //Offset of 32x32 tiles (relative to plane data)
   short Num16Tiles;                   //Number of 16x16 tiles
   long Off16Tiles;                    //Offset of 16x16 tiles (relative to plane data)
   short NumBitmaps;                   //Number of bitmaps in table
   long OffBitmaps;                    //Offset of bitmaps (relative to plane data)
   short NumSprites;                   //Number of sprites
   long OffSprites;                    //Offset of sprites (relative to plane data)
   short Compressed;                   //(Keen 1 only) Nonzero: LZ compressed data
} EgaHead;

typedef struct stBitmapHead {
   short Width;                        // width of bitmap
   short Height;                       // height of bitmap
   long Offset;                        // offset from start of bitmap data
   char Name[8];                       // bitmap name
} BitmapHead;

typedef struct stSpriteHead {
   short Width;                        // width of sprite
   short Height;                       // height of sprite
   short OffsetDelta;                  // i don't know what this is
   short OffsetParas;                  // don't know about this either
   short Rx1, Ry1, Rx2, Ry2;           // bounding box for hit detection
   char Name[16];                      // sprite name
} SpriteHead;

// The Class begins here!!
class CLatch {
public:
	CLatch();
	virtual ~CLatch();

	char loadGraphics(int episode, const char *path);

	EgaHead *getLatchHeader(void);

private:

	unsigned long BitmapBufferRAMSize;

	EgaHead LatchHeader;
	SpriteHead *SpriteTable;
	BitmapHead *BitmapTable;
	char *BitmapData;
	char *RawData;

	char loadHeader(int episode, const char *path);
	char load(int episode, const char *path);
	char loadSprites(int episode, const char *path);
	unsigned char getbit(char *buf, unsigned char plane);
	void setplanepositions(unsigned long p1, unsigned long p2, unsigned long p3,\
	                       unsigned long p4, unsigned long p5);

};

#endif /* CLATCH_H_ */
