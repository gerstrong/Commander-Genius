/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CControlsettings.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CSettings.h"
#include "../../StringUtils.h"

CControlsettings::CControlsettings(char &menu_type) :
CBaseMenu(menu_type) {
	std::string buf;
	std::string buf2;
	//int player = m_NumPlayers - 1;
	//mp_Dialog = new CDialog(mp_MenuSurface, 36, 16);

	/*g_pInput->getEventName(IC_LEFT, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Left:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);

	g_pInput->getEventName(IC_UP, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Up:     " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);

	g_pInput->getEventName(IC_RIGHT, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Right:  " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);

	g_pInput->getEventName(IC_DOWN, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Down:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);

	g_pInput->getEventName(IC_JUMP, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Jump:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);

	g_pInput->getEventName(IC_POGO, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Pogo:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);

	g_pInput->getEventName(IC_FIRE, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Fire:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);

	g_pInput->getEventName(IC_STATUS, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Status: " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, buf);

	g_pInput->getEventName(IC_HELP, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Help:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, buf);

	g_pInput->getEventName(IC_QUIT, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Quit:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, buf);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 2, 11, "Reset Controls");
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 13, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 14, "Return");*/
}

void CControlsettings::processSpecific()
{
	/*if( m_selection != -1)
	{
		if( m_selection < 11 )
		{
			if(m_selection < MAX_COMMANDS)
			{
				int item = m_selection;
				std::string buf;
				std::string buf2;

				buf = mp_Dialog->m_dlgobject[m_selection]->m_OptionText->m_text;
				buf = buf.erase(11);

				buf2 = "*Waiting for Input*";
				mp_Dialog->setObjectText(m_selection, buf + buf2);
				mp_Dialog->m_key = 'n';

				do
				{
					if(g_pInput->readNewEvent(m_NumPlayers-1,item))
					{
						g_pInput->getEventName(item, m_NumPlayers-1, buf2);
						mp_Dialog->setObjectText(m_selection, buf + buf2);
						mp_Dialog->m_key = 'u';
						m_selection = -1;
					}
				} while (buf2 == "*Waiting for Input*");
			}
			else
			{
				g_pInput->resetControls();
				cleanup();
				init(CONTROLS);
			}
			mp_Dialog->setObjectText(11, "Save and Return");
			//mp_Dialog->setObjectType(11, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(12, "Cancel");
		}
		else
		{
			if(m_selection == 11)
			{
				g_pInput->saveControlconfig();
			}
			m_goback = true;
		}
	}

	if(m_goback)
	{
		cleanup();
		m_NumPlayers = 0;
		init(CONTROLPLAYERS);
	}*/
}

CControlsettings::~CControlsettings() {
	//delete mp_Dialog;
}
