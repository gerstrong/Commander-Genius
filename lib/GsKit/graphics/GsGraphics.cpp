/*
 * GsGraphics.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "GsGraphics.h"
#include <base/GsLogging.h>
#include <base/utils/StringUtils.h>
#include <base/CInput.h>

///
// Creation Routines
///
void GsGraphics::createEmptyTilemaps(size_t num)
{
	freeTilemap();
    GsTilemap oneTilemap;
	Tilemap.assign(num, oneTilemap);
}

void GsGraphics::createEmptySprites(const int numVar, const int num_sprites)
{
	freeSprites();
    GsSprite sprite;

    std::vector<GsSprite> spriteVec;

    spriteVec.assign(num_sprites, sprite);

    Sprite.assign(numVar, spriteVec);
}

void GsGraphics::dumpSprites()
{
    for(size_t i = 0; i < Sprite[0].size(); i++)
	{
        GsSprite &thisSprite = Sprite[0][i];

		// Temporary for testing!!
		std::string fname = "sprite";
		fname += itoa(i);
		fname += ".bmp";
		SDL_SaveBMP(thisSprite.getSDLSurface(), fname.c_str());
	}
}

void GsGraphics::createEmptyBitmaps(Uint16 num_bmps)
{
	freeBitmaps(Bitmap);
    GsBitmap bitmap;
	Bitmap.assign(num_bmps, bitmap);
}

void GsGraphics::createEmptyMaskedBitmaps(Uint16 num_bmps)
{
	freeBitmaps(maskedBitmap);
    GsBitmap bitmap;
	maskedBitmap.assign(num_bmps, bitmap);
}

void GsGraphics::createEmptyMisGsBitmaps(Uint16 num_bmps)
{
    freeBitmaps(misGsBitmap);
    GsBitmap bitmap;
    misGsBitmap.assign(num_bmps, bitmap);
}



void GsGraphics::createEmptyFontmaps(Uint8 num_fonts)
{
	freeFonts();
	GsFont font;
	Font.assign(num_fonts, font);
}

/*void GsGraphics::createEmptyCursorMap(SDL_Surface *surface)
{
	freeCursor();
    mpCursor.reset(new GsCursor(surface));
}*/

/**
 * Draws some digits using galaxy style
 */
void GsGraphics::drawDigits(const std::string& text, Uint16 x, Uint16 y, SDL_Surface *blitsurface)
{
    GsTilemap &Tilemap = getTileMap(2);

	for(Uint16 i=0 ; i<text.size() ; i++)
	{
		const char c = text[i];
		Tilemap.drawTile(blitsurface, x+i*8, y, (c != ' ') ? 43+c-'1' : 41);
	}
}

/**
 * Draw one digits using galaxy style
 */
void GsGraphics::drawDigit(const char c, const Uint16 x, const Uint16 y, SDL_Surface *blitsurface)
{
    GsTilemap &Tilemap = getTileMap(2);
	Tilemap.drawTile(blitsurface, x, y, c);
}


///
// Destructors
///
/*void GsGraphics::freeCursor()
{
	mpCursor.release();
}*/
void GsGraphics::freeTilemap()
{
	if( !Tilemap.empty() )
		Tilemap.clear();
}
void GsGraphics::freeFonts()
{
  while ( !Font.empty() )
    Font.pop_back();
}

void GsGraphics::freeBitmaps(std::vector<GsBitmap> &Bitmap)
{
	while ( !Bitmap.empty() )
	{
		Bitmap.pop_back();
	}
}

void GsGraphics::freeSprites()
{
    if( !Sprite.empty() )
        Sprite.clear();
}

void GsGraphics::copyTileToSprite( const int var, Uint16 t, Uint16 s, Uint16 ntilestocopy )
{
	SDL_Rect src_rect, dst_rect;
	
	src_rect.w = src_rect.h = 16;
	dst_rect.w = dst_rect.h = 16;

    Sprite[var][s].setSize( 16, 16*ntilestocopy );
    Sprite[var][s].createSurface( Tilemap.at(1).getSDLSurface()->flags, Palette.m_Palette );
	
	for(Uint8 i=0 ; i<ntilestocopy ; i++)
	{
		src_rect.x = 16*((t+i)%13);
		src_rect.y = 16*((t+i)/13);
		
		dst_rect.x = 0;
		dst_rect.y = 16*i;

        BlitSurface( Tilemap.at(1).getSDLSurface(), &src_rect, Sprite[var][s].getSDLSurface(), &dst_rect);
	}
}

// draw an empty dialog box, for youseeinyourmind(), etc.
// Maybe this should go to GsFont, because it's drawn by fonts...
void GsGraphics::drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour)
{
	int x,y,i,j;
	SDL_Rect rect;
	rect.x = x1*8; rect.w = (w+1)*8;
	rect.y = y1*8; rect.h = (h+1)*8;
	
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

GsBitmap *GsGraphics::getBitmapFromStr(const std::string &name) const
{
	std::string s_name;
	for(unsigned int i=0 ; i<Bitmap.size() ; i++)
	{
		s_name = Bitmap[i].getName();

		if(s_name == name)
            return const_cast<GsBitmap*>(&Bitmap[i]);
	}

	std::string error = "Ooops! Wrong TextID ";
	error += "name";
	error += "used!";
	gLogging.textOut(error);

	return NULL;
}

GsSprite *GsGraphics::getSprite(const int var, const std::string &name) const
{
	std::string s_name;
    for(unsigned int i=0 ; i<Sprite[var].size() ; i++)
	{
        s_name = Sprite[var][i].getName();

		if(s_name == name)
            return const_cast<GsSprite*>(&(Sprite[var][i]));
	}

	return NULL;
}

///
// Process Routines
///



GsGraphics::~GsGraphics()
{
//	freeCursor();
	freeBitmaps(maskedBitmap);
	freeBitmaps(Bitmap);
	freeSprites();
	freeFonts();
	freeTilemap();
}
