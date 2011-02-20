/*
 * COPLEmulator.h
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 */

#ifndef COPLEMULATOR_H_
#define COPLEMULATOR_H_

#include "fileio/TypeDefinitions.h"
#include "hardware/dbopl.h"
#include <SDL.h>


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
	 * Wrapper for the original C Emulator function Chip__GenerateBlock2(&m_opl_chip, length, mix_buffer )
	 */
	inline void Chip__GenerateBlock2(const Bitu total, Bit32s* output )
	{
		::Chip__GenerateBlock2( &m_opl_chip, total, output );
	}

	/**
	 * Wrapper for the original C Emulator function Chip__WriteReg(Chip *self, Bit32u reg, Bit8u val )
	 */

	inline void Chip__WriteReg( const Bit32u reg, const Bit8u val )
	{
		::Chip__WriteReg( &m_opl_chip, reg, val );
	}

	unsigned int getIMFClockRate();

	/**
	 * Shutdown the emulator. This should only the called whenever the audio settings need to be shutdown
	 * or restarted like when the user changes the audio settings in the configuration while playing
	 */
	void shutdown();

private:

	const SDL_AudioSpec &m_AudioDevSpec;
	Chip m_opl_chip;
};

#endif /* COPLEMULATOR_H_ */
