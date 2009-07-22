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
#include <stdio.h>
#include <string.h>

////////////////////////
// section of defines //
////////////////////////

#define CSF    5

extern stSprite *sprites;

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

    	Sprite[i].width *= 8; // Another case where the width is divided by 8
    	Sprite[i].hitbox_l >>= 8;
    	Sprite[i].hitbox_u >>= 8;
    	Sprite[i].hitbox_r >>= 8;
    	Sprite[i].hitbox_b >>= 8;
    }

    return true;
}

bool CEGASprit::loadData(const char *filename, bool compresseddata)
{
	FILE* latchfile;
	char *RawData;

	latchfile = fopen(filename,"rb");

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
     sprites = new stSprite[301];
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
             if (p==3 && c==0) c = 16;
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
            sprites[s].maskdata[y][x] = (1 - Planes->getbit(RawData, 4));
         }
       }
     }

     delete Planes;

     if(RawData){ delete[] RawData; RawData = NULL;}

	return true;
}
