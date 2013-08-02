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
#include "engine/CPlanes.h"
#include "FindFile.h"
#include "sdl/CVideoDriver.h"
#include "engine/spritedefines.h"
#include "fileio/lz.h"
#include "fileio/ResourceMgmt.h"
#include "common/CBehaviorEngine.h"
#include "CVorticonSpriteObject.h"
#include "CResourceLoader.h"
#include "graphics/CGfxEngine.h"
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

//////////////////////////
// section of the class //
//////////////////////////

CEGASprit::CEGASprit(int planesize,
	long spritestartloc,
	int numsprites,
	long spriteloc,
	const std::string &gamepath,
	size_t episode) :
m_gamepath(gamepath),
m_Episode(episode),
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
	byte *RawData;
    	SDL_Surface *sfc;
    	Uint8* pixel;
    	Uint32 percent = 0;
	
	FILE* latchfile = OpenGameFile(filename.c_str(),"rb");
	
	if(!latchfile)
		return false;
	
	g_pResourceLoader->setPermilage(10);

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

	g_pResourceLoader->setPermilage(50);
	
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
    g_pGfxEngine->createEmptySprites(1,MAX_SPRITES+1);
	for(int i=0 ; i<m_numsprites ; i++)
	{
        CSprite &Sprite = g_pGfxEngine->getSprite(0,i);
		Sprite.setSize( EGASpriteModell[i].width, EGASpriteModell[i].height );
		Sprite.setBoundingBoxCoordinates( (EGASpriteModell[i].hitbox_l << STC),
				(EGASpriteModell[i].hitbox_u << STC),
				(EGASpriteModell[i].hitbox_r << STC),
				(EGASpriteModell[i].hitbox_b << STC) );
		Sprite.createSurface( g_pVideoDriver->mpVideoEngine->getBlitSurface()->flags,
				g_pGfxEngine->Palette.m_Palette );

		percent = (i*50)/m_numsprites;
		g_pResourceLoader->setPermilage(50+percent);
	}

	g_pResourceLoader->setPermilage(100);

	for(int p=0 ; p<4 ; p++)
	{
		for(int s=0 ; s<m_numsprites ; s++)
		{
            sfc = g_pGfxEngine->getSprite(0,s).getSDLSurface();
			if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
			pixel = (Uint8*) sfc->pixels;

			Planes.readPlane(p, pixel, sfc->w, sfc->h);

			if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);

			percent = (s*100)/m_numsprites;
			g_pResourceLoader->setPermilage(100+percent);
		}
	}

	g_pResourceLoader->setPermilage(200);

	// now load the 5th plane, which contains the sprite masks.
	// note that we invert the mask because our graphics functions
	// use white on black masks whereas keen uses black on white.
	for(int s=0 ; s<m_numsprites ; s++)
	{
        CSprite &Sprite = g_pGfxEngine->getSprite(0,s);
		SDL_Surface *pixsfc = Sprite.getSDLSurface();
		SDL_Surface *masksfc = Sprite.getSDLMaskSurface();

		if(SDL_MUSTLOCK(pixsfc)) SDL_LockSurface(pixsfc);
		if(SDL_MUSTLOCK(masksfc)) SDL_LockSurface(masksfc);

		pixel = (Uint8*) masksfc->pixels;

		for(int y=0 ; y<masksfc->h ; y++)
		{
			for(int x=0 ; x<masksfc->w ; x++)
			{
				if(Planes.getbit(4))
					pixel[y*masksfc->w + x] = ((Uint8*)pixsfc->pixels)[y*pixsfc->w + x];
				else
					pixel[y*masksfc->w + x] = 15;
			}
		}
		if(SDL_MUSTLOCK(masksfc)) SDL_UnlockSurface(masksfc);
		if(SDL_MUSTLOCK(pixsfc)) SDL_UnlockSurface(pixsfc);

		percent = (s*100)/m_numsprites;
		g_pResourceLoader->setPermilage(200+percent);
	}

	g_pResourceLoader->setPermilage(300);
	
	if(RawData){ delete[] RawData; RawData = NULL;}
	
	// Now load the special TGA Sprites if some are available
	//LoadSpecialSprites( g_pGfxEngine->getSpriteVec() );

    for(Uint16 s=0 ; s<g_pGfxEngine->getSpriteVec(0).size() ; s++)
	{
        CSprite &Sprite = g_pGfxEngine->getSprite(0,s);
		Sprite.optimizeSurface();

		percent = (s*50)/m_numsprites;
		g_pResourceLoader->setPermilage(300+percent);
	}

	g_pResourceLoader->setPermilage(350);

	std::set<std::string> filelist;
	FileListAdder fileListAdder;
	std::string gfxpath = JoinPaths(m_gamepath, "gfx");
	GetFileList(filelist, fileListAdder, gfxpath, false, FM_REG);
	FilterFilelist(filelist, "sprite");

	std::set<std::string>::iterator it = filelist.begin();
	int listsize = filelist.size();
	for( int c=0 ; it != filelist.end() ; it++, c++ )
	{
		std::string name=*it;
		int num = getRessourceID(name, "sprite");
		if(num < m_numsprites )
		{
            CSprite &Sprite = g_pGfxEngine->getSprite(0,num);
			std::string filename = getResourceFilename("gfx/"+name, m_gamepath, false, true);
			Sprite.loadHQSprite(filename);
		}

		percent = (c*150)/listsize;
		g_pResourceLoader->setPermilage(350+percent);
	}

	g_pResourceLoader->setPermilage(500);

    const int NoSprites = g_pGfxEngine->getSpriteVec(0).size();
	for(Uint16 s=0 ; s<NoSprites ; s++)
	{
        g_pGfxEngine->getSprite(0,s).applyTransparency();

		percent = (s*250)/NoSprites;
		g_pResourceLoader->setPermilage(500+percent);
	}
	
    LoadSpecialSprites( g_pGfxEngine->getSpriteVec(0) );

	g_pResourceLoader->setPermilage(750);
	// Apply the sprites for player 2,3 and 4
    DerivePlayerSprites( g_pGfxEngine->getSpriteVec(0) );
	g_pResourceLoader->setPermilage(900);


	// Now create special sprites, like those for effects and the doors!
    DeriveSpecialSprites( g_pGfxEngine->getTileMap(1), g_pGfxEngine->getSpriteVec(0) );
	g_pResourceLoader->setPermilage(950);

	// Here special Effects are applied, only when the option is enabled for it
	if(g_pVideoDriver->getSpecialFXConfig())
		ApplySpecialFX();

	g_pResourceLoader->setPermilage(1000);

    for(unsigned int i=1 ; i<MAX_PLAYERS ; i++)
    {
        g_pGfxEngine->getSpriteVec(i) = g_pGfxEngine->getSpriteVec(0);
    }

	return true;
}

