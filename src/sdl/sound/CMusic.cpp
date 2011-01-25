/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "sdl/sound/CSound.h"
#include "sdl/sound/hq_sound.h"
#include "sdl/sound/Sampling.h"
#include "sdl/sound/IMFPlayer.h"
#include "CMusic.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include <fstream>


const Uint32 GalaxySongAssignments[] =
{
	0x02F206,
	0x030679,
	0x03103D,
};


CMusic::CMusic() :
playmode(PLAY_MODE_STOP),
m_MusicFormat(MF_NONE),
usedMusicFile(""),
m_open(false)
{
	m_Audio_cvt.buf = NULL;
}

/**
 * \brief 	This function will load music using other dictionaries which are embedded in the Exe File.
 * 			Only galaxy supports that feature, and the original games will read two files form the EXE-file
 * 			AUDIOHED and AUDIODICT to get the right tune for the music player.
 */
bool CMusic::LoadFromAudioCK(const CExeFile& ExeFile, const int level)
{
	const int episode = ExeFile.getEpisode();
	m_AudioSpec = g_pSound->getAudioSpec();
	m_open = false;
	m_MusicFormat = MF_NONE;

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
			return false;

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
			return false;


		// Find the start of the embedded IMF files
		uint32_t outsize = 0;
		byte *imfdata = NULL;
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
			outsize = *AudioCompFileData32;
			imfdata = new byte[outsize];

			Huffman.expand( (byte*)(AudioCompFileData+audio_comp_data_start), imfdata, audio_end-audio_comp_data_start, outsize);
		}

		if( imfdata )
		{
			readIMFData( imfdata, outsize, m_AudioSpec );
			delete imfdata;

			m_MusicFormat = MF_IMF;
			g_pLogFile->ftextOut("Music Driver(): Music file from %s was read successfully!<br>", init_audiofilename.c_str());
			usedMusicFile = init_audiofilename;
			m_open = true;

			if(	m_Audio_cvt.buf )
				delete [] m_Audio_cvt.buf;

			return true;
		}
		else
			return false;
	}
	return false;
}

