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



struct DrawSpriteTask : CEvent
{
	CSprite *mSpritePtr;
	const Uint16 mx, my;
	const Uint8	mAlpha;
	DrawSpriteTask(CSprite *SpritePtr, const Uint16 x, const Uint16 y, const Uint8 alpha) :
    mSpritePtr(SpritePtr), mx(x), my(y), mAlpha(alpha)  {}
};

struct BlitSurfaceTask : CEvent
{
	std::shared_ptr<SDL_Surface> mSfcToBlit;
	std::shared_ptr<SDL_Rect> mSrcRect;
	std::shared_ptr<SDL_Rect> mDstRect;
	
	BlitSurfaceTask( std::shared_ptr<SDL_Surface> &sfcToBlit,
                    SDL_Rect *srcRect,
                    SDL_Rect *dstRect ) :
	mSfcToBlit(sfcToBlit)
	{
		if(srcRect) // because SDL can have NULL-Pointers in the Rect sources
		{			// Copy these objects, because they might vanish!
		    mSrcRect.reset(new SDL_Rect);
		    *mSrcRect = *srcRect;
			
		}
		if(dstRect)
		{
		    mDstRect.reset(new SDL_Rect);
		    *mDstRect = *dstRect;
		}
	}
	
	BlitSurfaceTask( std::shared_ptr<SDL_Surface> &sfcToBlit,
                    std::shared_ptr<SDL_Rect> &srcRect,
                    std::shared_ptr<SDL_Rect> &dstRect ) :
	mSfcToBlit(sfcToBlit),
	mSrcRect(srcRect),
	mDstRect(dstRect)
	{ }
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
