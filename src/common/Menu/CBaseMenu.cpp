/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CBaseMenu.h"
#include "../../sdl/CInput.h"

CBaseMenu::CBaseMenu(char &MenuType) :
	m_mustclose(false),
	m_MenuType(MenuType),
	m_selection(-1),
	mp_Dialog(NULL),
	m_restartVideo(false)
{}

void CBaseMenu::processCommon()
{
	// Get Input for selection
	if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
	{
		m_selection = mp_Dialog->getSelection();
	}
}

void CBaseMenu::postProcess()
{
	mp_Dialog->processInput();

	mp_Dialog->draw();
}

CBaseMenu::~CBaseMenu() {
	// TODO Auto-generated destructor stub
}
