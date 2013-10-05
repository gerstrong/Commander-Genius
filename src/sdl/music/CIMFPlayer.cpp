/*
 * CIMFPlayer.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 *
 *  Player which reproduces the IMF Data
 */

#include "engine/galaxy/res/CAudioGalaxy.h"
#include "CIMFPlayer.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include "FindFile.h"
#include "CLogFile.h"
#include <fstream>
#include <string>
#include <cassert>


CIMFPlayer::CIMFPlayer( const SDL_AudioSpec& AudioSpec, COPLEmulator& opl_emulator ) :
m_AudioDevSpec(AudioSpec),
m_opl_emulator(opl_emulator),
m_numreadysamples(0),
m_samplesPerMusicTick(m_AudioDevSpec.freq / m_opl_emulator.getIMFClockRate()),
m_IMFDelay(0),
mMixBuffer(new Sint32[m_AudioDevSpec.samples])
{}


bool CIMFPlayer::loadMusicFromFile(const std::string& filename)
{
    // Open the IMF File
    FILE *fp;
    word data_size;
    bool ok = false;
    
    if( ( fp = OpenGameFile(filename, "rb") ) == NULL )
	return false;
    
    int read_first = fread( &data_size, sizeof(word), 1, fp);
    
    if( read_first == 0)
	return false;

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
					       std::vector<uint32_t> &musiched,
						std::vector<uint8_t> &AudioCompFileData)


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
	AudioCompFileData.resize(audiofilecompsize);
	AudioFile.read((char*) &(AudioCompFileData.front()), audiofilecompsize);
	AudioFile.close();

	std::string audiohedfile = gpResource->audioHedFilename;
	
	if(!audiohedfile.empty())
	    audiohedfile = getResourceFilename( audiohedfile, ExeFile.getDataDirectory(), false, false);
	
	// The musiched is just one part of the AUDIOHED. It's not a separate file.
	// Open the AUDIOHED so we know where to mp_IMF_Data decompress
	if(readMusicHedFromFile(audiohedfile, musiched) == false)
	{	    
	    return readMusicHedInternal(ExeFile, musiched, audiofilecompsize);
	}
		
	return !musiched.empty();
}

bool CIMFPlayer::readMusicHedFromFile(const std::string fname, std::vector<uint32_t> &musiched)
{
    if(fname.empty())
        return false;
    
    std::ifstream file; 
    
    if(!OpenGameFileR(file, fname, std::ios::binary))
	return false;
    
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint32_t> audiohed(length/sizeof(uint32_t));
    
    file.read( reinterpret_cast<char*> (&audiohed.front()), length);
    
    file.close();
    
    size_t music_start = 0;
    
    // Find the start of the embedded IMF files
    for( int slot = audiohed.size()-2 ; slot>=0 ; slot-- )
    {
	const uint32_t audio_start = audiohed[slot];
	const uint32_t audio_end = audiohed[slot+1];

	// Caution: There are cases where audio_start > audio_end. I don't understand why, but in the original games it happens.
	// Those slots are invalid. In mods it doesn't seem to happen!
	// If they are equal, then the music starts there.
	if(audio_start >= audio_end)
	{
	    music_start = slot + 1;
	    break;
	}		
    }
    
    for( size_t i=music_start ; i<audiohed.size() ; i++ )
    {
	musiched.push_back(audiohed[i]);
    }    

    
    return true;
}

bool CIMFPlayer::readMusicHedInternal(const CExeFile& ExeFile,
				    std::vector<uint32_t> &musiched,
				    const size_t audiofilecompsize)
{
    	uint32_t number_of_audiorecs = 0;
	
	bool empty = true;
	
	const uint32_t *starthedptr = reinterpret_cast<uint32_t*>(ExeFile.getHeaderData());
	uint32_t *audiohedptr = const_cast<uint32_t*>(starthedptr);
	for( const uint32_t *endptr = (uint32_t*) (void*) ExeFile.getHeaderData()+ExeFile.getExeDataSize()/sizeof(uint32_t);
			audiohedptr < endptr ;
			audiohedptr++ )
	{
		if(*audiohedptr == audiofilecompsize)
		{
			for( const uint32_t *startptr = (uint32_t*) (void*) ExeFile.getHeaderData() ;
					audiohedptr > startptr ; audiohedptr-- )
			{
				// Get the number of Audio files we have
				number_of_audiorecs++;
				empty = false;
				if(*audiohedptr == 0x0)
					break;
			}
			break;
		}		
	}
	
	
	if(empty)
		return false;

	uint16_t music_start = 0;
	
	
	if(ExeFile.getEpisode() == 4)
	{
	    memcpy(&music_start, ExeFile.getRawData() + 0x8CEF, sizeof(uint16_t) );
	}
	else
	{
	    // Find the start of the embedded IMF files
	    for( int slot = number_of_audiorecs-2 ; slot>=0 ; slot-- )
	    {
		const uint32_t audio_start = audiohedptr[slot];
		const uint32_t audio_end = audiohedptr[slot+1];
		
		// Caution: There are cases where audio_start > audio_end. I don't understand why, but in the original games it happens.
		// Those slots are invalid. In mods it doesn't seem to happen!
		// If they are equal, then the music starts there.
		if(audio_start >= audio_end)
		{
		    music_start = slot + 1;
		    break;
		}
		
	    }
	}
	
	audiohedptr += music_start;
	
	for( size_t i=0 ; i<number_of_audiorecs-music_start ; i++ )
	{
	    musiched.push_back(*audiohedptr);
	    audiohedptr++;
	}
	
	return true;
}

