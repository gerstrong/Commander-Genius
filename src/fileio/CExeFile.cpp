/*
 * CExeFile.cpp
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 */

#include "CExeFile.h"
#include "compression/Cunlzexe.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include "fileio/ResourceMgmt.h"
#include "fileio/KeenFiles.h"
#include "fileio/compression/CHuffman.h"

CExeFile::CExeFile() :
m_datasize(0),
m_headersize(0),
m_episode(0),
m_crc(0),
m_headerdata(NULL),
m_rawdata(NULL)
{

	// Setup support map
	m_supportmap[100274][1] = true;
	m_supportmap[100484][1] = true;
	m_supportmap[398][1] = false;

	m_supportmap[118626][2] = true;
	m_supportmap[118672][2] = true;

	m_supportmap[127598][3] = true;
	m_supportmap[127616][3] = true;

	m_supportmap[263488][4] = true;
	m_supportmap[259232][4] = false;
	m_supportmap[258064][4] = false;

	m_supportmap[266096][5] = true;
	m_supportmap[262176][5] = false;

	m_supportmap[236112][6] = false;
	m_supportmap[271696][6] = false;


	// TODO: Setup version map

	// TODO: Setup CRC-Map

}


unsigned long CExeFile::fetchUncompressedHeaderSize(void *m_headerdata)
{
	byte *hdata = reinterpret_cast<byte*>(m_headerdata);
	for( unsigned long c=0 ; c<m_datasize ; c++)
	{
		if( hdata[c] == 0xBA && c % 0x200 == 0 )
			return c;
	}

	gLogging.textOut("The Exe file has an invalid header size!");

	return 0;
}

void CExeFile::dumpFile(const std::string& filename)
{
	std::ofstream ofile( filename.c_str() );
	ofile.write( reinterpret_cast<char*>(m_headerdata), m_datasize );
}

bool CExeFile::readData(const char episode, const std::string& datadirectory)
{
	crc32_init();

	std::string filename = datadirectory + "/keen" + itoa(episode) + ".exe";

	std::ifstream File;
	OpenGameFileR(File, filename, std::ios::binary);

	if(!File)
	{
		// try another filename (Used in Episode 4-6)
		filename = datadirectory + "/keen" + itoa(episode) + "e.exe";
		OpenGameFileR(File, filename, std::ios::binary);
	}

	if(!File)
	{
		// try another filename (Used in Episode 4-6) for demo versions
		filename = datadirectory + "/k" + itoa(episode) + "demo.exe";
		OpenGameFileR(File, filename, std::ios::binary);
	}
	
	if(!File)
		return false;	

	m_filename = filename;
	m_episode = episode;

    std::string localDataDir = datadirectory;
    if( localDataDir != "") if(*(localDataDir.end()-1) != '/') localDataDir += "/";

    CResource &keenFiles = gKeenFiles;
    keenFiles.gameDir = localDataDir;
		
	File.seekg(0,std::ios::end);
	m_datasize = File.tellg();
	File.seekg(0,std::ios::beg);

	unsigned char* m_data_temp = new unsigned char[m_datasize];
	File.read((char*)m_data_temp, m_datasize);

	File.close();

	Cunlzexe UnLZEXE;

	std::vector<unsigned char> decdata;
	if(UnLZEXE.decompress(m_data_temp, decdata))
	{
		m_datasize = decdata.size();
		mData.resize(m_datasize);
		m_headersize = UnLZEXE.HeaderSize();
		memcpy(mData.data(), &decdata[0], m_datasize);
	}
	else
	{
		mData.resize(m_datasize);
		memcpy(mData.data(), m_data_temp,m_datasize);
	}

	m_headerdata = mData.data();
	m_headersize = UnLZEXE.HeaderSize();
	if(!m_headersize)
		m_headersize = fetchUncompressedHeaderSize(m_headerdata);

	m_rawdata = mData.data() + m_headersize;

	const size_t offset_map[] = {
			/*Dummy:*/ 0x0,
			/*Keen 1:*/ 0x13050,
			/*Keen 2:*/ 0x17780,
			/*Keen 3:*/ 0x19820,
			/*Keen 4:*/ 0x2EE70,
			/*Keen 5:*/ 0x30340,
			/*Keen 6:*/ 0x30D30,
			/*Keen D:*/ 0x23A70
	};
	m_data_segment = m_rawdata+offset_map[m_episode];

	delete[] m_data_temp;

	m_crc = getcrc32( mData.data(), m_datasize );

	gLogging.ftextOut( "EXE processed with size of %d and crc of %X\n", m_datasize, m_crc );

	return true;
}

