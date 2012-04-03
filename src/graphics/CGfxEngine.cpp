/*
 * CGfxEngine.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CGfxEngine.h"

#include "../sdl/input/CInput.h"

#define SAFE_DELETE(x) 	if(x){ delete x; x=NULL;}

CGfxEngine::CGfxEngine() :
m_fxsurface(NULL),
mp_Effects(NULL),
mp_Cursor(NULL)
{}

///
// Creation Routines
///
void CGfxEngine::createEmptyTilemap(size_t num)
{
	freeTilemap();
	CTilemap oneTilemap;
	Tilemap.assign(num, oneTilemap);
}

void CGfxEngine::createEmptySprites(Uint16 num_sprites)
{
	freeSprites();
	CSprite sprite;
	Sprite.assign(num_sprites, sprite);
}

void CGfxEngine::createEmptyBitmaps(Uint16 num_bmps)
{
	freeBitmaps(Bitmap);
	CBitmap bitmap;
	Bitmap.assign(num_bmps, bitmap);
}

void CGfxEngine::createEmptyMaskedBitmaps(Uint16 num_bmps)
{
	freeBitmaps(maskedBitmap);
	CBitmap bitmap;
	maskedBitmap.assign(num_bmps, bitmap);
}


void CGfxEngine::createEmptyFontmaps(Uint8 num_fonts)
{
	freeFonts();
	CFont font;
	Font.assign(num_fonts, font);
}

void CGfxEngine::createEmptyCursorMap(SDL_Surface *surface)
{
	freeCursor();
	mp_Cursor = new CCursor(surface);
}

// This will store the effect pointer the developer created in one function
// You need this call to make you effect work!
void CGfxEngine::pushEffectPtr(CEffects *pEffect)
{
	SAFE_DELETE(mp_Effects);
	mp_Effects = pEffect;
}

/**
 * Draws some digits using galaxy style
 */
void CGfxEngine::drawDigits(const std::string& text, Uint16 x, Uint16 y, SDL_Surface *blitsurface)
{
	CTilemap &Tilemap = getTileMap(2);

	for(Uint16 i=0 ; i<text.size() ; i++)
	{
		const char c = text[i];
		Tilemap.drawTile(blitsurface, x+i*8, y, (c != ' ') ? 43+c-'1' : 41);
	}
}

/**
 * Draw one digits using galaxy style
 */
void CGfxEngine::drawDigit(const char c, const Uint16 x, const Uint16 y, SDL_Surface *blitsurface)
{
	CTilemap &Tilemap = getTileMap(2);
	Tilemap.drawTile(blitsurface, x, y, c);
}


///
// Destructors
///
void CGfxEngine::freeCursor()
{
	SAFE_DELETE(mp_Cursor);
}
void CGfxEngine::freeTilemap()
{
	if( !Tilemap.empty() )
		Tilemap.clear();
}
void CGfxEngine::freeFonts()
{
	while ( !Font.empty() )
	{
		CFont &font = Font.back();
		font.destroySurface();
		Font.pop_back();
	}
}

void CGfxEngine::freeBitmaps(std::vector<CBitmap> &Bitmap)
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
	Sprite[s].createSurface( Tilemap.at(1).getSDLSurface()->flags, Palette.m_Palette );
	
	for(Uint8 i=0 ; i<ntilestocopy ; i++)
	{
		src_rect.x = 16*((t+i)%13);
		src_rect.y = 16*((t+i)/13);
		
		dst_rect.x = 0;
		dst_rect.y = 16*i;

		SDL_BlitSurface( Tilemap.at(1).getSDLSurface(), &src_rect, Sprite[s].getSDLSurface(), &dst_rect);
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

	Font[0].drawCharacter(DialogSurface, 1, x1*8, y1*8);
	Font[0].drawCharacter(DialogSurface, 3, (x1+w)*8, y1*8);
	for(x=(x1*8)+8,i=0;i<w-1;i++)
	{
		Font[0].drawCharacter(DialogSurface, 2, x, y1*8);
		x+=8;
	}
	y=(y1+1)*8;
	for(j=0;j<h-2;j++)
	{
		for(x=(x1*8),i=0;i<=w;i++)
		{
			if (i==0) Font[0].drawCharacter(DialogSurface, 4, x, y);
			else if (i==w) Font[0].drawCharacter(DialogSurface, 5, x, y);
			x+=8;
		}
		y+=8;
	}
    for(x=(x1*8),i=0;i<=w;i++)
    {
		if (i==0) Font[0].drawCharacter(DialogSurface, 6, x, y);
		else if (i==w) Font[0].drawCharacter(DialogSurface, 8, x, y);
		else Font[0].drawCharacter(DialogSurface, 7, x, y);
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
void CGfxEngine::process()
{
	/*if(mp_Effects)
	{
		mp_Effects->process();

		if(runningEffect())
		{
			killEffect();
		}
	}*/
}

bool CGfxEngine::runningEffect(){
	return ( mp_Effects && mp_Effects->finished());
}

// Kills the effect when called
void CGfxEngine::killEffect()
{
	SAFE_DELETE(mp_Effects);
}

CGfxEngine::~CGfxEngine() {
	SAFE_DELETE(mp_Effects);
	freeCursor();
	freeBitmaps(maskedBitmap);
	freeBitmaps(Bitmap);
	freeSprites();
	freeFonts();
	freeTilemap();
}
