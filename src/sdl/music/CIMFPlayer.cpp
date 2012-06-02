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
#include "FindFile.h"
#include "CLogFile.h"
#include <fstream>
#include <string>
#include <cassert>

const Uint32 GalaxySongAssignments[] =
{
	0x02F206,
	0x030679,
	0x03103D,
};

CIMFPlayer::CIMFPlayer( const SDL_AudioSpec& AudioSpec, COPLEmulator& opl_emulator ) :
m_AudioDevSpec(AudioSpec),
m_opl_emulator(opl_emulator),
m_numreadysamples(0),
m_samplesPerMusicTick(m_AudioDevSpec.freq / m_opl_emulator.getIMFClockRate()),
m_IMFDelay(0),
m_mix_buffer(new Sint32[m_AudioDevSpec.samples])
{}


bool CIMFPlayer::loadMusicFromFile(const std::string& filename)
{
    // Open the IMF File
	FILE *fp;
	word data_size;
	bool ok = false;

	if( ( fp = OpenGameFile(filename, "rb") ) == NULL )
    	return false;

	/*read_first =*/ fread( &data_size, sizeof(word), 1, fp);
    if (data_size == 0) // Is the IMF file of Type-0?
    {
        fseek(fp, 0, SEEK_END);
        data_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }


	if(!m_IMF_Data.empty())
		m_IMF_Data.clear();
    const word imf_chunks = (data_size/sizeof(IMFChunkType));
    m_IMF_Data.reserve(imf_chunks);

    if( imf_chunks != fread( m_IMF_Data.getStartPtr(), sizeof(IMFChunkType), imf_chunks, fp ) )
    	g_pLogFile->textOut("The IMF-File seems to be corrupt.");
    else
    	ok = true;

    fclose(fp);

    return ok;
}


bool CIMFPlayer::readCompressedAudiointoMemory(const CExeFile& ExeFile,
									 	 	   uint32_t *&audiohedptr,
									 	 	   uint8_t *&AudioCompFileData)


{
	const int episode = ExeFile.getEpisode();

	if(m_AudioDevSpec.format == 0)
		return false;

	/// First get the size of the AUDIO.CK? File.
	uint32_t audiofilecompsize;
	std::string init_audiofilename = "AUDIO.CK" + itoa(episode);

	std::string audiofilename = getResourceFilename( init_audiofilename, ExeFile.getDataDirectory(), true, false);

	if( audiofilename == "" )
		return false;

	std::ifstream AudioFile;
	OpenGameFileR(AudioFile, audiofilename);

	// Read File Size to know much memory we need to allocate
	AudioFile.seekg( 0, std::ios::end );
	audiofilecompsize = AudioFile.tellg();
	AudioFile.seekg( 0, std::ios::beg );

	// create memory so we can store the Audio.ck there and use it later for extraction
	AudioCompFileData = new uint8_t[audiofilecompsize];
	AudioFile.read((char*)AudioCompFileData, audiofilecompsize);
	AudioFile.close();

	// Open the AUDIOHED so we know where tomp_IMF_Data decompress
	uint32_t number_of_audiorecs = 0;
	// That size must appear as integer in the ExeFile. Look for it!
#if defined(ANDROID)
	assert( reinterpret_cast<size_t>(ExeFile.getHeaderData()) % 4 == 0 ); // Make sure the pointer is aligned, or we'll get segfault on Android
#endif
	// TODO: Not sure, if that is a good idea
	audiohedptr = (uint32_t*) (void*) ExeFile.getHeaderData();
	bool found = false;
	for( const uint32_t *endptr = (uint32_t*) (void*) ExeFile.getHeaderData()+ExeFile.getExeDataSize()/sizeof(uint32_t);
			audiohedptr < endptr ;
			audiohedptr++ )
	{
		if(*audiohedptr == audiofilecompsize)
		{
			for( const uint32_t *startptr = (uint32_t*) (void*) ExeFile.getHeaderData() ;
					audiohedptr > startptr ; audiohedptr-- )
			{
				found = true;
				// Get the number of Audio files we have
				number_of_audiorecs++;
				if(*audiohedptr == 0x0)
					break;
			}
			break;
		}
	}

	if(!found)
		return false;

	// Find the start of the embedded IMF files
	bool gap_detected = false;
	uint32_t music_start = 0;

	for( uint32_t slot = 0 ; slot<number_of_audiorecs ; slot++ )
	{
		const uint32_t audio_start = audiohedptr[slot];
		const uint32_t audio_end = audiohedptr[slot+1];

		if(!gap_detected && audio_start == audio_end)
			gap_detected = true;

		if(gap_detected && audio_start != audio_end)
		{
			music_start = slot+1;
			break;
		}
	}

	audiohedptr += music_start;

	return true;
}

