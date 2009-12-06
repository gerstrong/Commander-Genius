/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CControlsettings.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CSettings.h"
#include "../../sdl/CVideoDriver.h"
#include "../../StringUtils.h"

CControlsettings::CControlsettings(char &menu_type, int chosenPlayerNumber) :
CBaseMenu(menu_type),
m_chosenPlayer(chosenPlayerNumber),
m_waiting_for_input(false)
{
	drawInitialCommands();
}

void CControlsettings::drawInitialCommands()
{
	std::string buf;
	std::string buf2;
	mp_Dialog = new CDialog(g_pVideoDriver->FGLayerSurface, 36, 13);
	mp_Dialog->setFrameTheme(DLG_THEME_OLDSCHOOL);

	g_pInput->getEventName(IC_LEFT, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Left:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);

	g_pInput->getEventName(IC_UP, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Up:     " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);

	g_pInput->getEventName(IC_RIGHT, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Right:  " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);

	g_pInput->getEventName(IC_DOWN, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Down:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);

	g_pInput->getEventName(IC_JUMP, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Jump:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);

	g_pInput->getEventName(IC_POGO, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Pogo:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);

	g_pInput->getEventName(IC_FIRE, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Fire:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);

	g_pInput->getEventName(IC_STATUS, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Status: " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, buf);

	g_pInput->getEventName(IC_HELP, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Help:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, buf);

	g_pInput->getEventName(IC_QUIT, m_chosenPlayer-1, buf2);
	buf = "P"+itoa(m_chosenPlayer)+" Quit:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, buf);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 2, 11, "Reset Controls");
}

void CControlsettings::processSpecific()
{	
	if(m_waiting_for_input) // This part onloy happens, when waiting for an input
	{
		processWaitInput();
	}
	else
	{
		processSelection();
	}
}

void CControlsettings::processWaitInput()
{
	int item = m_selection;
	while( !g_pInput->readNewEvent(m_chosenPlayer-1,item) );

	std::string buf;
	std::string buf2;

	buf = mp_Dialog->m_dlgobject[m_selection]->m_OptionText->m_text;
	buf = buf.erase(11);
	g_pInput->getEventName(item, m_chosenPlayer-1, buf2);
	mp_Dialog->setObjectText(m_selection, buf + buf2);
	mp_Dialog->m_key = 'u';
	m_selection = -1;
	m_waiting_for_input = false;
}

void CControlsettings::processSelection()
{
	if( g_pInput->getPressedCommand(IC_QUIT) )
	{
			g_pInput->saveControlconfig();
			// And close this menu...
			m_MenuType = CONTROLPLAYERS;
			m_mustclose = true;
			m_selection = -1;
	}
	
	if( m_selection != -1) // Normal selection function
	{
		if( m_selection < 11 )
		{
			if(m_selection < MAX_COMMANDS)
			{
				std::string buf;
				std::string buf2;
				m_waiting_for_input = true;
				g_pInput->flushAll();

				buf = mp_Dialog->m_dlgobject[m_selection]->m_OptionText->m_text;
				buf = buf.erase(11);

				buf2 = "*Waiting for Input*";
				mp_Dialog->setObjectText(m_selection, buf + buf2);
				mp_Dialog->m_key = 'n';
			}
			else if(m_selection == MAX_COMMANDS)
			{
				g_pInput->resetControls(m_chosenPlayer);
				delete mp_Dialog;
				drawInitialCommands();
				m_selection = -1;
			}
		}
	}
}

CControlsettings::~CControlsettings() {
	delete mp_Dialog;
}
