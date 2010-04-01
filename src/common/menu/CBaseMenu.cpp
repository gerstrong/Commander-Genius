/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CBaseMenu.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"

CBaseMenu::CBaseMenu() :
	m_mustclose(false),
	m_selection(-1),
	mp_Dialog(NULL),
	m_nextMenu(0),
	m_restartVideo(false)
{}

// This function tells if there is a dialog that must be opened next.
// When this happen, the current Dialog will be closed and the next will be
// opened
Uint8 CBaseMenu::getNextDialog()
{	return m_nextMenu;	}

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
	mp_Dialog->draw(g_pVideoDriver->FGLayerSurface);
}

CBaseMenu::~CBaseMenu() {
}
