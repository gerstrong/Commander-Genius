/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CAudioSettings.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "common/CSettings.h"
#include "sdl/sound/CSound.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CAudioSettings::CAudioSettings(Uint8 dlg_theme, CExeFile &ExeFile) :
CBaseMenu(dlg_theme),
m_ExeFile(ExeFile),
mp_VolumeMenu(NULL),
m_must_restart_sounddriver(false)
{
	m_current = -1;
	
	mp_Dialog = new CDialog(30, 7, INPUT_MODE_OPTION,m_dlg_theme);
	
	m_Rate = g_pSound->getAudioSpec().freq;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Rate: " + itoa(m_Rate) +" kHz");
	if( m_Rate == 11025 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 1;
	else if( m_Rate == 22050 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 2;
	else if( m_Rate == 44100 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 3;
	else if( m_Rate == 48000 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 4;
	
	m_Format = g_pSound->getAudioSpec().format;
	std::string buf;
	if(m_Format == AUDIO_S16)
		buf = "Format: 16 bits";
	else
		buf = "Format: 8 bits";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);
	if( m_Format == AUDIO_U8 ) mp_Dialog->m_dlgobject.at(1)->m_Option->m_value = 1;
	else if( m_Format == AUDIO_S16 ) mp_Dialog->m_dlgobject.at(1)->m_Option->m_value = 2;
	
	m_Mode = g_pSound->getAudioSpec().channels - 1;
	buf = "Channels: ";
	buf += m_Mode ? "Stereo": "Mono";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);
	mp_Dialog->m_dlgobject.at(2)->m_Option->m_value = g_pSound->getAudioSpec().channels;
	
	m_Soundblaster = g_pSound->getSoundBlasterMode();
	buf = "Mode: ";
	buf += m_Soundblaster ? "Soundblaster/Adlib" : "PC Speaker";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);
	mp_Dialog->m_dlgobject.at(3)->m_Option->m_value = m_Soundblaster;

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Adjust Volume");

}

void CAudioSettings::processSpecific()
{
	if(!m_suspended)
	{
		std::string buf;
		
		if( m_mustclose )
		{
			bool wasPlaying = g_pMusicPlayer->playing();

			// Check if the music is playing, stop it and restart it, if necessary
			if(m_Rate != g_pSound->getAudioSpec().freq ||
					m_Format != g_pSound->getAudioSpec().format ||
					m_Mode != g_pSound->getAudioSpec().channels - 1 )
			{
				g_pMusicPlayer->stop();

				// Close the sound driver
				g_pSound->destroy();
				g_pSound->setSoundmode(m_Rate, m_Mode ? true : false, m_Format);
				m_must_restart_sounddriver |= true;
			}

			g_pSound->setSoundBlasterMode(m_Soundblaster);

			g_pSettings->saveDrvCfg();

			if(m_must_restart_sounddriver)
			{
				// Reload the sounds effects, so they work with the new format
				g_pSound->init();

				g_pSound->loadSoundData(m_ExeFile);

				// Reload the music if was playing before we changed the settings
				if(wasPlaying)
				{
					g_pMusicPlayer->reload();
					g_pMusicPlayer->play();
				}
			}
		}
		
		if( m_current != mp_Dialog->getSelection())
		{
			m_current = mp_Dialog->getSelection();
		}
		else if( m_current != NO_SELECTION)
		{
			if(m_current == 0)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 4;
				setFrequencyFromSlot(mp_Dialog->m_dlgobject.at(0)->m_Option->m_value);
				mp_Dialog->setObjectText(0, "Rate: " + itoa(m_Rate) + " kHz");
			}
		}
		
		if( m_selection != NO_SELECTION )
		{
			if(m_selection == 0)
			{
				mp_Dialog->m_dlgobject.at(0)->m_Option->m_value += 1;
				if(mp_Dialog->m_dlgobject.at(0)->m_Option->m_value > mp_Dialog->m_max)
				mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 1;
				m_must_restart_sounddriver |= true;
			}
			else if(m_selection == 1)
			{
				if( m_Format == AUDIO_S16 ) m_Format = AUDIO_U8;
				else if( m_Format == AUDIO_U8 ) m_Format = AUDIO_S16;
				buf = "Format: ";
				buf += (m_Format == AUDIO_S16) ? "16 bits" : "8 bits";
				mp_Dialog->setObjectText(1, buf);
				m_must_restart_sounddriver |= true;
			}
			else if(m_selection == 2)
			{
				m_Mode = !m_Mode;
				buf = "Channels: ";
				buf += m_Mode ? "Stereo" : "Mono";
				mp_Dialog->setObjectText(2, buf);
				m_must_restart_sounddriver |= true;
			}
			else if(m_selection == 3)
			{
				m_Soundblaster = !m_Soundblaster;
				buf = "Mode: ";
				buf += m_Soundblaster ? "Soundblaster/Adlib" : "PC Speaker";
				mp_Dialog->setObjectText(3, buf);
			}
			else if(m_selection == 4)
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

void CAudioSettings::setFrequencyFromSlot(int value)
{
	switch(value)
	{
	case 1: m_Rate = 11025; break;
	case 2: m_Rate = 22050; break;
	case 3: m_Rate = 44100; break;
	case 4: m_Rate = 48000; break;
	}
}

CAudioSettings::~CAudioSettings() {
	if(mp_Dialog) delete mp_Dialog;
	mp_Dialog = NULL;
}
