/*
 * CGfxEngine.h
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 *
 *  This is the new Graphics Engine. It will replace the old one.
 *  It blit with SDL directly instead of using arrays copies
 */

#ifndef CGFXENGINE_H_
#define CGFXENGINE_H_

#include <graphics/GsFont.h>
#include "CTilemap.h"
#include "CSprite.h"
#include "CBitmap.h"
#include "CPalette.h"
#include "CCursor.h"

#include <base/video/GsEffectController.h>
#include <vector>
#include <memory>

#include "CSingleton.h"
#define g_pGfxEngine CGfxEngine::Get()

class CGfxEngine : public CSingleton<CGfxEngine>
{
public:
	virtual ~CGfxEngine();
	
    void createEmptySprites(const int numVar, const int num_sprites);
	void dumpSprites();
	void createEmptyBitmaps(Uint16 num_bmps);
    void createEmptyMaskedBitmaps(Uint16 num_bmps);
    void createEmptyMiscBitmaps(Uint16 num_bmps);
    void createEmptyFontmaps(Uint8 num_fonts);
	void createEmptyTilemaps(size_t num);
	void createEmptyCursorMap(SDL_Surface *surface);
	
	void drawDigits(const std::string& text, Uint16 x, Uint16 y, SDL_Surface *blitsurface);
	void drawDigit(const char c, const Uint16 x, const Uint16 y, SDL_Surface *blitsurface);

	void freeBitmaps(std::vector<CBitmap> &Bitmap);
	void freeSprites();
	void freeTilemap();
	void freeFonts();
	void freeCursor();
	
    void copyTileToSprite( const int var, Uint16 t, Uint16 s, Uint16 ntilestocopy );
	
	void drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour = 0xFFFFFF);
    int getNumSprites(const int var) { return Sprite[var].size(); }

	CTilemap &getTileMap(size_t tilemap) { return Tilemap.at(tilemap); }
	std::vector<CTilemap> &getTileMaps() { return Tilemap; }
    CBitmap &getBitmapFromId(Uint16 slot) { return Bitmap.at(slot); }
    CBitmap &getMaskedBitmap(Uint16 slot) { return maskedBitmap.at(slot); }
    CBitmap &getMiscBitmap(Uint16 slot) { return miscBitmap.at(slot); }
    CBitmap *getBitmapFromStr(const std::string &name) const;

    CSprite *getSprite(const int var, const std::string &name) const;


	
    CSprite &getSprite(const int var, const int slot) { return Sprite[var][slot]; }
    std::vector<CSprite> &getSpriteVec(const int var) { return Sprite[var]; }
	GsFont &getFont(Uint8 index) { return Font.at(index); }
	CCursor *getCursor() { return mpCursor.get(); }


	
    void ponder();
    void render();
	
	CPalette Palette;
	
private:
	std::vector<CTilemap> Tilemap;
	std::vector<GsFont> Font;
	std::vector<CBitmap> Bitmap;
    std::vector<CBitmap> maskedBitmap;
    std::vector<CBitmap> miscBitmap;
    std::vector< std::vector<CSprite> > Sprite;
	std::unique_ptr<CCursor> mpCursor;
};

#endif /* CGFXENGINE_H_ */
