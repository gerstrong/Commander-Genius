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
//#include "ai/CEGABitmap.h"
#include "graphics/effects/CColorMerge.h"
#include "graphics/effects/CPixelate.h"
#include <widgets/GsMenuController.h>
#include <base/CInput.h>

#include "menu/CMainMenu.h"
#include "ai/CEGABitmap.h"

namespace vorticon
{

////
// Creation Routine
////
Title::Title( CMap &map ) :
mTime(0),
mMap(map)
{}

bool Title::init(int Episode)
{
	SDL_Surface *pSurface;
	GsBitmap *pBitmap;
	gTimer.ResetSecondsTimer();
	mTime = 10; // show the title screen for 10 secs.
	pSurface = gVideoDriver.mpVideoEngine->getBlitSurface();
	if(!gVideoDriver.getSpecialFXConfig())
        gEffectController.setupEffect(new CColorMerge(16));
	else
        gEffectController.setupEffect(new CPixelate(16));
	
    if( (pBitmap = gGraphics.getBitmapFromStr("TITLE")) != NULL )
	{
		const int width = 160-(pBitmap->getWidth()/2);
		std::unique_ptr<CSpriteObject> obj(new CEGABitmap( &mMap, pSurface, pBitmap ));
		obj->setScrPos( width, 0 );		
        pBitmap->_draw( width, 0, pSurface );
		obj->draw();
		mObjects.push_back(std::move(obj));
	}


    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    if(gameRes.w == 320 && gameRes.h == 200 )
    {
        if( (pBitmap = gGraphics.getBitmapFromStr("F1HELP")) != NULL )
        {
            const int width = (Episode == 3) ? 128 : 96;
            pBitmap = gGraphics.getBitmapFromStr("F1HELP");
            std::unique_ptr<CSpriteObject> obj(new CEGABitmap( &mMap, pSurface, pBitmap ));
            obj->setScrPos( width, 182 );
            pBitmap->_draw( width, 182, pSurface);
            mObjects.push_back(move(obj));
        }
    }
	
	mMap.changeTileArrayY(2, 15, 2, gGraphics.getTileMap(1).EmptyBackgroundTile());

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
