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

#include "../CSingleton.h"
#define g_pGfxEngine CGfxEngine::Get()

class CGfxEngine : public CSingleton<CGfxEngine>
{
public:
	CGfxEngine();
	virtual ~CGfxEngine();

	void setColorPalettes(SDL_Color *Palette);

	void drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h);

	CFont Font;
	CTilemap Tilemap;
};

#endif /* CGFXENGINE_H_ */
