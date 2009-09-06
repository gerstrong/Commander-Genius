/*
 * CEGALatch.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#include "CEGALatch.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"
#include "CPlanes.h"
#include "../funcdefs.h"
#include "../keen.h"
#include "../FindFile.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include <string.h>

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

	RawData = NULL;
}

CEGALatch::~CEGALatch() {
	if(RawData) delete [] RawData, RawData = NULL;
}

bool CEGALatch::loadHead( char *data )
{
	Uint16 height, width;
	char name[9];

    const std::string default_names[] = { "TITLE", "IDLOGO", "F1HELP", "HIGHSCOR",
  		  "NAME", "SCORE", "PARTS", "GAMEOVER", "AN", "PRESENT", "APOGEE", "KEENSHIP", "WINDON",
  		  "WINDOFF", "ONEMOMEN", "OFAN", "PRODUCT", "IDSOFT" }; // in case the names are empty

	data += m_bitmaptablelocation;

	g_pGfxEngine->createEmptyBitmaps(m_bitmaps);
	for(int i=0 ; i<m_bitmaps ; i++)
	{
		memcpy(&width,data+16*i,2);
		memcpy(&height,data+16*i+2,2);
		memcpy(name,data+16*i+8,8);
		width *= 8; // The width is always divided by eight when read

		name[8] = 0; // Ensure null-terminated!
		if( name[0] != 0 ) g_pGfxEngine->Bitmap[i]->setName( name );
		else g_pGfxEngine->Bitmap[i]->setName( default_names[i] );

		g_pGfxEngine->Bitmap[i]->setDimensions(width,height);
	}

	return true;
}

bool CEGALatch::loadData(const std::string& filename, bool compresseddata)
{
	char *RawData;
    CBitmap *bitmap;
    Uint16 width, height;
    SDL_Surface *sfc;

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
     // Load these graphics into the CFont Class of CGfxEngine
     char *offset;
     CFont *Font = g_pGfxEngine->createEmptyFontmap();
     Font->CreateSurface( g_pGfxEngine->Palette.m_Palette, SDL_SWSURFACE );
     sfc = Font->getSDLSurface();
     if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
     char *pixel = (char*) sfc->pixels;
     int c=0;
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
        	   offset = pixel + 128*8*(t/16) + 8*(t%16) + 128*y + x;
        	   if (p==0) c = 0;
        	   else c = *offset;
        	   // read a bit out of the current plane, shift it into the
        	   // correct position and merge it
        	   c |= (Planes->getbit(RawData, p) << p);
        	   // map black pixels to color 16 because of the way the
        	   // vorticon death sequence works in ep1
        	   *offset = c;
           }
         }
       }
     }
     Font->generateSpecialTwirls();
     Font->generateGlowFonts();
     Font->generateInverseFonts();
     Font->optimizeSurface();
     if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
     delete Planes;

     // Load 32x32 Tiles
     // TODO: Add a read method for 32x32 Tiles

     // ** read the 16x16 tiles **
     Planes = new CPlanes(plane1 + m_tiles16location,
                       plane2 + m_tiles16location,
                       plane3 + m_tiles16location,
                       plane4 + m_tiles16location,
                       0);
     Uint8 *u_offset;
     g_pGfxEngine->createEmptyTilemap();
     CTilemap *Tilemap = g_pGfxEngine->Tilemap;
     Tilemap->CreateSurface( g_pGfxEngine->Palette.m_Palette, SDL_SWSURFACE );
     sfc = Tilemap->getSDLSurface();
     if(SDL_MUSTLOCK(sfc))	SDL_LockSurface(sfc);
     Uint8 *u_pixel = (Uint8*) sfc->pixels;

     Uint8 tiledata[MAX_TILES+1][16][16];
     for(int p=0;p<4;p++)
     {
       for(int t=0;t<m_num16tiles;t++)
       {
         for(int y=0;y<16;y++)
         {
           for(int x=0;x<16;x++)
           {
               u_offset = u_pixel + 16*13*16*(t/13) + 16*(t%13)  + 16*13*y + x;
               if (p==0) c = 0;
               else c = tiledata[t][y][x];
               c |= (Planes->getbit(RawData, p) << p);
               tiledata[t][y][x] = c;

           	   if( t>0 && TileProperty[t-1][BEHAVIOR] == 65534 )  // This is for masked tiles.
           	   {
           		   if(c==15)
           			   *u_offset = COLORKEY;
           		   else
           			   *u_offset = *(u_pixel + 16*13*16*((t-1)/13) + 16*((t-1)%13)  + 16*13*y + x);
           			   //*u_offset = g_pVideoDriver->SpriteLayerSurface->format->colorkey;
           	   }
               else
            	   *u_offset = c;

           }
         }
       }
     }
     Tilemap->optimizeSurface();
     if(SDL_MUSTLOCK(sfc))	SDL_UnlockSurface(sfc);
     delete Planes;

     ////////////////////
     /// Load Bitmaps ///
     ////////////////////

     // set up the getbit() function
     Planes = new CPlanes(plane1 + m_bitmaplocation,
                       plane2 + m_bitmaplocation,
                       plane3 + m_bitmaplocation,
                       plane4 + m_bitmaplocation,
                       0);

     // decode bitmaps into the BitmapData structure. The bitmaps are
     // loaded into one continuous stream of image data, with the bitmaps[]
     // array giving pointers to where each bitmap starts within the stream.

     // In case there is a strange mod or defect episode, put some names to them!
     for(int b=0 ; b<m_bitmaps ; b++)
     {
    	 bitmap = g_pGfxEngine->Bitmap[b];
    	 bitmap->createSurface(g_pVideoDriver->getScrollSurface()->flags, g_pGfxEngine->Palette.m_Palette);
     }

     for(int p=0 ; p<4 ; p++)
     {
       for(int b=0 ; b<m_bitmaps ; b++)
       {
           // this points to the location that we're currently
           // decoding bitmap data to
    	   bitmap = g_pGfxEngine->Bitmap[b];

    	   sfc= bitmap->getSDLSurface();
    	   if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
    	   Uint8* pixel = (Uint8*) sfc->pixels;
    	   width = bitmap->getWidth(); height = bitmap->getHeight();
    	   // Now read the raw data
    	   for(int y=0 ; y<height ; y++)
    	   {
			   for(int x=0 ; x<width ; x++)
			   {
				 if (p==0) c = 0;
				 else c = pixel[y*width + x];
				 c |= (Planes->getbit(RawData, p) << p);
				 pixel[y*width + x] = c;
			   }
    	   }
    	   if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
       }
     }
     delete Planes;

     if(RawData){ delete[] RawData; RawData = NULL;}

	return true;
}
