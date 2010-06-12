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
#include "../CPlanes.h"
#include "../../keen.h"
#include "../../game.h"
#include "../../FindFile.h"
#include "../../sdl/CVideoDriver.h"
#include "../../engine/spritedefines.h"
#include "../../lz.h"
#include "../../fileio/ResourceMgmt.h"
#include "../../funcdefs.h"
#include <SDL.h>
#include <stdio.h>
#include <string.h>

////////////////////////
// section of defines //
////////////////////////

#define DOOR_YELLOW        2
#define DOOR_RED           3
#define DOOR_GREEN         4
#define DOOR_BLUE          5

// Reference to ../common/tga.cpp
char LoadTGA(const std::string &file, unsigned char **image, int *widthout, int *heightout);

//////////////////////////
// section of the class //
//////////////////////////

CEGASprit::CEGASprit(int planesize,
					 long spritestartloc,
					 int numsprites,
					 long spriteloc,
					 const std::string &gamepath) :
m_gamepath(gamepath),
EGASpriteModell(NULL)
{
	m_planesize = planesize;
	m_spritestartloc = spritestartloc;
	m_numsprites = numsprites;
	m_spriteloc = spriteloc;
}


bool CEGASprit::loadHead(char *data)
{
	data += m_spritestartloc;
	
	EGASpriteModell = new st_sprite[m_numsprites];
	
    for(int i=0 ; i<m_numsprites ; i++ )
    {
    	memcpy(&(EGASpriteModell[i].width),data+128*i,2);
    	memcpy(&(EGASpriteModell[i].height),data+128*i+2,2);
    	memcpy(&(EGASpriteModell[i].location_offset),data+128*i+4,2);
    	memcpy(&(EGASpriteModell[i].location),data+128*i+6,2);
    	memcpy(&(EGASpriteModell[i].hitbox_l),data+128*i+8,2);
    	memcpy(&(EGASpriteModell[i].hitbox_u),data+128*i+10,2);
    	memcpy(&(EGASpriteModell[i].hitbox_r),data+128*i+12,2);
    	memcpy(&(EGASpriteModell[i].hitbox_b),data+128*i+14,2);
    	memcpy(EGASpriteModell[i].name,data+128*i+16,12);
    	memcpy(&(EGASpriteModell[i].hv_offset),data+128*i+28,4);
		
    	EGASpriteModell[i].width *= 8; // Where the width is divided by 8
    	EGASpriteModell[i].hitbox_l >>= 8;
    	EGASpriteModell[i].hitbox_u >>= 8;
    	EGASpriteModell[i].hitbox_r >>= 8;
    	EGASpriteModell[i].hitbox_b >>= 8;
    }
	
    return true;
}

