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

#include "CFont.h"
#include "CTilemap.h"
#include "CSprite.h"
#include "CBitmap.h"
#include "CPalette.h"
#include "CCursor.h"
#include "effects/CEffects.h"
#include <vector>

#include "../CSingleton.h"
#define g_pGfxEngine CGfxEngine::Get()

enum backgroundcolours{
	WHITE, GRAY
};

class CGfxEngine : public CSingleton<CGfxEngine>
{
public:
	CGfxEngine();
	virtual ~CGfxEngine();
	
	void createEmptySprites(Uint16 num_sprites);
	void createEmptyBitmaps(Uint16 num_bmps);
	void createEmptyFontmaps(Uint8 num_fonts);
	void createEmptyTilemap(stTile *pTileProperties, int numtiles);
	void createEmptyCursorMap(SDL_Surface *surface);
	
	void pushEffectPtr(CEffects *pEffect);

	void freeBitmaps();
	void freeSprites();
	void freeTilemap();
	void freeFonts();
	
	void copyTileToSprite( Uint16 t, Uint16 s, Uint16 ntilestocopy );
	
	void drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour = 0xFFFFFF);
	int getNumSprites() { return Sprite.size(); }

	CBitmap &getBitmap(Uint16 slot) { return Bitmap.at(slot); }
	CBitmap *getBitmap(const std::string &name);

	CEffects *Effect() { return mp_Effects; }
	bool applyingEffects() { return mp_Effects!=NULL; }
	
	CSprite &getSprite(Uint16 slot) { return Sprite[slot]; }
	std::vector<CSprite> &getSpriteVec() { return Sprite; }
	CFont &getFont(Uint8 index) { return Font.at(index); }
	CCursor *getCursor() { return mp_Cursor; }
	
	void process();
	void killEffect();
	
	CTilemap *Tilemap;
	CPalette Palette;
	
private:
	std::vector<CFont> Font;
	SDL_Surface *m_fxsurface;
	CEffects *mp_Effects;
	std::vector<CBitmap> Bitmap;
	std::vector<CBitmap> maskedBitmap;
	std::vector<CSprite> Sprite;
	CCursor *mp_Cursor;
};

#endif /* CGFXENGINE_H_ */
