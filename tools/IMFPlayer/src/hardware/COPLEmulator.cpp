/*
 * COPLEmulator.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 *
 *  This class processes the data of the ISF and IMF File Formats.
 *  The real emulator of the OPL Hardware is located in dpopl.cpp. Nevertheless
 *  this class interprets the data and communicates with the OPL emulator.
 *  It also takes care of the startup and shutdown of that emu.
 */

#include "COPLEmulator.h"

const int IMF_CLOCK_RATE = 560;

COPLEmulator::COPLEmulator(const SDL_AudioSpec &AudioSpec) :
m_AudioDevSpec(AudioSpec)
{}

COPLEmulator::~COPLEmulator() {
	shutdown();
}

void COPLEmulator::AlSetFXInst(Instrument &inst)
{
    byte c,m;

    m = 0;      // modulator cell for channel 0
    c = 3;      // carrier cell for channel 0

    Chip__WriteReg( m + alChar, inst.mChar);
    Chip__WriteReg( m + alScale,inst.mScale);
    Chip__WriteReg( m + alAttack,inst.mAttack);
    Chip__WriteReg( m + alSus,inst.mSus);
    Chip__WriteReg( m + alWave,inst.mWave);

    Chip__WriteReg( c + alChar,inst.cChar);
    Chip__WriteReg( c + alScale,inst.cScale);
    Chip__WriteReg( c + alAttack,inst.cAttack);
    Chip__WriteReg( c + alSus,inst.cSus);
    Chip__WriteReg( c + alWave,inst.cWave);

    Chip__WriteReg( alFeedCon, 0);
}

///////////////////////////////////////////////////////////////////////////
//
//      DetectAdLib() - Determines if there's an AdLib (or SoundBlaster
//              emulating an AdLib) present
//
///////////////////////////////////////////////////////////////////////////
void COPLEmulator::StartOPLforAdlibSound()
{
    for (int i = 1; i <= 0xf5; i++)       // Zero all the registers
    	Chip__WriteReg( i, 0);

    Chip__WriteReg( 1, 0x20);             // Set WSE=1
//    alOut(8, 0);                // Set CSM=0 & SEL=0

	Chip__WriteReg( alEffects, 0);
    AlSetFXInst(m_alZeroInst);
}

void COPLEmulator::init()
{
    DBOPL_InitTables();
    Chip__Chip(&m_opl_chip);
    Chip__Setup(&m_opl_chip, m_AudioDevSpec.freq);

    StartOPLforAdlibSound();
}

unsigned int COPLEmulator::getIMFClockRate()
{	return IMF_CLOCK_RATE;	}


///////////////////////////////////////////////////////////////////////////
//
//      SDL_ALStopSound() - Turns off any sound effects playing through the
//              AdLib card
//
///////////////////////////////////////////////////////////////////////////
void COPLEmulator::ALStopSound()
{
    Chip__WriteReg( alFreqH + 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
//      ShutAL() - Shuts down the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
void COPLEmulator::ShutAL()
{
    Chip__WriteReg( alEffects,0);
    Chip__WriteReg( alFreqH + 0,0);
    AlSetFXInst(m_alZeroInst);
}

void COPLEmulator::shutdown()
{
    Chip__WriteReg( alEffects,0);
    for (int i = 1; i < 0xf5; i++)
    	Chip__WriteReg( i, 0);
}

void COPLEmulator::clear()
{
    //Chip.clear();
}
