/*
 * CTitle.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * See CTitle.h for more information
 */

#include "CTitle.h"
#include <base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include <graphics/CColorMerge.h>
#include <graphics/CPixelate.h>
#include <widgets/GsMenuController.h>
#include <base/CInput.h>

#include "engine/core/menu/MainMenu.h"
#include "ai/CEGABitmap.h"

#include <engine/core/CBehaviorEngine.h>

namespace vorticon
{

////
// Creation Routine
////
Title::Title(std::shared_ptr<CMap> &map ) :
mpMap(map)
{}

bool Title::init(int Episode)
{
	SDL_Surface *pSurface;
	GsBitmap *pBitmap;
	gTimer.ResetSecondsTimer();
	mTime = 10; // show the title screen for 10 secs.
	pSurface = gVideoDriver.mpVideoEngine->getBlitSurface();
    const auto modern = gBehaviorEngine.mOptions[GameOption::MODERN].value;
    if(!gBehaviorEngine.mOptions[GameOption::SPECIALFX].value)
        gEffectController.setupEffect(new CColorMerge(16));
    else
        gEffectController.setupEffect(new CPixelate(2.0f, modern));
	
    if( (pBitmap = gGraphics.getBitmapFromStr(0, "TITLE")) != nullptr )
	{
		const int width = 160-(pBitmap->width()/2);
        std::unique_ptr<CSpriteObject> obj(new CEGABitmap( mpMap, pSurface, pBitmap ));
		obj->setScrPos( width, 0 );		
        pBitmap->_draw( width, 0, pSurface );
		obj->draw();
		mObjects.push_back(std::move(obj));
	}


    const GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    if(gameRes.dim.x == 320 && gameRes.dim.y == 200 )
    {
        if( (pBitmap = gGraphics.getBitmapFromStr(0, "F1HELP")) != nullptr )
        {
            const int width = (Episode == 3) ? 128 : 96;
            pBitmap = gGraphics.getBitmapFromStr(0, "F1HELP");
            std::unique_ptr<CSpriteObject> obj(new CEGABitmap( mpMap, pSurface, pBitmap ));
            obj->setScrPos( width, 182 );
            pBitmap->_draw( width, 182, pSurface);
            mObjects.push_back(move(obj));
        }
    }
	
    mpMap->changeTileArrayY(2, 15, 2, gGraphics.getTileMap(1).EmptyBackgroundTile());

	mFinished = false;

	return true;
}

////
// Process Routines
////
void Title::ponder()
{
	if( mTime == 0) mFinished = true;
	else mTime -= gTimer.HasSecElapsed();
	
	for( auto &obj : mObjects )
	{
	    obj->process();
	}

    // If menu is closed, no effects are running, furthermore if a button was pressed
    if( !gEffectController.runningEffect() && !gMenuController.active() )
	{
        if( gInput.getPressedAnyCommand() || gInput.mouseClicked() )
		{
            gEventManager.add(new OpenMainMenuEvent());
		}	    
	}

}

void Title::render()
{
    for( auto &obj : mObjects )
    {
        obj->draw();
    }

}

}
