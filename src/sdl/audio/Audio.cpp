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

#include <SDL_mixer.h>

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
/*inline static void CCallback(void *unused, Uint8 *stream, int len)
{
    SDL_memset(stream, 0, len);

    // let it call a method on my (singleton) sound object
    Audio::get().callback(unused, stream, len);
}*/

Audio::Audio() :
m_MusicVolume(SDL_MIX_MAXVOLUME),
m_SoundVolume(SDL_MIX_MAXVOLUME)
{
	mAudioSpec.channels = 2; // Stereo Sound
	mAudioSpec.format = AUDIO_S16; // 16-bit sound
	mAudioSpec.freq = 44100; // high quality

    updateFuncPtrs();
}

Audio::~Audio()
{
    // Wait until callback function finishes its calls
    while(mCallbackRunning);
}

bool Audio::init()
{
	gLogging.ftextOut("Starting the sound driver...<br>");
    //SDL_AudioSpec obtained;

	// now start up the SDL sound system
	mAudioSpec.silence = 0;

	switch (mAudioSpec.freq)
	{
		case 11025: mAudioSpec.samples = 256; break;
		case 22050: mAudioSpec.samples = 512; break;
        default: mAudioSpec.samples = 1024; break;
	}
    //mAudioSpec.callback = CCallback;
    mAudioSpec.userdata = nullptr;

    // Initialize audio system
    if( Mix_OpenAudio(mAudioSpec.freq,
                      mAudioSpec.format,
                      mAudioSpec.channels,
                      mAudioSpec.samples) < 0 )
    {
        gLogging << "Mix_OpenAudio: " << Mix_GetError();
        return false;
    }

    {
        int n = Mix_GetNumChunkDecoders();

        gLogging << "There are "<< n << " available chunk(sample) decoders";

        for(int i=0; i<n; ++i)
        {
            gLogging << "	" << Mix_GetChunkDecoder(i);
        }

        n = Mix_GetNumMusicDecoders();

        gLogging << "There are " << n << "available music decoders:";

        for(int i=0; i<n; ++i)
        {
            gLogging << "	" << Mix_GetMusicDecoder(i);
        }
    }


    // print out some info on the audio device and stream
    int audio_rate;
    Uint16 audio_format;
    int audio_channels;

    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

    const auto bits = audio_format&0xFF;

    mAudioSpec.freq = audio_rate;
    mAudioSpec.format = audio_format;
    mAudioSpec.channels = Uint8(audio_channels);

    gLogging << "Opened audio at " << audio_rate << " Hz "
             << bits << " bit "
             << (audio_channels>1?"stereo":"mono")
             << ", " << mAudioSpec.size << " bytes audio buffer.\n";

    const unsigned int channels = 32;
    Mix_AllocateChannels(channels);


    mSndChnlVec.clear();

    mSndChnlVec.assign(channels, CSoundChannel(mAudioSpec));

    Mix_VolumeMusic(m_MusicVolume);
    Mix_Volume(-1, m_SoundVolume);

    //SDL_PauseAudio(0);

    gLogging << "Sound System: SDL sound system initialized.<br>";

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
    {
        mixAudio = mixAudioSigned16;
    }
    else if(mAudioSpec.format == AUDIO_U8)
    {
        mixAudio = mixAudioUnsigned8;
    }
}


void Audio::destroy()
{
	stopAllSounds();

    Mix_HaltChannel(-1);

    if(!mSndChnlVec.empty())
    {
        mSndChnlVec.clear();
    }

	// Shutdown the OPL Emulator here!
	gLogging.ftextOut("SoundDrv_Stop(): shut down.<br>");

	m_OPL_Player.shutdown();

    Mix_CloseAudio();
}

// stops all currently playing sounds
void Audio::stopAllSounds()
{
    for( auto &snd_chnl : mSndChnlVec )
    {
		snd_chnl.stopSound();
    }
}

// pauses any currently playing sounds
void Audio::pauseAudio()
{
    Mix_Pause(-1);
    Mix_PauseMusic();
}

