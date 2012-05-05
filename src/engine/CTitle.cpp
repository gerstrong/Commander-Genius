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
	pSurface = g_pVideoDriver->mp_VideoEngine->getBlitSurface();
	if(!g_pVideoDriver->getSpecialFXConfig())
		g_pGfxEngine->setupEffect(new CColorMerge(16));
	else
		g_pGfxEngine->setupEffect(new CPixelate(16));
	
	if( (pBitmap = g_pGfxEngine->getBitmap("TITLE")) != NULL )
	{
		p_object = new CEGABitmap( &m_map, pSurface, pBitmap );
		p_object->setScrPos( 160-(pBitmap->getWidth()/2), 0 );
		m_objects.push_back(p_object);
	}

	if( (pBitmap = g_pGfxEngine->getBitmap("F1HELP")) != NULL )
	{
		pBitmap = g_pGfxEngine->getBitmap("F1HELP");
		p_object = new CEGABitmap( &m_map, pSurface, pBitmap );

		p_object->setScrPos( (Episode == 3) ? 128 : 96, 182 );
		m_objects.push_back(p_object);
	}
	
	m_map.changeTileArrayY(2, 15, 2, g_pGfxEngine->getTileMap(1).EmptyBackgroundTile());

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
	{
		if(m_objects.back())
			delete m_objects.back();
		m_objects.pop_back();
	}
}

