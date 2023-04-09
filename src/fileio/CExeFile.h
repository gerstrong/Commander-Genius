/*
 * CExeFile.h
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 *
 *  This special class reads the whole exe-file
 *  into the memory and manages stuff
 *  In later Games, like Keen 4. The Exe Files must be opened many times
 *  for several reason. We are trying to avoid this here, by pushing all data
 *  of the exe-file into our memory and do the operations here!
 */

#ifndef CEXEFILE_H_
#define CEXEFILE_H_

#include <base/TypeDefinitions.h>
#include <base/audio/music/CIMFPlayer.h>

#include <vector>
#include <string>
#include <map>

class CExeFile {
  public:
    CExeFile();
    
    char getEpisode() const
    { return mEpisode;	}
    
    size_t getExeDataSize() const
    { return mDatasize;	}
    
    std::string getFileName()
    { return mFilename; }
    
    unsigned long fetchUncompressedHeaderSize(const void *m_headerdata);
    
    // in case you just want to see the Exe-File which CG sustains in the memory
    void dumpFile(const std::string& filename);
    void dumpDataFile(const std::string& filename);
    
    bool readGenericExeData(const std::string &name,
                            const unsigned int episode,
                            const std::string& datadirectory);

    /**
     * @brief readKeenExeData  Attempt to read the data of the given episode
     * @param episode   Episode for which to read for
     * @param datadirectory path where the data is located
     * @param fileList passing a filelist will only consider
     * @return if everything went well true, otherwise false
     */
    bool readKeenExeData(const unsigned int episode,
                  const std::string& datadirectory);

    /**
     * @brief readMainLuaScript Try to get a main Lua script load
     * @param episode Episode for which to read for
     * @param datadirectory path where the data is located
     * @return if everything went well true, otherwise false
     */
    bool readMainLuaScript(const unsigned int episode,
                              const std::string& datadirectory);

    /**
     * @brief Tells whether The Exe-File is supported by CG or not.
     *        This Information is hard-coded in the CExefile constructor
     */
    bool isSupported();
    int getEXEVersion() const;
    int getEXECrc();
    bool readExeImageSize(unsigned char *p_data_start, unsigned long *imglen, unsigned long *headerlen) const;
    
    bool isDemo() const
    {	return mIsADemo;	}

    gs_byte* getRawData() const
    {	return mRawdata;	}

    size_t getRawDataSize() const
    {	return mRawdataSize;	}

    void* getHeaderData() const
    {	return mHeaderdata;	}

    gs_byte* getDSegPtr() const
    {	return mDataSegmentPtr; }

    bool loadMusicTrack(RingBuffer<IMFChunkType> &imfData, const int track) const;

    bool isLuaScript() const
    {   return mIsLuaScript;   }

    auto getBasename() const
    {  return mBasename; }

private:

    bool readMusicHedFromFile(const std::string &fname,
                  std::vector<uint32_t> &musiched) const;

    bool unpackAudioInterval(RingBuffer<IMFChunkType> &imfData,
                              const std::vector<uint8_t> &AudioCompFileData,
                              const int audio_start,
                              const int audio_end) const;

    bool readMusicHedInternal(RingBuffer<IMFChunkType> &imfData,
                              std::vector<uint32_t> &musiched,
                              const size_t audiofilecompsize) const;


    bool readCompressedAudiointoMemory(RingBuffer<IMFChunkType> &imfData,
                                       std::vector<uint32_t> &musiched,
                                       std::vector<uint8_t> &AudioCompFileData) const;



	struct EXE_HEADER
	{
		unsigned short mzid;
		unsigned short image_l;
		unsigned short image_h;
		unsigned short num_relocs;
		unsigned short header_size;
		unsigned short min_paras;
		unsigned short max_paras;
		unsigned short init_ss;
		unsigned short init_sp;
		unsigned short checksum;
		unsigned short init_ip;
		unsigned short init_cs;
		unsigned short reloc_offset;
		unsigned short overlay_num;
	};

    size_t mDatasize = 0;
    size_t mHeadersize = 0;
    size_t mRawdataSize = 0;
    std::string mBasename;
    int mEpisode = 0;
    bool mIsLuaScript = false;
    bool mIsADemo;
    unsigned int mCrc = 0;
	std::vector<gs_byte> mData;
    void *mHeaderdata = nullptr;
    gs_byte *mRawdata = nullptr;
    gs_byte *mDataSegmentPtr = nullptr;
    std::string mFilename;

    std::map< size_t, std::map<int , bool> > mSupportmap;
};

bool imfMusicTrackloader(RingBuffer<IMFChunkType> &imfData, const int track);

#endif /* CEXEFILE_H_ */
