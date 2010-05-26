/*
 * CVolumeSettings.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include "CBaseMenu.h"
#include "CVolumeSettings.h"
#include "../../sdl/sound/CSound.h"
#include "../../sdl/CInput.h"

CVolumeSettings::CVolumeSettings(Uint8 dlg_theme) :
CBaseMenu(dlg_theme)
{
	mp_Dialog = new CDialog(24, 7, INPUT_MODE_UP_DOWN, m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 1, "    Adjust Volume:    ");
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 2, "  Music:              ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "");
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 4, "  Sound:              ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "");

	mp_Dialog->m_dlgobject.at(2)->m_Option->m_value = g_pSound->getMusicVolume()/8;
	mp_Dialog->m_dlgobject.at(4)->m_Option->m_value = g_pSound->getSoundVolume()/8;
	mp_Dialog->m_dlgobject.at(2)->m_Option->m_FontMapID = 1;
	mp_Dialog->m_dlgobject.at(4)->m_Option->m_FontMapID = 1;
	mp_Dialog->setSelection(4);

	mp_Dialog->m_noise = false;
	mp_Dialog->setInputMode(INPUT_MODE_SLIDER);
	mp_Dialog->processInput();
	mp_Dialog->setSelection(2);
}

void CVolumeSettings::processSpecific()
{
	if(mp_Dialog->getSelection() == 2)
	{
		g_pSound->setMusicVolume(mp_Dialog->m_dlgobject.at(2)->m_Option->m_value*8);
		mp_Dialog->m_noise = false;
	}
	else if(mp_Dialog->getSelection() == 4)
	{
		g_pSound->setSoundVolume(mp_Dialog->m_dlgobject.at(4)->m_Option->m_value*8);
		mp_Dialog->m_noise = true;
	}
}

CVolumeSettings::~CVolumeSettings()
{}
