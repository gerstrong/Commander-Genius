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

struct IMFChunkType
{
	byte al_reg;
	byte al_dat;
	word Delay;
};

class CIMFPlayer : public CMusicPlayer
{
public:
    CIMFPlayer( COPLEmulator& opl_emulator = gSound.getOPLEmulatorRef());


	/**
	 * \brief 	This function will load music using other dictionaries which are embedded in the Exe File.
	 * 			Only galaxy supports that feature, and the original games will read two files form the EXE-file
	 * 			AUDIOHED and AUDIODICT to get the right tune for the music player.
	 */
    bool loadMusicFromFile(const std::string& filename);

	void OPLUpdate(byte *buffer, const unsigned int length);

    bool open(const bool lock);
    void close(const bool lock);
	void readBuffer(Uint8* buffer, Uint32 length);


    bool loadMusicTrack(const int track);

private:

		
    bool unpackAudioInterval(const std::string &dataPath,
                const std::vector<uint8_t> &AudioCompFileData,
                const int start,
                const int end);

	RingBuffer<IMFChunkType> m_IMF_Data;
    COPLEmulator &m_opl_emulator;

	Uint32 m_numreadysamples;
	Uint32 m_samplesPerMusicTick;
	unsigned int m_IMFDelay;
    std::vector<Sint32> mMixBuffer;
};

#endif /* CIMFPLAYER_H_ */
