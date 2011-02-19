/*
 * COPLEmulator.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 *
 *  This class processes the data of the ISF and IMF File Formats.
 *  The real emulator of the OPL Hardware is located in dpopl.cpp. Nevertheless
 *  this class interprets the data and communicates with the OPL emulator.
 *  It also takes care of the startup and shutdown of that emu.
 */

#include "COPLEmulator.h"

const int IMF_CLOCK_RATE = 560;

COPLEmulator::COPLEmulator(const SDL_AudioSpec &AudioSpec) :
m_AudioDevSpec(AudioSpec),
m_numreadysamples(0),
m_soundTimeCounter(5),
m_mix_buffer(NULL)
{}

COPLEmulator::~COPLEmulator() {
	shutdown();
}

void COPLEmulator::init()
{
    m_samplesPerMusicTick = m_AudioDevSpec.freq / IMF_CLOCK_RATE;    // SDL_t0FastAsmService played at imf_clock_rate Hz
    m_mix_buffer = new Bit32s[m_samplesPerMusicTick];

    DBOPL_InitTables();
    Chip__Chip(&m_opl_chip);
    Chip__Setup(&m_opl_chip, m_AudioDevSpec.freq);
}

template<typename T>
void COPLEmulator::OPLUpdate(T *buffer, unsigned int length)
{
	Chip__GenerateBlock2(&m_opl_chip, length, m_mix_buffer );

    // Mix into the destination buffer, doubling up into stereo.

    for (unsigned int i=0; i<length; ++i)
    {
    	int pulse = (Sint16) m_mix_buffer[i];

    	if(sizeof(T) != sizeof(Sint16))
    		pulse = (pulse<<(sizeof(T)<<3))>>(sizeof(Sint16)<<3);

    	pulse += m_AudioDevSpec.silence;

    	for (unsigned int j=0; j<m_AudioDevSpec.channels; j++)
    		buffer[i * m_AudioDevSpec.channels + j] = pulse;
    }
}

longword COPLEmulator::readBufferFromIMF(Uint8* buffer, const longword wavesize, const byte *imfdata)
{
	if(m_AudioDevSpec.format == AUDIO_S16)
		return readBufferFromIMFTemplatized((Sint16*) buffer, wavesize, imfdata);
	else if(m_AudioDevSpec.format == AUDIO_U8)
		return readBufferFromIMFTemplatized((Uint8*) buffer, wavesize, imfdata);
	else if(m_AudioDevSpec.format == AUDIO_S8)
		return readBufferFromIMFTemplatized((Sint8*) buffer, wavesize, imfdata);
	else if(m_AudioDevSpec.format == AUDIO_U16)
		return readBufferFromIMFTemplatized((Uint16*) buffer, wavesize, imfdata);
	return 0;
}

template<typename T>
longword COPLEmulator::readBufferFromIMFTemplatized(T *stream, const longword wavesize, const byte *imfdata)
{
	longword imfdatawritten = 0;
	IMFChunkType *imf_Chunk_ptr = (IMFChunkType*) imfdata;

	/// if a delay of the instruments is pending, play it
    Uint32 sampleslen = wavesize/(m_AudioDevSpec.channels*sizeof(T));

    // while the waveform is not filled
    while(1)
    {
        if(m_numreadysamples)
        {
            if(m_numreadysamples < sampleslen)
            {
            	// Ever time a tune has been played call this.
            	OPLUpdate(stream, m_numreadysamples);
                stream += m_numreadysamples*m_AudioDevSpec.channels;
                sampleslen -= m_numreadysamples;
            }
            else
            {
            	// Read the last stuff left in the emulators buffer. At this point the stream in full
            	OPLUpdate(stream, sampleslen);
            	m_numreadysamples -= sampleslen;
                break;
            }
        }

        // This cycle takes the loaded data and writes the value to the Emulator
        // It's waveform is read back later...
        //if(m_Delay>0)
        	//m_Delay--;

        // If m_Delay reached zero get the next tone pass it to the emulator and set a new delay
        if(m_Delay == 0)
        {
        	// read the next instrument play to to the OPL Emulator and put a new delay
        	Chip__WriteReg(&m_opl_chip, imf_Chunk_ptr->al_reg, imf_Chunk_ptr->al_dat );
        	m_Delay = imf_Chunk_ptr->Delay;
        	imf_Chunk_ptr++;
        	imfdatawritten += sizeof(IMFChunkType);
        }
        else
        	m_Delay--;

        m_numreadysamples = m_samplesPerMusicTick;
    }

    return imfdatawritten;
}


void COPLEmulator::shutdown()
{
	if(m_mix_buffer)
		delete [] m_mix_buffer;
}