bool CEGASprit::loadData(const std::string& filename, bool compresseddata)
{
	std::string file;
	byte *RawData;
    SDL_Surface *sfc;
    SDL_Surface *pixsfc;
    Uint8* pixel;
	
	FILE* latchfile = OpenGameFile(filename.c_str(),"rb");
	
	if(!latchfile)
		return false;
	
	RawData = new byte[m_planesize * 5];
    // get the data out of the file into the memory, decompressing it if necessary.
    if (compresseddata)
    {
		if (lz_decompress(latchfile, RawData))
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
	
	CPlanes Planes(RawData + m_spriteloc);
	Planes.setOffsets(plane1, plane2, plane3, plane4, plane5);
	
	// load the image data
	g_pGfxEngine->createEmptySprites(MAX_SPRITES+1);
	for(int i=0 ; i<m_numsprites ; i++)
	{
		CSprite &Sprite = g_pGfxEngine->getSprite(i);
		Sprite.setSize( EGASpriteModell[i].width, EGASpriteModell[i].height );
		Sprite.setBouncingBoxCoordinates( (EGASpriteModell[i].hitbox_l << (CSF-TILE_S)),
				(EGASpriteModell[i].hitbox_u << (CSF-TILE_S)),
				(EGASpriteModell[i].hitbox_r << (CSF-TILE_S)),
				(EGASpriteModell[i].hitbox_b << (CSF-TILE_S)) );
		Sprite.createSurface( g_pVideoDriver->BlitSurface->flags,
				g_pGfxEngine->Palette.m_Palette );
	}

	for(int p=0 ; p<4 ; p++)
	{
		for(int s=0 ; s<m_numsprites ; s++)
		{
			sfc = g_pGfxEngine->getSprite(s).getSDLSurface();
			if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
			pixel = (Uint8*) sfc->pixels;

			Planes.readPlane(p, pixel, sfc->w, sfc->h);

			if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
		}
	}

	// now load the 5th plane, which contains the sprite masks.
	// note that we invert the mask because our graphics functions
	// use white on black masks whereas keen uses black on white.
	for(int s=0 ; s<m_numsprites ; s++)
	{
		CSprite &Sprite = g_pGfxEngine->getSprite(s);
		pixsfc = Sprite.getSDLSurface();
		sfc = Sprite.getSDLMaskSurface();

		if(SDL_MUSTLOCK(pixsfc)) SDL_LockSurface(pixsfc);
		if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);

		pixel = (Uint8*) sfc->pixels;

		for(int y=0 ; y<sfc->h ; y++)
		{
			for(int x=0 ; x<sfc->w ; x++)
			{
				pixel[y*sfc->w + x] = Planes.getbit(4) ? ((Uint8*)pixsfc->pixels)[y*pixsfc->w + x] : 15;
			}
		}
		if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
		if(SDL_MUSTLOCK(pixsfc)) SDL_UnlockSurface(pixsfc);
	}

	
	if(RawData){ delete[] RawData; RawData = NULL;}
	
	// Now load the special TGA Sprites if some are available
	LoadSpecialSprites( g_pGfxEngine->getSpriteVec() );

	for(Uint16 s=0 ; s<g_pGfxEngine->getSpriteVec().size() ; s++)
	{
		CSprite &Sprite = g_pGfxEngine->getSprite(s);
		Sprite.optimizeSurface();
		file = getResourceFilename("gfx/sprite" + itoa(s) + ".bmp", m_gamepath, false, true);
		Sprite.loadHQSprite(file);
		Sprite.applyTransparency();
	}
	
	// Apply the sprites for player 2,3 and 4
	DerivePlayerSprites( g_pGfxEngine->getSpriteVec() );

	// Now create special sprites, like those for effects and the doors!
	DeriveSpecialSprites( g_pGfxEngine->getTileMap(0), g_pGfxEngine->getSpriteVec() );

	// Here special Effects are applied, only when the option is enabled for it
	if(g_pVideoDriver->getSpecialFXConfig())
		ApplySpecialFX();

	return true;
}

// load a 32-bit RGBA TGA file into sprite 's', and add colors to the palette
// as needed so that it can be shown exactly as found in the file.
// returns nonzero on failure.
char CEGASprit::LoadTGASprite( const std::string &filename, CSprite &sprite )
{
	unsigned char *image, *base;
	int x,y;
	int w,h;
	unsigned char r,g,b,a;
	int c;
	Uint8 *pixel, *maskpx;
	
	// Look in local location than in global, if tga was not found!
	if (LoadTGA(filename, &image, &w, &h))
		return 1;
	
	if (w > MAX_SPRITE_WIDTH || h > MAX_SPRITE_HEIGHT)
	{
		free(image);
		return 1;
	}
	
	base = image;
	sprite.setSize(w, h);
	sprite.createSurface( g_pVideoDriver->BlitSurface->flags, g_pGfxEngine->Palette.m_Palette );
	
	SDL_Surface *sfc = sprite.getSDLSurface();
	SDL_Surface *msksfc = sprite.getSDLMaskSurface();
	
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
	
	sprite.m_bboxX1=0;
	sprite.m_bboxY1=0;
	sprite.m_bboxX2=sprite.getWidth();
	sprite.m_bboxY2=sprite.getHeight();
	
	delete [] base;
	
	return 0;
}

