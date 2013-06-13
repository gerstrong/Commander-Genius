/*
 * DrawEvents.h
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  This header file will hold all the draw events that are used during the gameplay
 */

#ifndef DRAWEVENTS_H_
#define DRAWEVENTS_H_

#include <memory>
#include "engine/CEvent.h"
#include "graphics/CSprite.h"
#include "graphics/CTilemap.h"
#include "graphics/CBitmap.h"
#include "gui/CGUIDialog.h"
#include "common/CMap.h"
#include "sdl/extensions.h"


// GUI Based Draw Task

struct DrawGUIRenderTask : CEvent
{
	CGUIDialog *mpDialog;

	DrawGUIRenderTask( CGUIDialog *pDialog ) : mpDialog(pDialog) {}
};

// Tile based Draw Tasks

struct DrawForegroundTilesTask : CEvent
{
    CMap &mMap;
    DrawForegroundTilesTask(CMap &map) : mMap(map) {};
};


struct DrawAnimatedTileTask : CEvent
{
	CTilemap 	*mTileMapPtr;
	const Uint16 mx, my;
	const Uint16 mtile;

	DrawAnimatedTileTask(CTilemap *TileMapPtr, const Uint16 x, const Uint16 y, const Uint16 tile) :
		mTileMapPtr(TileMapPtr), mx(x), my(y), mtile(tile) {}
};

// Sprite based Draw Tasks

struct DrawBlinkingSpriteTask : CEvent
{
	CSprite *mSpritePtr;
	const Uint16 mx, my;
	DrawBlinkingSpriteTask(CSprite *SpritePtr, const Uint16 x, const Uint16 y) :
		mSpritePtr(SpritePtr), mx(x), my(y) {}
};



struct DrawBitmapTask : CEvent
{
	CBitmap *mBmpPtr;
	const Uint16 mx, my;
	DrawBitmapTask(CBitmap *BmpPtr, const Uint16 x, const Uint16 y):
		mBmpPtr(BmpPtr), mx(x), my(y) {}
};


struct BlitScrollSurfaceTask : CEvent
{};


#endif /* DRAWEVENTS_H_ */
