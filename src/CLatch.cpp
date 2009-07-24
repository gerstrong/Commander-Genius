/*
 * CLatch.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "CLatch.h"
#include "vorticon/CPlanes.h"
#include "fileio.h"
#include "keen.h"
#include "keenext.h"
#include "StringUtils.h"

#include "CLogFile.h"
//#include "vorticon/CEGAGraphics.h"

CLatch::CLatch() {
	SpriteTable = NULL;
	BitmapTable = NULL;
	BitmapData 	= NULL;
	RawData		= NULL;
}

CLatch::~CLatch() {

	if (SpriteTable){ delete[] SpriteTable; SpriteTable = NULL;}
	if (BitmapTable){ delete[] BitmapTable; BitmapTable = NULL;}
	if (RawData){ delete[] RawData; RawData = NULL;}
	if (BitmapData)
	{
	   delete[] BitmapData; BitmapData = NULL;
	   g_pLogFile->textOut(BLACK," Bitmap data released to system.<br>");
	}
}

char CLatch::loadGraphics(int episode, const char *path)
{
   int retval = 0;

   if (SpriteTable){ delete[] SpriteTable; SpriteTable = NULL;}
   if (BitmapTable){ delete[] BitmapTable; BitmapTable = NULL;}
   if (BitmapData){ delete[] BitmapData; BitmapData = NULL;}
   if (RawData){ delete[] RawData; RawData = NULL;}

   if (loadHeader(episode, path)) { retval = 1; }
   if (load(episode, path)) { retval = 1;  }
   if (loadSprites(episode, path)) { retval = 1;  }

   return retval;
}

// load the EGAHEAD file
char CLatch::loadHeader(int episode, const char *path)
{
	FILE *headfile;
	unsigned long SpriteTableRAMSize;
	unsigned long BitmapTableRAMSize;
	std::string buf;
	int i,j,k;
	std::string fname;
	std::string buffer;

	buffer = formatPathString(path);
	buffer += "egahead.ck";

    fname = buffer + itoa(episode);

    /*CEGAGraphics *EGAGraphics;

    EGAGraphics = new CEGAGraphics(episode);

    EGAGraphics->loadData();

    delete EGAGraphics;*/

    headfile = fopen(fname.c_str(), "rb");
    if (!headfile)
    {
    	g_pLogFile->ftextOut("latch_loadheader(): unable to open '%s'.<br>", fname.c_str());
    	return 1;
    }

    g_pLogFile->ftextOut("latch_loadheader(): reading main header from '%s'...<br>", fname.c_str());

    // read the main header data from EGAHEAD
    LatchHeader.LatchPlaneSize = fgetl(headfile);
    LatchHeader.SpritePlaneSize = fgetl(headfile);
    LatchHeader.OffBitmapTable = fgetl(headfile);
    LatchHeader.OffSpriteTable = fgetl(headfile);
    LatchHeader.Num8Tiles = fgeti(headfile);
    LatchHeader.Off8Tiles = fgetl(headfile);
    LatchHeader.Num32Tiles = fgeti(headfile);
    LatchHeader.Off32Tiles = fgetl(headfile);
    LatchHeader.Num16Tiles = fgeti(headfile);
    LatchHeader.Off16Tiles = fgetl(headfile);
    LatchHeader.NumBitmaps = fgeti(headfile);
    LatchHeader.OffBitmaps = fgetl(headfile);
    LatchHeader.NumSprites = fgeti(headfile);
    LatchHeader.OffSprites = fgetl(headfile);
    LatchHeader.Compressed = fgeti(headfile);

    g_pLogFile->ftextOut("   LatchPlaneSize = 0x%04x<br>", LatchHeader.LatchPlaneSize);
    g_pLogFile->ftextOut("   SpritePlaneSize = 0x%04x<br>", LatchHeader.SpritePlaneSize);
    g_pLogFile->ftextOut("   OffBitmapTable = 0x%04x<br>", LatchHeader.OffBitmapTable);
    g_pLogFile->ftextOut("   OffSpriteTable = 0x%04x<br>", LatchHeader.OffSpriteTable);
    g_pLogFile->ftextOut("   Num8Tiles = %d<br>", LatchHeader.Num8Tiles);
    g_pLogFile->ftextOut("   Off8Tiles = 0x%04x<br>", LatchHeader.Off8Tiles);
    g_pLogFile->ftextOut("   Num32Tiles = %d<br>", LatchHeader.Num32Tiles);
    g_pLogFile->ftextOut("   Off32Tiles = 0x%04x<br>", LatchHeader.Off32Tiles);
    g_pLogFile->ftextOut("   Num16Tiles = %d<br>", LatchHeader.Num16Tiles);
    g_pLogFile->ftextOut("   Off16Tiles = 0x%04x<br>", LatchHeader.Off16Tiles);
    g_pLogFile->ftextOut("   NumBitmaps = %d<br>", LatchHeader.NumBitmaps);
    g_pLogFile->ftextOut("   OffBitmaps = 0x%04x<br>", LatchHeader.OffBitmaps);
    g_pLogFile->ftextOut("   NumSprites = %d<br>", LatchHeader.NumSprites);
    g_pLogFile->ftextOut("   OffSprites = 0x%04x<br>", LatchHeader.OffSprites);
    g_pLogFile->ftextOut("   Compressed = %d<br>", LatchHeader.Compressed);

    /// read in the sprite table

    // allocate memory for the sprite table
    SpriteTableRAMSize = sizeof(SpriteHead) * (LatchHeader.NumSprites + 1);
    g_pLogFile->ftextOut("latch_loadheader(): Allocating %d bytes for sprite table.<br>", SpriteTableRAMSize);

    SpriteTable = new SpriteHead[SpriteTableRAMSize];
    if (!SpriteTable)
    {
      g_pLogFile->ftextOut("latch_loadheader(): Can't allocate sprite table!<br>");
      return 1;
    }

    g_pLogFile->ftextOut("latch_loadheader(): Reading sprite table from '%s'...<br>", fname.c_str());

    fseek(headfile, LatchHeader.OffSpriteTable, SEEK_SET);
    for(i=0;i<LatchHeader.NumSprites;i++)
    {
      SpriteTable[i].Width = fgeti(headfile) * 8;
      SpriteTable[i].Height = fgeti(headfile);
      SpriteTable[i].OffsetDelta = fgeti(headfile);
      SpriteTable[i].OffsetParas = fgeti(headfile);
      SpriteTable[i].Rx1 = (fgeti(headfile) >> 8);
      SpriteTable[i].Ry1 = (fgeti(headfile) >> 8);
      SpriteTable[i].Rx2 = (fgeti(headfile) >> 8);
      SpriteTable[i].Ry2 = (fgeti(headfile) >> 8);
      for(j=0;j<16;j++) SpriteTable[i].Name[j] = fgetc(headfile);
      // for some reason each sprite occurs 4 times in the table.
      // we're only interested in the first occurance.
      // These are copies for smoother rendering. Not needed since SDL and HW_SURFACE
      for(j=0;j<3;j++)
      {
        for(k=0;k < static_cast<int>(sizeof(SpriteHead));k++) fgetc(headfile);
      }

    }

    // read in the bitmap table

    // allocate memory for the bitmap table
    BitmapTableRAMSize = sizeof(BitmapHead) * (LatchHeader.NumBitmaps + 1);
    g_pLogFile->ftextOut("latch_loadheader(): Allocating %d bytes for bitmap table.<br>", BitmapTableRAMSize);

    BitmapTable = new BitmapHead[BitmapTableRAMSize];
    if (!BitmapTable)
    {
      g_pLogFile->ftextOut("latch_loadheader(): Can't allocate bitmap table!<br>");
      return 1;
    }

    g_pLogFile->ftextOut("latch_loadheader(): reading bitmap table from '%s'...<br>", fname.c_str());

    fseek(headfile, LatchHeader.OffBitmapTable, SEEK_SET);

    BitmapBufferRAMSize = 0;
    for(i=0;i<LatchHeader.NumBitmaps;i++)
    {
      BitmapTable[i].Width = fgeti(headfile) * 8;
      BitmapTable[i].Height = fgeti(headfile);
      BitmapTable[i].Offset = fgetl(headfile);
      for(j=0;j<8;j++) BitmapTable[i].Name[j] = fgetc(headfile);

      // keep a tally of the bitmap sizes so we'll know how much RAM we have
      // to allocate for all of the bitmaps once they're decoded
      BitmapBufferRAMSize += (BitmapTable[i].Width * BitmapTable[i].Height);

      // print the bitmap info to the console for debug
      for(j=0;j<8;j++) buf[j] = BitmapTable[i].Name[j];
      buf[j] = 0;
      g_pLogFile->ftextOut("   Bitmap '%s': %dx%d at offset %04x. RAMAllocSize=0x%04x<br>", buf.c_str(),BitmapTable[i].Width,BitmapTable[i].Height,BitmapTable[i].Offset,BitmapBufferRAMSize);
    }
    BitmapBufferRAMSize++;

    fclose(headfile);
    return 0;
}