bool CIMFPlayer::unpackAudioAt(	const CExeFile& ExeFile,
								const uint8_t *AudioCompFileData,
								const uint32_t *audiohedptr,
								const Uint32 slot )
{
	const uint32_t audio_start = audiohedptr[slot];
	const uint32_t audio_end = audiohedptr[slot+1];

	// Open the Huffman dictionary and get AUDIODICT
	CHuffman Huffman;
	Huffman.readDictionaryNumber( ExeFile, 0 );

	if( audio_start < audio_end )
	{
#if defined(ANDROID)
		assert( AudioCompFileData + audio_start % 4 == 0 ); // Make sure the pointer is aligned, or we'll get segfault on Android
#endif
		const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);
		const uint32_t *AudioCompFileData32 = (uint32_t*) (void*) (AudioCompFileData + audio_start);
		const uint32_t emb_file_data_size = *AudioCompFileData32;


		byte imf_data[emb_file_data_size];
		byte *imf_data_ptr = imf_data;
		Huffman.expand( (byte*)(AudioCompFileData+audio_comp_data_start), imf_data, audio_end-audio_comp_data_start, emb_file_data_size);

		word data_size;

		if (*imf_data_ptr == 0) // Is the IMF file of Type-0?
			data_size = emb_file_data_size;
		else
		{
			data_size = *((word*) (void*) imf_data_ptr);
			imf_data_ptr+=sizeof(word);
		}


		if(!m_IMF_Data.empty())
			m_IMF_Data.clear();

		const word imf_chunks = data_size/sizeof(IMFChunkType);
		m_IMF_Data.reserve(imf_chunks);
		memcpy(m_IMF_Data.getStartPtr(), imf_data_ptr, data_size);
		return true;
	}
	else
	{
		return false;
	}
}

void CIMFPlayer::freeCompressedAudio(const uint8_t *AudioCompFileData)
{
	delete [] AudioCompFileData;
}

bool CIMFPlayer::loadMusicForLevel(const CExeFile& ExeFile, const int level)
{
	// Now get the proper music slot reading the assignment table.
	Uint16 music_order = 0;
	const int Idx = ExeFile.getEpisode()-4;
	memcpy( &music_order, ExeFile.getRawData()+GalaxySongAssignments[Idx]+level*sizeof(Uint16), sizeof(Uint16));

	return loadMusicTrack(ExeFile, music_order);
}

bool CIMFPlayer::loadMusicTrack(const CExeFile& ExeFile, const int track)
{
	// Now get the proper music slot reading the assignment table.
	uint8_t *AudioCompFileData = NULL;
	uint32_t *audiohedptr = NULL;

	readCompressedAudiointoMemory(ExeFile, audiohedptr, AudioCompFileData);

	unpackAudioAt(ExeFile, AudioCompFileData, audiohedptr, track);

	// TODO: I think we need a better and more stable check here!
	freeCompressedAudio(AudioCompFileData);

	return true;
}


CIMFPlayer::~CIMFPlayer()
{
	if(m_mix_buffer)
		delete [] m_mix_buffer;
}

bool CIMFPlayer::open()
{
	m_numreadysamples = m_IMFDelay = 0;
	m_samplesPerMusicTick = m_AudioDevSpec.freq / m_opl_emulator.getIMFClockRate();

	return !m_IMF_Data.empty();
}

void CIMFPlayer::close()
{
	m_numreadysamples = m_IMFDelay = 0;
	m_opl_emulator.ShutAL();
	m_opl_emulator.shutdown();

	play(false);
	m_IMF_Data.gotoStart();
	return;
}

void CIMFPlayer::OPLUpdate(byte *buffer, const unsigned int length)
{
	m_opl_emulator.Chip__GenerateBlock2( length, m_mix_buffer );

    // Mix into the destination buffer, doubling up into stereo.

	if(m_AudioDevSpec.format == AUDIO_S16)
	{
		Sint16 *buf16 = (Sint16*) (void*) buffer;
		for (unsigned int i=0; i<length; ++i)
		{
			for (unsigned int j=0; j<m_AudioDevSpec.channels; j++)
			{
				*buf16 = m_mix_buffer[i] + m_AudioDevSpec.silence;
				buf16++;
			}
		}
	}
	else if(m_AudioDevSpec.format == AUDIO_U8)
	{
		for (unsigned int i=0; i<length; ++i)
		{
			for (unsigned int j=0; j<m_AudioDevSpec.channels; j++)
			{
				*buffer = (m_mix_buffer[i]>>8) + m_AudioDevSpec.silence;
				buffer++;
			}
		}
	}
}

void CIMFPlayer::readBuffer(Uint8* buffer, Uint32 length)
{
	if(!m_playing)
		return;

	/// if a delay of the instruments is pending, play it
	Uint32 sampleslen = m_AudioDevSpec.samples;
	Uint32 sample_mult = m_AudioDevSpec.channels;
	sample_mult = (m_AudioDevSpec.format == AUDIO_S16) ? sample_mult*sizeof(Sint16) : sample_mult*sizeof(Uint8) ;

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
