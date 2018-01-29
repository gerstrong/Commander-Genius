/*
 * CIMFPlayer.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 *
 *  Player which reproduces the IMF Data
 */

#include "CIMFPlayer.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include "fileio/KeenFiles.h"
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include <fstream>
#include <string>
#include <cassert>

#if defined (USE_SDLMIXER)
#include <SDL_mixer.h>
#endif


CIMFPlayer::CIMFPlayer( COPLEmulator& opl_emulator ) :
m_opl_emulator(opl_emulator)
{
    m_samplesPerMusicTick = gSound.getAudioSpec().freq /
            m_opl_emulator.getIMFClockRate();
}


bool CIMFPlayer::loadMusicFromFile(const std::string& filename)
{    
    // Open the IMF File
    FILE *fp;
    word data_size;
    bool ok = false;
    
    if( ( fp = OpenGameFile(filename, "rb") ) == nullptr )
    {
        return false;
    }
    
    int read_first = fread( &data_size, sizeof(word), 1, fp);
    
    if( read_first == 0)
    {
        return false;
    }

    if (data_size == 0) // Is the IMF file of Type-0?
    {
        fseek(fp, 0, SEEK_END);
        data_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }
    
#if !defined(USE_SDLMIXER)
    SDL_LockAudio();
#endif
    
    if(!m_IMF_Data.empty())
        m_IMF_Data.clear();

    const word imf_chunks = (data_size/sizeof(IMFChunkType));
    m_IMF_Data.reserve(imf_chunks);
    
    if( imf_chunks != fread( m_IMF_Data.getStartPtr(), sizeof(IMFChunkType), imf_chunks, fp ) )
    {
        gLogging.textOut("The IMF-File seems to be corrupt.");
    }
    else
    {
        ok = true;
    }
    
    fclose(fp);

    SDL_UnlockAudio();
    
    return ok;
}


bool CIMFPlayer::loadMusicTrack(const int track)
{
#if !defined(USE_SDLMIXER)
    SDL_LockAudio();
#endif

    if( m_IMF_Data.empty() )
        m_IMF_Data.clear();

    if(!gKeenFiles.exeFile.loadMusicTrack(m_IMF_Data, track))
    {
#if !defined(USE_SDLMIXER)
        SDL_UnlockAudio();
#endif
        return false;
    }

#if !defined(USE_SDLMIXER)
    SDL_UnlockAudio();
#endif

    return true;
}



bool CIMFPlayer::open(const bool lock)
{
#if !defined(USE_SDLMIXER)
    if(lock) SDL_LockAudio();
#endif

	m_numreadysamples = m_IMFDelay = 0;
    m_samplesPerMusicTick = gSound.getAudioSpec().freq / m_opl_emulator.getIMFClockRate();
	
	m_opl_emulator.setup();

#if !defined(USE_SDLMIXER)
    if(lock) SDL_UnlockAudio();
#endif

	return !m_IMF_Data.empty();
}

void CIMFPlayer::close(const bool lock)
{
#if !defined(USE_SDLMIXER)
    if(lock) SDL_LockAudio();
#endif

	play(false);
	m_IMF_Data.gotoStart();	
	m_numreadysamples = m_IMFDelay = 0;	
	m_opl_emulator.ShutAL();
	m_opl_emulator.shutdown();

#if !defined(USE_SDLMIXER)
    if(lock) SDL_UnlockAudio();
#endif

	return;
}