// load special clonekeen-specific sprites from the .raw files
// Code by Caitlin Shaw
void CEGASprit::LoadSpecialSprites( std::vector<CSprite> &sprite )
{
	sprite[PT100_SPRITE].generateSprite( 100 );
	sprite[PT200_SPRITE].generateSprite( 200 );
	sprite[PT500_SPRITE].generateSprite( 500 );
	sprite[PT1000_SPRITE].generateSprite( 1000 );
	sprite[PT5000_SPRITE].generateSprite( 5000 );
}

void CEGASprit::DerivePlayerSprites( std::vector<CSprite> &sprites )
{
	// create the sprites for player 2, 3 and 4
	for( size_t i=0 ; i<48 ; i++ )
	{
		size_t s = SECOND_PLAYER_BASEFRAME+i;
		sprites.at(i).copy( sprites.at(s), g_pGfxEngine->Palette.m_Palette );
		sprites.at(s).replaceSpriteColor( 13, 11 ,0 );
		sprites.at(s).replaceSpriteColor( 5, 3 ,0 );
		sprites.at(s).replaceSpriteColor( 12, 9 ,0 );
		sprites.at(s).replaceSpriteColor( 4, 1 ,0 );
		sprites.at(s).optimizeSurface();
	}
	for(size_t i=0;i<48;i++)
	{
		size_t s = THIRD_PLAYER_BASEFRAME+i;

		sprites.at(i).copy( sprites.at(s), g_pGfxEngine->Palette.m_Palette );
		sprites.at(s).replaceSpriteColor( 13, 10, 0 ); // Shirt light
		sprites.at(s).replaceSpriteColor( 5, 2, 0 ); // Shirt dark
		sprites.at(s).replaceSpriteColor( 12, 2, 16 ); // Shoes light
		sprites.at(s).replaceSpriteColor( 4, 0, 16 ); // Shoes dark
		sprites.at(s).optimizeSurface();

	}
	for(size_t i=0;i<48;i++)
	{
		size_t s = FOURTH_PLAYER_BASEFRAME+i;
		sprites.at(i).copy( sprites.at(s), g_pGfxEngine->Palette.m_Palette );
		sprites.at(s).replaceSpriteColor( 13, 14, 0 ); // Shirt light
		sprites.at(s).replaceSpriteColor( 5, 6, 0 ); // Shirt dark
		sprites.at(s).replaceSpriteColor( 12, 6, 16 ); // Shoes light
		sprites.at(s).replaceSpriteColor( 4, 0, 16 ); // Shoes dark
		sprites.at(s).optimizeSurface();
	}

    for(Uint16 s=0 ; s<g_pGfxEngine->getSpriteVec(0).size() ; s++)
	{
        CSprite &Sprite = g_pGfxEngine->getSprite(0,s);
		Sprite.optimizeSurface();
	}

	// Try to load player sprites here!
	std::set<std::string> filelist;
	FileListAdder fileListAdder;
	std::string gfxpath = JoinPaths(m_gamepath, "gfx");
	GetFileList(filelist, fileListAdder, gfxpath, false, FM_REG);
	FilterFilelist(filelist, "sprite");
	std::set<std::string>::iterator it = filelist.begin();
	for( ; it != filelist.end() ; it++ )
	{
		std::string name=*it;
		int num = getRessourceID(name, "sprite");
		if( num>=SECOND_PLAYER_BASEFRAME && num<FOURTH_PLAYER_BASEFRAME+48 )
		{
            CSprite &Sprite = g_pGfxEngine->getSprite(0,num);
			std::string filename = getResourceFilename("gfx/"+name, m_gamepath, false, true);
			Sprite.loadHQSprite(filename);
			Sprite.applyTransparency();
		}
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
		if( TileProperties.at(t).behaviour==19 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDR_SPRITE));
		if( TileProperties.at(t).behaviour==20 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDG_SPRITE));
		if( TileProperties.at(t).behaviour==21 )	CreateYellowSpriteofTile( tilemap, t, sprites.at(PTCARDB_SPRITE));
		
		// Single Bullet in Ep3
		if( TileProperties.at(t).behaviour==28 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(SHOTUP_SPRITE) );
		
		if( TileProperties.at(t).behaviour==27 )
			CreateYellowSpriteofTile( tilemap, t, sprites.at(ANKHUP_SPRITE) );
	}

	for(size_t i=1 ; i < TileProperties.size() ; i++ )
	{
		if(TileProperties.at(i).behaviour == DOOR_YELLOW)
            g_pGfxEngine->copyTileToSprite(0,i-1, DOOR_YELLOW_SPRITE, 2);

		if(TileProperties.at(i).behaviour == DOOR_RED)
            g_pGfxEngine->copyTileToSprite(0,i-1, DOOR_RED_SPRITE, 2);

		if(TileProperties.at(i).behaviour == DOOR_GREEN)
            g_pGfxEngine->copyTileToSprite(0,i-1, DOOR_GREEN_SPRITE, 2);

		if(TileProperties.at(i).behaviour == DOOR_BLUE)
            g_pGfxEngine->copyTileToSprite(0,i-1, DOOR_BLUE_SPRITE, 2);
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
	sprite.createSurface( g_pVideoDriver->mpVideoEngine->getBlitSurface()->flags,
						  g_pGfxEngine->Palette.m_Palette );
	sprite.optimizeSurface();
	
	SDL_Surface *src_sfc = sprite.getSDLSurface();
	
	SDL_BlitSurface(tilemap.getSDLSurface(), &tile_rect, src_sfc, NULL);
	
	if(SDL_MUSTLOCK(src_sfc)) SDL_LockSurface(src_sfc);
	
	if(src_sfc->format->BitsPerPixel == 8) return;

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
	switch(m_Episode)
	{
	case 1:
        g_pGfxEngine->getSprite(0,OBJ_RAY_DEFSPRITE_EP1).applyTranslucency(200);
        g_pGfxEngine->getSprite(0,RAY_FRAME_ZAP_EP1).applyTranslucency(200);
        g_pGfxEngine->getSprite(0,RAY_FRAME_ZOT_EP1).applyTranslucency(200);
		break;
	case 2:
        g_pGfxEngine->getSprite(0,OBJ_RAY_DEFSPRITE_EP2).applyTranslucency(200);
        g_pGfxEngine->getSprite(0,RAY_FRAME_ZAP_EP2).applyTranslucency(200);
        g_pGfxEngine->getSprite(0,RAY_FRAME_ZOT_EP2).applyTranslucency(200);
		break;
	case 3:
        g_pGfxEngine->getSprite(0,OBJ_RAY_DEFSPRITE_EP3).applyTranslucency(200);
        g_pGfxEngine->getSprite(0,RAY_FRAME_ZAP_EP3).applyTranslucency(200);
        g_pGfxEngine->getSprite(0,RAY_FRAME_ZOT_EP3).applyTranslucency(200);
		break;
	default: break;
	}

    g_pGfxEngine->getSprite(0,PT100_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PT200_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PT500_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PT1000_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PT5000_SPRITE).applyTranslucency(196);

    g_pGfxEngine->getSprite(0,PTCARDY_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PTCARDG_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PTCARDR_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,PTCARDB_SPRITE).applyTranslucency(196);

    g_pGfxEngine->getSprite(0,GUNUP_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,SHOTUP_SPRITE).applyTranslucency(196);
    g_pGfxEngine->getSprite(0,ANKHUP_SPRITE).applyTranslucency(196);

}

CEGASprit::~CEGASprit() {
	if(EGASpriteModell) delete [] EGASpriteModell, EGASpriteModell = NULL;
}
