/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "StringUtils.h"
#include "CSelectionMenu.h"
#include "CControlsettings.h"
#include "common/CBehaviorEngine.h"

#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"

const std::string actionsnames[MAX_COMMANDS] =
{
		"Left:        ",
		"Right:       ",
		"Up:          ",
		"Down:        ",
		"Upper left:  ",
		"Upper right: ",
		"Lower left:  ",
		"Lower right: ",
		"Jump:        ",
		"Pogo:        ",
		"Fire:        ",
		"Status:      ",
		"Help:        ",
		"Quit:        "
};

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CControlsettings::CControlsettings( Uint8 dlg_theme ) :
CBaseMenu(dlg_theme),
m_chosenPlayer(0),
m_waiting_for_input(false)
{
	/*m_suspended = true;

	std::list<std::string> players_list;
	for(size_t i=1 ; i<=MAX_PLAYERS ; i++)
		players_list.push_back(itoa(i) + " Player");

	// first create the players selection screen
	mp_SubMenu = new CSelectionMenu<Uint8>( m_chosenPlayer, players_list, dlg_theme );

	// Then create the controls selection screen
	mp_Dialog = new CDialog(36, 18, INPUT_MODE_UP_DOWN, m_dlg_theme);

	for(unsigned int i=0 ; i<MAX_COMMANDS ; i++)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, actionsnames[i]);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, MAX_COMMANDS+1, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 2, MAX_COMMANDS+2, "Reset Controls");*/
}

void CControlsettings::setControlsText()
{
	// Here it sets up the items after the player number was chosen
	// Then create the controls selection screen
	/*std::string buf, buf2;

	for(unsigned int i=0 ; i<MAX_COMMANDS ; i++)
	{
		buf2 = g_pInput->getEventName(IC_LEFT+i, m_chosenPlayer-1);
		buf = actionsnames[i] + buf2;
		mp_Dialog->setObjectText(i, buf);
	}
	
	buf = "Two Button Firing " + getSwitchString(g_pInput->getTwoButtonFiring(m_chosenPlayer-1));
	mp_Dialog->m_dlgobject.at(MAX_COMMANDS)->m_Option->m_FontMapID = 1;
	mp_Dialog->setObjectText(MAX_COMMANDS, buf);*/
}

void CControlsettings::processSpecific()
{	
	/*if(!m_suspended)
	{
		if(m_waiting_for_input) // This part only happens, when waiting for an input
			processWaitInput();
		else
			processSelection();
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
	}*/
}

void CControlsettings::processWaitInput()
{
	/*int item = m_selection;
	while( !g_pInput->readNewEvent(m_chosenPlayer-1,item) );

	std::string buf;
	std::string buf2;

	buf = mp_Dialog->m_dlgobject[m_selection]->m_Option->m_text;
	buf = buf.erase(actionsnames[item].size());
	buf2 = g_pInput->getEventName(item, m_chosenPlayer-1);
	mp_Dialog->setObjectText(m_selection, buf + buf2);
	mp_Dialog->setInputMode(INPUT_MODE_UP_DOWN);
	m_selection = -1;
	m_waiting_for_input = false;*/
}

void CControlsettings::processSelection()
{
	/*if( m_mustclose ) // if the menu must close, save the settings
	{
		g_pInput->saveControlconfig();
		m_selection = NO_SELECTION;
	}
	
	if( m_selection != NO_SELECTION) // Normal selection function
	{
		if( m_selection < MAX_COMMANDS+2 )
		{
			if(m_selection < MAX_COMMANDS)
			{
				std::string buf, buf2;
				m_waiting_for_input = true;
				g_pInput->flushAll();

				buf = mp_Dialog->m_dlgobject[m_selection]->m_Option->m_text;
				buf = buf.erase(actionsnames[m_selection].size());

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
				buf = "Two Button Firing " + getSwitchString(g_pInput->getTwoButtonFiring(m_chosenPlayer-1));
				mp_Dialog->setObjectText(m_selection, buf);
				m_selection = -1;
			}
			else if(m_selection == MAX_COMMANDS+1)
			{
				g_pInput->resetControls(m_chosenPlayer);
				setControlsText();
				m_selection = -1;
			}
		}
	}*/
}