void CIMFPlayer::OPLUpdate(byte *buffer, const unsigned int length)
{    
    auto &audioSpec = gSound.getAudioSpec();

    if(mMixBuffer.empty())
    {
        gLogging.textOut("Creating enough Audio Buffer for the IMF-Player");
        mMixBuffer.assign(audioSpec.samples, 0);
    }

    m_opl_emulator.Chip__GenerateBlock2( length, mMixBuffer.data() );

    // Mix into the destination buffer, doubling up into stereo.
    if(audioSpec.format == AUDIO_S16)
	{
		Sint16 *buf16 = (Sint16*) (void*) buffer;
		for (unsigned int i=0; i<length; ++i)
		{
		    Sint32 mix = mMixBuffer[i];
		    
		    if(mix > 32767)
			    mix = 32767;
		    else if(mix < -32768)
			    mix = -32768;
		    
            for (unsigned int j=0; j<audioSpec.channels; j++)
			{
                *buf16 = mix + audioSpec.silence;
				buf16++;
			}
		}
	}
    else if(audioSpec.format == AUDIO_U8)
	{
		for (unsigned int i=0; i<length; ++i)
		{
		    Sint32 mix = mMixBuffer[i]>>8;
		    
		    if(mix > 255)
			    mix = 255;
		    else if(mix < 0)
			    mix = 0;
		    
            for (unsigned int j=0; j<audioSpec.channels; j++)
			{
                *buffer = mix + audioSpec.silence;
				buffer++;
			}
		}
	}
}

void CIMFPlayer::readBuffer(Uint8* buffer, Uint32 length)
{
    if(!m_playing)
        return;

    auto &audioSpec = gSound.getAudioSpec();

    
    /// if a delay of the instruments is pending, play it
    Uint32 sampleslen = audioSpec.samples;
    Uint32 sample_mult = audioSpec.channels;
    sample_mult = (audioSpec.format == AUDIO_S16) ? sample_mult*sizeof(Sint16) : sample_mult*sizeof(Uint8) ;
	
	// while the waveform is not filled
    while(1)
    {
        while( m_IMFDelay == 0 )
        {
            //read next IMF event
            const IMFChunkType Chunk = m_IMF_Data.getNextElement();
            m_IMFDelay = Chunk.Delay;

            //write reg+val to opl chip
            m_opl_emulator.Chip__WriteReg( Chunk.al_reg, Chunk.al_dat );
            m_numreadysamples = m_samplesPerMusicTick*m_IMFDelay;
        }

        //generate <delay> ticks of audio
        if(m_numreadysamples < sampleslen)
        {
            // Every time a tune has been played call this.
            OPLUpdate( buffer, m_numreadysamples );
            buffer += m_numreadysamples*sample_mult;
            sampleslen -= m_numreadysamples;
            m_IMFDelay = 0;
        }
        else
        {
            // Read the last stuff left in the emulators buffer. At this point the stream buffer is nearly full
            OPLUpdate( buffer, sampleslen );
            m_numreadysamples -= sampleslen;
            break;
        }
    }
}



#if defined(USE_SDLMIXER)
////// Hooks for SDL_Mixer Only ///////

// We still a local to file declared object from the IMFPlayer class.
// That one is only used here!

CIMFPlayer locIMFPlayer;

int locImfMusPos = 0;

bool loadIMFFile(const std::string &fname)
{
    if(locIMFPlayer.loadMusicFromFile(fname))
    {
        locIMFPlayer.close(false);
        locIMFPlayer.open(false);
        Mix_HookMusic(imfMusicPlayer, &locImfMusPos);
        Mix_HookMusicFinished(musicFinished);
        locIMFPlayer.play(true);
        return true;
    }

    return false;
}

bool loadIMFTrack(const int track)
{
    if(locIMFPlayer.loadMusicTrack(track))
    {
        locIMFPlayer.close(false);
        locIMFPlayer.open(false);
        Mix_HookMusic(imfMusicPlayer, &locImfMusPos);
        Mix_HookMusicFinished(musicFinished);
        locIMFPlayer.play(true);
        return true;
    }

    return false;
}


void imfMusicPlayer(void *udata,
                    Uint8 *stream,
                    int len)
{
    int pos = *static_cast<int*>(udata);

    // Fill buffer with music
    locIMFPlayer.readBuffer(stream,
                            static_cast<Uint32>(len));

    // set udata for next time
    pos+=len;
    *static_cast<int*>(udata) = pos;
}

// make a music finished function
void musicFinished()
{
    // Not sure, what should happen here.
    locIMFPlayer.close(false);
}


#endif



