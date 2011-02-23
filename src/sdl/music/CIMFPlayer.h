/*
 * CIMFPlayer.h
 *
 *  Created on: 17.02.2011
 *      Author: gerhard
 */

#ifndef CIMFPLAYER_H_
#define CIMFPLAYER_H_

#include "CMusicPlayer.h"
#include "fileio/TypeDefinitions.h"
#include "fileio/CExeFile.h"
#include "sdl/sound/CSound.h"
#include "CRingBuffer.h"
#include <SDL.h>
#include <string>

struct IMFChunkType
{
	byte al_reg;
	byte al_dat;
	word Delay;
};

class CIMFPlayer : public CMusicPlayer {
public:
	CIMFPlayer(const std::string& filename, const SDL_AudioSpec& AudioSpec);
	CIMFPlayer(const CExeFile& ExeFile, const int level, const SDL_AudioSpec &AudioSpec);

	~CIMFPlayer();

	template<typename T>
	void OPLUpdate(T *buffer, const unsigned int length);

	bool open();
	void close();
	void readBuffer(Uint8* buffer, Uint32 length);

private:
	RingBuffer<IMFChunkType> m_IMF_Data;
    const SDL_AudioSpec m_AudioDevSpec;
    COPLEmulator &m_opl_emulator;

	Uint32 m_numreadysamples;
	Uint32 m_soundTimeCounter;
	const Uint32 m_samplesPerMusicTick;
	unsigned int m_TimeCount;
	unsigned int m_IMFReadTimeCount;
	Sint32* m_mix_buffer;
};

#endif /* CIMFPLAYER_H_ */
