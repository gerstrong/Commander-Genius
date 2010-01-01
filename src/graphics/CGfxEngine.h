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
	
	CSprite *createEmptySprites(Uint16 num_sprites);
	CBitmap *createEmptyBitmaps(Uint16 num_bmps);
	CTilemap *createEmptyTilemap(stTile *pTileProperty, int numtiles);
	CFont *createEmptyFontmap();
	
	void pushEffectPtr(CEffects *pEffect);

	void freeBitmaps();
	void freeSprites();
	void freeTilemap();
	void freeFonts();
	
	void copyTileToSprite( Uint16 t, Uint16 s, Uint16 ntilestocopy );
	
	void drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h, Uint32 colour = 0xFFFFFF);
	int getNumSprites() { return Sprite.size(); }
	CEffects *Effect();
	
	CBitmap *getBitmap(const std::string &name);
	
	void process();
	
	CFont *Font;
	CTilemap *Tilemap;
	CPalette Palette;
	std::vector<CSprite> Sprite;
	std::vector<CBitmap*> Bitmap;
	
private:
	SDL_Surface *m_fxsurface;
	CEffects *mp_Effects;
};

#endif /* CGFXENGINE_H_ */
