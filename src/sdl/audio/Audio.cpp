/*
 * Audio.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "Audio.h"
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include "sdl/audio/music/CMusic.h"

#include <fstream>


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
    Audio::GetNoPtrChk()->callback(unused, stream, len);
}

Audio::Audio() :
m_callback_running(false),
m_MusicVolume(SDL_MIX_MAXVOLUME),
m_SoundVolume(SDL_MIX_MAXVOLUME),
mUseSoundBlaster(false),
m_OPL_Player(mAudioSpec),
m_pause_gameplay(false)
{
	mAudioSpec.channels = 2; // Stereo Sound
	mAudioSpec.format = AUDIO_S16; // 16-bit sound
	mAudioSpec.freq = 44100; // high quality

    updateFuncPtrs();
}

Audio::~Audio()
{
    // Wait until callback function finishes its calls
    while(m_callback_running);
}

bool Audio::init()
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

    m_MixedForm.resize(mAudioSpec.size);

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

    const unsigned int channels = 32;

    if(!mSndChnlVec.empty())
        mSndChnlVec.clear();

    mSndChnlVec.assign(channels, CSoundChannel(mAudioSpec));

    SDL_PauseAudio(0);

	gLogging.ftextOut("Sound System: SDL sound system initialized.<br>");

	// Let's initialize the OPL Emulator here!
	m_OPL_Player.init();

    updateFuncPtrs();

	return true;
}


void (*mixAudio)(Uint8*, const Uint8*, Uint32, Uint32);

void mixAudioUnsigned8(Uint8 *dst, const Uint8 *src, Uint32 len, Uint32 volume);

void mixAudioSigned16(Uint8 *dst, const Uint8 *src, Uint32 len, Uint32 volume);


/**
 * @brief updateFuncPtrs Depending on the audio setup it will update the mixAudio function pointer.
 */
void Audio::updateFuncPtrs()
{
    if(mAudioSpec.format == AUDIO_S16)
        mixAudio = mixAudioSigned16;
    else if(mAudioSpec.format == AUDIO_U8)
        mixAudio = mixAudioUnsigned8;
}


void Audio::destroy()
{
	stopAllSounds();

    SDL_LockAudio();
	SDL_CloseAudio();

	if(!m_MixedForm.empty())
		m_MixedForm.clear();

    if(!mSndChnlVec.empty())
        mSndChnlVec.clear();

	// Shutdown the OPL Emulator here!
	gLogging.ftextOut("SoundDrv_Stop(): shut down.<br>");

	m_OPL_Player.shutdown();
}

// stops all currently playing sounds
void Audio::stopAllSounds()
{
    for( auto &snd_chnl : mSndChnlVec )
		snd_chnl.stopSound();
}

// pauses any currently playing sounds
void Audio::pauseSound()
{
	SDL_PauseAudio(1);
}

// resumes playing a previously paused sound
void Audio::resumeSounds()
{
	SDL_PauseAudio(0);
}

