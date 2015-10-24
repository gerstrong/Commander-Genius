#include "audiodreams.h"

#include "fileio/KeenFiles.h"
#include <base/GsLogging.h>

const uint NUMSNDCHUNKS	= 84;

extern "C"
{
extern	uint8_t	*audiohead;
extern FILE*			audiohandle;	// handle to AUDIOT / AUDIO

extern	uint8_t *audiosegs[NUMSNDCHUNKS];

extern void MM_GetPtr (memptr *baseptr, unsigned long size);

extern bool CA_FarRead(FILE* handle, uint8_t *dest, int32_t length);

}




bool AudioDreams::readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot, const byte *pcsdata, const Uint8 formatsize)
{
    byte *pcsdata_ptr = (byte*)pcsdata;
    const longword size = READLONGWORD(pcsdata_ptr);
    soundslot.priority = READWORD(pcsdata_ptr);

    const SDL_AudioSpec &audioSpec = g_pSound->getAudioSpec();

    std::vector<Sint16> waveform;
    // TODO:  There should be a better way of determining if sound is signed or not...
    int AMP;
    if ((audioSpec.format == AUDIO_S8) || (audioSpec.format == AUDIO_S16))
        AMP = ((((1<<(formatsize*8))>>2)-1)*PC_Speaker_Volume)/100;
    else
        AMP = ((((1<<(formatsize*8))>>1)-1)*PC_Speaker_Volume)/100;

    generateWave(waveform, pcsdata_ptr, size, false, AMP, audioSpec);

    if(formatsize == 1)
    {
        std::vector<Uint8> wave8;
        std::vector<Sint16>::iterator it = waveform.begin();
        for( ; it != waveform.end(); it++ )
            wave8.push_back(*it);
        soundslot.setupWaveForm((Uint8*)&wave8[0], wave8.size()*sizeof(Uint8));
    }
    else
        soundslot.setupWaveForm((Uint8*)&waveform[0], waveform.size()*sizeof(Sint16));


    return true;
}



void AudioDreams::CacheAudioChunk(int32_t chunk)
{
    //id0_long_t	pos,compressed,expanded;
    //memptr	bigbufferseg;
    //id0_byte_t	id0_far *source;

    /*if (audiosegs[chunk])
    {
        MM_SetPurge ((memptr *)&audiosegs[chunk],0);
        return;							// allready in memory
    }*/

    //OptimizeNodes (audiodict);
    int32_t *audiostarts = (int32_t*)audiohead;

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
    uint8_t pos = audiostarts[chunk];
    uint8_t compressed = audiostarts[chunk+1]-pos;

    fseek(audiohandle,pos,SEEK_SET);

#ifndef AUDIOHEADERLINKED

    MM_GetPtr ((memptr *)&audiosegs[chunk], compressed);
    CA_FarRead(audiohandle, audiosegs[chunk], compressed);

#else

    if (compressed<=BUFFERSIZE)
    {
        CA_FarRead(audiohandle,(id0_byte_t *)bufferseg,compressed);
        source = (id0_byte_t *)bufferseg;
    }
    else
    {
        MM_GetPtr(&bigbufferseg,compressed);
        CA_FarRead(audiohandle,(id0_byte_t *)bigbufferseg,compressed);
        source = (id0_byte_t *)bigbufferseg;
    }

    // REFKEEN - Big Endian support
    expanded = BE_Cross_Swap32LE(*(id0_long_t id0_far *)source);
    //expanded = *(id0_long_t id0_far *)source;
    source += 4;			// skip over length
    MM_GetPtr ((memptr *)&audiosegs[chunk],expanded);
    CAL_HuffExpand (source,audiosegs[chunk],expanded,audiohuffman);

    if (compressed>BUFFERSIZE)
        MM_FreePtr(&bigbufferseg);
#endif
    // REFKEEN - Big Endian support, possibly the most complicated case,
    // since we need to know the exact type of each chunk.
    //
    // FIXME (consider): With matching proper data and definitions, this
    // should be OK, but otherwise one could consider delaying the swaps
    // to runtime (although it's a stretch).

    // Sanity check before doing the swaps on BE
#if !((STARTPCSOUNDS <= STARTADLIBSOUNDS) && (STARTADLIBSOUNDS <= STARTDIGISOUNDS) && (STARTDIGISOUNDS <= STARTMUSIC))
#error "ID_CA.C sanity check for Big-Endian byte swaps (audio chunks) has failed!"
#endif

#ifdef REFKEEN_ARCH_BIG_ENDIAN
    if (chunk < STARTMUSIC) // Sound effects
    {
        SoundCommon *sndCommonPtr = (SoundCommon *)audiosegs[chunk];
        sndCommonPtr->length = BE_Cross_Swap32LE(sndCommonPtr->length);
        sndCommonPtr->priority = BE_Cross_Swap16LE(sndCommonPtr->priority);
        if ((chunk >= STARTDIGISOUNDS) /*&& (chunk < STARTMUSIC)*/) // Digitized sounds
        {
            SampledSound *sampledSndPtr = (SampledSound *)audiosegs[chunk];
            sampledSndPtr->hertz = BE_Cross_Swap16LE(sampledSndPtr->hertz);
        }
    }
    else // Music chunk
    {
        // NOTE: Doing nothing since we don't even know how was
        // this used; Original code may be a bit incomplete anyway.
    }
#endif


/*
//
// Base offsets
//
#define STARTPCSOUNDS		0
#define STARTADLIBSOUNDS	28
#define STARTDIGISOUNDS		56
#define STARTMUSIC		84
*/

    auto &audioSpec = g_pSound->getAudioSpec();

    if(chunk < 28)
    {
        readPCSpeakerSoundintoWaveForm( m_soundslot[chunk], (const byte *)audiosegs[chunk], (audioSpec.format == AUDIO_S16) ? 2 : 1 );
    }
    /*else
    {
        readISFintoWaveForm( m_soundslot[snd], imfdata, outsize, (audioSpec.format == AUDIO_S16) ? 2 : 1 );
    }*/
}


bool AudioDreams::LoadAudioFiles()
{
    //CResource &keenFiles = gKeenFiles;
    //CExeFile &exeFile = keenFiles.exeFile;

    m_soundslot.assign(NUMSNDCHUNKS, CSoundSlot());

    for (int i=0 ; i<NUMSNDCHUNKS ; i++)
    {
        CacheAudioChunk(i);
    }

    return true;
}

bool AudioDreams::loadSoundData()
{
    COPLEmulator &OPLEmulator = g_pSound->getOPLEmulatorRef();

    OPLEmulator.shutdown();
    OPLEmulator.init();

    const bool ok = LoadAudioFiles();

    if(!ok)
    {
        gLogging.textOut("AudioDreams::loadSoundData(): The function cannot read audio of that game");
        return false;
    }

    return true;
}