// load the EGALATCH file

char CLatch::load(int episode, const char *path)
{
FILE *latchfile;
unsigned long plane1, plane2, plane3, plane4;
	std::string fname;
int x,y,t,b,c,p;
char *bmdataptr;
unsigned long RawDataSize;
//unsigned char ch;

	std::string buffer;

	buffer = formatPathString(path);
	buffer += "egalatch.ck";
	fname = buffer + itoa(episode);

    g_pLogFile->ftextOut("latch_loadlatch(): Opening file '%s'.<br>", fname.c_str());

    latchfile = fopen(fname.c_str(), "rb");
    if (!latchfile)
    {
      g_pLogFile->ftextOut("latch_loadlatch(): Unable to open '%s'!<br>", fname.c_str());
      return 1;
    }

    // figure out how much RAM we'll need to read all 4 planes of
    // latch data into memory.
    RawDataSize = (LatchHeader.LatchPlaneSize * 4);
    RawData = new char[RawDataSize];
    if (!RawData)
    {
      g_pLogFile->ftextOut("latch_loadlatch(): Unable to allocate RawData buffer!<br>");
      return 1;
    }

    // get the data out of the file into memory, decompressing if necessary.
    if (LatchHeader.Compressed)
    {
      g_pLogFile->ftextOut("latch_loadlatch(): Decompressing...<br>");
      if (lz_decompress(latchfile, (unsigned char*) RawData)) return 1;
    }
    else
    {
      g_pLogFile->ftextOut("latch_loadlatch(): Reading %d bytes...<br>", RawDataSize);
      if(!fread(RawData, 1, RawDataSize, latchfile))
      {
    	  g_pLogFile->ftextOut(RED,"latch_loadlatch(): Error reading the file...<br>");
    	  return 1;
      }
    }
    fclose(latchfile);

    // these are the offsets of the different video planes as
    // relative to each other--that is if a pixel in plane1
    // is at N, the byte for that same pixel in plane3 will be
    // at (N + plane3).
    plane1 = 0;
    plane2 = (LatchHeader.LatchPlaneSize * 1);
    plane3 = (LatchHeader.LatchPlaneSize * 2);
    plane4 = (LatchHeader.LatchPlaneSize * 3);

    // ** read the 8x8 tiles **
    g_pLogFile->ftextOut("latch_loadlatch(): Decoding 8x8 tiles...<br>", fname.c_str());

    // set up the getbit() function

    CPlanes *Planes = new CPlanes(plane1 + LatchHeader.Off8Tiles, \
                      plane2 + LatchHeader.Off8Tiles, \
                      plane3 + LatchHeader.Off8Tiles, \
                      plane4 + LatchHeader.Off8Tiles, \
                      0);

    for(p=0;p<4;p++)
    {
      for(t=0;t<LatchHeader.Num8Tiles;t++)
      {
        for(y=0;y<8;y++)
        {
          for(x=0;x<8;x++)
          {
            // if we're on the first plane start with black,
            // else merge with the previously accumulated data
            if (p==0)
            {
              c = 0;
            }
            else
            {
              c = font[t][y][x];
            }
            // read a bit out of the current plane, shift it into the
            // correct position and merge it
            c |= (Planes->getbit(RawData, p) << p);
            // map black pixels to color 16 because of the way the
            // vorticon death sequence works in ep1
            if (p==3 && c==0) c = 16;
            font[t][y][x] = c;
          }
        }
      }
    }

    delete Planes;

    // ** read the 16x16 tiles **
    g_pLogFile->ftextOut("latch_loadlatch(): Decoding 16x16 tiles...<br>", fname.c_str());

    // set up the getbit() function
    Planes = new CPlanes(plane1 + LatchHeader.Off16Tiles, \
                      plane2 + LatchHeader.Off16Tiles, \
                      plane3 + LatchHeader.Off16Tiles, \
                      plane4 + LatchHeader.Off16Tiles, \
                      0);

    for(p=0;p<4;p++)
    {
      for(t=0;t<LatchHeader.Num16Tiles;t++)
      {
        for(y=0;y<16;y++)
        {
          for(x=0;x<16;x++)
          {
            if (p==0)
            {
              c = 0;
            }
            else
            {
              c = tiledata[t][y][x];
            }
            c |= (Planes->getbit(RawData, p) << p);
            if (p==3 && c==0) c = 16;

           	tiledata[t][y][x] = c;
          }
        }
      }
    }

    delete Planes;

    // ** read the bitmaps **
    g_pLogFile->ftextOut("latch_loadlatch(): Allocating %d bytes for bitmap data...<br>", BitmapBufferRAMSize);
    BitmapData = new char[BitmapBufferRAMSize];
    if (!BitmapData)
    {
      g_pLogFile->ftextOut("Cannot allocate memory for bitmaps.<br>");
      return 1;
    }

    g_pLogFile->ftextOut("latch_loadlatch(): Decoding bitmaps...<br>", fname.c_str());

    // set up the getbit() function
    Planes = new CPlanes(plane1 + LatchHeader.OffBitmaps, \
                      plane2 + LatchHeader.OffBitmaps, \
                      plane3 + LatchHeader.OffBitmaps, \
                      plane4 + LatchHeader.OffBitmaps, \
                      0);

    // decode bitmaps into the BitmapData structure. The bitmaps are
    // loaded into one continous stream of image data, with the bitmaps[]
    // array giving pointers to where each bitmap starts within the stream.

    for(p=0;p<4;p++)
    {
      // this points to the location that we're currently
      // decoding bitmap data to
      bmdataptr = &BitmapData[0];

      for(b=0;b<LatchHeader.NumBitmaps;b++)
      {
        bitmaps[b].xsize = BitmapTable[b].Width;
        bitmaps[b].ysize = BitmapTable[b].Height;
        bitmaps[b].bmptr =  (unsigned char*) bmdataptr;
        memcpy(&bitmaps[b].name[0], &BitmapTable[b].Name[0], 8);
        bitmaps[b].name[8] = 0;  //ensure null-terminated

        for(y=0;y<bitmaps[b].ysize;y++)
        {
          for(x=0;x<bitmaps[b].xsize;x++)
          {
            if (p==0)
            {
              c = 0;
            }
            else
            {
              c = *bmdataptr;
            }
            c |= (Planes->getbit(RawData, p) << p);
            if (p==3 && c==0) c = 16;
            *bmdataptr = c;
            bmdataptr++;
          }
        }
      }
    }

    delete Planes;

    if(RawData){ delete[] RawData; RawData = NULL;}

    return 0;
}



