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
	CIMFPlayer( const SDL_AudioSpec &AudioSpec, COPLEmulator& opl_emulator = *g_pSound->getOPLEmulatorPtr());


	/**
	 * \brief 	This function will load music using other dictionaries which are embedded in the Exe File.
	 * 			Only galaxy supports that feature, and the original games will read two files form the EXE-file
	 * 			AUDIOHED and AUDIODICT to get the right tune for the music player.
	 */
//	bool loadMusicTrack(const CExeFile& ExeFile, const int track);
    bool loadMusicFromFile(const std::string& filename);

	void OPLUpdate(byte *buffer, const unsigned int length);

	bool open();
	void close();
	void readBuffer(Uint8* buffer, Uint32 length);

    /**
      * \brief If any other entity already created a ring of data suitable for the player,
      *        use this method and pass the data.
      */
    void swapRing(RingBuffer<IMFChunkType> &&ring);

private:

	bool readMusicHedFromFile(const std::string fname, 
				  std::vector<uint32_t> &musiched);
		
    bool unpackAudioInterval(const std::string &dataPath,
                const std::vector<uint8_t> &AudioCompFileData,
                const int start,
                const int end);

	RingBuffer<IMFChunkType> m_IMF_Data;
    const SDL_AudioSpec& m_AudioDevSpec;
    COPLEmulator &m_opl_emulator;

	Uint32 m_numreadysamples;
	Uint32 m_samplesPerMusicTick;
	unsigned int m_IMFDelay;
    std::vector<Sint32> mMixBuffer;
};

#endif /* CIMFPLAYER_H_ */
