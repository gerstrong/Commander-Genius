/*
 * GsGraphics.h
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 *
 *  This is the new Graphics Engine. It will replace the old one.
 *  It blit with SDL directly instead of using arrays copies
 */

#ifndef __GSGRAPHICS_H__
#define __GSGRAPHICS_H__

#include <base/Singleton.h>

#include "GsFont.h"
#include "GsTilemap.h"
#include "GsSprite.h"
#include "GsBitmap.h"
#include "GsPalette.h"
#include "GsCursor.h"

#include <base/video/GsEffectController.h>
#include <vector>
#include <memory>


#define gGraphics GsGraphics::get()


class GsGraphics : public GsSingleton<GsGraphics>
{
public:
	virtual ~GsGraphics();
	
    void createEmptySprites(const int numVar, const int num_sprites);
	void dumpSprites();
	void createEmptyBitmaps(Uint16 num_bmps);
    void createEmptyMaskedBitmaps(Uint16 num_bmps);
    void createEmptyMisGsBitmaps(Uint16 num_bmps);
    void createEmptyFontmaps(Uint8 num_fonts);
	void createEmptyTilemaps(size_t num);
	void createEmptyCursorMap(SDL_Surface *surface);
	
	void drawDigits(const std::string& text, Uint16 x, Uint16 y, SDL_Surface *blitsurface);
	void drawDigit(const char c, const Uint16 x, const Uint16 y, SDL_Surface *blitsurface);
	
    void copyTileToSprite( const int var, Uint16 t, Uint16 s, Uint16 ntilestocopy );
	
	void drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour = 0xFFFFFF);
    int getNumSprites(const int var) { return Sprite[var].size(); }

    GsTilemap &getTileMap(size_t tilemap) { return Tilemap.at(tilemap); }
    std::vector<GsTilemap> &getTileMaps() { return Tilemap; }
    GsBitmap &getBitmapFromId(Uint16 slot) { return Bitmap.at(slot); }
    GsBitmap &getMaskedBitmap(Uint16 slot) { return maskedBitmap.at(slot); }
    GsBitmap &getMisGsBitmap(Uint16 slot) { return misGsBitmap.at(slot); }
    GsBitmap *getBitmapFromStr(const std::string &name) const;

    GsSprite *getSprite(const int var, const std::string &name) const;
	
    GsSprite &getSprite(const int var, const int slot) { return Sprite[var][slot]; }
    std::vector<GsSprite> &getSpriteVec(const int var) { return Sprite[var]; }
	GsFont &getFont(Uint8 index) { return Font.at(index); }
	
    GsPalette Palette;

    void freeTilemap();
    void freeFonts();

	
private:

    void freeBitmaps(std::vector<GsBitmap> &Bitmap);
    void freeSprites();

    std::vector<GsTilemap> Tilemap;
	std::vector<GsFont> Font;
    std::vector<GsBitmap> Bitmap;
    std::vector<GsBitmap> maskedBitmap;
    std::vector<GsBitmap> misGsBitmap;
    std::vector< std::vector<GsSprite> > Sprite;

};

#endif /* __GSGRAPHICS_H__ */