// returns true if sound snd is currently playing
bool Audio::isPlaying(const GameSound snd)
{
    std::vector<CSoundChannel>::iterator snd_chnl = mSndChnlVec.begin();
    for( ; snd_chnl != mSndChnlVec.end() ; snd_chnl++ )
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

// if sound snd is currently playing, stop it immediately
void Audio::stopSound(const GameSound snd)
{
    std::vector<CSoundChannel>::iterator snd_chnl = mSndChnlVec.begin();
    for( ; snd_chnl != mSndChnlVec.end() ; snd_chnl++)
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
bool Audio::forcedisPlaying()
{
    std::vector<CSoundChannel>::iterator snd_chnl = mSndChnlVec.begin();
    for( ; snd_chnl != mSndChnlVec.end() ; snd_chnl++)
		if(snd_chnl->isForcedPlaying())
			return true;

	return false;
}

void Audio::callback(void *unused, Uint8 *stream, int len)
{
    m_callback_running = true;

    // Subcallbacks for so far are only used by the dosfusion system
    for(auto &subCallback : mSubCallbackVec)
    {
        subCallback(unused, stream, len);
    }

	if(!mpAudioRessources)        
    {
        m_callback_running = false;
		return;
    }

    m_MixedForm.resize(len);

	Uint8* buffer = m_MixedForm.data();

    if (g_pMusicPlayer->playing())
    {
    	g_pMusicPlayer->readWaveform(buffer, len);
        mixAudio(stream, buffer, len, m_MusicVolume);
    }

    bool any_sound_playing = false;
    std::vector<CSoundChannel>::iterator snd_chnl = mSndChnlVec.begin();
    for( ; snd_chnl != mSndChnlVec.end() ; snd_chnl++)
   	{
		if(snd_chnl->isPlaying())
		{
			any_sound_playing |= true;
			snd_chnl->readWaveform( buffer, len);
            mixAudio(stream, buffer, len, m_SoundVolume);
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
void Audio::playSound(	const GameSound snd,
			const SoundPlayMode mode )
{
	playStereosound(snd, mode, 0);
}

void Audio::playStereofromCoord( const GameSound snd,
				  const SoundPlayMode mode,
                  const int xCoord )
{
    if(mAudioSpec.channels == 2)
    {
        int bal = (xCoord - (320>>1));	// Faster calculation of balance transformation

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




void Audio::playStereosound(const GameSound snd, const char mode, const short balance)
{
    if( mSndChnlVec.empty() ) return;

    if(!mpAudioRessources)
        return;

	CSoundSlot *mp_Slots = mpAudioRessources->getSlotPtr();
	int slotplay = sndSlotMap[snd];
	const int speaker_snds_end_off = mpAudioRessources->getNumberofSounds()/2;

	if(slotplay >= speaker_snds_end_off)
		return;

    if(mUseSoundBlaster && mp_Slots[slotplay+speaker_snds_end_off].getSoundData())
		slotplay += speaker_snds_end_off;

	if (mode==PLAY_NORESTART && isPlaying(snd))
		return;

	playStereosoundSlot(slotplay, mode, balance);
}


void Audio::playStereosoundSlot(unsigned char slotplay, const char mode, const short balance)
{
	CSoundSlot *mp_Slots = mpAudioRessources->getSlotPtr();
	CSoundSlot &new_slot = mp_Slots[slotplay];

	if(mode==PLAY_PAUSEALL)
		m_pause_gameplay = true;

	// stop all other sounds if this sound has maximum priority
	if ( mode==PLAY_FORCE )
		stopAllSounds();

	// first try to find an empty channel
	std::vector<CSoundChannel>::iterator snd_chnl;
    for( snd_chnl = mSndChnlVec.begin() ; snd_chnl != mSndChnlVec.end() ; snd_chnl++)
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

void Audio::setupSoundData(const std::map<GameSound, int> &slotMap,
                           CAudioResources *audioResPtr)
{
    assert(audioResPtr);

    SDL_LockAudio();

    sndSlotMap = slotMap;
    mpAudioRessources.reset(audioResPtr);

    SDL_UnlockAudio();
}

void Audio::unloadSoundData()
{
    // Wait for callback to finish running...
    while(m_callback_running);

    SDL_LockAudio();

    mpAudioRessources.release();
    m_MixedForm.clear();

    SDL_UnlockAudio();
}



bool Audio::pauseGamePlay()
{
	return m_pause_gameplay;
}



std::list<std::string> Audio::getAvailableRateList() const
{
	std::list<std::string> rateStrList;

	for( unsigned int i=0 ; i<numAvailableRates ; i++ )
		rateStrList.push_back( itoa(availableRates[i]) );

	return rateStrList;
}



void Audio::setSettings( const SDL_AudioSpec& audioSpec,
	 	  	  	  	  	  const bool useSB )
{
    mUseSoundBlaster = useSB;

	// Check if rate matches to those available in the system
	for( unsigned int i=0 ; i<numAvailableRates ; i++ )
	{
		if( availableRates[i] == audioSpec.freq )
		{
			mAudioSpec = audioSpec;
			break;
		}
	}

    updateFuncPtrs();
}



void Audio::setSettings( const int rate,
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





