/*
 * CEGASprit.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 *
 *  Yes, it should be CEGASprite, bute since the file name
 *  is called EGASPRIT.CK? it is left that way,
 *  to make the understanding clearer
 */

#include "CEGASprit.h"
#include "CPlanes.h"
#include "../keen.h"
#include "../include/game.h"
#include "../FindFile.h"
#include "../sdl/CVideoDriver.h"
#include <stdio.h>
#include <string.h>

////////////////////////
// section of defines //
////////////////////////

#define CSF    5

// Reference to ../common/tga.cpp
char LoadTGA(const std::string &file, unsigned char **image, int *widthout, int *heightout);

//////////////////////////
// section of the class //
//////////////////////////

CEGASprit::CEGASprit(int planesize,
					long spritestartloc,
					int numsprites,
					long spriteloc)
{
	m_planesize = planesize;
	m_spritestartloc = spritestartloc;
	m_numsprites = numsprites;
	m_spriteloc = spriteloc;
	Sprite = NULL;
}

CEGASprit::~CEGASprit() {
	if(Sprite) delete [] Sprite, Sprite = NULL;
}


bool CEGASprit::loadHead(char *data)
{
	data += m_spritestartloc;

	Sprite = new st_sprite[m_numsprites];

    for(int i=0 ; i<m_numsprites ; i++ )
    {
    	memcpy(&(Sprite[i].width),data+128*i,2);
    	memcpy(&(Sprite[i].height),data+128*i+2,2);
    	memcpy(&(Sprite[i].location_offset),data+128*i+4,2);
    	memcpy(&(Sprite[i].location),data+128*i+6,2);
    	memcpy(&(Sprite[i].hitbox_l),data+128*i+8,2);
    	memcpy(&(Sprite[i].hitbox_u),data+128*i+10,2);
    	memcpy(&(Sprite[i].hitbox_r),data+128*i+12,2);
    	memcpy(&(Sprite[i].hitbox_b),data+128*i+14,2);
    	memcpy(Sprite[i].name,data+128*i+16,12);
    	memcpy(&(Sprite[i].hv_offset),data+128*i+28,4);

    	Sprite[i].width *= 8; // Where the width is divided by 8
    	Sprite[i].hitbox_l >>= 8;
    	Sprite[i].hitbox_u >>= 8;
    	Sprite[i].hitbox_r >>= 8;
    	Sprite[i].hitbox_b >>= 8;
    }

    return true;
}

