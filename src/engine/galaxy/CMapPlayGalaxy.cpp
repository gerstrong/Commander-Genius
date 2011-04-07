/*
 * CMapPlayGalaxy.cpp
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 */

#include "CMapPlayGalaxy.h"

CMapPlayGalaxy::CMapPlayGalaxy(CExeFile &ExeFile, CInventory &Inventory) :
m_active(false),
m_ExeFile(ExeFile),
m_Inventory(Inventory),
mp_option(g_pBehaviorEngine->m_option)
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

void CMapPlayGalaxy::process()
{
	// Check if the engine need to be paused
	const bool pause = m_Inventory.showStatus() || !m_MessageBoxes.empty();

	// Animate the tiles of the map
	m_Map.m_animation_enabled = !pause;
	m_Map.animateAllTiles();

	if(!pause)
	{
		for(size_t i=0 ; i<m_ObjectPtr.size() ; i++)
		{
			CObject* p_Object = m_ObjectPtr[i];

			if(p_Object->exists)
			{
				// Process the AI of the object as it's given
				p_Object->process();

				// process all the objects' events
				p_Object->processEvents();

				// Check collision between objects
				for(size_t j=0 ; j<m_ObjectPtr.size() ; j++)
				{
					CObject *theOtherObj = m_ObjectPtr[j];
					if( theOtherObj != p_Object )
						p_Object->getTouchedBy(*theOtherObj);
				}
			}
		}
	}

	g_pVideoDriver->blitScrollSurface();

	std::vector<CObject*>::iterator obj;

	// Draw all the sprites but no player
	for( obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		if((*obj)->honorPriority && (*obj)->m_type != OBJ_PLAYER)
			(*obj)->draw();
	}

	// Now only draw the player sprite. So everything expect maked tiles are below his layer
	for( obj = m_ObjectPtr.begin() ; obj!=m_ObjectPtr.end() ; obj++ )
		if((*obj)->m_type == OBJ_PLAYER)
			(*obj)->draw();

	// Draw masked tiles here!
	m_Map.drawForegroundTiles();

	for( obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		if(!(*obj)->honorPriority)
			(*obj)->draw();
	}

	if(mp_option[OPT_HUD].value )
		m_Inventory.drawHUD();

	// Draw some Textboxes with Messages only if one of those is open and needs to be drawn
	if(!m_MessageBoxes.empty())
	{
		CMessageBoxGalaxy *pMB = m_MessageBoxes.front();
		pMB->process();

		if(pMB->isFinished())
		{
			delete(pMB);
			pMB = NULL;
			m_MessageBoxes.pop_front();
		}
		return;
	}


	//// Special Keyboard Input

	/// Cheat Codes
	if( g_pInput->getHoldedKey(KF10) )
	{
		if(g_pInput->getHoldedKey(KJ))
		{
			m_Cheatmode.jump = !m_Cheatmode.jump;
			std::string jumpstring = "Jump-Cheat has been ";
			jumpstring += ((m_Cheatmode.jump) ? "enabled" : "disabled");
			m_MessageBoxes.push_back(new CMessageBoxGalaxy(jumpstring));
		}
		else if(g_pInput->getHoldedKey(KG))
		{
			m_Cheatmode.god = !m_Cheatmode.god;
			std::string godstring = "God-Mode has been ";
			godstring += ((m_Cheatmode.jump) ? "enabled" : "disabled");
			m_MessageBoxes.push_back(new CMessageBoxGalaxy(godstring));
		}
		else if(g_pInput->getHoldedKey(KI))
		{
			m_Cheatmode.items = true;
			m_MessageBoxes.push_back(new CMessageBoxGalaxy("Get all Items!"));
			m_Inventory.Item.triggerAllItemsCheat();
			m_Cheatmode.items = true;
		}
		else if(g_pInput->getHoldedKey(KN))
		{
			m_Cheatmode.noclipping = true;
			m_MessageBoxes.push_back(new CMessageBoxGalaxy("No clipping toggle!"));
		}

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
