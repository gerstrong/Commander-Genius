/*
 * CEGALatch.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#include "CEGALatch.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"
#include "../CLogFile.h"
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

bool CEGALatch::loadHead( char *data, short m_episode )
{
	Uint16 height, width;
	char name[9];

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
		else {
			if (m_episode == 1) {
				const std::string default_names[] = { "TITLE", "IDLOGO", "F1HELP", "HIGHSCOR",
					"NAME", "SCORE", "PARTS", "GAMEOVER", "AN", "PRESENT", "APOGEE", "KEENSHIP", "WINDON",
					"WINDOFF", "ONEMOMEN", "OFAN", "PRODUCT", "IDSOFT" }; // in case the names are empty
							g_pGfxEngine->Bitmap[i]->setName( default_names[i] );
			}else if (m_episode == 2) {
				const std::string default_names[] = { "TITLE", "IDLOGO", "F1HELP", "HIGHSCOR",
					"NAME", "SCORE", "CITIES", "GAMEOVER", "AN", "PRESENT", "APOGEE", "KEENSHIP", "WINDON",
					"WINDOFF", "ONEMOMEN", "OFAN", "PRODUCT", "IDSOFT" }; // in case the names are empty
							g_pGfxEngine->Bitmap[i]->setName( default_names[i] );
			}else if (m_episode == 3) {
				const std::string default_names[] = { "TITLE", "IDLOGO", "F1HELP", "HIGHSCOR",
					"NAME", "SCORE", "GAMEOVER", "AN", "PRESENT", "APOGEE", "ONEMOMEN", "OFAN", "PRODUCT", "IDSOFT" }; // in case the names are empty
							g_pGfxEngine->Bitmap[i]->setName( default_names[i] );
			}
		}

		g_pGfxEngine->Bitmap[i]->setDimensions(width,height);
	}

	return true;
}

bool CEGALatch::loadData( std::string &m_path, short m_episode, bool compresseddata )
{
	std::string filename;
	char *RawData;
    CBitmap *bitmap;
    Uint16 width, height;
    SDL_Surface *sfc;

	if(m_path == "") filename = "data/egalatch.ck" + itoa(m_episode);
	else filename = "data/" + m_path + "/egalatch.ck" + itoa(m_episode);

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
           	   *u_offset = c;
           }
         }
       }
     }
     if(SDL_MUSTLOCK(sfc))	SDL_UnlockSurface(sfc);

     // Load Hires, VGA, SVGA Tiles into the tilemap
     if(m_path == "") filename = "data/ck" + itoa(m_episode) + "tiles.bmp";
     else filename = "data/" + m_path + "/ck" + itoa(m_episode) + "tiles.bmp";
     if(Tilemap->loadHiresTile(filename))
    	 g_pLogFile->textOut(GREEN, "Hi-res Bitmap for Tiles was loaded successfully!");

     // Adapt the tilemap to the display, so they are faster blit
     Tilemap->optimizeSurface();

     // make masked tiles according to it's surfaces
     applyMasks();

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

// Convert the normal tiles to masked tiles
void CEGALatch::applyMasks()
{
	SDL_Surface *sfc;
	Uint32 u_colour = 0;
	SDL_Rect rect;
	Uint8 bpp;
	Uint8 r,g,b,alpha;
	Uint8 *u_offset;

	sfc = g_pGfxEngine->Tilemap->getSDLSurface();

	if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);

	bpp = sfc->format->BytesPerPixel;
	rect.w = rect.h = 1;

	for( Uint16 t=0 ; t<m_num16tiles ; t++ )
	{
		if( TileProperty[t][BEHAVIOR] == 65534 )  // This is for masked tiles.
		{
			for( Uint16 x=0 ; x<16 ; x++ )
			{
				for( Uint16 y=0 ; y<16 ; y++ )
				{
					u_offset = (Uint8*)sfc->pixels + bpp*((y+16*((t+1)/13))*13*16 + 16*((t+1)%13) + x);
					memcpy( &u_colour, u_offset, bpp);
					SDL_GetRGB( u_colour, sfc->format, &r, &g, &b);

					rect.x = 16*((t+1)%13) + x;
					rect.y = y+16*((t+1)/13);

					if( r>=250 && g>=250 && b>=250 ) //In this case set colourkey
					{
						SDL_FillRect(sfc, &rect, SDL_MapRGBA(sfc->format, 0, 0, 0, 0));
					}
					else // Get the pixel of the previous tile. If the mask has colour, use alpha channel, black is opaque
					{
						alpha = 255 - (r*g*b)/3;
						u_offset = (Uint8*)sfc->pixels + bpp*((y+16*(t/13))*13*16 + 16*(t%13) + x);
						memcpy( &u_colour, u_offset, bpp);
						SDL_GetRGB( u_colour, sfc->format, &r, &g, &b);
						SDL_FillRect(sfc, &rect, SDL_MapRGBA(sfc->format,r,g,b,alpha));
					}
				}
			}
		}
	}

	if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
}