bool CEGASprit::loadData(const std::string& filename, bool compresseddata)
{
	char *RawData;
    SDL_Surface *sfc;
    Uint8* pixel;

	FILE* latchfile = OpenGameFile(filename.c_str(),"rb");

	if(!latchfile)
		return false;

	RawData = new char[m_planesize * 5];
    // get the data out of the file into the memory, decompressing it if necessary.
    if (compresseddata)
    {
      if (lz_decompress(latchfile, (unsigned char*) RawData))
    	  return 1;
    }
    else
    {
    	for(int i=0 ; i<(m_planesize*5) ; i++)
    		RawData[i] = fgetc(latchfile);
    }

    fclose(latchfile);

    // TODO: Try to blit the Font map here!
     // these are the offsets of the different video planes as
     // relative to each other--that is if a pixel in plane1
     // is at N, the byte for that same pixel in plane3 will be
     // at (N + plane3).
     unsigned long plane1, plane2, plane3, plane4, plane5;

     plane1 = 0;
     plane2 = (m_planesize * 1);
     plane3 = (m_planesize * 2);
     plane4 = (m_planesize * 3);
     plane5 = (m_planesize * 4);

     CPlanes *Planes = new CPlanes(plane1 + m_spriteloc,
							 plane2 + m_spriteloc,
							 plane3 + m_spriteloc,
							 plane4 + m_spriteloc,
							 plane5 + m_spriteloc);

     // TODO: Create surfaces which can be blitted directly to the blit surface or maybe screen.
     // load the image data

     g_pGfxEngine->createEmptySprites(MAX_SPRITES+1);
     for(int s=0 ; s<m_numsprites ; s++)
     {
    	 g_pGfxEngine->Sprite[s]->setSize( Sprite[s].width, Sprite[s].height );
    	 g_pGfxEngine->Sprite[s]->setBouncingBoxCoordinates( (Sprite[s].hitbox_l << CSF),
															(Sprite[s].hitbox_u << CSF),
															(Sprite[s].hitbox_r << CSF),
															(Sprite[s].hitbox_b << CSF) );
    	 g_pGfxEngine->Sprite[s]->createSurface( g_pVideoDriver->SpriteLayerSurface->flags,
											g_pVideoDriver->MyPalette );
     }

     char c;
     for(int p=0 ; p<4 ; p++)
     {
       for(int s=0 ; s<m_numsprites ; s++)
       {
      	 sfc = g_pGfxEngine->Sprite[s]->getSDLSurface();
      	 if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
      	 pixel = (Uint8*) sfc->pixels;

         for(int y=0 ; y<sfc->h ; y++)
         {
           for(int x=0 ; x<sfc->w ; x++)
           {
             if (p==0) c = 0;
             else c = pixel[y*sfc->w + x];

             c |= (Planes->getbit(RawData, p) << p);
             //if (p==3 && c==0) c = 16;
             pixel[y*sfc->w + x] = c;
           }
         }

         if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
       }
     }

     // now load the 5th plane, which contains the sprite masks.
     // note that we invert the mask because our graphics functions
     // use white on black masks whereas keen uses black on white.
     for(int s=0 ; s<m_numsprites ; s++)
     {
       sfc = g_pGfxEngine->Sprite[s]->getSDLSurface();
       if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
       pixel = (Uint8*) sfc->pixels;

       for(int y=0 ; y<sfc->h ; y++)
       {
         for(int x=0 ; x<sfc->w ; x++)
         {
            //sprites[s].maskdata[y][x] =  Planes->getbit(RawData, 4);
            if(Planes->getbit(RawData, 4)) pixel[y*sfc->w + x] = COLORKEY;
         }
       }

       if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
     }

     delete Planes;

     if(RawData){ delete[] RawData; RawData = NULL;}

     // Now create special sprites for some neat effects!
     DeriveSpecialSprites( g_pGfxEngine->Tilemap, &g_pGfxEngine->Sprite[0] );

     // Now load the special TGA Sprites if some are available
     LoadSpecialSprites( g_pGfxEngine->Sprite );

     for(Uint16 s=0 ; s<g_pGfxEngine->Sprite.size() ; s++)
    	 g_pGfxEngine->Sprite[s]->optimizeSurface();

	return true;
}


// load a 32-bit RGBA TGA file into sprite 's', and add colors to the palette
// as needed so that it can be shown exactly as found in the file.
// returns nonzero on failure.
char CEGASprit::LoadTGASprite( const std::string &filename, CSprite *sprite )
{
unsigned char *image, *base;
int x,y;
int w,h;
unsigned char r,g,b,a;
int c;
std::string fname;
Uint8* pixel;

	fname = GFXDIR + filename;
	if (LoadTGA(fname, &image, &w, &h))
		return 1;

	if (w > MAX_SPRITE_WIDTH || h > MAX_SPRITE_HEIGHT)
	{
		free(image);
		return 1;
	}

	base = image;
	sprite->setSize(w, h);
	sprite->createSurface( g_pVideoDriver->SpriteLayerSurface->flags, g_pVideoDriver->MyPalette );

	SDL_Surface *sfc = sprite->getSDLSurface();

	if(SDL_MUSTLOCK(sfc))	SDL_LockSurface(sfc);
	pixel = (Uint8*) sfc->pixels;
	for(y=h-1;y>=0;y--)
	{
		for(x=0;x<w;x++)
		{
			b = *image++; g = *image++; r = *image++; a = *image++;
			if (a & 128)
			{
				c = pal_getcolor(r, g, b);
				if (c==-1) c = pal_addcolor(r, g, b);
				if (c==-1) return 1;

				pixel[y*w + x] = c;
			}
			else
				pixel[y*w + x] = COLORKEY;
		}
	}
	if(SDL_MUSTLOCK(sfc))	SDL_UnlockSurface(sfc);

	sprite->m_bboxX1=0;
	sprite->m_bboxY1=0;
	sprite->m_bboxX2=sprite->getWidth();
	sprite->m_bboxY2=sprite->getHeight();

	delete [] base;

	return 0;
}

