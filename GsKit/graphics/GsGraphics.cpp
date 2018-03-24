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

#include <base/video/GsEffectController.h>

#include "GsFont.h"
#include "GsTilemap.h"
#include "GsSprite.h"
#include "GsBitmap.h"
#include "GsPalette.h"
#include "GsCursor.h"


///
// Creation Routines
///
void GsGraphics::createEmptyTilemaps(const size_t num)
{
	freeTilemap();
    Tilemap.resize(num);
}

void GsGraphics::createEmptySprites(const size_t numVar,
                                    const size_t numSprites)
{
	freeSprites();
    GsSprite sprite;

    std::vector<GsSprite> spriteVec;

    spriteVec.assign(numSprites, sprite);

    mSprite.assign(numVar, spriteVec);
}

void GsGraphics::dumpSprites()
{
    for(size_t i = 0; i < mSprite[0].size(); i++)
	{
        GsSprite &thisSprite = mSprite[0][i];

        //const auto thisSfc = thisSprite.Surface().getSDLSurface();
        auto &thisSfc = thisSprite.Surface();

		// Temporary for testing!!
		std::string fname = "sprite";
		fname += itoa(i);
		fname += ".bmp";                
        SDL_SaveBMP(thisSfc.getSDLSurface(), fname.c_str());
	}
}


void GsGraphics::optimizeSprites()
{    
    for(auto &spriteVec : mSprite)
    {
        for(auto &sprite : spriteVec)
        {            
            sprite.optimizeSurface();
        }
    }    
}

void GsGraphics::createEmptyBitmaps(Uint16 num_bmps)
{
    freeBitmaps(mBitmap);
    GsBitmap bitmap;
    mBitmap.assign(num_bmps, bitmap);
}

void GsGraphics::createEmptyMaskedBitmaps(Uint16 num_bmps)
{
	freeBitmaps(maskedBitmap);
    GsBitmap bitmap;
	maskedBitmap.assign(num_bmps, bitmap);
}

void GsGraphics::createEmptyMisGsBitmaps(Uint16 num_bmps)
{
    freeBitmaps(miscGsBitmap);
    GsBitmap bitmap;
    miscGsBitmap.assign(num_bmps, bitmap);
}



void GsGraphics::createEmptyFontmaps(Uint8 num_fonts)
{
	freeFonts();
    Font.assign(num_fonts, GsFont());
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
    if( !mSprite.empty() )
        mSprite.clear();
}

void GsGraphics::copyTileToSprite( const int var, Uint16 t, Uint16 s, Uint16 ntilestocopy )
{
	SDL_Rect src_rect, dst_rect;

    auto tileMapPtr = Tilemap.at(1).getSDLSurface();
	
	src_rect.w = src_rect.h = 16;
	dst_rect.w = dst_rect.h = 16;

    mSprite[var][s].setSize( 16, 16*ntilestocopy );
    mSprite[var][s].createSurface( tileMapPtr->flags, Palette.m_Palette );
	
	for(Uint8 i=0 ; i<ntilestocopy ; i++)
	{
		src_rect.x = 16*((t+i)%13);
		src_rect.y = 16*((t+i)/13);
		
		dst_rect.x = 0;
		dst_rect.y = 16*i;

        //auto spriteSfc = mSprite[var][s].Surface().getSDLSurface();
        //BlitSurface( tileMapPtr, &src_rect, spriteSfc.get(), &dst_rect);

        auto &spriteSfc = mSprite[var][s].Surface();
        BlitSurface( tileMapPtr, &src_rect, spriteSfc.getSDLSurface(), &dst_rect);
	}
}

// draw an empty dialog box, for youseeinyourmind(), etc.
// Maybe this should go to GsFont, because it's drawn by fonts...
void GsGraphics::drawDialogBox(SDL_Surface *DialogSurface,
                               int x1, int y1,
                               int w, int h,
                               Uint32 colour)
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

int GsGraphics::getNumSprites(const int var)
{   return mSprite[var].size(); }

GsTilemap &GsGraphics::getTileMap(size_t tilemap)
{   return Tilemap.at(tilemap); }

std::vector<GsTilemap> &GsGraphics::getTileMaps()
{   return Tilemap; }

GsBitmap &GsGraphics::getBitmapFromId(Uint16 slot)
{   return mBitmap[slot];    }

GsBitmap &GsGraphics::getMaskedBitmap(Uint16 slot)
{   return maskedBitmap[slot];   }

GsBitmap &GsGraphics::getMiscGsBitmap(Uint16 slot)
{   return miscGsBitmap[slot];   }


GsSprite &GsGraphics::getSprite(const int var, const int slot)
{   return mSprite[var][slot];  }

std::vector<GsSprite> &GsGraphics::getSpriteVec(const int var)
{   return mSprite[var];    }

GsSprite &GsGraphics::getSpecialSpriteRef(const std::string &name)
{   return mSpecialSpriteMap[name];    }

GsFont &GsGraphics::getFont(Uint8 index) { return Font.at(index); }

GsBitmap *GsGraphics::getBitmapFromStr(const std::string &name) const
{
	std::string s_name;
    for(unsigned int i=0 ; i<mBitmap.size() ; i++)
	{
        s_name = mBitmap[i].getName();

		if(s_name == name)
        {
            return const_cast<GsBitmap*>(&mBitmap[i]);
        }
	}

	std::string error = "Ooops! Wrong TextID ";
	error += "name";
	error += "used!";
	gLogging.textOut(error);

    return nullptr;
}

GsSprite *GsGraphics::getSprite(const int var, const std::string &name) const
{
	std::string s_name;
    for(unsigned int i=0 ; i<mSprite[var].size() ; i++)
	{
        s_name = mSprite[var][i].getName();

		if(s_name == name)
        {
            return const_cast<GsSprite*>(&(mSprite[var][i]));
        }
	}

    return nullptr;
}

///
// Process Routines
///



GsGraphics::~GsGraphics()
{
	freeBitmaps(maskedBitmap);
    freeBitmaps(mBitmap);
	freeSprites();
	freeFonts();
	freeTilemap();
}
