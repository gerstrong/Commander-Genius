/*
 * CGfxEngine.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CGfxEngine.h"

CGfxEngine::CGfxEngine() {
	Font = NULL;
	Tilemap = NULL;
	m_fxsurface = NULL;
}

CGfxEngine::~CGfxEngine() {
	freeBitmaps();
	freeSprites();
	freeFonts();
	freeTilemap();
}

///
// Creation Routines
///
CTilemap *CGfxEngine::createEmptyTilemap(stTile *pTileProperties, int numtiles)
{
	Tilemap = new CTilemap(pTileProperties, numtiles);
	return Tilemap;
}

CFont *CGfxEngine::createEmptyFontmap()
{
	Font = new CFont();
	return Font;
}

CSprite *CGfxEngine::createEmptySprites(Uint16 num_sprites)
{
	Sprite.reserve(num_sprites);
	for(Uint16 i=0 ; i<num_sprites ; i++ )
		Sprite.push_back( new CSprite() );
	
	if(!Sprite.empty())	return Sprite[0];
	else return NULL;
}

CBitmap *CGfxEngine::createEmptyBitmaps(Uint16 num_bmps)
{
	Bitmap.reserve(num_bmps);
	for(Uint16 i=0 ; i<num_bmps ; i++ )
		Bitmap.push_back( new CBitmap() );
	
	if (!Bitmap.empty()) return Bitmap[0];
	else return NULL;
	
}

void CGfxEngine::freeTilemap()
{
	if(Tilemap) delete Tilemap;
}
void CGfxEngine::freeFonts()
{
	if(Font) delete Font;
}

void CGfxEngine::freeBitmaps()
{
	CBitmap *bitmap;
	while( Bitmap.size() > 0 )
	{
		bitmap = *Bitmap.end();
		if(bitmap) delete bitmap;
		Bitmap.pop_back();
	}
}

void CGfxEngine::freeSprites()
{
	CSprite *sprite;
	while( Sprite.size() > 0 )
	{
		sprite = *Sprite.end();
		if(sprite) delete sprite;
		Sprite.pop_back();
	}
}

CBitmap *CGfxEngine::getBitmap(const std::string &name)
{
	std::string s_name;
	for(Uint8 i=0 ; i<Bitmap.size() ; i++)
	{
		s_name = Bitmap[i]->getName();
		
		if(s_name == name)
			return Bitmap[i];
	}
	return NULL;
}

void CGfxEngine::copyTileToSprite( Uint16 t, Uint16 s, Uint16 ntilestocopy )
{
	SDL_Rect src_rect, dst_rect;
	
	src_rect.w = src_rect.h = 16;
	dst_rect.w = dst_rect.h = 16;
	
	
	Sprite[s]->setSize( 16, 16*ntilestocopy );
	Sprite[s]->createSurface( Tilemap->getSDLSurface()->flags, Palette.m_Palette );
	
	for(Uint8 i=0 ; i<ntilestocopy ; i++)
	{
		src_rect.x = 16*((t+i)%13);
		src_rect.y = 16*((t+i)/13);
		
		dst_rect.x = 0;
		dst_rect.y = 16*i;
		
		SDL_BlitSurface( Tilemap->getSDLSurface(), &src_rect, Sprite[s]->getSDLSurface(), &dst_rect);
	}
}

// draw an empty dialog box, for youseeinyourmind(), etc.
// Maybe this should go to CFont, because it's drawn by fonts...
void CGfxEngine::drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour)
{
	int x,y,i,j;
	SDL_Rect rect;
	rect.x = x1*9; rect.w = w*8;
	rect.y = y1*9; rect.h = h*8;
	
	SDL_FillRect(DialogSurface, &rect, colour);

	Font->drawCharacter(DialogSurface, 1, x1*8, y1*8);
	Font->drawCharacter(DialogSurface, 3, (x1+w)*8, y1*8);
	for(x=(x1*8)+8,i=0;i<w-1;i++)
	{
		Font->drawCharacter(DialogSurface, 2, x, y1*8);
		x+=8;
	}
	y=(y1+1)*8;
	for(j=0;j<h-2;j++)
	{
		for(x=(x1*8),i=0;i<=w;i++)
		{
			if (i==0) Font->drawCharacter(DialogSurface, 4, x, y);
			else if (i==w) Font->drawCharacter(DialogSurface, 5, x, y);
			x+=8;
		}
		y+=8;
	}
    for(x=(x1*8),i=0;i<=w;i++)
    {
		if (i==0) Font->drawCharacter(DialogSurface, 6, x, y);
		else if (i==w) Font->drawCharacter(DialogSurface, 8, x, y);
		else Font->drawCharacter(DialogSurface, 7, x, y);
		x+=8;
    }
}
