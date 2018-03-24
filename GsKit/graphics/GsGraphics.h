/*
 * GsGraphics.h
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 *
 *  This is the new Graphics Engine. It will replace the old one.
 *  It blit with SDL directly instead of using arrays copies
 */

#ifndef GSGRAPHICS_H
#define GSGRAPHICS_H


#include <base/TypeDefinitions.h>

#include "GsTilemap.h"
#include "GsSprite.h"
#include "GsBitmap.h"
#include "GsPalette.h"
#include "GsCursor.h"

#include <vector>
#include <memory>
#include <unordered_map>

#include <base/Singleton.h>
#define gGraphics GsGraphics::get()


class GsFont;

class GsGraphics : public GsSingleton<GsGraphics>
{
public:
	virtual ~GsGraphics();
	
    void dumpSprites();

    void createEmptySprites(const size_t numVar,
                            const size_t num_sprites);

	void createEmptyBitmaps(Uint16 num_bmps);
    void createEmptyMaskedBitmaps(Uint16 num_bmps);
    void createEmptyMisGsBitmaps(Uint16 num_bmps);
    void createEmptyFontmaps(Uint8 num_fonts);
    void createEmptyTilemaps(const size_t num);
	void createEmptyCursorMap(SDL_Surface *surface);
	
	void drawDigits(const std::string& text, Uint16 x, Uint16 y, SDL_Surface *blitsurface);
	void drawDigit(const char c, const Uint16 x, const Uint16 y, SDL_Surface *blitsurface);
	
    void copyTileToSprite( const int var, Uint16 t, Uint16 s, Uint16 ntilestocopy );
	
	void drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour = 0xFFFFFF);
    int getNumSprites(const int var);

    GsTilemap &getTileMap(size_t tilemap);
    std::vector<GsTilemap> &getTileMaps();
    GsBitmap &getBitmapFromId(Uint16 slot);
    GsBitmap &getMaskedBitmap(Uint16 slot);
    GsBitmap &getMiscGsBitmap(Uint16 slot);
    GsBitmap *getBitmapFromStr(const std::string &name) const;

    GsSprite *getSprite(const int var, const std::string &name) const;
	
    GsSprite &getSprite(const int var, const int slot);
    std::vector<GsSprite> &getSpriteVec(const int var);

    GsSprite &getSpecialSpriteRef(const std::string &name);
    
    void optimizeSprites();

    GsFont &getFont(Uint8 index);
	
    GsPalette Palette;

    void freeTilemap();
    void freeFonts();

	
private:

    void freeBitmaps(std::vector<GsBitmap> &mBitmap);
    void freeSprites();

	std::vector<GsFont> Font;
    std::vector<GsTilemap> Tilemap;
    std::vector<GsBitmap> mBitmap;
    std::vector<GsBitmap> maskedBitmap;
    std::vector<GsBitmap> miscGsBitmap;
    std::vector< std::vector<GsSprite> > mSprite;
    
    std::unordered_map<std::string, GsSprite> mSpecialSpriteMap;
};

#endif /* GSGRAPHICS_H */
