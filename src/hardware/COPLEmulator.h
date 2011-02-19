/*
 * COPLEmulator.h
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 */

#ifndef COPLEMULATOR_H_
#define COPLEMULATOR_H_

#include "fileio/TypeDefinitions.h"
#include "dbopl.h"
#include <SDL.h>

struct IMFChunkType
{
	byte al_reg;
	byte al_dat;
	word Delay;
};

class COPLEmulator {
public:
	COPLEmulator(const SDL_AudioSpec &AudioSpec);
	~COPLEmulator();

	/**
	 * This function takes care of initializing the OPL Emulator.
	 * It should be called whenever the Sound Device starts or restarts after changing audio settings
	 */
	void init();

	/**
	 * This will play the DB OPL Emulator pass it more channels if any.
	 */
	template<typename T>
	void OPLUpdate(T *buffer, unsigned int length);

	/**
	 * This functions read a portion of the given imf input data and convert it to a waveform
	 */
	longword readBufferFromIMF(Uint8* buffer, const longword wavesize, const byte *imfdata);

	/**
	 * Shutdown the emulator. This should only the called whenever the audio settings need to be shutdown
	 * or restarted like when the user changes the audio settings in the configuration while playing
	 */
	void shutdown();

private:
	/**
	 * This is the templatized version of the readBufferFromIMF function.
	 * This should only be called by readBufferFromIMF.
	 */
	template<typename T>
	longword readBufferFromIMFTemplatized(T *stream, const longword wavesize, const byte *imfdata);

	const SDL_AudioSpec &m_AudioDevSpec;
	Uint32 m_numreadysamples;
	Uint32 m_soundTimeCounter;
	Uint32 m_samplesPerMusicTick;
	word m_Delay;
	Chip m_opl_chip;
	Bit32s *m_mix_buffer;
};

#endif /* COPLEMULATOR_H_ */
