/*
 * CTitle.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * See CTitle.h for more information
 */

#include "CTitle.h"
#include "sdl/CTimer.h"
#include "sdl/CVideoDriver.h"
#include "vorticon/ai/CEGABitmap.h"
#include "graphics/effects/CColorMerge.h"
#include "graphics/effects/CPixelate.h"

////
// Creation Routine
////
CTitle::CTitle( CMap &map ) :
mTime(0),
mMap(map)
{}

bool CTitle::init(int Episode)
{
	CSpriteObject *pObject;
	SDL_Surface *pSurface;
	CBitmap *pBitmap;
	g_pTimer->ResetSecondsTimer();
	mTime = 10; // show the title screen for 10 secs.
	pSurface = g_pVideoDriver->mp_VideoEngine->getBlitSurface();
	if(!g_pVideoDriver->getSpecialFXConfig())
		g_pGfxEngine->setupEffect(new CColorMerge(16));
	else
		g_pGfxEngine->setupEffect(new CPixelate(16));
	
	if( (pBitmap = g_pGfxEngine->getBitmap("TITLE")) != NULL )
	{
		const int width = 160-(pBitmap->getWidth()/2);
		pObject = new CEGABitmap( &mMap, pSurface, pBitmap );
		pObject->setScrPos( width, 0 );
		mObjects.push_back(pObject);
		pBitmap->_draw( pSurface, width, 0);
		pObject->draw();
	}

	if( (pBitmap = g_pGfxEngine->getBitmap("F1HELP")) != NULL )
	{
		const int width = (Episode == 3) ? 128 : 96;
		pBitmap = g_pGfxEngine->getBitmap("F1HELP");
		pObject = new CEGABitmap( &mMap, pSurface, pBitmap );
		pObject->setScrPos( width, 182 );
		pBitmap->_draw( pSurface, width, 182);
		mObjects.push_back(pObject);
	}
	
	mMap.changeTileArrayY(2, 15, 2, g_pGfxEngine->getTileMap(1).EmptyBackgroundTile());

	mFinished = false;

	return true;
}

////
// Process Routine
////
void CTitle::process()
{
	if( mTime == 0) mFinished = true;
	else mTime -= g_pTimer->HasSecElapsed();

	std::vector< SmartPointer<CSpriteObject> >::iterator obj = mObjects.begin();
	for( ; obj != mObjects.end() ; obj++ )
	{
		obj->get()->process();
	}

}
