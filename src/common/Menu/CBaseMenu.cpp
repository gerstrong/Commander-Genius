/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "sdl/input/CInput.h"
#include "CBaseMenu.h"

CBaseMenu::CBaseMenu(Uint8 dlg_theme) :
	m_mustclose(false),
	m_selection(NO_SELECTION),
	mp_Dialog(NULL),
	m_dlg_theme(dlg_theme),
	m_suspended(false),
	m_noenter(false)
{}

std::string CBaseMenu::getSwitchString(const bool value)
{
	std::string text;
	if(m_dlg_theme == DLG_THEME_GALAXY)
	{
		text = (value==true) ? "on" : "off";
	}
	else
	{
		const int start_off = (value==true) ? 28 : 20;
		for(int c=start_off ; c<start_off+4 ; c++)
			text += c;
	}
	return text;
}

void CBaseMenu::processCommon()
{
	if(!m_suspended)
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

