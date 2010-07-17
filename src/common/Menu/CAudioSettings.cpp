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
	m_select = -1;
	
	mp_Dialog = new CDialog(20, 6, INPUT_MODE_OPTION,m_dlg_theme);
	
	m_Rate = g_pSound->getAudioSpec().freq;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Rate: " + itoa(m_Rate) +" kHz");
	if( m_Rate == 11025 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 1;
	else if( m_Rate == 22050 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 2;
	else if( m_Rate == 44100 ) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 3;
	
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
	buf = "Mode: ";
	buf += m_Mode ? "Stereo": "Mono";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);
	mp_Dialog->m_dlgobject.at(2)->m_Option->m_value = g_pSound->getAudioSpec().channels;
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
		
		if( m_select != mp_Dialog->getSelection())
		{
			m_select = mp_Dialog->getSelection();
		}
		else if( m_select != NO_SELECTION)
		{
			if(m_select == 0)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 3;
				setValues(0, mp_Dialog->m_dlgobject.at(0)->m_Option->m_value);
				mp_Dialog->setObjectText(0, "Rate: " + itoa(m_Rate) + " kHz");
			}
			else if(m_select == 1)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 2;
				setValues(1, mp_Dialog->m_dlgobject.at(1)->m_Option->m_value);
				buf = "Format: ";
				buf += (m_Format == AUDIO_S16) ? "16 bits" : "8 bits";
				mp_Dialog->setObjectText(1, buf);
			}
			else if(m_select == 2)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 2;
				setValues(2, mp_Dialog->m_dlgobject.at(2)->m_Option->m_value);
				buf = "Mode: ";
				buf += m_Mode ? "Stereo" : "Mono";
				mp_Dialog->setObjectText(2, buf);
			}
		}
		
		if( m_selection != NO_SELECTION )
		{
			
			if(m_selection == 0)
			{
				mp_Dialog->m_dlgobject.at(0)->m_Option->m_value += 1;
				if(mp_Dialog->m_dlgobject.at(0)->m_Option->m_value > mp_Dialog->m_max)
				mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 1;
			}
			else if(m_selection == 1)
			{
				mp_Dialog->m_dlgobject.at(1)->m_Option->m_value += 1;
				if(mp_Dialog->m_dlgobject.at(1)->m_Option->m_value > mp_Dialog->m_max)
				mp_Dialog->m_dlgobject.at(1)->m_Option->m_value = 1;
			}
			else if(m_selection == 2)
			{
				mp_Dialog->m_dlgobject.at(2)->m_Option->m_value += 1;
				if(mp_Dialog->m_dlgobject.at(2)->m_Option->m_value > mp_Dialog->m_max)
				mp_Dialog->m_dlgobject.at(2)->m_Option->m_value = 1;
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

void CAudioSettings::setValues(int item, int value)
{
	if(item == 0)
	{
		switch(value)
		{
			case 1: m_Rate = 11025; break;
			case 2: m_Rate = 22050; break;
			case 3: m_Rate = 44100; break;
		}
	}
	else if(item == 1)
	{
		switch(value)
		{
			case 1: m_Format = AUDIO_U8; break;
			case 2: m_Format = AUDIO_S16; break;
		}
	}
	else if(item == 2)
	{
		switch(value)
		{
			case 1: m_Mode = 0; break;
			case 2: m_Mode = 1; break;
		}
	}
}

CAudioSettings::~CAudioSettings() {
	delete mp_Dialog;
}
