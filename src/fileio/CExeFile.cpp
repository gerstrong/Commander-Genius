/*
 * CExeFile.cpp
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 */

#include "fileio/ResourceMgmt.h"
#include "fileio/crc.h"

#include "CExeFile.h"
#include "compression/Cunlzexe.h"
#include "compression/Cunlzexe.h"
#include "engine/depklite/depklite.h"
#include "fileio/KeenFiles.h"
#include "fileio/compression/CHuffman.h"


#include <base/utils/FindFile.h>
#include <base/GsLogging.h>

#include <cstring>
#include <iostream>
#include <fstream>


CExeFile::CExeFile()
{

	// Setup support map
    mSupportmap[100274][1] = true;
    mSupportmap[100484][1] = true;
    mSupportmap[49684][1] = false;

    mSupportmap[118626][2] = true;
    mSupportmap[118672][2] = true;

    mSupportmap[127598][3] = true;
    mSupportmap[127616][3] = true;

    mSupportmap[263488][4] = true;
    mSupportmap[259232][4] = false;
    mSupportmap[258064][4] = false;

    mSupportmap[266096][5] = true;
    mSupportmap[262176][5] = false;

    mSupportmap[236112][6] = false;
    mSupportmap[271696][6] = false;

    // Dreams
    mSupportmap[213536][7] = true;

	// TODO: Setup CRC-Map

}


unsigned long CExeFile::fetchUncompressedHeaderSize(const void *m_headerdata)
{
    const gs_byte *hdata = reinterpret_cast<const gs_byte*>(m_headerdata);
    for( unsigned long c=0 ; c<mDatasize ; c++)
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
    ofile.write( reinterpret_cast<char*>(mHeaderdata), mDatasize );
}

void CExeFile::dumpDataFile(const std::string& filename)
{
	std::ofstream ofile( filename.c_str() );
    mRawdataSize = mDatasize - mHeadersize;
    ofile.write( reinterpret_cast<char*>(mRawdata), mRawdataSize);
}


bool CExeFile::readGenericExeData(const std::string &name,
                                  const unsigned int episode,
                                  const std::string& datadirectory)
{
    const std::string filename =  JoinPaths(datadirectory,
                                      name + itoa(episode) + ".exe" );

    std::ifstream File;
    if(!OpenGameFileR(File, filename, std::ios::binary))
        return false;


    // If we still have no file found, the directory with the game cannot be used at all.
    if(!File)
    {
        return false;
    }

    mBasename = name;
    mFilename = filename;
    mEpisode = episode;
    mIsADemo = false;

    std::string localDataDir = datadirectory;
    if( localDataDir != "")
    {
        if(*(localDataDir.end()-1) != '/')
            localDataDir += "/";
    }

    auto &keenFiles = gKeenFiles;
    keenFiles.gameDir = localDataDir;

    File.seekg(0,std::ios::end);
    mDatasize = File.tellg();
    File.seekg(0,std::ios::beg);

    // Read all the file into the memory
    std::vector<unsigned char> dataTemp(mDatasize);
    File.read((char*)dataTemp.data(), mDatasize);
    File.close();

    Cunlzexe UnLZEXE;

    std::vector<unsigned char> decdata;
    if(UnLZEXE.decompress(dataTemp.data(), decdata))
    {
        mDatasize = decdata.size();
        mData.resize(mDatasize);
        mHeadersize = UnLZEXE.HeaderSize();
        memcpy(mData.data(), &decdata[0], mDatasize);
    }
    else
    {
        mData.resize(mDatasize);
        memcpy(mData.data(), dataTemp.data(),mDatasize);
    }

    mHeaderdata = mData.data();
    mHeadersize = UnLZEXE.HeaderSize();
    if(!mHeadersize)
    {
        mHeadersize = fetchUncompressedHeaderSize(mHeaderdata);
    }

    mRawdata = mData.data() + mHeadersize;
    mCrc = getcrc32( mData.data(), mDatasize );

    gLogging.ftextOut( "EXE processed with size of %d and crc of %X\n", mDatasize, mCrc );

    return true;
}


