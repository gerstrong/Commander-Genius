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
#include <stdio.h>
#include <string.h>

////////////////////////
// section of defines //
////////////////////////

#define CSF    5

extern stSprite *sprites;

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
	sprites = NULL;
}

CEGASprit::~CEGASprit() {
	if(Sprite) delete [] Sprite, Sprite = NULL;
	if(sprites) delete [] sprites, sprites = NULL;
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
     if(sprites) { delete [] sprites; sprites=NULL; }
     sprites = new stSprite[MAX_SPRITES+1];
     char c;
     for(int p=0 ; p<4 ; p++)
     {
       for(int s=0 ; s<m_numsprites ; s++)
       {
         sprites[s].xsize = Sprite[s].width;
         sprites[s].ysize = Sprite[s].height;
         sprites[s].bboxX1 = (Sprite[s].hitbox_l << CSF);
         sprites[s].bboxY1 = (Sprite[s].hitbox_u << CSF);
         sprites[s].bboxX2 = (Sprite[s].hitbox_r << CSF);
         sprites[s].bboxY2 = (Sprite[s].hitbox_b << CSF);

         for(int y=0 ; y<sprites[s].ysize ; y++)
         {
           for(int x=0 ; x<sprites[s].xsize ; x++)
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
             //if (p==3 && c==0) c = 16;
             sprites[s].imgdata[y][x] = c;
           }
         }
       }
     }

     // now load the 5th plane, which contains the sprite masks.
     // note that we invert the mask because our graphics functions
     // use white on black masks whereas keen uses black on white.
     for(int s=0 ; s<m_numsprites ; s++)
     {
       for(int y=0 ; y<sprites[s].ysize ; y++)
       {
         for(int x=0 ; x<sprites[s].xsize ; x++)
         {
            sprites[s].maskdata[y][x] =  Planes->getbit(RawData, 4);
         }
       }
     }

     delete Planes;

     if(RawData){ delete[] RawData; RawData = NULL;}

     // Now create special sprites for some neat effects!
     DeriveSpecialSprites();

     // Now load the special TGA Sprites if some are available
     LoadSpecialSprites();

	return true;
}


// load a 32-bit RGBA TGA file into sprite 's', and add colors to the palette
// as needed so that it can be shown exactly as found in the file.
// returns nonzero on failure.
char CEGASprit::LoadTGASprite( const std::string &filename, int s )
{
unsigned char *image, *base;
int x,y;
int w,h;
unsigned char r,g,b,a;
int c;
std::string fname;

	fname = GFXDIR + filename;
	if (LoadTGA(fname, &image, &w, &h))
		return 1;

	if (w > MAX_SPRITE_WIDTH || h > MAX_SPRITE_HEIGHT)
	{
		free(image);
		//lprintf("LoadTGASprite: image %s too big (%dx%d, max %dx%d)!\n", fname, w, h, MAX_SPRITE_WIDTH, MAX_SPRITE_HEIGHT);
		return 1;
	}

	//lprintf("LoadTGASprite: Parse %s; %dx%d\n", fname, w, h);

	base = image;
	for(y=h-1;y>=0;y--)
	for(x=0;x<w;x++)
	{
		b = *image++; g = *image++; r = *image++; a = *image++;
		if (a & 128)
		{
			c = pal_getcolor(r, g, b);
			if (c==-1) c = pal_addcolor(r, g, b);
			if (c==-1)
			{
				//g_pLogFile->textOut(RED,"LoadTGASprite: out of colorspace");
				return 1;
			}

			sprites[s].imgdata[y][x] = c;
			sprites[s].maskdata[y][x] = 0;
		}
		else
		{
			sprites[s].imgdata[y][x] = 0;
			sprites[s].maskdata[y][x] = 1;
		}
	}
	sprites[s].xsize = w;
	sprites[s].ysize = h;

	// detect the sprite's bounding box
	for(x=0;x<sprites[s].xsize-1;x++)
	{
		for(y=0;y<sprites[s].ysize;y++) if (sprites[s].maskdata[y][x]) goto break1;
	}
break1: ;
	sprites[s].bboxX1 = x << CSF;

	for(x=sprites[s].xsize-1;x>0;x--)
	{
		for(y=0;y<sprites[s].ysize;y++) if (sprites[s].maskdata[y][x]) goto break2;
	}
break2: ;
	sprites[s].bboxX2 = x << CSF;

	for(y=0;y<sprites[s].ysize-1;y++)
	{
		for(x=0;x<sprites[s].xsize-1;x++) if (sprites[s].maskdata[y][x]) goto break3;
	}
break3: ;
	sprites[s].bboxY1 = y << CSF;

	for(y=sprites[s].ysize-1;y>0;y--)
	{
		for(x=0;x<sprites[s].xsize-1;x++) if (sprites[s].maskdata[y][x]) goto break4;
	}
break4: ;
	sprites[s].bboxY2 = y << CSF;

	delete [] base;
	return 0;
}

