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

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CAudioSettings::CAudioSettings(Uint8 dlg_theme, CExeFile &ExeFile) :
		CBaseMenu(dlg_theme),
		m_ExeFile(ExeFile),
		mp_VolumeMenu(NULL)
{

		mp_Dialog = new CDialog(20, 6, INPUT_MODE_UP_DOWN,m_dlg_theme);

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

void CAudioSettings::processSpecific()
{
	if(!m_suspended)
	{
		std::string buf;

		if( m_mustclose )
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

			g_pSound->loadSoundData(m_ExeFile);

			// Reload the music if was playing before we changed the settings
			if(wasPlaying)
			{
				g_pMusicPlayer->reload(g_pSound->getAudioSpec());
				g_pMusicPlayer->play();
			}
		}

		if( m_selection != NO_SELECTION )
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
				mp_VolumeMenu = new CVolumeSettings(m_dlg_theme);
				m_suspended = true;
			}
			m_selection = -1;
		}
	}
	else
	{
		mp_VolumeMenu->processCommon();
		mp_VolumeMenu->processSpecific();
		mp_VolumeMenu->postProcess();

		if(mp_VolumeMenu->mustClose())
		{
			SAFE_DELETE(mp_VolumeMenu);
			m_suspended = false;
		}
	}
}

CAudioSettings::~CAudioSettings() {
	delete mp_Dialog;
}