bool CIMFPlayer::unpackAudioInterval(	const CExeFile& ExeFile,
				const std::vector<uint8_t> &AudioCompFileData,
				const int audio_start,
				const int audio_end)
{
	// Open the Huffman dictionary and get AUDIODICT
	CHuffman Huffman;
	
	std::string audioDictfilename = getResourceFilename( gpResource->audioDictFilename, ExeFile.getDataDirectory(), false, false);
	
	if(audioDictfilename.empty())
	    Huffman.readDictionaryNumber( ExeFile, 0 );
	else
	    Huffman.readDictionaryFromFile( audioDictfilename );

	if( audio_start < audio_end )
	{
		const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);

		uint32_t emb_file_data_size;
		memcpy( &emb_file_data_size, &AudioCompFileData[audio_start], sizeof(uint32_t) );

		byte imf_data[emb_file_data_size];
		byte *imf_data_ptr = imf_data;
		Huffman.expand( (byte*)(&AudioCompFileData[audio_comp_data_start]), imf_data, audio_end-audio_comp_data_start, emb_file_data_size);

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

bool CIMFPlayer::loadMusicForLevel(const CExeFile& ExeFile, const int level)
{
	// Now get the proper music slot reading the assignment table.
	Uint16 music_order = 0;
	const int Idx = ExeFile.getEpisode()-4;
	byte *data = ExeFile.getRawData()+GalaxySongAssignments[Idx]+level*sizeof(Uint16);
	memcpy( &music_order, data, sizeof(Uint16));
	
	if(music_order > 20)
	{
	  g_pLogFile->textOut("Sorry, this track is invalid! Please report it the developers.");
	  return false;
	}

	return loadMusicTrack(ExeFile, music_order);
}

bool CIMFPlayer::loadMusicTrack(const CExeFile& ExeFile, const int track)
{
	// Now get the proper music slot reading the assignment table.
	std::vector<uint8_t> AudioCompFileData;
	std::vector<uint32_t> musiched;	
	
	if( readCompressedAudiointoMemory(ExeFile, musiched, AudioCompFileData) )
	{
	    unpackAudioInterval(ExeFile, 
				AudioCompFileData, 
				musiched[track], 
				musiched[track+1]);
	}

	return true;
}


bool CIMFPlayer::open()
{
	m_numreadysamples = m_IMFDelay = 0;
	m_samplesPerMusicTick = m_AudioDevSpec.freq / m_opl_emulator.getIMFClockRate();
	
	m_opl_emulator.setup();
	
	return !m_IMF_Data.empty();
}

void CIMFPlayer::close()
{
	play(false);
	m_IMF_Data.gotoStart();	
	m_numreadysamples = m_IMFDelay = 0;	
	m_opl_emulator.ShutAL();
	m_opl_emulator.shutdown();

	return;
}


#include <fstream>


void CIMFPlayer::OPLUpdate(byte *buffer, const unsigned int length)
{
    if(!mMixBuffer)
    {
        g_pLogFile->textOut("Warning OPL Buffer is empty!");
        return;
    }

    m_opl_emulator.Chip__GenerateBlock2( length, mMixBuffer.get() );

    // Mix into the destination buffer, doubling up into stereo.
	if(m_AudioDevSpec.format == AUDIO_S16)
	{
		Sint16 *buf16 = (Sint16*) (void*) buffer;
		for (unsigned int i=0; i<length; ++i)
		{
		    Sint32 mix = mMixBuffer[i];
		    
		    if(mix > 32767)
			    mix = 32767;
		    else if(mix < -32768)
			    mix = -32768;
		    
			for (unsigned int j=0; j<m_AudioDevSpec.channels; j++)
			{
				*buf16 = mix + m_AudioDevSpec.silence;
				buf16++;
			}
		}
	}
	else if(m_AudioDevSpec.format == AUDIO_U8)
	{
		for (unsigned int i=0; i<length; ++i)
		{
		    Sint32 mix = mMixBuffer[i]>>8;
		    
		    if(mix > 255)
			    mix = 255;
		    else if(mix < 0)
			    mix = 0;
		    
			for (unsigned int j=0; j<m_AudioDevSpec.channels; j++)
			{
				*buffer = mix + m_AudioDevSpec.silence;
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
