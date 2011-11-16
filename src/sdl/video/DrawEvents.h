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

#include "engine/CEvent.h"
#include "graphics/CSprite.h"
#include "graphics/CTilemap.h"
//#include "gui/CGUIDialog.h"


// GUI Based Draw Task

/*struct DrawGUIRenderTask : CEvent
{
	CGUIDialog	*mpDialog;

	DrawGUIRenderTask(CGUIDialog *pDialog) : mpDialog(pDialog) {}
};*/

// Tile based Draw Tasks

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



struct DrawSpriteTask : CEvent
{
	CSprite *mSpritePtr;
	const Uint16 mx, my;
	const Uint8	mAlpha;
	DrawSpriteTask(CSprite *SpritePtr, const Uint16 x, const Uint16 y, const Uint8 alpha) :
		mSpritePtr(SpritePtr), mx(x), my(y), mAlpha(alpha)  {}
};


#endif /* DRAWEVENTS_H_ */
