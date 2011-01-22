/*
 * CSound.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "CSound.h"
#include "keen.h"
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include "CLogFile.h"
#include "StringUtils.h"
#include "FindFile.h"
#include "sdl/sound/CMusic.h"
#include "sdl/sound/Mixer.h"

#include <fstream>

#define SAFE_DELETE_ARRAY(x) if(x) delete[] x; x=NULL;

// define a callback function we can work with
void CCallback(void *unused, Uint8 *stream, int len)
{
    // let it call a method on my (singleton) sound object
    g_pSound->callback(unused, stream, len);
}

CSound::CSound() :
m_active(false),
m_Episode(0),
m_DataDirectory(""),
m_mixing_channels(0),
m_MusicVolume(SDL_MIX_MAXVOLUME),
m_SoundVolume(SDL_MIX_MAXVOLUME),
m_pMixedForm(NULL)		// Mainly used by the callback function. Declared once and allocated
{
	AudioSpec.channels = 2; // Stereo Sound
#if defined(CAANOO) || defined(WIZ) || defined(GP2X) || defined(DINGOO) || defined(ANDROID)
	AudioSpec.format = AUDIO_S16; // 16-bit sound
#else
	AudioSpec.format = AUDIO_U8; // 8-bit sound
#endif
	AudioSpec.freq = 44100; // high quality
}

bool CSound::init(void)
{
	g_pLogFile->ftextOut("Starting sound driver...<br>");
	char name[256];
	SDL_AudioSpec *desired, *obtained;

	desired = &AudioSpec;
	obtained = new SDL_AudioSpec;

	// now start up the SDL sound system
	AudioSpec.silence = 0;

	switch (AudioSpec.freq)
	{
		case 11025: AudioSpec.samples = 256; break;
		case 22050: AudioSpec.samples = 512; break;
		default: AudioSpec.samples = 1024; break;
	}
	AudioSpec.callback = CCallback;
	AudioSpec.userdata = NULL;

	/* Initialize variables */
	if( SDL_OpenAudio(desired, obtained) < 0 )
	{
		g_pLogFile->ftextOut("SoundDrv_Start(): Couldn't open audio: %s<br>", SDL_GetError());
		g_pLogFile->ftextOut("Sound will be disabled.<br>");
		AudioSpec.channels = 0;
		AudioSpec.format = 0;
		AudioSpec.freq = 0;
		m_active = false;
		return false;
	}

	memcpy(&AudioSpec,obtained,sizeof(SDL_AudioSpec));
	delete obtained;

	m_pMixedForm = (Uint8 *) malloc(AudioSpec.size); // To make sure it's 4-byte aligned use malloc() instead of new()

	g_pLogFile->ftextOut("SDL_AudioSpec:<br>");
	g_pLogFile->ftextOut("  freq: %d<br>", AudioSpec.freq);
	g_pLogFile->ftextOut("  channels: %d<br>", AudioSpec.channels);
	g_pLogFile->ftextOut("  audio buffer size: %d<br>", AudioSpec.size);
	switch( AudioSpec.format )
	{
		case AUDIO_U8:
			g_pLogFile->ftextOut("  format: AUDIO_U8<br>" );
			break;
		case AUDIO_S8:
			g_pLogFile->ftextOut("  format: AUDIO_S8<br>" );
			break;
		case AUDIO_U16:
			g_pLogFile->ftextOut("  format: AUDIO_U16<br>" );
			break;
		case AUDIO_S16:
			g_pLogFile->ftextOut("  format: AUDIO_S16<br>" );
			break;
		default:
			g_pLogFile->ftextOut("  format: UNKNOWN %d<br>", AudioSpec.format );
	}
	g_pLogFile->ftextOut("Using audio driver: %s<br>", SDL_AudioDriverName(name, 32));

	m_mixing_channels = 15;

	if(!m_soundchannel.empty())
		m_soundchannel.clear();
	m_soundchannel.assign(m_mixing_channels, CSoundChannel(AudioSpec));

	SDL_PauseAudio(0);

	g_pLogFile->ftextOut("Sound System: SDL sound system initialized.<br>");
	m_active = true;

	return true;
}

void CSound::destroy(void)
{
	stopAllSounds();

	if (m_active)
	{
		SDL_PauseAudio(1);
		SDL_CloseAudio();
		m_active = false;
		m_mixing_channels = 0;
	}
	if(m_pMixedForm)
		free(m_pMixedForm);
	m_pMixedForm = NULL;

	if(!m_soundchannel.empty())
		m_soundchannel.clear();

	if(!m_soundslot.empty())
		m_soundslot.clear();

	g_pLogFile->ftextOut("SoundDrv_Stop(): shut down.<br>");
}

