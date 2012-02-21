/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "sdl/input/CInput.h"
#include "CBaseMenu.h"

CBaseMenu::CBaseMenu(const Uint8 dlgTheme) /*:
	m_mustclose(false),
	m_selection(NO_SELECTION),
	mp_Dialog(NULL),
	m_dlg_theme(dlg_theme),
	m_suspended(false),
	m_noenter(false)*/
{}

void CBaseMenu::process()
{
	/*if(!m_suspended)
	{
		// Get Input for selection
		if(!m_noenter)
		{
			if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
			{
				// This also handles the on/off switches
				m_selection = mp_Dialog->getSelection();
			}
		}

		// Get Input for selection
		if( g_pInput->getPressedCommand(IC_QUIT) )
		{
			m_mustclose = true;
		}

	}*/

}

CBaseMenu::~CBaseMenu()
{

}
