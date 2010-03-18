/*
 * CTitle.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * See CTitle.h for more information
 */

#include "CTitle.h"
#include "../sdl/CTimer.h"
#include "../sdl/CVideoDriver.h"
#include "ai/CEGABitmap.h"
//#include "../graphics/effects/CColorMerge.h"
#include "../graphics/effects/CPixelate.h"

////
// Creation Routine
////
CTitle::CTitle(std::vector<CObject*> &Objects, CMap &map) :
m_objects(Objects),
m_time(0),
m_map(map)
{}

bool CTitle::init(int Episode)
{
	CObject *p_object;
	SDL_Surface *pSurface;
	CBitmap *pBitmap;
	g_pTimer->ResetSecondsTimer();
	m_time = 10; // show the title screen for 10 secs.
	pSurface = g_pVideoDriver->BlitSurface;
	//g_pGfxEngine->pushEffectPtr(new CColorMerge(8));
	g_pGfxEngine->pushEffectPtr(new CPixelate(3));
	
	pBitmap = g_pGfxEngine->getBitmap("TITLE");
	p_object = new CEGABitmap( &m_map, pSurface, pBitmap );
	p_object->setScrPos( 160-(pBitmap->getWidth()/2), 0 );
	m_objects.push_back(p_object);

	pBitmap = g_pGfxEngine->getBitmap("F1HELP");
	p_object = new CEGABitmap( &m_map, pSurface, pBitmap );

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();
	p_object->setScrPos( (Episode == 3) ? 128 : 96, gameres.h-18 );

	if(gameres.h > 200) // This happens, when the have higher game res height, and ugly unseen
	{ 					// normally hidden tiles are seen. We replace those tiles
		for(Uint16 x=0 ; x<22 ; x++)
		{
			m_map.setTile(x, 0, g_pGfxEngine->Tilemap->EmptyBackgroundTile(), true);
			m_map.setTile(x, 1, g_pGfxEngine->Tilemap->EmptyBackgroundTile(), true);
		}
	}

	m_objects.push_back(p_object);
	
	m_finished = false;

	return true;
}

////
// Process Routine
////
void CTitle::process()
{
	if( m_time == 0) m_finished = true;
	else m_time -= g_pTimer->HasSecElapsed();
}

////
// Cleanup Routine
////
CTitle::~CTitle() {
	while( !m_objects.empty() )
		m_objects.pop_back();
}

