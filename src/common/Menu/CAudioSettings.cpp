/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CAudioSettings.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CSettings.h"
#include "../../sdl/sound/CSound.h"
#include "../../hqp/CMusic.h"
#include "../../StringUtils.h"

CAudioSettings::CAudioSettings(char &menu_type,
		std::string &Gamepath, char &Episode) :
		CBaseMenu(menu_type),
		m_Gamepath(Gamepath),
		m_Episode(Episode)
{
	if(m_MenuType == VOLUME)
	{
		mp_Dialog = new CDialog(g_pVideoDriver->FGLayerSurface, 24, 7);
		mp_Dialog->setFrameTheme(DLG_THEME_OLDSCHOOL);

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

		mp_Dialog->m_key = 's';
		mp_Dialog->processInput();
		mp_Dialog->setSelection(2);
	}
	else
	{
		mp_Dialog = new CDialog(g_pVideoDriver->FGLayerSurface, 20, 6);
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
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Adjust Volume");
	}
}

void CAudioSettings::processSpecific()
{
	std::string buf;

	if(m_MenuType == VOLUME)
		{
			if(mp_Dialog->getSelection() == 2)
				g_pSound->setMusicVolume(mp_Dialog->m_dlgobject.at(2)->m_Option->m_value*8);
			else if(mp_Dialog->getSelection() == 4)
				g_pSound->setSoundVolume(mp_Dialog->m_dlgobject.at(4)->m_Option->m_value*8);
		}
	
	if( g_pInput->getPressedCommand(IC_QUIT) )
	{
		if(m_MenuType == VOLUME)
		{
			m_MenuType = AUDIO;
			m_mustclose = true;
		}
		else
		{
			CSettings Settings;

			// Check if the music is playing, stop it and restart it, if necessary
			bool wasPlaying = g_pMusicPlayer->playing();
			g_pMusicPlayer->stop();

			// Close the sound driver
			g_pSound->destroy();
			g_pSound->setSoundmode(m_Rate, m_Mode ? true : false, m_Format);
			Settings.saveDrvCfg();

			// Reload the sounds effects, so they work with the new format
			g_pSound->init();
			g_pSound->loadSoundData(m_Episode, m_Gamepath);

			// Reload the music if was playing before we changed the settings
			if(wasPlaying)
			{
				g_pMusicPlayer->reload(g_pSound->getAudioSpec());
				g_pMusicPlayer->play();
			}

			m_MenuType = CONFIGURE;
			m_mustclose = true;
		}
	}

	if( m_selection != -1)
	{
		if(m_MenuType == VOLUME)
		{
		}
		else
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
			else if(m_selection == 3)
			{
				m_MenuType = VOLUME;
				m_mustclose = true;
			}
		}
		m_selection = -1;
	}
}

CAudioSettings::~CAudioSettings() {
	delete mp_Dialog;
}
