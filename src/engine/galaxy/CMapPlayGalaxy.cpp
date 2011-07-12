/*
 * CMapPlayGalaxy.cpp
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 */

#include "CMapPlayGalaxy.h"
#include "engine/galaxy/ai/CPlayerBase.h"
#include "common/CBehaviorEngine.h"

CMapPlayGalaxy::CMapPlayGalaxy(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode) :
m_active(false),
m_ExeFile(ExeFile),
m_Inventory(Inventory),
mp_option(g_pBehaviorEngine->m_option),
m_Cheatmode(Cheatmode)
{}




bool CMapPlayGalaxy::isActive()
{	return m_active;	}




void CMapPlayGalaxy::setActive(bool value)
{
	m_active = value;

	if(m_active)
	{
		m_Map.drawAll();
	    // Set Scrollbuffer
	    g_pVideoDriver->setScrollBuffer(&m_Map.m_scrollx_buf, &m_Map.m_scrolly_buf);
	}
}




/**
 * return the name of the opened level
 */
std::string CMapPlayGalaxy::getLevelName()
{
	return m_Map.getLevelName();
}







void CMapPlayGalaxy::process(const bool msgboxactive)
{
	// Check if the engine need to be paused
	const bool pause = m_Inventory.showStatus() || msgboxactive;

	// Animate the tiles of the map
	m_Map.m_animation_enabled = !pause;
	m_Map.animateAllTiles();

	if(!pause)
	{
		for(size_t i=0 ; i<m_ObjectPtr.size() ; i++)
		{
			CObject* p_Object = m_ObjectPtr[i];

			// If the Player is not only dying, but also lost it's existence, meaning he got out of the screen
			// how the death-message or go gameover.
			if( galaxy::CPlayerBase *player = dynamic_cast<galaxy::CPlayerBase*>(p_Object) )
			{
				// Is he really dead?
				if( !player->exists )
					player->processDead();
			}

			if( p_Object->exists && p_Object->calcVisibility() )
			{
				// Process the AI of the object as it's given
				p_Object->process();

				// Check the post-collision stuff, where some
				// sprites are pushed out in case they get stuck
				//p_Object->processPushOutCollision();

				// process all the objects' events
				p_Object->processEvents();

				// Check collision between objects
				for(size_t j=0 ; j<m_ObjectPtr.size() ; j++)
				{
					CObject *theOtherObj = m_ObjectPtr[j];
					if( theOtherObj != p_Object )
					{
						p_Object->isNearby(*theOtherObj);
						if( p_Object->hitdetect(*theOtherObj) )
							p_Object->getTouchedBy(*theOtherObj);
					}
				}
			}
		}
	}

	g_pVideoDriver->blitScrollSurface();

	std::vector<CObject*>::reverse_iterator obj;

	// Draw all the sprites but no player
	for( obj=m_ObjectPtr.rbegin() ;
			obj!=m_ObjectPtr.rend() ; obj++ )
	{
		if((*obj)->honorPriority && (*obj)->m_type != OBJ_PLAYER)
			(*obj)->draw();
	}

	// Now only draw the player sprite. So everything expect maked tiles are below his layer
	for( obj = m_ObjectPtr.rbegin() ; obj!=m_ObjectPtr.rend() ; obj++ )
		if((*obj)->m_type == OBJ_PLAYER)
			(*obj)->draw();

	// Draw masked tiles here!
	m_Map.drawForegroundTiles();

	for( obj=m_ObjectPtr.rbegin() ;
			obj!=m_ObjectPtr.rend() ; obj++ )
	{
		if(!(*obj)->honorPriority)
			(*obj)->draw();
	}

	if(mp_option[OPT_HUD].value )
		m_Inventory.drawHUD();


	CEventContainer &EventContainer = g_pBehaviorEngine->m_EventList;
	if( EventSpawnObject *ev =  EventContainer.occurredEvent<EventSpawnObject>() )
	{
		m_ObjectPtr.push_back( const_cast<CObject*>(ev->pObject) );
		EventContainer.pop_Event();
	}

}




CMapPlayGalaxy::~CMapPlayGalaxy()
{
	while(!m_ObjectPtr.empty())
	{
		delete m_ObjectPtr.back();
		m_ObjectPtr.pop_back();
	}
}