// load special clonekeen-specific sprites from the .raw files
// Code by Caitlin Shaw
void CEGASprit::LoadSpecialSprites( std::vector<CSprite*> &sprite )
{
	LoadTGASprite("100.tga", sprite[PT100_SPRITE] );
	LoadTGASprite("200.tga", sprite[PT200_SPRITE] );
	LoadTGASprite("500.tga", sprite[PT500_SPRITE] );
	LoadTGASprite("1000.tga", sprite[PT1000_SPRITE] );
	LoadTGASprite("5000.tga", sprite[PT5000_SPRITE] );
	LoadTGASprite("demobox.tga", sprite[DEMOBOX_SPRITE] );
	LoadTGASprite("arrowlr.tga", sprite[ARROWLR_SPRITE] );
	LoadTGASprite("arrowud.tga", sprite[ARROWUD_SPRITE] );
	LoadTGASprite("arrowul.tga", sprite[ARROWUL_SPRITE] );
	LoadTGASprite("arrowur.tga", sprite[ARROWUR_SPRITE] );
	LoadTGASprite("arrowu.tga", sprite[ARROWU_SPRITE] );
	LoadTGASprite("arrowd.tga", sprite[ARROWD_SPRITE] );
}

// This function has the task to make some items-tiles
// be painted into yellow, so they look nice, when they are
// collected
void CEGASprit::DeriveSpecialSprites( CTilemap *tilemap, CSprite **sprite )
{
	Uint16 t;
	for( t=0 ; t<numtiles ; t++)
	{
		// The Gun!
		if( TileProperty[t][BEHAVIOR]==15 )
			CreateYellowSpriteofTile( tilemap, t, sprite[GUNUP_SPRITE] );

		// Keycards
		if( TileProperty[t][BEHAVIOR]>=18 && TileProperty[t][BEHAVIOR]<=21 )
			CreateYellowSpriteofTile( tilemap, t, sprite[PTCARDB_SPRITE+TileProperty[t][BEHAVIOR]-18]);

		// Single Bullet in Ep3
		if( TileProperty[t][BEHAVIOR]==28 )
			CreateYellowSpriteofTile( tilemap, t, sprite[SHOTUP_SPRITE] );

		if( TileProperty[t][BEHAVIOR]==27 )
			CreateYellowSpriteofTile( tilemap, t, sprite[ANKHUP_SPRITE] );
	}
}

void CEGASprit::CreateYellowSpriteofTile( CTilemap *tilemap, Uint16 tile, CSprite *sprite )
{
	SDL_Rect tile_rect;

	tile_rect.x = 16*(tile%13);
	tile_rect.y = 16*(tile/13);
	tile_rect.w = tile_rect.h= 16;

	sprite->setSize(tile_rect.w, tile_rect.h);
	sprite->createSurface( g_pVideoDriver->SpriteLayerSurface->flags,
						   g_pVideoDriver->MyPalette );

	SDL_Surface *src_sfc = sprite->getSDLSurface();

	SDL_BlitSurface(tilemap->getSDLSurface(), &tile_rect, src_sfc, NULL);

	if(SDL_MUSTLOCK(src_sfc)) SDL_LockSurface(src_sfc);

	// The first pixel is usually the transparent one on items. Use it!
	Uint8* pixel = (Uint8*) src_sfc->pixels;
	Uint8 transparent_colour = pixel[0];
	for(Uint8 x=0 ; x<16 ; x++)
	{
		for(Uint8 y=0 ; y<16 ; y++)
		{
			if(*pixel == transparent_colour) *pixel = COLORKEY;
			else if(*pixel != 0 && *pixel != 16)	*pixel = 14; // Yellow (Light brown)

			pixel++;
		}
	}

	if(SDL_MUSTLOCK(src_sfc)) SDL_UnlockSurface(src_sfc);
}


