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


//      Register addresses
// Operator stuff
#define alChar          0x20
#define alScale         0x40
#define alAttack        0x60
#define alSus           0x80
#define alWave          0xe0
// Channel stuff
#define alFreqL         0xa0
#define alFreqH         0xb0
#define alFeedCon       0xc0
// Global stuff
#define alEffects       0xbd


typedef struct
{
    byte    mChar,cChar,
            mScale,cScale,
            mAttack,cAttack,
            mSus,cSus,
            mWave,cWave,
            nConn,

            // These are only for Muse - these bytes are really unused
            voice,
            mode;
    byte    unused[3];
} Instrument;

class COPLEmulator
{
public:
	COPLEmulator(const SDL_AudioSpec &AudioSpec);

	~COPLEmulator();

	/**
	 * This function takes care of initializing the OPL Emulator.
	 * It should be called whenever the Sound Device starts or restarts after changing audio settings
	 */
	void init();
	
	// Call this if a new song is loaded or audio settings are changed. 
	// init() calls this as well.
	void setup();

	void AlSetFXInst(Instrument &inst);

	/**
	 *
	 *  StartOPLforAdlibSound() - Sets up the emulator for Adlib Sounds. Important in Keen Galaxy and later games.
	 *
	*/
	void StartOPLforAdlibSound();

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
	void setIMFClockrate(const unsigned int clock_rate);

	/**
	 * Stops the Adlib sounds from playing
	 */
	void ALStopSound();

	/**
	 * Shuts down the AdLib card for sound effects
	 */
	void ShutAL();

	/**
	 * Shutdown the emulator. This should only the called whenever the audio settings need to be shutdown
	 * or restarted like when the user changes the audio settings in the configuration while playing
	 */
	void shutdown();
	
	void clear();
	
	void dump();

private:

	const SDL_AudioSpec &m_AudioDevSpec;
	Chip m_opl_chip;
	Instrument	m_alZeroInst;
	unsigned int m_imf_clock_rate;
};

#endif /* COPLEMULATOR_H_ */