// retrieves a bit from plane "plane". the positions of the planes
// should have been previously initilized with setplanepositions()


char CLatch::loadSprites(int episode, const char *path)
{
FILE *spritfile;
unsigned long plane1, plane2, plane3, plane4, plane5;
	std::string fname;
int x,y,s,c,p;
unsigned long RawDataSize;
	std::string buffer;
CPlanes *Planes;

	buffer = formatPathString(path);
	buffer += "egasprit.ck";
	fname = buffer + itoa(episode);

    g_pLogFile->ftextOut("latch_loadsprites(): Opening file '%s'.<br>", fname.c_str());

    spritfile = fopen(fname.c_str(), "rb");
    if (!spritfile)
    {
      g_pLogFile->ftextOut("latch_loadsprites(): Unable to open '%s'!<br>", fname.c_str());
      return 1;
    }

    RawDataSize = (LatchHeader.SpritePlaneSize * 5);
    RawData = new char[RawDataSize];
    if (!RawData)
    {
      g_pLogFile->ftextOut("latch_loadlatch(): Unable to allocate RawData buffer!<br>");
      return 1;
    }

    if (LatchHeader.Compressed)
    {
      g_pLogFile->ftextOut("latch_loadsprites(): Decompressing...<br>");
      if (lz_decompress(spritfile, (unsigned char*) RawData)) return 1;
    }
    else
    {
      g_pLogFile->ftextOut("latch_loadsprites(): Reading %d bytes...<br>", RawDataSize);
      if(!fread(RawData, 1, RawDataSize,  spritfile))
      {
    	  g_pLogFile->ftextOut(RED,"latch_loadsprites(): Error reading the file...<br>");
    	  return 1;
      }
    }
    fclose(spritfile);

    // these are the offsets of the different video planes as
    // relative to each other--that is if a pixel in plane1
    // is at N, the byte for that same pixel in plane3 will be
    // at (N + plane3).
    plane1 = 0;
    plane2 = (LatchHeader.SpritePlaneSize * 1);
    plane3 = (LatchHeader.SpritePlaneSize * 2);
    plane4 = (LatchHeader.SpritePlaneSize * 3);
    plane5 = (LatchHeader.SpritePlaneSize * 4);

    // ** read the sprites **
    g_pLogFile->ftextOut("latch_loadsprites(): Decoding sprites...<br>", fname.c_str());

    // set up the getbit() function
    Planes = new CPlanes(plane1 + LatchHeader.OffSprites, \
                      plane2 + LatchHeader.OffSprites, \
                      plane3 + LatchHeader.OffSprites, \
                      plane4 + LatchHeader.OffSprites, \
                      plane5 + LatchHeader.OffSprites);

    // load the image data
    for(p=0;p<4;p++)
    {
      for(s=0;s<LatchHeader.NumSprites;s++)
      {
        sprites[s].xsize = SpriteTable[s].Width;
        sprites[s].ysize = SpriteTable[s].Height;
        sprites[s].bboxX1 = (SpriteTable[s].Rx1 << CSF);
        sprites[s].bboxY1 = (SpriteTable[s].Ry1 << CSF);
        sprites[s].bboxX2 = (SpriteTable[s].Rx2 << CSF);
        sprites[s].bboxY2 = (SpriteTable[s].Ry2 << CSF);
        for(y=0;y<sprites[s].ysize;y++)
        {
          for(x=0;x<sprites[s].xsize;x++)
          {
            if (p==0)
            {
              c = 0;
            }
            else
            {
              c = sprites[s].imgdata[y][x];
            }
            c |= (Planes->getbit(RawData, p) << p);
            if (p==3 && c==0) c = 16;
            sprites[s].imgdata[y][x] = c;
          }
        }
      }
    }

    // now load the 5th plane, which contains the sprite masks.
    // note that we invert the mask because our graphics functions
    // use white on black masks whereas keen uses black on white.
    for(s=0;s<LatchHeader.NumSprites;s++)
    {
      for(y=0;y<sprites[s].ysize;y++)
      {
        for(x=0;x<sprites[s].xsize;x++)
        {
           sprites[s].maskdata[y][x] = (1 - Planes->getbit(RawData, 4));
        }
      }
    }

    delete Planes;

    return 0;
}

EgaHead *CLatch::getLatchHeader(void){ return &LatchHeader;}

