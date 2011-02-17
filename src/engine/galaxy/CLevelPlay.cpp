/*
 * CLevelPlay.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CLevelPlay.h"
#include "CMapLoaderGalaxy.h"
#include "sdl/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CMusic.h"
#include "CLogFile.h"

namespace galaxy {

CLevelPlay::CLevelPlay(CExeFile &ExeFile,
		CInventory &Inventory, stOption *p_option) :
m_active(false),
m_ExeFile(ExeFile),
m_Inventory(Inventory),
mp_option(p_option)
{ }

bool CLevelPlay::isActive()
{	return m_active;	}

void CLevelPlay::setActive(bool value)
{	m_active = value;	}

bool CLevelPlay::loadLevel(const Uint16 level)
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(m_ExeFile, m_ObjectPtr, m_Inventory);

	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	MapLoader.loadMap(m_Map, level);

    // Load the Background Music
	g_pMusicPlayer->stop();

    if(!g_pMusicPlayer->load(m_ExeFile, level))
    	g_pLogFile->textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    else
    	g_pMusicPlayer->play();

	m_Map.drawAll();
	return true;
}

void CLevelPlay::process()
{
	// Animate the tiles of the map
	m_Map.animateAllTiles();

	for(size_t i=0 ; i<m_ObjectPtr.size() ; i++)
	{
		CObject* p_Object = m_ObjectPtr[i];

		if(p_Object->exists)
		{
			p_Object->process();

			// Check collision between objects
			/*for( std::vector<CObject*>::iterator theOtherObj=m_ObjectPtr.begin() ;
					theOtherObj != m_ObjectPtr.end() ; theOtherObj++ )*/
			for(size_t j=0 ; j<m_ObjectPtr.size() ; j++)
			{
				CObject *theOtherObj = m_ObjectPtr[j];
				if( theOtherObj != p_Object )
					p_Object->getTouchedBy(*theOtherObj);
			}
		}
	}

	g_pVideoDriver->blitScrollSurface();

	for( std::vector<CObject*>::iterator obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		if((*obj)->honorPriority)
			(*obj)->draw();
	}

	// Draw masked tiles here!
	m_Map.drawForegroundTiles();

	for( std::vector<CObject*>::iterator obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		if(!(*obj)->honorPriority)
			(*obj)->draw();
	}

	if(mp_option[OPT_HUD].value )
		m_Inventory.drawHUD();
}

}