// load special clonekeen-specific sprites from the .raw files
// Code by Caitlin Shaw
void CEGASprit::LoadSpecialSprites( std::vector<CSprite> &sprite )
{
	LoadTGASprite(getResourceFilename("gfx/100.tga", m_gamepath, false, true), sprite[PT100_SPRITE] );
	LoadTGASprite(getResourceFilename("gfx/200.tga", m_gamepath, false, true), sprite[PT200_SPRITE] );
	LoadTGASprite(getResourceFilename("gfx/500.tga", m_gamepath, false, true), sprite[PT500_SPRITE] );
	LoadTGASprite(getResourceFilename("gfx/1000.tga", m_gamepath, false, true), sprite[PT1000_SPRITE] );
	LoadTGASprite(getResourceFilename("gfx/5000.tga", m_gamepath, false, true), sprite[PT5000_SPRITE] );
	//LoadTGASprite("demobox.tga", sprite[DEMOBOX_SPRITE] );
	//LoadTGASprite("arrowlr.tga", sprite[ARROWLR_SPRITE] );
	//LoadTGASprite("arrowud.tga", sprite[ARROWUD_SPRITE] );
	//LoadTGASprite("arrowul.tga", sprite[ARROWUL_SPRITE] );
	//LoadTGASprite("arrowur.tga", sprite[ARROWUR_SPRITE] );
	//LoadTGASprite("arrowu.tga", sprite[ARROWU_SPRITE] );
	//LoadTGASprite("arrowd.tga", sprite[ARROWD_SPRITE] );
}

void CEGASprit::DerivePlayerSprites( std::vector<CSprite> &sprites )
{
    // create the sprites for player 2, 3 and 4
	size_t s;
	for(size_t i=0;i<48;i++)
	{
		s = SECOND_PLAYER_BASEFRAME+i;
		sprites.at(i).copy( sprites.at(s), g_pGfxEngine->Palette.m_Palette );
		sprites.at(s).replaceSpriteColor( 13, 11 ,0 );
		sprites.at(s).replaceSpriteColor( 5, 3 ,0 );
		//sprites.at(s).replaceSpriteColor( 9, 14 ,8 );
		//sprites.at(s).replaceSpriteColor( 1, 6 ,8 );
		sprites.at(s).replaceSpriteColor( 12, 9 ,0 );
		sprites.at(s).replaceSpriteColor( 4, 1 ,0 );
		sprites.at(s).optimizeSurface();
	}
	for(size_t i=0;i<48;i++)
	{
		s = THIRD_PLAYER_BASEFRAME+i;
		sprites.at(i).copy( sprites.at(s), g_pGfxEngine->Palette.m_Palette );
		sprites.at(s).replaceSpriteColor( 13, 10, 0 ); // Shirt light
		sprites.at(s).replaceSpriteColor( 5, 2, 0 ); // Shirt dark
		//sprites.at(s).replaceSpriteColor( 9, 10, 8 ); // Trousers light
		//sprites.at(s).replaceSpriteColor( 1, 2, 8 ); // Trousers dark
		sprites.at(s).replaceSpriteColor( 12, 2, 16 ); // Shoes light
		sprites.at(s).replaceSpriteColor( 4, 0, 16 ); // Shoes dark
		sprites.at(s).optimizeSurface();
	}
	for(size_t i=0;i<48;i++)
	{
		s = FOURTH_PLAYER_BASEFRAME+i;
		sprites.at(i).copy( sprites.at(s), g_pGfxEngine->Palette.m_Palette );
		sprites.at(s).replaceSpriteColor( 13, 14, 0 ); // Shirt light
		sprites.at(s).replaceSpriteColor( 5, 6, 0 ); // Shirt dark
		//sprites.at(s).replaceSpriteColor( 9, 11, 8 ); // Trousers light
		//sprites.at(s).replaceSpriteColor( 1, 9, 8 ); // Trousers dark
		sprites.at(s).replaceSpriteColor( 12, 6, 16 ); // Shoes light
		sprites.at(s).replaceSpriteColor( 4, 0, 16 ); // Shoes dark
		sprites.at(s).optimizeSurface();
	}
}