// resumes playing a previously paused sound
void Audio::resumeAudio()
{
    Mix_Resume(-1);
    Mix_ResumeMusic();
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
            {
                snd_chnl->stopSound();
            }
        }
	}
}

void Audio::setSoundVolume(const Uint8 volume,
                           const bool updateMixer)
{
    m_SoundVolume = volume;

    if(updateMixer)
    {
        Mix_Volume(-1, volume);
    }
}
void Audio::setMusicVolume(const Uint8 volume,
                           const bool updateMixer)
{
    m_MusicVolume = volume;

    if(updateMixer)
    {
        Mix_VolumeMusic(volume);
    }
}


// returns true if a sound is currently playing in SoundPlayMode::PLAY_FORCE mode
bool Audio::forcedisPlaying()
{
    std::vector<CSoundChannel>::iterator snd_chnl = mSndChnlVec.begin();
    for( ; snd_chnl != mSndChnlVec.end() ; snd_chnl++)
    {
		if(snd_chnl->isForcedPlaying())
        {
			return true;
        }
    }

	return false;
}


void Audio::playSound(const GameSound snd,
                      const SoundPlayMode mode )
{
	playStereosound(snd, mode, 0);
}

void Audio::playStereofromCoord(const GameSound snd,
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

void Audio::playStereosound(const GameSound snd,
                            const SoundPlayMode mode,
                            const short balance)
{
    if( mSndChnlVec.empty() ) return;

    if( !mpAudioRessources ) return;

	CSoundSlot *mp_Slots = mpAudioRessources->getSlotPtr();
    int slotplay = sndSlotMap[snd];

	const int speaker_snds_end_off = mpAudioRessources->getNumberofSounds()/2;

    if (slotplay >= speaker_snds_end_off)
    {
		return;
    }

    if (mUseSoundBlaster && mp_Slots[slotplay+speaker_snds_end_off].getSoundData())
    {
		slotplay += speaker_snds_end_off;
    }

    if (mode == SoundPlayMode::PLAY_NORESTART && isPlaying(snd))
    {
		return;
    }

    playStereosoundSlot(slotplay, mode, balance);
}


void Audio::playStereosoundSlot(unsigned char slotplay,
                                const SoundPlayMode mode,
                                const short balance)
{
    CSoundSlot *pSlots = mpAudioRessources->getSlotPtr();
    CSoundSlot &chosenSlot = pSlots[slotplay];

    if(mode == SoundPlayMode::PLAY_PAUSEALL)
    {
        mPauseGameplay = true;
    }

	// stop all other sounds if this sound has maximum priority
    if ( mode == SoundPlayMode::PLAY_FORCE )
    {
		stopAllSounds();
    }


	// first try to find an empty channel
    for( auto &sndChnl : mSndChnlVec)
	{
        // check if channel is still playing

        auto slotPtr = sndChnl.getCurrentSoundPtr();

        if(slotPtr)
        {
            CSoundSlot &currentSlot = *(slotPtr);

            if (!sndChnl.isPlaying() ||
                chosenSlot.priority >= currentSlot.priority )
            {
                if(mAudioSpec.channels == 2)
                {
                    sndChnl.setBalance(balance);
                }

                sndChnl.setupSound(chosenSlot,
                                  (mode==SoundPlayMode::PLAY_FORCE) ? true : false );
                break;
            }
        }
        else if (!sndChnl.isPlaying())
        {
            if(mAudioSpec.channels == 2)
            {
                sndChnl.setBalance(balance);
            }

            sndChnl.setupSound(chosenSlot,
                                (mode==SoundPlayMode::PLAY_FORCE) ? true : false );

            break;
        }
	}
}

void Audio::setupSoundData(const std::map<GameSound, int> &slotMap,
                           CAudioResources *audioResPtr)
{
    assert(audioResPtr);

    sndSlotMap = slotMap;
    mpAudioRessources.reset(audioResPtr);
}

void Audio::unloadSoundData()
{
    // Wait for callback to finish running...
    while(mCallbackRunning);

    mpAudioRessources.release();
}



bool Audio::pauseGamePlay()
{
    return mPauseGameplay;
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





