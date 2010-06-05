/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CControlsettings.h"
#include "CPlayersMenu.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CSettings.h"
#include "../../sdl/CVideoDriver.h"
#include "../../StringUtils.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CControlsettings::CControlsettings( Uint8 dlg_theme ) :
CBaseMenu(dlg_theme),
m_chosenPlayer(0),
m_waiting_for_input(false)
{
	m_suspended = true;

	// first create the players selection screen
	mp_SubMenu = new CPlayersMenu( m_chosenPlayer, dlg_theme );

	// Then create the controls selection screen
	mp_Dialog = new CDialog(36, 14, INPUT_MODE_UP_DOWN, m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Left:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Up:     ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Right:  ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Down:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Jump:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Pogo:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Fire:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Status: ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, "Help:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Quit:   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 11, "Two Button Firing : ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 2, 12, "Reset Controls");
}

void CControlsettings::setControlsText()
{
	// Here it sets up the items after the player number was chosen
	// Then create the controls selection screen
	std::string buf;
	std::string buf2;

	buf2 = g_pInput->getEventName(IC_LEFT, m_chosenPlayer-1);
	buf = "Left:   " + buf2;
	mp_Dialog->setObjectText(0, buf);

	buf2 = g_pInput->getEventName(IC_UP, m_chosenPlayer-1);
	buf = "Up:     " + buf2;
	mp_Dialog->setObjectText(1, buf);

	buf2 = g_pInput->getEventName(IC_RIGHT, m_chosenPlayer-1);
	buf = "Right:  " + buf2;
	mp_Dialog->setObjectText(2, buf);

	buf2 = g_pInput->getEventName(IC_DOWN, m_chosenPlayer-1);
	buf = "Down:   " + buf2;
	mp_Dialog->setObjectText(3, buf);

	buf2 = g_pInput->getEventName(IC_JUMP, m_chosenPlayer-1);
	buf = "Jump:   " + buf2;
	mp_Dialog->setObjectText(4, buf);

	buf2 = g_pInput->getEventName(IC_POGO, m_chosenPlayer-1);
	buf = "Pogo:   " + buf2;
	mp_Dialog->setObjectText(5, buf);

	buf2 = g_pInput->getEventName(IC_FIRE, m_chosenPlayer-1);
	buf = "Fire:   " + buf2;
	mp_Dialog->setObjectText(6, buf);

	buf2 = g_pInput->getEventName(IC_STATUS, m_chosenPlayer-1);
	buf = "Status: " + buf2;
	mp_Dialog->setObjectText(7, buf);

	buf2 = g_pInput->getEventName(IC_HELP, m_chosenPlayer-1);
	buf = "Help:   " + buf2;
	mp_Dialog->setObjectText(8, buf);

	buf2 = g_pInput->getEventName(IC_QUIT, m_chosenPlayer-1);
	buf = "Quit:   " + buf2;
	mp_Dialog->setObjectText(9, buf);

	buf = "Two Button Firing : ";
	buf += g_pInput->getTwoButtonFiring(m_chosenPlayer-1) ? "(on)" : "(off)";
	mp_Dialog->setObjectText(10, buf);
}

void CControlsettings::processSpecific()
{	
	if(!m_suspended)
	{
		if(m_waiting_for_input) // This part only happens, when waiting for an input
		{
			processWaitInput();
		}
		else
		{
			processSelection();
		}
	}
	else
	{
		mp_SubMenu->processCommon();
		mp_SubMenu->processSpecific();
		mp_SubMenu->postProcess();

		if(mp_SubMenu->mustClose())
		{
			SAFE_DELETE(mp_SubMenu);
			m_suspended = false;
			if(m_chosenPlayer == 0)
			{
				m_mustclose = true;
				m_suspended = true;
			}
			else
				setControlsText();
		}
	}
}

void CControlsettings::processWaitInput()
{
	int item = m_selection;
	while( !g_pInput->readNewEvent(m_chosenPlayer-1,item) );

	std::string buf;
	std::string buf2;

	buf = mp_Dialog->m_dlgobject[m_selection]->m_Option->m_text;
	buf = buf.erase(8);
	buf2 = g_pInput->getEventName(item, m_chosenPlayer-1);
	mp_Dialog->setObjectText(m_selection, buf + buf2);
	mp_Dialog->setInputMode(INPUT_MODE_UP_DOWN);
	m_selection = -1;
	m_waiting_for_input = false;
}

void CControlsettings::processSelection()
{
	if( m_mustclose ) // if the menu must close, save the settings
	{
		g_pInput->saveControlconfig();
		m_selection = NO_SELECTION;
	}
	
	if( m_selection != NO_SELECTION) // Normal selection function
	{
		if( m_selection < 12 )
		{
			if(m_selection < MAX_COMMANDS)
			{
				std::string buf;
				std::string buf2;
				m_waiting_for_input = true;
				g_pInput->flushAll();

				buf = mp_Dialog->m_dlgobject[m_selection]->m_Option->m_text;
				buf = buf.erase(8);

				buf2 = "*Waiting for Input*";
				mp_Dialog->setObjectText(m_selection, buf + buf2);
				mp_Dialog->setInputMode(INPUT_MODE_NOTHING);
			}
			else if(m_selection == MAX_COMMANDS)
			{
				std::string buf;
				bool twb = g_pInput->getTwoButtonFiring(m_chosenPlayer-1);
				twb = !twb;
				g_pInput->setTwoButtonFiring(m_chosenPlayer-1, twb);
				buf = "Two Button Firing : ";
				buf += g_pInput->getTwoButtonFiring(m_chosenPlayer-1) ? "(On)" : "(Off)";
				mp_Dialog->setObjectText(m_selection, buf);
				m_selection = -1;
			}
			else if(m_selection == MAX_COMMANDS+1)
			{
				g_pInput->resetControls(m_chosenPlayer);
				delete mp_Dialog;
				setControlsText();
				m_selection = -1;
			}
		}
	}
}

CControlsettings::~CControlsettings() {
	delete mp_Dialog;
}
