/*
 * CGfxEngine.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CGfxEngine.h"

#define SAFE_DELETE(x) 	if(x){ delete x; x=NULL;}

CGfxEngine::CGfxEngine() :
Tilemap(NULL),
m_fxsurface(NULL),
mp_Effects(NULL)
{}

///
// Creation Routines
///
void CGfxEngine::createEmptyTilemap(stTile *pTileProperties, int numtiles)
{
	Tilemap = new CTilemap(pTileProperties, numtiles);
}

void CGfxEngine::createEmptySprites(Uint16 num_sprites)
{
	CSprite sprite;
	Sprite.assign(num_sprites, sprite);
}

void CGfxEngine::createEmptyBitmaps(Uint16 num_bmps)
{
	CBitmap bitmap;
	Bitmap.assign(num_bmps, bitmap);
}

// This will store the effect pointer the developer created in one function
// You need this call to make you effect work!
void CGfxEngine::pushEffectPtr(CEffects *pEffect) {
	SAFE_DELETE(mp_Effects);
	mp_Effects = pEffect;
}

///
// Destructors
///
void CGfxEngine::freeTilemap()
{
	if(Tilemap) delete Tilemap;
	Tilemap = NULL;
}
void CGfxEngine::freeFonts()
{
	Font.DestroySurface();
}

void CGfxEngine::freeBitmaps()
{
	while ( !Bitmap.empty() )
	{
		CBitmap &bitmap = Bitmap.back();
		bitmap.destroySurface();
		Bitmap.pop_back();
	}
}

void CGfxEngine::freeSprites()
{
	while( !Sprite.empty() )
	{
		CSprite &sprite = Sprite.back();
		sprite.freeSurfaces();
		Sprite.pop_back();
	}
}

void CGfxEngine::copyTileToSprite( Uint16 t, Uint16 s, Uint16 ntilestocopy )
{
	SDL_Rect src_rect, dst_rect;
	
	src_rect.w = src_rect.h = 16;
	dst_rect.w = dst_rect.h = 16;

	Sprite[s].setSize( 16, 16*ntilestocopy );
	Sprite[s].createSurface( Tilemap->getSDLSurface()->flags, Palette.m_Palette );
	
	for(Uint8 i=0 ; i<ntilestocopy ; i++)
	{
		src_rect.x = 16*((t+i)%13);
		src_rect.y = 16*((t+i)/13);
		
		dst_rect.x = 0;
		dst_rect.y = 16*i;

		SDL_BlitSurface( Tilemap->getSDLSurface(), &src_rect, Sprite[s].getSDLSurface(), &dst_rect);
	}
}

// draw an empty dialog box, for youseeinyourmind(), etc.
// Maybe this should go to CFont, because it's drawn by fonts...
void CGfxEngine::drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour)
{
	int x,y,i,j;
	SDL_Rect rect;
	rect.x = x1*8; rect.w = w*8;
	rect.y = y1*8; rect.h = h*8;
	
	SDL_FillRect(DialogSurface, &rect, colour);

	Font.drawCharacter(DialogSurface, 1, x1*8, y1*8);
	Font.drawCharacter(DialogSurface, 3, (x1+w)*8, y1*8);
	for(x=(x1*8)+8,i=0;i<w-1;i++)
	{
		Font.drawCharacter(DialogSurface, 2, x, y1*8);
		x+=8;
	}
	y=(y1+1)*8;
	for(j=0;j<h-2;j++)
	{
		for(x=(x1*8),i=0;i<=w;i++)
		{
			if (i==0) Font.drawCharacter(DialogSurface, 4, x, y);
			else if (i==w) Font.drawCharacter(DialogSurface, 5, x, y);
			x+=8;
		}
		y+=8;
	}
    for(x=(x1*8),i=0;i<=w;i++)
    {
		if (i==0) Font.drawCharacter(DialogSurface, 6, x, y);
		else if (i==w) Font.drawCharacter(DialogSurface, 8, x, y);
		else Font.drawCharacter(DialogSurface, 7, x, y);
		x+=8;
    }
}

///
// Getters
///

CBitmap *CGfxEngine::getBitmap(const std::string &name)
{
	std::string s_name;
	for(Uint8 i=0 ; i<Bitmap.size() ; i++)
	{
		s_name = Bitmap[i].getName();

		if(s_name == name)
			return &Bitmap[i];
	}
	return NULL;
}

///
// Process Routines
///
void CGfxEngine::process(){
	if(mp_Effects){
		mp_Effects->process();

		if(mp_Effects->finished())
			SAFE_DELETE(mp_Effects);
	}
}

CGfxEngine::~CGfxEngine() {
	SAFE_DELETE(mp_Effects);
	freeBitmaps();
	freeSprites();
	freeFonts();
	freeTilemap();
}