bool CMusic::load(const std::string &musicfile)
{
	if(musicfile == "")
		return false;

	m_AudioSpec = g_pSound->getAudioSpec();
	m_open = false;
	m_MusicFormat = MF_NONE;

	if(m_AudioSpec.format != 0)
	{
		std::string extension = GetFileExtension(musicfile);

		if(extension == "imf")
		{
			if(!openIMFFile(musicfile, m_AudioSpec))
			{
				g_pLogFile->textOut(PURPLE,"Music Driver(): IMF file could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());
				return false;
			}

			m_MusicFormat = MF_IMF;

			g_pLogFile->ftextOut("Music Driver(): File \"%s\" opened successfully!<br>", musicfile.c_str());
			usedMusicFile = musicfile;
			m_open = true;

			if(	m_Audio_cvt.buf )
				delete [] m_Audio_cvt.buf;

			return true;
		}
		else if(extension == "ogg")
		{
#if defined(OGG) || defined(TREMOR)
			if(!openOGGStream(musicfile.c_str(), &m_AudioFileSpec, m_oggStream))
			{
				g_pLogFile->textOut(PURPLE,"Music Driver(): OGG file could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());
				return false;
			}

			m_MusicFormat = MF_OGG;
			g_pLogFile->ftextOut("Music Driver(): File \"%s\" opened successfully!<br>", musicfile.c_str());
			usedMusicFile = musicfile;
			m_open = true;
			int ret = SDL_BuildAudioCVT(&m_Audio_cvt,
					m_AudioFileSpec.format, m_AudioFileSpec.channels, m_AudioFileSpec.freq,
					m_AudioSpec.format, m_AudioSpec.channels, m_AudioSpec.freq);
			if(ret == -1)
				return false;

			if(	m_Audio_cvt.buf )
				delete [] m_Audio_cvt.buf;
			const size_t &length = g_pSound->getAudioSpec().size;
			m_Audio_cvt.len = (length*m_Audio_cvt.len_mult)/m_Audio_cvt.len_ratio;
			m_Audio_cvt.buf = new Uint8[m_Audio_cvt.len];

			return true;
#else
			g_pLogFile->ftextOut("Music Driver(): OGG or TREMOR-Support is disabled! Please use another build<br>");
#endif
		}
	}
	else
		g_pLogFile->textOut(PURPLE,"Music Driver(): I would like to open the music for you. But your Soundcard is disabled!!<br>");
	
	return false;
}

void CMusic::reload()
{
	stop();
	load(usedMusicFile);
}

void CMusic::play()
{
	if(usedMusicFile != "")
		playmode = PLAY_MODE_PLAY;
}

void CMusic::pause()
{
	if(usedMusicFile != "")
		playmode = PLAY_MODE_PAUSE;
}


void CMusic::stop(void)
{
	if( m_open)
	{
		if(  m_MusicFormat == MF_IMF )
			SD_Shutdown();

#if defined(OGG) || defined(TREMOR)
		if(  m_MusicFormat == MF_OGG )
			cleanupOGG(m_oggStream);
#endif
	}

	if(	m_Audio_cvt.buf )
		delete [] m_Audio_cvt.buf;

	m_Audio_cvt.buf = NULL;

	playmode = PLAY_MODE_STOP;
	m_open = false;
}

void CMusic::readBuffer(Uint8* buffer, size_t length) // length only refers to the part(buffer) that has to be played
{
	if(playmode != PLAY_MODE_PLAY)
		return;

	if(m_MusicFormat == MF_IMF)
	{
		SDL_IMFMusicPlayer(buffer, length);
	}
	else if(m_MusicFormat == MF_OGG)
	{
#if defined(OGG) || defined(TREMOR)
		bool rewind = false;

		// read the ogg stream
		if( m_AudioSpec.freq == 48000 )
		{
			size_t insize = (m_Audio_cvt.len*441)/480;
			size_t mult = m_AudioFileSpec.channels;

			if(m_AudioFileSpec.format == AUDIO_S16)
				mult <<= 1;

			insize /= mult;
			insize++;
			insize *= mult;

			rewind = readOGGStreamAndResample(m_oggStream, (char*)m_Audio_cvt.buf, m_Audio_cvt.len, insize, m_AudioFileSpec);
		}
		else
		{
			rewind = readOGGStream(m_oggStream, (char*)m_Audio_cvt.buf, m_Audio_cvt.len, m_AudioFileSpec);
		}

		// then convert it into SDL Audio buffer
		// Conversion to SDL Format
		SDL_ConvertAudio(&m_Audio_cvt);

		memcpy(buffer, m_Audio_cvt.buf, length);

		if(rewind)
		{
			reload();
			play();
		}
#else
		return;
#endif
	}

}

bool CMusic::LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename)
{
	bool fileloaded = false;
    std::ifstream Tablefile;

    std::string musicpath = getResourceFilename("music/table.cfg", gamepath, false, true);

    if(musicpath != "")
    	fileloaded = OpenGameFileR(Tablefile, musicpath);
	
    if(fileloaded)
    {
    	std::string str_buf;
    	char c_buf[256];
		
    	while(!Tablefile.eof())
    	{
        	Tablefile.get(c_buf, 256, ' ');
    		while(c_buf[0] == '\n') memmove (c_buf, c_buf+1, 254);
        	str_buf = c_buf;
    		if( str_buf == levelfilename )	// found the level! Load the song!
    		{
    			// Get the song filename and load it!
    			Tablefile.get(c_buf, 256);
    			str_buf = c_buf;
    			TrimSpaces(str_buf);
    			std::string filename = getResourceFilename("music/" + str_buf, gamepath, false, true);
    			if( load(filename) )
    				play();
    			Tablefile.close();
    			return true;
    		}
    		Tablefile.get(c_buf, 256);
    		while(!Tablefile.get() == '\n'); // Skip the '\n' delimiters, so next name will be read.
    	}
    	Tablefile.close();
    }
	return false;
}

void CMusic::unload(void)
{
	usedMusicFile = "";
	playmode = PLAY_MODE_STOP;
	m_Audio_cvt.buf = NULL;
}


CMusic::~CMusic() {

	if(	m_Audio_cvt.buf )
		delete [] m_Audio_cvt.buf;

	unload();
}

