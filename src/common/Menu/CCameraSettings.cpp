/*
 * CCameraSettings.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CCameraSettings.h"
#include "../../StringUtils.h"

CCameraSettings::CCameraSettings(Uint8 dlg_theme):
CBaseMenu(dlg_theme),
m_CameraBounds(g_pVideoDriver->getCameraBounds())
{
	/*std::string buf;
	mp_Dialog = new CDialog(18, 13, INPUT_MODE_UP_DOWN, m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 1, " Adjust Bounds: ");
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 2, " Left (50-270): ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, " "+itoa(m_CameraBounds.left));
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 4, " Right (50-270):");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, " "+itoa(m_CameraBounds.right));
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 6, " Up (50-150):   ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, " "+itoa(m_CameraBounds.up));
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 8, " Down (50-150): ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, " "+itoa(m_CameraBounds.down));
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 10, " Speed (1-50):  ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 11, " "+itoa(m_CameraBounds.speed));

	mp_Dialog->setInputMode(INPUT_MODE_COUNTER);*/
}

void CCameraSettings::processSpecific()
{
	/*if(mp_Dialog->getSelection() == 2)
	{
		mp_Dialog->m_min = 50;
		mp_Dialog->m_max = 270;
		mp_Dialog->m_length = 3;
	}
	else if(mp_Dialog->getSelection() == 4)
	{
		mp_Dialog->m_min = 50;
		mp_Dialog->m_max = 270;
		mp_Dialog->m_length = 3;
	}
	else if(mp_Dialog->getSelection() == 6)
	{
		mp_Dialog->m_min = 50;
		mp_Dialog->m_max = 150;
		mp_Dialog->m_length = 3;
	}
	else if(mp_Dialog->getSelection() == 8)
	{
		mp_Dialog->m_min = 50;
		mp_Dialog->m_max = 150;
		mp_Dialog->m_length = 3;
	}
	else if(mp_Dialog->getSelection() == 10)
	{
		mp_Dialog->m_min = 1;
		mp_Dialog->m_max = 50;
		mp_Dialog->m_length = 2;
	}*/

	/*m_CameraBounds.left = atoi(mp_Dialog->m_dlgobject.at(2)->m_Option->m_text);
	m_CameraBounds.right = atoi(mp_Dialog->m_dlgobject.at(4)->m_Option->m_text);
	m_CameraBounds.up = atoi(mp_Dialog->m_dlgobject.at(6)->m_Option->m_text);
	m_CameraBounds.down = atoi(mp_Dialog->m_dlgobject.at(8)->m_Option->m_text);
	m_CameraBounds.speed = atoi(mp_Dialog->m_dlgobject.at(10)->m_Option->m_text);*/

#ifndef NOKEYBOARD
	/*if( m_selection != NO_SELECTION)
	{
		if(mp_Dialog->getInputMode(INPUT_MODE_COUNTER))
		{
			mp_Dialog->m_name = "";
			mp_Dialog->setInputMode(INPUT_MODE_INTEGER);
			m_selection = NO_SELECTION;
		}
		else if (mp_Dialog->getInputMode(INPUT_MODE_INTEGER))
		{
			if(mp_Dialog->m_name == "")
			{
				mp_Dialog->setObjectText(m_selection, " "+itoa(mp_Dialog->m_int));
				mp_Dialog->m_name = itoa(mp_Dialog->m_int);
			}
			else
			{
				if(atoi(mp_Dialog->m_name)>mp_Dialog->m_max)
					mp_Dialog->m_name = itoa(mp_Dialog->m_max);
				else if(atoi(mp_Dialog->m_name)<mp_Dialog->m_min)
					mp_Dialog->m_name = itoa(mp_Dialog->m_min);

				mp_Dialog->setObjectText(m_selection, " "+mp_Dialog->m_name);
			}
			mp_Dialog->setInputMode(INPUT_MODE_COUNTER);
			m_selection = NO_SELECTION;
		}
	}*/
#endif
}

CCameraSettings::~CCameraSettings()
{
	g_pVideoDriver->saveCameraBounds(m_CameraBounds);
}
