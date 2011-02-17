/*
 * CIMFPlayer.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerhard
 */

#include "CIMFPlayer.h"
#include "sdl/sound/IMFPlayer.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include "FindFile.h"
#include <fstream>
#include <string>

const Uint32 GalaxySongAssignments[] =
{
	0x02F206,
	0x030679,
	0x03103D,
};

CIMFPlayer::CIMFPlayer(const std::string& filename, const SDL_AudioSpec& AudioSpec) :
mp_imfdata(NULL),
m_data_size(0),
m_AudioSpec(AudioSpec)
{
    // Load the IMF File here!
	FILE *fp;

	if( ( fp = OpenGameFile(filename, "rb") ) == NULL )
    	return;

    // Read the whole binary file into the memory
    fseek(fp, 0, SEEK_END);
    m_data_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    mp_imfdata = new byte[m_data_size];

    if( m_data_size == fread( mp_imfdata, sizeof(byte), m_data_size, fp ) )
    {
    	fclose(fp);
    	OPL_Startup(AudioSpec);
    }
    else
    	fclose(fp);
}

/**
 * \brief 	This function will load music using other dictionaries which are embedded in the Exe File.
 * 			Only galaxy supports that feature, and the original games will read two files form the EXE-file
 * 			AUDIOHED and AUDIODICT to get the right tune for the music player.
 */
CIMFPlayer::CIMFPlayer(const CExeFile& ExeFile, const int level, const SDL_AudioSpec &AudioSpec) :
mp_imfdata(NULL),
m_data_size(0),
m_AudioSpec(AudioSpec)
{
	const int episode = ExeFile.getEpisode();

	if(m_AudioSpec.format != 0)
	{
		// Open the Huffman dictionary and get AUDIODICT
		CHuffman Huffman;
		Huffman.readDictionaryNumber( ExeFile, 0 );

		/// First get the size of the AUDIO.CK? File.
		uint32_t audiofilecompsize;
		std::string init_audiofilename = "AUDIO.CK" + itoa(episode);

		std::string audiofilename = getResourceFilename( init_audiofilename, ExeFile.getDataDirectory(), true, false);

		if( audiofilename == "" )
			return;

		std::ifstream AudioFile;
		OpenGameFileR(AudioFile, audiofilename);

		// Read File Size and allocate memory so we can read it
		AudioFile.seekg( 0, std::ios::end );
		audiofilecompsize = AudioFile.tellg();
		AudioFile.seekg( 0, std::ios::beg );

		// create memory so we can store the Audio.ck there and use it later for extraction
		uint8_t *AudioCompFileData = new uint8_t[audiofilecompsize];
		AudioFile.read((char*)AudioCompFileData, audiofilecompsize);
		AudioFile.close();

		// Open the AUDIOHED so we know where to decompress
		uint32_t number_of_audiorecs = 0;
		// That size must appear as integer in the ExeFile. Look for it!
		uint32_t *audiohedptr = (uint32_t*) ExeFile.getHeaderData();
		bool found = false;
		for( const uint32_t *endptr = (uint32_t*) ExeFile.getHeaderData()+ExeFile.getExeDataSize()/sizeof(uint32_t);
				audiohedptr < endptr ;
				audiohedptr++ )
		{
			if(*audiohedptr == audiofilecompsize)
			{
				for( const uint32_t *startptr = (uint32_t*) ExeFile.getHeaderData() ;
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
			return ;

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

		// Now get the proper music slot reading the assignment table.
		Uint16 music_order = 0;

		memcpy( &music_order, ExeFile.getRawData()+GalaxySongAssignments[episode-4]+level*sizeof(Uint16), sizeof(Uint16));

		/// Now we have all the data we need.
		// decompress every file of AUDIO.CK? using huffman decompression algorithm
		uint32_t slot = music_start+music_order;
		const uint32_t audio_start = audiohedptr[slot];
		const uint32_t audio_end = audiohedptr[slot+1];

		if( audio_start < audio_end )
		{
			const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);
			const uint32_t *AudioCompFileData32 = (uint32_t*) (AudioCompFileData + audio_start);
			m_data_size = *AudioCompFileData32;
			mp_imfdata = new byte[m_data_size];

			Huffman.expand( (byte*)(AudioCompFileData+audio_comp_data_start), mp_imfdata, audio_end-audio_comp_data_start, m_data_size);
		}
	}
}

CIMFPlayer::~CIMFPlayer() {
	close();
}

bool CIMFPlayer::open()
{
	if(!m_data_size)
		return false;

	OPL_Startup (m_AudioSpec);
	return readIMFData( mp_imfdata, m_data_size, m_AudioSpec );
}

void CIMFPlayer::readBuffer(Uint8* buffer, Uint32 length)
{
	if(!m_playing)
		return;

	PlayIMF(buffer, length);
}

void CIMFPlayer::close()
{
	if(mp_imfdata)
		delete [] mp_imfdata;
	mp_imfdata = NULL;
	m_data_size = 0;
	m_playing = false;
	OPL_Shutdown();
}
