/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "../../sdl/CInput.h"
#include "CBaseMenu.h"

CBaseMenu::CBaseMenu(Uint8 dlg_theme) :
	m_mustclose(false),
	m_selection(NO_SELECTION),
	mp_Dialog(NULL),
	m_dlg_theme(dlg_theme),
	m_suspended(false),
	m_noenter(false)
{}

void CBaseMenu::processCommon()
{
	if(!m_suspended)
	{
		// Get Input for selection
		if(!m_noenter)
		{
			if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
			{
				m_selection = mp_Dialog->getSelection();
			}
		}

		// Get Input for selection
		if( g_pInput->getPressedCommand(IC_QUIT) )
		{
			m_mustclose = true;
		}

	}
}

void CBaseMenu::postProcess()
{
	if(!m_suspended)
	{
		if(!mp_Dialog)
		{
			m_mustclose = true;
			return;
		}

		mp_Dialog->processInput();
		mp_Dialog->draw();
	}

}

bool CBaseMenu::mustClose()
{	return m_mustclose;	}

CBaseMenu::~CBaseMenu() {
	if(mp_Dialog) delete mp_Dialog;
	mp_Dialog = NULL;
}

