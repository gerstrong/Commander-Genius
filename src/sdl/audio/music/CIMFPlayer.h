/*
 * CIMFPlayer.h
 *
 *  Created on: 17.02.2011
 *      Author: gerhard
 */

#ifndef CIMFPLAYER_H_
#define CIMFPLAYER_H_

#include "CMusicPlayer.h"
#include <base/TypeDefinitions.h>
#include "sdl/audio/Audio.h"
#include "CRingBuffer.h"
#include <SDL.h>
#include <string>

#include <SDL_mixer.h>

struct IMFChunkType
{
	byte al_reg;
	byte al_dat;
	word Delay;
};

class CIMFPlayer : public CMusicPlayer
{

public:
    CIMFPlayer( COPLEmulator& opl_emulator);


	/**
	 * \brief 	This function will load music using other dictionaries which are embedded in the Exe File.
	 * 			Only galaxy supports that feature, and the original games will read two files form the EXE-file
	 * 			AUDIOHED and AUDIODICT to get the right tune for the music player.
	 */
    bool loadMusicFromFile(const std::string& filename) override;

	void OPLUpdate(byte *buffer, const unsigned int length);

    bool open(const bool lock) override;
    void close(const bool lock) override;
	void readBuffer(Uint8* buffer, Uint32 length) override;


    bool loadMusicTrack(const int track) override;

private:

		
    bool unpackAudioInterval(const std::string &dataPath,
                const std::vector<uint8_t> &AudioCompFileData,
                const int start,
                const int end);

	RingBuffer<IMFChunkType> m_IMF_Data;
    COPLEmulator &m_opl_emulator;

    Uint32 m_numreadysamples = 0;
    Uint32 m_samplesPerMusicTick =0;
    unsigned int m_IMFDelay = 0;
    std::vector<Sint32> mMixBuffer;

};


// External Music hooks for the imf player


/**
 * @brief loadIMFFile
 * @param fname
 * @return
 */
bool loadIMFFile(const std::string &fname);

/**
 * @brief loadIMFTrack
 * @param track
 * @return
 */
bool loadIMFTrack(const int track);

/**
 * @brief imfMusicPlayer Music play function
 * @param udata   udata to be a pointer to an int
 * @param stream
 * @param len
 */
void imfMusicPlayer(void *udata,
                    Uint8 *stream,
                    int len);

/**
 * @brief musicFinished
 */
void musicFinished();



/**
 * @brief unhookAll
 */
void unhookAll();



#endif /* CIMFPLAYER_H_ */
