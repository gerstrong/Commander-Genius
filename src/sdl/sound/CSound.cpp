/*
 * CSound.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "CSound.h"
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include "CLogFile.h"
#include "StringUtils.h"
#include "FindFile.h"
#include "sdl/music/CMusic.h"
#include "sdl/sound/Mixer.h"
#include "common/CBehaviorEngine.h"

#include "engine/vorticon/CAudioVorticon.h"
#include "engine/galaxy/CAudioGalaxy.h"

#include <fstream>



// This central list tells which frequencies can be used for your soundcard.
// In case you want to add some more, just modify this list
static const unsigned int numAvailableRates = 5;
static const int availableRates[numAvailableRates]=
{
		11025,
		22050,
		44100,
		48000,
		49716
};


// define a callback function we can work with
inline static void CCallback(void *unused, Uint8 *stream, int len)
{
    // let it call a method on my (singleton) sound object
    CSound::GetNoPtrChk()->callback(unused, stream, len);
}

CSound::CSound() :
m_callback_running(false),
m_mixing_channels(0),
m_MusicVolume(SDL_MIX_MAXVOLUME),
m_SoundVolume(SDL_MIX_MAXVOLUME),
m_sound_blaster_mode(false),
mp_SndSlotMap(NULL),
m_OPL_Player(mAudioSpec),
m_pause_gameplay(false)
{
	mAudioSpec.channels = 2; // Stereo Sound
#if defined(CAANOO) || defined(WIZ) || defined(GP2X) || defined(DINGOO) || defined(ANDROID)
	mAudioSpec.format = AUDIO_S16; // 16-bit sound
#else
	mAudioSpec.format = AUDIO_U8; // 8-bit sound
#endif
	mAudioSpec.freq = 44100; // high quality
}

CSound::~CSound()
{
	while(m_callback_running);

	m_pAudioRessources = NULL;
}

bool CSound::init()
{
	g_pLogFile->ftextOut("Starting the sound driver...<br>");
	char name[256];
	SDL_AudioSpec obtained;

	// now start up the SDL sound system
	mAudioSpec.silence = 0;

	switch (mAudioSpec.freq)
	{
		case 11025: mAudioSpec.samples = 256; break;
		case 22050: mAudioSpec.samples = 512; break;
		default: mAudioSpec.samples = 1024; break;
	}
	mAudioSpec.callback = CCallback;
	mAudioSpec.userdata = NULL;

	// Initialize variables
	if( SDL_OpenAudio(&mAudioSpec, &obtained) < 0 )
	{
		g_pLogFile->ftextOut("SoundDrv_Start(): Couldn't open audio: %s<br>", SDL_GetError());
		g_pLogFile->ftextOut("Sound will be disabled.<br>");
		mAudioSpec.channels = 0;
		mAudioSpec.format = 0;
		mAudioSpec.freq = 0;
		return false;
	}

	mAudioSpec = obtained;

	m_MixedForm.reserve(mAudioSpec.size);

	g_pLogFile->ftextOut("SDL_AudioSpec:<br>");
	g_pLogFile->ftextOut("  freq: %d<br>", mAudioSpec.freq);
	g_pLogFile->ftextOut("  channels: %d<br>", mAudioSpec.channels);
	g_pLogFile->ftextOut("  audio buffer size: %d<br>", mAudioSpec.size);
	switch( mAudioSpec.format )
	{
		case AUDIO_U8:
			g_pLogFile->ftextOut("  format: AUDIO_U8<br>" );
			break;
		case AUDIO_S16:
			g_pLogFile->ftextOut("  format: AUDIO_S16<br>" );
			break;
		default:
			g_pLogFile->ftextOut("  format: UNKNOWN %d<br>", mAudioSpec.format );
			break;
	}
	g_pLogFile->ftextOut("Using audio driver: %s<br>", SDL_AudioDriverName(name, 32));

	m_mixing_channels = 15;

	if(!m_soundchannel.empty())
		m_soundchannel.clear();
	m_soundchannel.assign(m_mixing_channels, CSoundChannel(mAudioSpec));

	SDL_PauseAudio(0);

	g_pLogFile->ftextOut("Sound System: SDL sound system initialized.<br>");

	// Let's initialize the OPL Emulator here!
	m_OPL_Player.init();

	return true;
}

void CSound::destroy()
{
	stopAllSounds();

	SDL_LockAudio();
	SDL_CloseAudio();
	m_mixing_channels = 0;

	if(!m_MixedForm.empty())
		m_MixedForm.clear();

	if(!m_soundchannel.empty())
		m_soundchannel.clear();

	// Shutdown the OPL Emulator here!
	g_pLogFile->ftextOut("SoundDrv_Stop(): shut down.<br>");

	m_OPL_Player.shutdown();
}

// stops all currently playing sounds
void CSound::stopAllSounds()
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
		snd_chnl->stopSound();
}

// pauses any currently playing sounds
void CSound::pauseSound()
{
	SDL_PauseAudio(1);
}

// resumes playing a previously paused sound
void CSound::resumeSounds()
{
	SDL_PauseAudio(0);
}

// returns true if sound snd is currently playing
bool CSound::isPlaying(const GameSound snd)
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
	{
		if (snd_chnl->isPlaying())
		{
			CSoundSlot *pSlotToStop = m_pAudioRessources->getSlotPtrAt(snd);
			if (snd_chnl->getCurrentSoundPtr() == pSlotToStop)
				return true;
		}
	}
	return false;
}

// if sound snd is currently playing, stops it immediately
void CSound::stopSound(const GameSound snd)
{
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
	{
		if (snd_chnl->isPlaying())
		{
			CSoundSlot *pSlotToStop =  m_pAudioRessources->getSlotPtrAt(snd);
			if( snd_chnl->getCurrentSoundPtr() == pSlotToStop )
				snd_chnl->stopSound();
		}
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

void CSound::callback(void *unused, Uint8 *stream, int len)
{
	if(m_pAudioRessources.get() == NULL)
		return;

	m_callback_running = true;

	Uint8* buffer = m_MixedForm.data();

    if (g_pMusicPlayer->playing())
    {
    	g_pMusicPlayer->readWaveform(buffer, len);
    	mixAudio(stream, buffer, len, m_MusicVolume, mAudioSpec.format);
    }

    bool any_sound_playing = false;
	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
   	{
		if(snd_chnl->isPlaying())
		{
			any_sound_playing |= true;
			snd_chnl->readWaveform( buffer, len);
   			mixAudio(stream, buffer, len, m_SoundVolume, mAudioSpec.format);
		}
    }

	if(!any_sound_playing)
		// means no sound is playing
		m_pause_gameplay = false;

	m_callback_running = false;
}

// if priorities allow, plays the sound "snd".
// nonzero return value indicates a higher priority sound is playing.
void CSound::playSound(	const GameSound snd,
						const SoundPlayMode mode )
{
	playStereosound(snd, mode, 0);
}

void CSound::playStereofromCoord( const GameSound snd,
								  const SoundPlayMode mode,
								  const unsigned int xcoordinate)
{
    if(mAudioSpec.channels == 2)
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

void CSound::playStereosound(const GameSound snd, const char mode, const short balance)
{
	if( m_mixing_channels == 0 ) return;

	CSoundSlot *mp_Slots = m_pAudioRessources->getSlotPtr();
	unsigned char slotplay = mp_SndSlotMap[snd];
	const unsigned int speaker_snds_end_off = m_pAudioRessources->getNumberofSounds()/2;

	if(slotplay >= speaker_snds_end_off)
		return;

	if(m_sound_blaster_mode && mp_Slots[slotplay+speaker_snds_end_off].getSoundData())
		slotplay += speaker_snds_end_off;

	CSoundSlot &new_slot = mp_Slots[slotplay];

	if (mode==PLAY_NORESTART && isPlaying(snd))
		return;

	if(mode==PLAY_PAUSEALL)
		m_pause_gameplay = true;

	// if a forced sound is playing then let it play
	if (forcedisPlaying()) return;

	// stop all other sounds if this sound has maximum priority
	if ( mode==PLAY_FORCE )
		stopAllSounds();

	// first try to find an empty channel
	std::vector<CSoundChannel>::iterator snd_chnl;
	for( snd_chnl = m_soundchannel.begin() ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
	{
		CSoundSlot &current_slot = *snd_chnl->getCurrentSoundPtr();

		if ( !snd_chnl->isPlaying() || ( new_slot.priority >= current_slot.priority ) )
		{
			if(mAudioSpec.channels == 2)
				snd_chnl->setBalance(balance);

			snd_chnl->setupSound( new_slot, (mode==PLAY_FORCE) ? true : false );
			break;
		}
	}
}

bool CSound::loadSoundData()
{
	const CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
	if(ExeFile.getEpisode() >= 1 && ExeFile.getEpisode() <= 3) // Vorticon based Keengame
	{
		m_pAudioRessources = new CAudioVorticon(ExeFile, mAudioSpec);
		mp_SndSlotMap = const_cast<unsigned char*>(SndSlotMapVort);
		return(m_pAudioRessources->loadSoundData());
	}
	else if(ExeFile.getEpisode() >= 4 && ExeFile.getEpisode() <= 7) // Galaxy based Keengame
	{
		m_pAudioRessources = new CAudioGalaxy(ExeFile, mAudioSpec);
		mp_SndSlotMap = const_cast<unsigned char*>(SndSlotMapGalaxy);
		return(m_pAudioRessources->loadSoundData());
	}

	return false;
}

void CSound::unloadSoundData()
{
	while(m_callback_running);
	m_pAudioRessources = NULL;
}



bool CSound::pauseGamePlay()
{
	return m_pause_gameplay;
}



std::list<std::string> CSound::getAvailableRateList() const
{
	std::list<std::string> rateStrList;

	for( unsigned int i=0 ; i<numAvailableRates ; i++ )
		rateStrList.push_back( itoa(availableRates[i]) );

	return rateStrList;
}



void CSound::setSettings( const SDL_AudioSpec& audioSpec,
	 	  	  	  	  	  const bool useSB )
{
	m_sound_blaster_mode = useSB;

	// Check if rate matches to those available in the system
	for( unsigned int i=0 ; i<numAvailableRates ; i++ )
	{
		if( availableRates[i] == audioSpec.freq )
		{
			mAudioSpec = audioSpec;
			break;
		}
	}


}



void CSound::setSettings( const int rate,
						  const int channels,
						  const int format,
	 	  	  	  	  	  const bool useSB )
{
	SDL_AudioSpec nAudio = mAudioSpec;

	nAudio.freq = rate;
	nAudio.channels = channels;
	nAudio.format = format;

	setSettings(nAudio, useSB);
}