// load special clonekeen-specific sprites from the .raw files
// Code by Caitlin Shaw
void CEGASprit::LoadSpecialSprites()
{
	LoadTGASprite("100.tga", PT100_SPRITE);
	LoadTGASprite("200.tga", PT200_SPRITE);
	LoadTGASprite("500.tga", PT500_SPRITE);
	LoadTGASprite("1000.tga", PT1000_SPRITE);
	LoadTGASprite("5000.tga", PT5000_SPRITE);
	LoadTGASprite("demobox.tga", DEMOBOX_SPRITE);
	LoadTGASprite("arrowlr.tga", ARROWLR_SPRITE);
	LoadTGASprite("arrowud.tga", ARROWUD_SPRITE);
	LoadTGASprite("arrowul.tga", ARROWUL_SPRITE);
	LoadTGASprite("arrowur.tga", ARROWUR_SPRITE);
	LoadTGASprite("arrowu.tga", ARROWU_SPRITE);
	LoadTGASprite("arrowd.tga", ARROWD_SPRITE);
}

// This function has the task to make some items-tiles
// be painted into yellow, so they look nice, when they are
// collected
void CEGASprit::DeriveSpecialSprites()
{
	Uint16 t;
	for( t=0 ; t<numtiles ; t++)
	{
		// The Gun!
		if( TileProperty[t][BEHAVIOR]==15 )
			CreateYellowSpriteofTile(t,GUNUP_SPRITE);

		// Keycards
		if( TileProperty[t][BEHAVIOR]>=18 && TileProperty[t][BEHAVIOR]<=21 )
			CreateYellowSpriteofTile(t,PTCARDB_SPRITE+TileProperty[t][BEHAVIOR]-18);

		// Single Bullet in Ep3
		if( TileProperty[t][BEHAVIOR]==28 )
			CreateYellowSpriteofTile(t,SHOTUP_SPRITE);

		if( TileProperty[t][BEHAVIOR]==27 )
			CreateYellowSpriteofTile(t,ANKHUP_SPRITE);

	}
}

void CEGASprit::CreateYellowSpriteofTile(Uint16 tile, Uint16 sprite)
{
	// The first pixel is usually the transparent on items. Use it!
	Uint8 transparent_colour = tiledata[tile][0][0];

	sprites[sprite].xsize = sprites[sprite].ysize = 16;

	for(Uint8 x=0 ; x<16 ; x++)
	{
		for(Uint8 y=0 ; y<16 ; y++)
		{
			if(tiledata[tile][x][y] == transparent_colour)
			{
				sprites[sprite].maskdata[x][y] = 15;
				sprites[sprite].imgdata[x][y] = 0;
			}
			else
			{
				sprites[sprite].maskdata[x][y] = 0;
				if(tiledata[tile][x][y] != 0 && tiledata[tile][x][y] != 16)
					sprites[sprite].imgdata[x][y] = 14; // Yellow (Light brown)
				else
					sprites[sprite].imgdata[x][y] = tiledata[tile][x][y]; // Take the normal there!
			}

		}
	}
}