bool CExeFile::readKeenExeData(const unsigned int episode,
                        const std::string& datadirectory)
{
	bool demo = false;

    std::string filename =  JoinPaths(datadirectory,
                                      "keen" + itoa(episode) + ".exe" );

	std::ifstream File;
	OpenGameFileR(File, filename, std::ios::binary);

    // Galaxy Keen games use the "e" letter for whatever reason
	if(!File)
	{
		// try another filename (Used in Episode 4-6)
        filename = JoinPaths(datadirectory,
                             "keen" + itoa(episode) + "e.exe");
		OpenGameFileR(File, filename, std::ios::binary);
	}

    // Demo version, I think we support none yet have the suffix "demo" at the end
    if(!File)
    {
        // try another filename (Used in Episode 4-6) for demo versions
        filename = JoinPaths(datadirectory,
                             "k" + itoa(episode) + "demo.exe");
        OpenGameFileR(File, filename, std::ios::binary);
        if(File)
            demo = true;
    }

    // Keen Dreams section. It is called "KDREAMS.EXE" for what I know. Remember, case sensitity is
    // solved by OpenGameFileR.
    if(!File)
    {
        // try another filename (Used in Episode 4-6) for demo versions
        filename = JoinPaths(datadirectory, "kdreams.exe");
        OpenGameFileR(File, filename, std::ios::binary);

        // This is only for Keen Dreams so it has to be 7!
        if(episode != 7)
        {
            return false;
        }
    }

    // If we still have no file found, the directory with the game cannot be used at all.
	if(!File)
    {
		return false;	
    }

    const auto fullfilePath = GetFullFileName(filename);
    mFilename = filename;
    mEpisode = episode;
    mIsADemo = demo;

    std::string localDataDir = datadirectory;
    if( localDataDir != "")
    {
        if(*(localDataDir.end()-1) != '/')
            localDataDir += "/";
    }

    auto &keenFiles = gKeenFiles;
    keenFiles.gameDir = localDataDir;
		
	File.seekg(0,std::ios::end);
    mDatasize = File.tellg();
	File.seekg(0,std::ios::beg);

    // Read all the file into the memory
    std::vector<unsigned char> dataTemp(mDatasize);
    File.read((char*)dataTemp.data(), mDatasize);
	File.close();

	Cunlzexe UnLZEXE;

	std::vector<unsigned char> decdata;
    if(UnLZEXE.decompress(dataTemp.data(), decdata))
	{
        mDatasize = decdata.size();
        mData.resize(mDatasize);
        mHeadersize = UnLZEXE.HeaderSize();
        memcpy(mData.data(), &decdata[0], mDatasize);
	}
    else if(depklite_unpack(dataTemp, decdata))
    {
        mDatasize = decdata.size();
        mData.resize(mDatasize);
        memcpy(mData.data(), &decdata[0], mDatasize);
	}
    else
    {
        mData.resize(mDatasize);
        memcpy(mData.data(), dataTemp.data(),mDatasize);
    }

    mHeaderdata = mData.data();
    mHeadersize = UnLZEXE.HeaderSize();

    if(!mHeadersize)
    {
        mHeadersize = fetchUncompressedHeaderSize(mHeaderdata);
    }

    mRawdata = mData.data() + mHeadersize;
    mRawdataSize = mData.size() - mHeadersize;

	const size_t offset_map[] = {
			/*Dummy:*/ 0x0,
			/*Keen 1:*/ 0x13050,
			/*Keen 2:*/ 0x17780,
			/*Keen 3:*/ 0x19820,
			/*Keen 4:*/ 0x2EE70,
			/*Keen 5:*/ 0x30340,
			/*Keen 6:*/ 0x30D30,
            /*Keen 7:*/ 0x23A70,  // Keen Dreams
			/*Keen 6:*/ 0x2A4F0   // Demo
	};

    size_t offsetIdx = (demo && episode == 6) ? 8 : episode;

    mDataSegment = mRawdata + offset_map[offsetIdx];

    mCrc = getcrc32( mData.data(), mDatasize );

    gLogging.ftextOut( "EXE processed with size of %d and crc of %X\n", mDatasize, mCrc );

	return true;
}


bool CExeFile::readMainLuaScript(const unsigned int episode,
                                 const std::string& datadirectory)
{
    std::string filename = datadirectory + "/keen" + itoa(episode) + ".lua";

    std::ifstream File;
    OpenGameFileR(File, filename, std::ios::binary);

    // If we still have no file found, the directory with the game cannot be used at all.
    if(!File)
        return false;

    mFilename = filename;
    mEpisode = episode;

    std::string localDataDir = datadirectory;
    if( localDataDir != "")
    {
        if(*(localDataDir.end()-1) != '/')
            localDataDir += "/";
    }

    auto &keenFiles = gKeenFiles;
    keenFiles.gameDir = localDataDir;

    gLogging << "Lua script \"" << filename << "\" detected!";

    mIsLuaScript = true;

    return true;
}

bool CExeFile::isSupported()
{
    if(mIsLuaScript) return true;

    if( mSupportmap.find(mDatasize) ==
            mSupportmap.end())
    {
        return false;
    }

    if( mSupportmap[mDatasize].find(mEpisode) ==
            mSupportmap[mDatasize].end() )
    {
		return false;
    }

    return  mSupportmap[mDatasize][mEpisode];
}

int CExeFile::getEXEVersion() const
{
    switch (mDatasize)
    {
    case 100274:
        return (mEpisode == 1) ? 110 : -1;
    case 100484:
        return (mEpisode == 1) ? 131 : -1;
    case 100132:
        return (mEpisode == 1) ? 134 : -1;

    case 118626:
        if(mEpisode != 2)
            return -1;
        else
            return 100;
    case 118672:
        if(mEpisode != 2)
            return -1;
        else
            return 131;

    case 127598:
        if(mEpisode != 3)
            return -1;
        else
            return 100;
    case 127616:
        if(mEpisode != 3)
            return -1;
        else
            return 131;

        // For Keen 4
    case 263488:
        if(mEpisode != 4)
            return -1;
        else
            return 140;
    case 259232:
        if(mEpisode != 4)
            return -1;
        else
            return 110;
    case 258064:
        if(mEpisode != 4)
            return -1;
        else
            return 100;

        // For Keen 5
    case 266096:
        if(mEpisode != 5)
            return -1;
        else
            return 140;

    case 262176:
        if(mEpisode != 5)
            return -1;
        else
            return 100;

        // For Keen 6
    case 236112:
        if(mEpisode != 6)
            return -1;
        else
            return 100;

    case 271696:
        if(mEpisode != 6)
            return -1;
        else
            return 140;

        // Keen Dreams
    case 213536:
        if(mEpisode != 7)
            return -1;
        else
            return 100; // TODO: Not sure, if it really is version 1.00. Check!

    default: return -2;
    }
}

