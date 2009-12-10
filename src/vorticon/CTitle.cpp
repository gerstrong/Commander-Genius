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

////
// Creation Routine
////
CTitle::CTitle(std::vector<CObject*> &Objects) :
m_objects(Objects),
m_time(0)
{}

bool CTitle::init(int Episode)
{
	CObject *p_object;
	SDL_Surface *pSurface;
	CBitmap *pBitmap;
	g_pTimer->ResetSecondsTimer();
	m_time = 10; // show the title screen for 10 secs.
	pSurface = g_pVideoDriver->BlitSurface;
	
	pBitmap = g_pGfxEngine->getBitmap("TITLE");
	p_object = new CEGABitmap( pSurface, pBitmap );
	p_object->setScrPos( 160-(pBitmap->getWidth()/2), 0 );
	m_objects.push_back(p_object);

	pBitmap = g_pGfxEngine->getBitmap("F1HELP");
	p_object = new CEGABitmap( pSurface, pBitmap );

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();
	p_object->setScrPos( (Episode == 3) ? 128 : 96, gameres.h-18 );

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

