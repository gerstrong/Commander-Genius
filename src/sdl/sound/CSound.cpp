/*
 * CSound.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "CSound.h"
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include <lib/base/GsLogging.h>
//#include "StringUtils.h"
#include <base/FindFile.h>
#include "sdl/music/CMusic.h"
#include "common/CBehaviorEngine.h"

//#include "engine/vorticon/CAudioVorticon.h"
//#include "engine/galaxy/res/CAudioGalaxy.h"

#include <fstream>


// Forward declaration which makes Mixer.h obsolete.
void mixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, Uint8 volume, Uint16 format);


// This central list tells which frequencies can be used for your soundcard.
// In case you want to add some more, just modify this list
#if defined(ANDROID)
static const unsigned int numAvailableRates = 3;
static const int availableRates[numAvailableRates]=
{
		11025,
		22050,
		44100
};

#else
static const unsigned int numAvailableRates = 5;
static const int availableRates[numAvailableRates]=
{
		11025,
		22050,
		44100,
		48000,
		49716
};
#endif


// define a callback function we can work with
inline static void CCallback(void *unused, Uint8 *stream, int len)
{    
    SDL_memset(stream, 0, len);

    // let it call a method on my (singleton) sound object
    CSound::GetNoPtrChk()->callback(unused, stream, len);
}

CSound::CSound() :
m_callback_running(false),
m_mixing_channels(0),
m_MusicVolume(SDL_MIX_MAXVOLUME),
m_SoundVolume(SDL_MIX_MAXVOLUME),
m_sound_blaster_mode(false),
m_OPL_Player(mAudioSpec),
m_pause_gameplay(false)
{
	mAudioSpec.channels = 2; // Stereo Sound
	mAudioSpec.format = AUDIO_S16; // 16-bit sound
	mAudioSpec.freq = 44100; // high quality
}

CSound::~CSound()
{
    // Wait until callback function finishes its calls
    while(m_callback_running);
}

bool CSound::init()
{
	gLogging.ftextOut("Starting the sound driver...<br>");
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
		gLogging.ftextOut("SoundDrv_Start(): Couldn't open audio: %s<br>", SDL_GetError());
		gLogging.ftextOut("Sound will be disabled.<br>");
		mAudioSpec.channels = 0;
		mAudioSpec.format = 0;
		mAudioSpec.freq = 0;
		return false;
	}

	mAudioSpec = obtained;

	m_MixedForm.reserve(mAudioSpec.size);

	gLogging.ftextOut("SDL_AudioSpec:<br>");
	gLogging.ftextOut("  freq: %d<br>", mAudioSpec.freq);
	gLogging.ftextOut("  channels: %d<br>", mAudioSpec.channels);
	gLogging.ftextOut("  audio buffer size: %d<br>", mAudioSpec.size);
	switch( mAudioSpec.format )
	{
		case AUDIO_U8:
			gLogging.ftextOut("  format: AUDIO_U8<br>" );
			break;
		case AUDIO_S16:
			gLogging.ftextOut("  format: AUDIO_S16<br>" );
			break;
		default:
			gLogging.ftextOut("  format: UNKNOWN %d<br>", mAudioSpec.format );
			break;
	}
#if SDL_VERSION_ATLEAST(2, 0, 0)
    gLogging.ftextOut("Using audio driver: %s<br>", SDL_GetCurrentAudioDriver());
#else
   // gLogging.ftextOut("Using audio driver: %s<br>", SDL_AudioDriverName(name, 32));
#endif

	//m_mixing_channels = 15;
	m_mixing_channels = 32;

	if(!m_soundchannel.empty())
		m_soundchannel.clear();
	m_soundchannel.assign(m_mixing_channels, CSoundChannel(mAudioSpec));

	SDL_PauseAudio(0);

	gLogging.ftextOut("Sound System: SDL sound system initialized.<br>");

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
	gLogging.ftextOut("SoundDrv_Stop(): shut down.<br>");

	m_OPL_Player.shutdown();
}

// stops all currently playing sounds
void CSound::stopAllSounds()
{
	for( auto &snd_chnl : m_soundchannel )
		snd_chnl.stopSound();
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
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++ )
	{
		if (snd_chnl->isPlaying())
		{
			CSoundSlot *pSlotToStop = mpAudioRessources->getSlotPtrAt(snd);
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
			CSoundSlot *pSlotToStop =  mpAudioRessources->getSlotPtrAt(snd);
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
	if(!mpAudioRessources)
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
	{
		// means no sound is playing
		m_pause_gameplay = false;
	}

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
				  const int xcoordinate )
{
    if(mAudioSpec.channels == 2)
    {
    	int bal = (xcoordinate - (320>>1));	// Faster calculation of balance transformation

    	if(bal < -255)
	{
	    bal = -255;
	}
    	else if(bal > 255)
	{
	    bal = 255;
	}

    	playStereosound(snd, mode, bal);
    }
    else
    {
    	playSound(snd, mode);
    }
}




void CSound::playStereosound(const GameSound snd, const char mode, const short balance)
{
	if( m_mixing_channels == 0 ) return;

	CSoundSlot *mp_Slots = mpAudioRessources->getSlotPtr();
	int slotplay = sndSlotMap[snd];
	const int speaker_snds_end_off = mpAudioRessources->getNumberofSounds()/2;

	if(slotplay >= speaker_snds_end_off)
		return;

	if(m_sound_blaster_mode && mp_Slots[slotplay+speaker_snds_end_off].getSoundData())
		slotplay += speaker_snds_end_off;

	if (mode==PLAY_NORESTART && isPlaying(snd))
		return;

	playStereosoundSlot(slotplay, mode, balance);
}


void CSound::playStereosoundSlot(unsigned char slotplay, const char mode, const short balance)
{
	CSoundSlot *mp_Slots = mpAudioRessources->getSlotPtr();
	CSoundSlot &new_slot = mp_Slots[slotplay];

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
    /*const CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
    const unsigned int ep = ExeFile.getEpisode();
    if(ep >= 1 && ep <= 3) // Vorticon based Keengame
    {
        std::unique_ptr<CAudioVorticon> vorticonAudio(new CAudioVorticon(ExeFile, mAudioSpec));
        const bool ok = vorticonAudio->loadSoundData();
        sndSlotMap = vorticonAudio->sndSlotMap;
        mpAudioRessources = move(vorticonAudio);
        return ok;
    }
    else if(ep >= 4 && ep <= 7) // Galaxy based Keengame
    {
        std::unique_ptr<CAudioGalaxy> galaxyAudio(new CAudioGalaxy(ExeFile, mAudioSpec));
        const bool ok = galaxyAudio->loadSoundData();
        sndSlotMap = galaxyAudio->sndSlotMapGalaxy[ep];
        mpAudioRessources = move(galaxyAudio);
        return ok;
    }*/

    return false;
}

void CSound::setupSoundData(const std::map<GameSound, int> &slotMap,
                           CAudioResources *audioResPtr)
{
    assert(audioResPtr);

    sndSlotMap = slotMap;
    mpAudioRessources.reset(audioResPtr);
}

void CSound::unloadSoundData()
{
    // Wait for callback to finish running...
    while(m_callback_running);

    mpAudioRessources.release();
    m_MixedForm.clear();
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