int CExeFile::getEXECrc()
{
    int version = getEXEVersion();

    switch( mEpisode )
    {
    case 1:
        switch( version )
        {
        case 110:
            if(mCrc != 1)
                return -1;
            else
                return 1;
        case 131:
            if(mCrc != 0x195771AE)
                return -1;
            else
                return true;
        case 134:
            if(mCrc != 0xB6D35EB8)
                return -1;
            else
                return 1;
        default: return -2;
        }
    case 2:
        switch( version )
        {
        case 100:
            if(mCrc != 1)
                return -1;
            else
                return 1;
        case 131:
            if(mCrc != 0x94E464B4)
                return -1;
            else
                return true;
        }
    case 3:
        switch( version )
        {
        case 100:
            if(mCrc != 1)
                return -1;
            else
                return 1;
        case 131:
            if(mCrc != 0x70D3264D)
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
    {
        Huffman.readDictionaryNumber( *this, 0, 0 );
    }
    else
    {
        Huffman.readDictionaryFromFile( audioDictfilename );
    }

    if( audio_start < audio_end )
    {
        const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);

        uint32_t emb_file_data_size;
        memcpy( &emb_file_data_size, &AudioCompFileData[audio_start], sizeof(uint32_t) );

        gs_byte imfDataPrim[emb_file_data_size];
        gs_byte *imfDataPtr = imfDataPrim;


        const unsigned long insize = audio_end-audio_comp_data_start;

        // In some mods no compression is performed, so just copy the bytes
        // into the other array
        if(emb_file_data_size == insize)
        {
            memcpy(imfDataPrim,
                    (gs_byte*)(&AudioCompFileData[audio_comp_data_start]),
                    insize);
        }
        else
        {
            Huffman.expand( (gs_byte*)(&AudioCompFileData[audio_comp_data_start]),
                            imfDataPrim,
                            insize,
                            emb_file_data_size);
        }

        word data_size;

        if (*imfDataPtr == 0) // Is the IMF file of Type-0?
        {
            data_size = emb_file_data_size;
        }
        else
        {
            data_size = *((word*) (void*) imfDataPtr);
            imfDataPtr += sizeof(word);
        }


        if(!imfData.empty())
        {
            imfData.clear();
        }

        const word imf_chunks = data_size/sizeof(IMFChunkType);
        imfData.resize(imf_chunks);
        memcpy(imfData.getStartPtr(), imfDataPtr, data_size);
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
    for( const uint32_t *endptr = starthedptr+getExeDataSize()/sizeof(uint32_t);
         audiohedptr < endptr ;
         audiohedptr++ )
    {
        if(*audiohedptr == audiofilecompsize)
        {
            for( const uint32_t *startptr = starthedptr ;
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
        const uint32_t record = *audiohedptr;
        musiched.push_back(record);
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

    const std::string audiofilename = getResourceFilename( init_audiofilename,
                                                     gKeenFiles.gameDir,
                                                     true, false);

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
    AudioFile.read((char*) &(AudioCompFileData.front()),
                    audiofilecompsize);
    AudioFile.close();

    std::string audiohedfile = gKeenFiles.audioHedFilename;

    if(!audiohedfile.empty())
    {
        audiohedfile = getResourceFilename( audiohedfile, gKeenFiles.gameDir, false, false);
    }

    // The musiched is just one part of the AUDIOHED. It's not a separate file.
    // Open the AUDIOHED so we know where to mp_IMF_Data decompress
    if(readMusicHedFromFile(audiohedfile, musiched) == false)
    {
        return readMusicHedInternal(imfData, musiched, audiofilecompsize);
    }

    return !musiched.empty();
}



bool CExeFile::loadMusicTrack(RingBuffer<IMFChunkType> &imfData,
                              const int track) const
{
    // Now get the proper music slot reading the assignment table.
    std::vector<uint8_t> AudioCompFileData;
    std::vector<uint32_t> musiched;

    assert(track >= 0);

    if( readCompressedAudiointoMemory(imfData, musiched, AudioCompFileData) )
    {
        unpackAudioInterval(imfData,
                AudioCompFileData,
                musiched[track],
                musiched[track+1]);
    }

    return true;
}

bool imfMusicTrackloader(RingBuffer<IMFChunkType> &imfData, const int track)
{
    return gKeenFiles.exeFile.loadMusicTrack(imfData, track);
}


