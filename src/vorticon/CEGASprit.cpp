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
#include "../game.h"
#include "../FindFile.h"
#include "../sdl/CVideoDriver.h"
#include "../vorticon/spritedefines.h"
#include <stdio.h>
#include <string.h>

////////////////////////
// section of defines //
////////////////////////

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
    SDL_Surface *pixsfc;
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
	
	// load the image data
	g_pGfxEngine->createEmptySprites(MAX_SPRITES+1);
	for(int i=0 ; i<m_numsprites ; i++)
	{
		g_pGfxEngine->Sprite[i]->setSize( Sprite[i].width, Sprite[i].height );
		g_pGfxEngine->Sprite[i]->setBouncingBoxCoordinates( (Sprite[i].hitbox_l << (CSF-TILE_S)),
														    (Sprite[i].hitbox_u << (CSF-TILE_S)),
														    (Sprite[i].hitbox_r << (CSF-TILE_S)),
														    (Sprite[i].hitbox_b << (CSF-TILE_S)) );
		g_pGfxEngine->Sprite[i]->createSurface( g_pVideoDriver->BlitSurface->flags,
											   g_pGfxEngine->Palette.m_Palette );
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
		pixsfc = g_pGfxEngine->Sprite[s]->getSDLSurface();
		sfc = g_pGfxEngine->Sprite[s]->getSDLMaskSurface();
		
		if(SDL_MUSTLOCK(pixsfc)) SDL_LockSurface(pixsfc);
		if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
		
		pixel = (Uint8*) sfc->pixels;
		
		for(int y=0 ; y<sfc->h ; y++)
		{
			for(int x=0 ; x<sfc->w ; x++)
			{
				pixel[y*sfc->w + x] = Planes->getbit(RawData, 4) ? ((Uint8*)pixsfc->pixels)[y*pixsfc->w + x] : 15;
			}
		}
		
		
		if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
		if(SDL_MUSTLOCK(pixsfc)) SDL_UnlockSurface(pixsfc);
	}
	
	delete Planes;
	
	if(RawData){ delete[] RawData; RawData = NULL;}
	
	// Now load the special TGA Sprites if some are available
	LoadSpecialSprites( g_pGfxEngine->Sprite );
	
	for(Uint16 s=0 ; s<g_pGfxEngine->Sprite.size() ; s++)
	{
		g_pGfxEngine->Sprite[s]->optimizeSurface();
		g_pGfxEngine->Sprite[s]->applyTransparency();
	}
	
	// Now create special sprites for some neat effects!
	DeriveSpecialSprites( g_pGfxEngine->Tilemap, g_pGfxEngine->Sprite );
	
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
	Uint8 *pixel, *maskpx;
	
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
	sprite->createSurface( g_pVideoDriver->BlitSurface->flags, g_pGfxEngine->Palette.m_Palette );
	
	SDL_Surface *sfc = sprite->getSDLSurface();
	SDL_Surface *msksfc = sprite->getSDLMaskSurface();
	
	if(SDL_MUSTLOCK(sfc))	SDL_LockSurface(sfc);
	if(SDL_MUSTLOCK(msksfc))	SDL_LockSurface(msksfc);
	
	pixel = (Uint8*) sfc->pixels;
	maskpx = (Uint8*) msksfc->pixels;
	
	for(y=h-1;y>=0;y--)
	{
		for(x=0;x<w;x++)
		{
			b = *image++; g = *image++; r = *image++; a = *image++;
			if (a & 128)
			{
				c = g_pGfxEngine->Palette.getcolor(r, g, b);
				if (c==-1) c = g_pGfxEngine->Palette.addcolor(r, g, b);
				if (c==-1) return 1;
				
				pixel[y*w + x] = c;
				maskpx[y*w + x] = 15;
			}
			else
				maskpx[y*w + x] = 0;
		}
	}
	
	if(SDL_MUSTLOCK(msksfc)) SDL_UnlockSurface(msksfc);
	if(SDL_MUSTLOCK(sfc))	 SDL_UnlockSurface(sfc);
	
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
void CEGASprit::DeriveSpecialSprites( CTilemap *tilemap, std::vector<CSprite*> &sprites )
{
	for( Uint16 t=0 ; t<tilemap->m_numtiles ; t++)
	{
		// The Gun!
		if( tilemap->mp_tiles[t].behaviour==15 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(GUNUP_SPRITE) );
		
		// Keycards
		if( tilemap->mp_tiles[t].behaviour>=18 && tilemap->mp_tiles[t].behaviour<=21 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDB_SPRITE+tilemap->mp_tiles[t].behaviour-18));
		
		// Single Bullet in Ep3
		if( tilemap->mp_tiles[t].behaviour==28 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(SHOTUP_SPRITE) );
		
		if( tilemap->mp_tiles[t].behaviour==27 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(ANKHUP_SPRITE) );
	}
}

void CEGASprit::CreateYellowSpriteofTile( CTilemap *tilemap, Uint16 tile, CSprite* sprite )
{
	SDL_Rect tile_rect;
	
	tile_rect.x = 16*(tile%13);
	tile_rect.y = 16*(tile/13);
	tile_rect.w = tile_rect.h= 16;
	
	sprite->setSize(tile_rect.w, tile_rect.h);
	sprite->createSurface( g_pVideoDriver->BlitSurface->flags,
						  g_pGfxEngine->Palette.m_Palette );
	sprite->optimizeSurface();
	
	SDL_Surface *src_sfc = sprite->getSDLSurface();
	
	SDL_BlitSurface(tilemap->getSDLSurface(), &tile_rect, src_sfc, NULL);
	
	if(SDL_MUSTLOCK(src_sfc)) SDL_LockSurface(src_sfc);
	
	// The first pixel is usually the transparent one on items. Use it!
	Uint8* pixel = (Uint8*)src_sfc->pixels;
	Uint32 transparent_colour;
	Uint32 colour;
	Uint8 r,g,b,a;
	memcpy(&transparent_colour, pixel, src_sfc->format->BytesPerPixel);
	for(Uint8 x=0 ; x<16 ; x++)
	{
		for(Uint8 y=0 ; y<16 ; y++)
		{
			memcpy(&colour, pixel, src_sfc->format->BytesPerPixel);
			SDL_GetRGBA( colour, src_sfc->format, &r, &g, &b, &a );
			if( colour == transparent_colour )
				a = 0;
			if( r!=0 && g!=0 && b!=0 && a!=0)
				r = g = 255;
			
			colour = SDL_MapRGBA( src_sfc->format, r, g, b, a );
			memcpy( pixel, &colour ,src_sfc->format->BytesPerPixel);
			
			pixel += src_sfc->format->BytesPerPixel;
		}
	}
	
	if(SDL_MUSTLOCK(src_sfc)) SDL_UnlockSurface(src_sfc);
}