// This function has the task to make some items-tiles
// be painted into yellow, so they look nice, when they are
// collected
void CEGASprit::DeriveSpecialSprites( CTilemap &tilemap, std::vector<CSprite> &sprites )
{
	std::vector<CTileProperties>& TileProperties = g_pBehaviorEngine->getTileProperties();
	// Yellow sprites a special effect when items are collected
	for( Uint16 t=0 ; t<TileProperties.size() ; t++)
	{
		// The Gun!
		if( TileProperties.at(t).behaviour==15 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(GUNUP_SPRITE) );
		
		// Keycards
		if( TileProperties.at(t).behaviour==18 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDY_SPRITE));
		if( TileProperties.at(t).behaviour==19 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDG_SPRITE));
		if( TileProperties.at(t).behaviour==20 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDR_SPRITE));
		if( TileProperties.at(t).behaviour==21 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDB_SPRITE));
		
		// Single Bullet in Ep3
		if( TileProperties.at(t).behaviour==28 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(SHOTUP_SPRITE) );
		
		if( TileProperties.at(t).behaviour==27 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(ANKHUP_SPRITE) );
	}

	for(size_t i=0 ; i < TileProperties.size() ; i++ )
	{
		if(TileProperties.at(i).behaviour == DOOR_YELLOW)
			g_pGfxEngine->copyTileToSprite(i-1, DOOR_YELLOW_SPRITE, 2);

		if(TileProperties.at(i).behaviour == DOOR_RED)
			g_pGfxEngine->copyTileToSprite(i-1, DOOR_RED_SPRITE, 2);

		if(TileProperties.at(i).behaviour == DOOR_GREEN)
			g_pGfxEngine->copyTileToSprite(i-1, DOOR_GREEN_SPRITE, 2);

		if(TileProperties.at(i).behaviour == DOOR_BLUE)
			g_pGfxEngine->copyTileToSprite(i-1, DOOR_BLUE_SPRITE, 2);
	 }

    // TODO: Demo-Sprite must be added. This time loaded from one TGA File! The TGA is already there!
}

void CEGASprit::CreateYellowSpriteofTile( CTilemap &tilemap, Uint16 tile, CSprite& sprite )
{
	SDL_Rect tile_rect;
	
	tile_rect.x = 16*(tile%13);
	tile_rect.y = 16*(tile/13);
	tile_rect.w = tile_rect.h= 16;
	
	sprite.setSize(tile_rect.w, tile_rect.h);
	sprite.createSurface( g_pVideoDriver->BlitSurface->flags,
						  g_pGfxEngine->Palette.m_Palette );
	sprite.optimizeSurface();
	
	SDL_Surface *src_sfc = sprite.getSDLSurface();
	
	SDL_BlitSurface(tilemap.getSDLSurface(), &tile_rect, src_sfc, NULL);
	
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

/**
 * \brief	This function applies translucy to some sprite objects when Special FX option is enabled
 */
void CEGASprit::ApplySpecialFX()
{
	g_pGfxEngine->getSprite(OBJ_RAY_DEFSPRITE_EP1).applyTranslucency(200);
	g_pGfxEngine->getSprite(OBJ_RAY_DEFSPRITE_EP2).applyTranslucency(200);
	g_pGfxEngine->getSprite(OBJ_RAY_DEFSPRITE_EP3).applyTranslucency(200);

	g_pGfxEngine->getSprite(RAY_FRAME_ZAP_EP1).applyTranslucency(200);
	g_pGfxEngine->getSprite(RAY_FRAME_ZOT_EP1).applyTranslucency(200);
	g_pGfxEngine->getSprite(RAY_FRAME_ZAP_EP2).applyTranslucency(200);
	g_pGfxEngine->getSprite(RAY_FRAME_ZOT_EP2).applyTranslucency(200);
	g_pGfxEngine->getSprite(RAY_FRAME_ZAP_EP3).applyTranslucency(200);
	g_pGfxEngine->getSprite(RAY_FRAME_ZOT_EP3).applyTranslucency(200);

	g_pGfxEngine->getSprite(PT100_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PT200_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PT500_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PT1000_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PT5000_SPRITE).applyTranslucency(196);

	g_pGfxEngine->getSprite(PTCARDY_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PTCARDG_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PTCARDR_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(PTCARDB_SPRITE).applyTranslucency(196);

	g_pGfxEngine->getSprite(GUNUP_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(SHOTUP_SPRITE).applyTranslucency(196);
	g_pGfxEngine->getSprite(ANKHUP_SPRITE).applyTranslucency(196);

}

CEGASprit::~CEGASprit() {
	if(EGASpriteModell) delete [] EGASpriteModell, EGASpriteModell = NULL;
}
