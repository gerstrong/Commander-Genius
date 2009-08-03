/*
 * CEGALatch.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#include "CEGALatch.h"
#include "../sdl/CVideoDriver.h"
#include "CPlanes.h"
#include "../funcdefs.h"
#include "../keen.h"
//#include "../externals.h"
#include "../FindFile.h"
#include <stdio.h>
#include <string.h>

char *BitmapData; // TODO: Partial solution. This BitmapData must become member of this class!

CEGALatch::CEGALatch( int planesize,
						long bitmaptablelocation,
						short fonttiles,
						long fontlocation,
						long screentiles,
						long screenlocation,
						short num16tiles,
						long tiles16location,
						short bitmaps,
						long bitmaplocation)
{
	m_latchplanesize = planesize;
	m_bitmaptablelocation = bitmaptablelocation;
	m_fonttiles = fonttiles;
	m_fontlocation = fontlocation;
	m_screentiles = screentiles;
	m_screenlocation = screenlocation;
	m_num16tiles = num16tiles;
	m_tiles16location = tiles16location;
	m_bitmaps = bitmaps;
	m_bitmaplocation = bitmaplocation;

	Bitmap = NULL;

	RawData = NULL;
}

CEGALatch::~CEGALatch() {
	if(Bitmap) delete [] Bitmap, Bitmap = NULL;
	if(RawData) delete [] RawData, RawData = NULL;
}

bool CEGALatch::loadHead( char *data )
{
	data += m_bitmaptablelocation;
	Bitmap = new st_Bitmap[m_bitmaps];

	for(int i=0 ; i<m_bitmaps ; i++)
	{
		memcpy(&(Bitmap[i].width),data+16*i,2);
		memcpy(&(Bitmap[i].height),data+16*i+2,2);
		memcpy(&(Bitmap[i].location),data+16*i+4,4);
		memcpy(Bitmap[i].name,data+16*i+8,8);
		Bitmap[i].width *= 8; // The width is always divided by eight
	}

	return true;
}

bool CEGALatch::loadData(const std::string& filename, bool compresseddata)
{
	char *RawData;

	FILE* latchfile = OpenGameFile(filename.c_str(),"rb");

	if(!latchfile)
		return false;

	RawData = new char[m_latchplanesize * 4];
    // get the data out of the file into the memory, decompressing it if necessary.
    if (compresseddata)
    {
      if (lz_decompress(latchfile, (unsigned char*) RawData))
    	  return 1;
    }
    else
    {
    	for(int i=0 ; i<(m_latchplanesize*4) ; i++)
    		RawData[i] = fgetc(latchfile);
    }

    fclose(latchfile);

     // these are the offsets of the different video planes as
     // relative to each other--that is if a pixel in plane1
     // is at N, the byte for that same pixel in plane3 will be
     // at (N + plane3).
     unsigned long plane1, plane2, plane3, plane4;

     plane1 = 0;
     plane2 = (m_latchplanesize * 1);
     plane3 = (m_latchplanesize * 2);
     plane4 = (m_latchplanesize * 3);

     // ** read the 8x8 tiles **

     // set up the getbit() function of CPlanes class
     CPlanes *Planes = new CPlanes(plane1 + m_fontlocation,
							 plane2 + m_fontlocation,
							 plane3 + m_fontlocation,
							 plane4 + m_fontlocation,
							 0);

	// Load 8x8 Tiles
     unsigned char c=0;
     for(int p=0;p<4;p++)
     {
       for(int t=0;t<m_fonttiles;t++)
       {
         for(int y=0;y<8;y++)
         {
           for(int x=0;x<8;x++)
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

     // Load 32x32 Tiles

     // TODO: Add a read function for 32x32 Tiles

     // Load 16x16 Tiles
     Planes = new CPlanes(plane1 + m_tiles16location,
                       plane2 + m_tiles16location,
                       plane3 + m_tiles16location,
                       plane4 + m_tiles16location,
                       0);

     for(int p=0;p<4;p++)
     {
       for(int t=0;t<m_num16tiles;t++)
       {
         for(int y=0;y<16;y++)
         {
           for(int x=0;x<16;x++)
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

     // Load Bitmaps
     // figure out how much RAM we'll need to read all 4 planes of
     // latch data into memory.
     BitmapBufferRAMSize = 0;
     for(int i=0;i<m_bitmaps;i++)
     {
       // keep a tally of the bitmap sizes so we'll know how much RAM we have
       // to allocate for all of the bitmaps once they're decoded
       BitmapBufferRAMSize += (Bitmap[i].width *
							   Bitmap[i].height);
     }
     BitmapBufferRAMSize++;

     BitmapData = new char[BitmapBufferRAMSize];

     // set up the getbit() function
     Planes = new CPlanes(plane1 + m_bitmaplocation,
                       plane2 + m_bitmaplocation,
                       plane3 + m_bitmaplocation,
                       plane4 + m_bitmaplocation,
                       0);

     // decode bitmaps into the BitmapData structure. The bitmaps are
     // loaded into one continuous stream of image data, with the bitmaps[]
     // array giving pointers to where each bitmap starts within the stream.


     const char defbitmapname[][9] = { "TITLE", "IDLOGO", "F1HELP", "HIGHSCOR",
   		  "NAME", "SCORE", "PARTS", "GAMEOVER", "AN", "PRESENT", "APOGEE", "KEENSHIP", "WINDON",
   		  "WINDOFF", "ONEMOMEN", "OFAN", "PRODUCT", "IDSOFT"};

     // In case there is a strange mod or defect episode, put some names to it!

     char *bmdataptr;

     for(int p=0 ; p<4 ; p++)
     {
       // this points to the location that we're currently
       // decoding bitmap data to
       bmdataptr = &BitmapData[0];

       for(int b=0 ; b<m_bitmaps ; b++)
       {
         bitmaps[b].xsize = Bitmap[b].width;
         bitmaps[b].ysize = Bitmap[b].height;
         if( Bitmap[b].name[0] == 0 && b<18 )
        	 strcpy(bitmaps[b].name,defbitmapname[b]);
         else
        	 memcpy(bitmaps[b].name, Bitmap[b].name, 8);
         bitmaps[b].name[8] = 0;  //ensure null-terminated

         bitmaps[b].bmptr =  (unsigned char*) bmdataptr;

         for(int y=0 ; y<bitmaps[b].ysize ; y++)
         {
           for(int x=0 ; x<bitmaps[b].xsize ; x++)
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

	return true;
}