// stops all currently playing sounds
void CSound::stopAllSounds()
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
		snd_chnl->stopSound();
}

// pauses any currently playing sounds
void CSound::pauseSound(void)
{
	if (m_active) SDL_PauseAudio(1);
}

// resumes playing a previously paused sound
void CSound::resumeSounds(void)
{
	if (m_active) SDL_PauseAudio(0);
}

// returns true if sound snd is currently playing
bool CSound::isPlaying(GameSound snd)
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
	{
		if (snd_chnl->isPlaying())
			if (snd_chnl->getCurrentsound() == snd)
				return true;
	}
	return false;
}

// if sound snd is currently playing, stops it immediately
void CSound::stopSound(GameSound snd)
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
	{
		if (snd_chnl->isPlaying())
			if (snd_chnl->getCurrentsound() == snd)
				snd_chnl->stopSound();
	}
}

// returns true if a sound is currently playing in PLAY_FORCE mode
bool CSound::forcedisPlaying(void)
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
		if(snd_chnl->isForcedPlaying())
			return true;

	return false;
}

int maxval = 0;

void CSound::callback(void *unused, Uint8 *stream, int len)
{
    if (g_pMusicPlayer->playing() == PLAY_MODE_PLAY)
    {
    	g_pMusicPlayer->readBuffer(m_pMixedForm, len);
    	mixAudio(stream, m_pMixedForm, len, m_MusicVolume, AudioSpec.format);
    }

	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
   	{
		snd_chnl->readWaveform(m_pMixedForm, len, AudioSpec.channels, AudioSpec.freq);
   		mixAudio(stream, m_pMixedForm, len, m_SoundVolume, AudioSpec.format);
    }
}

// if priorities allow, plays the sound "snd".
// nonzero return value indicates a higher priority sound is playing.
void CSound::playSound(GameSound snd, char mode)
{
	playStereosound(snd, mode, 0);
}

void CSound::playStereofromCoord(GameSound snd, char mode, unsigned int xcoordinate)
{
    if(AudioSpec.channels == 2)
    {
    	int bal;

    	bal = ((short)(xcoordinate) - (320>>1));	// Faster calculation of balance transformation

    	if(bal < -127)
    		bal = -127;
    	else if(bal > 127)
    		bal = 127;

    	playStereosound(snd, mode, bal);
	}
    else
    	playSound(snd, mode);
}

void CSound::playStereosound(GameSound snd, char mode, short balance)
{
	if(!(m_mixing_channels && m_active)) return;

	std::vector<CSoundChannel>::iterator snd_chnl;

	if (mode==PLAY_NORESTART)
	{
		if (isPlaying(snd))
			return;
	}

	// if a forced sound is playing then let it play
	if (forcedisPlaying()) return;

	// stop all other sounds if this sound has maximum priority
	if ( mode==PLAY_FORCE )
		stopAllSounds();

	// first try to find an empty channel
	for( snd_chnl = m_soundchannel.begin() ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
	{
		if (!snd_chnl->isPlaying())
		{
			if(AudioSpec.channels == 2)
				snd_chnl->setBalance(balance);

			snd_chnl->enableHighQuality(m_soundslot[snd].isHighQuality());
			snd_chnl->setupSound( snd, 0, true, 0, (mode==PLAY_FORCE) ? true : false, AudioSpec.format );
			break;
		}
	}
}

void CSound::setGameData(CExeFile &ExeFile)
{
	m_Episode = ExeFile.getEpisode();
	m_DataDirectory = ExeFile.getDataDirectory();
}


bool CSound::loadSoundData(CExeFile &ExeFile)
{
	g_pLogFile->textOut("Error! This function shouldn't be called here. It seems that no episode is loaded!");
	return false;
}

void CSound::setSoundmode(int freq, bool stereo, Uint16 format)
{
	AudioSpec.channels = (stereo==true) ? 2 : 1;
	AudioSpec.format = format;

	switch(freq)
	{
		case 0: break; // means that the actual frequency stays untouched
		case 11025:
		case 22050:
		case 44100:
		case 48000:
			AudioSpec.freq = freq;
			break;
		default:
			AudioSpec.freq = 44100;
	}
}
