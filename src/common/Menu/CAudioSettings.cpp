/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CAudioSettings.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CSettings.h"
#include "../../sdl/sound/CSound.h"
#include "../../StringUtils.h"

CAudioSettings::CAudioSettings(char &menu_type,
								std::string &Gamepath, char &Episode) :
CBaseMenu(menu_type),
m_Gamepath(Gamepath),
m_Episode(Episode)
{
	mp_Dialog = new CDialog(g_pVideoDriver->FGLayerSurface, 22, 8);
	mp_Dialog->setFrameTheme(DLG_THEME_OLDSCHOOL);

	m_Rate = g_pSound->getAudioSpec().freq;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Rate: " + itoa(m_Rate) +" kHz");

	m_Format = g_pSound->getAudioSpec().format;
	std::string buf;
	if(m_Format == AUDIO_S16)
		buf = "Format: 16 bits";
	else
		buf = "Format: 8 bits";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);

	m_Mode = g_pSound->getAudioSpec().channels - 1;
	buf = "Mode: ";
	buf += m_Mode ? "Stereo": "Mono";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Confirm");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Cancel");
}

void CAudioSettings::processSpecific()
{
	std::string buf;

	if( m_selection != -1)
	{
		if(m_selection == 0)
		{
			switch(m_Rate)
			{
				case 22050: m_Rate = 44100; break;
				case 11025: m_Rate = 22050; break;
				default: m_Rate = 11025; break;
			}
			mp_Dialog->setObjectText(0, "Rate: " + itoa(m_Rate) + " kHz");
		}
		else if(m_selection == 1)
		{
			if( m_Format == AUDIO_S16 ) m_Format = AUDIO_U8;
			else if( m_Format == AUDIO_U8 ) m_Format = AUDIO_S16;

			buf = "Format: ";
			buf += (m_Format == AUDIO_S16) ? "16 bits" : "8 bits";
			mp_Dialog->setObjectText(1, buf);
		}
		else if(m_selection == 2)
		{
			m_Mode = !m_Mode;
			buf = "Mode: ";
			buf += m_Mode ? "Stereo" : "Mono";
			mp_Dialog->setObjectText(2, buf);
		}
		else if(m_selection == 3) // Confirm
		{
			// TODO: Cannot save, because m_Gamepath and m_Episode need to be passed as reference
			CSettings Settings;
			g_pSound->destroy();
			g_pSound->setSoundmode(m_Rate, m_Mode ? true : false, m_Format);
			Settings.saveDrvCfg();
			g_pSound->init();
			g_pSound->loadSoundData(m_Episode, m_Gamepath);
			m_MenuType = CONFIGURE;
			m_mustclose = true;
		}
		else if(m_selection == 4) // Cancel
		{
			m_MenuType = CONFIGURE;
			m_mustclose = true;
		}
		m_selection = -1;
	}
}

CAudioSettings::~CAudioSettings() {
	delete mp_Dialog;
}
