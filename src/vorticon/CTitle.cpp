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

////
// Creation Routine
////
CTitle::CTitle() {
	m_finished = false;
	m_time = 0;
	mp_bmp_surface = g_pVideoDriver->FGLayerSurface;
}

bool CTitle::init()
{
	g_pTimer->ResetSecondsTimer();
	m_time = 10; // show the title screen for 10 sec
	mp_bitmap_title = g_pGfxEngine->getBitmap("TITLE");
	mp_bitmap_f1help = g_pGfxEngine->getBitmap("F1HELP");

	// draw those Bitmaps just once as only animated tiles are redrawn here.
	mp_bitmap_title->draw( mp_bmp_surface, 160, 0);
	mp_bitmap_f1help->draw( mp_bmp_surface, 160, 160);

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
void CTitle::cleanup()
{
	
}

CTitle::~CTitle() {

}