bool CExeFile::Supported()
{

	if( m_supportmap.find(m_datasize) == m_supportmap.end())
		return false;

	if( m_supportmap[m_datasize].find(m_episode) == m_supportmap[m_datasize].end() )
		return false;

	return  m_supportmap[m_datasize][m_episode];
}

int CExeFile::getEXEVersion()
{
    switch (m_datasize)
    {
		case 100274:
			if(m_episode != 1)
				return -1;
			else
				return 110;
		case 100484:
			if(m_episode != 1)
				return -1;
			else
				return 131;
		case 398:
			if(m_episode != 1)
				return -1;
			else
				return 134;

		case 118626:
			if(m_episode != 2)
				return -1;
			else
				return 100;
		case 118672:
			if(m_episode != 2)
				return -1;
			else
				return 131;

		case 127598:
			if(m_episode != 3)
				return -1;
			else
				return 100;
		case 127616:
			if(m_episode != 3)
				return -1;
			else
				return 131;

		// For Keen 4
		case 263488:
			if(m_episode != 4)
				return -1;
			else
				return 140;
		case 259232:
			if(m_episode != 4)
				return -1;
			else
				return 110;
		case 258064:
			if(m_episode != 4)
				return -1;
			else
				return 100;

		// For Keen 5
		case 266096:
			if(m_episode != 5)
				return -1;
			else
				return 140;

		case 262176:
			if(m_episode != 5)
				return -1;
			else
				return 100;

		// For Keen 6
		case 236112:
			if(m_episode != 6)
				return -1;
			else
				return 100;

		case 271696:
			if(m_episode != 6)
				return -1;
			else
				return 140;

		default: return -2;
    }
}

int CExeFile::getEXECrc()
{
    int version = getEXEVersion();

    switch( m_episode )
    {
        case 1:
            switch( version )
		{
			case 110:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x195771AE)
					return -1;
				else
					return true;
			case 134:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			default: return -2;
		}
        case 2:
            switch( version )
		{
			case 100:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x94E464B4)
					return -1;
				else
					return true;
		}
        case 3:
            switch( version )
		{
			case 100:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x70D3264D)
					return -1;
				else
					return 1;
		}
        case 4:
		case 5:
		case 6:
        {
        	// TODO: CRC-Flags for Episode 4 must be implemented here!
        	return 1;
        }
		default: return -2;
    }
}

const unsigned short EXEMZ = 0x5A4D;
const unsigned short EXEZM = 0x4D5A;

/* SM: Modified so we can give a value of "headerlen" we're expecting... this way
 * we might support any exe file in the future */
bool CExeFile::readExeImageSize(unsigned char *p_data_start, unsigned long *imglen, unsigned long *headerlen) const
{
	EXE_HEADER head;

	/* Read the header from the file if we can */
	memcpy(&head, p_data_start,sizeof(EXE_HEADER));
	/* Check that the 'MZ' id is present */
	p_data_start += sizeof(EXE_HEADER);
	if(head.mzid == EXEMZ || head.mzid == EXEZM)
	{
		/* Calculate the image size */
		if (!*headerlen) {
			*imglen = ((unsigned long)head.image_h - 1) * 512L + head.image_l - (unsigned long)head.header_size * 16L;
			*headerlen = (unsigned long)head.header_size * 16L;
		}
		else *imglen = ((unsigned long)head.image_h - 1) * 512L + head.image_l - *headerlen;
		return true;
	}

	// If we got here, something failed
	*imglen = *headerlen = 0;
	return false;
}


bool CExeFile::unpackAudioInterval( RingBuffer<IMFChunkType> &imfData,
                const std::vector<uint8_t> &AudioCompFileData,
                const int audio_start,
                const int audio_end) const
{
    std::string audioDictfilename = getResourceFilename( gKeenFiles.audioDictFilename, gKeenFiles.gameDir, false, false);

    // Open the Huffman dictionary and get AUDIODICT
    CHuffman Huffman;

    if(audioDictfilename.empty())
        Huffman.readDictionaryNumber( *this, 0 );
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


        if(!imfData.empty())
            imfData.clear();

        const word imf_chunks = data_size/sizeof(IMFChunkType);
        imfData.reserve(imf_chunks);
        memcpy(imfData.getStartPtr(), imf_data_ptr, data_size);
        return true;
    }
    else
    {
        return false;
    }
}

bool CExeFile::readMusicHedInternal(RingBuffer<IMFChunkType> &imfData,
                    std::vector<uint32_t> &musiched,
                    const size_t audiofilecompsize) const
{
    uint32_t number_of_audiorecs = 0;

    bool empty = true;

    const uint32_t *starthedptr = reinterpret_cast<uint32_t*>(getHeaderData());
    uint32_t *audiohedptr = const_cast<uint32_t*>(starthedptr);
    for( const uint32_t *endptr = (uint32_t*) (void*) getHeaderData()+getExeDataSize()/sizeof(uint32_t);
            audiohedptr < endptr ;
            audiohedptr++ )
    {
        if(*audiohedptr == audiofilecompsize)
        {
            for( const uint32_t *startptr = (uint32_t*) (void*) getHeaderData() ;
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


    if(getEpisode() == 4)
    {
        memcpy(&music_start, getRawData() + 0x8CEF, sizeof(uint16_t) );
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



bool CExeFile::readMusicHedFromFile(const std::string &fname, std::vector<uint32_t> &musiched) const
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



bool CExeFile::readCompressedAudiointoMemory(RingBuffer<IMFChunkType> &imfData,
                           std::vector<uint32_t> &musiched,
                           std::vector<uint8_t> &AudioCompFileData) const


{
    const int episode = getEpisode();

    /// First get the size of the AUDIO.CK? File.
    uint32_t audiofilecompsize;
    std::string init_audiofilename = "AUDIO.CK" + itoa(episode);

    std::string audiofilename = getResourceFilename( init_audiofilename, gKeenFiles.gameDir, true, false);

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

    std::string audiohedfile = gKeenFiles.audioHedFilename;

    if(!audiohedfile.empty())
        audiohedfile = getResourceFilename( audiohedfile, gKeenFiles.gameDir, false, false);

    // The musiched is just one part of the AUDIOHED. It's not a separate file.
    // Open the AUDIOHED so we know where to mp_IMF_Data decompress
    if(readMusicHedFromFile(audiohedfile, musiched) == false)
    {
        return readMusicHedInternal(imfData, musiched, audiofilecompsize);
    }

    return !musiched.empty();
}



bool CExeFile::loadMusicTrack(RingBuffer<IMFChunkType> &imfData, const int track) const
{
    // Now get the proper music slot reading the assignment table.
    std::vector<uint8_t> AudioCompFileData;
    std::vector<uint32_t> musiched;

    if( readCompressedAudiointoMemory(imfData, musiched, AudioCompFileData) )
    {
        unpackAudioInterval(imfData,
                AudioCompFileData,
                musiched[track],
                musiched[track+1]);
    }

    return true;
}



