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
#include <fstream>

const int KEEN_IMF_CLOCK_RATE = 560;

COPLEmulator::COPLEmulator(const SDL_AudioSpec &AudioSpec) :
m_AudioDevSpec(AudioSpec),
m_imf_clock_rate(KEEN_IMF_CLOCK_RATE)
{}

COPLEmulator::~COPLEmulator()
{
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
    m_opl_chip.clear();
    
    DBOPL_InitTables();
    Chip__Chip(&m_opl_chip);
    setup();
    //Chip__Setup(&m_opl_chip, m_AudioDevSpec.freq);

    StartOPLforAdlibSound();
}

void COPLEmulator::setup()
{
    Chip__Setup(&m_opl_chip, m_AudioDevSpec.freq);
}


unsigned int COPLEmulator::getIMFClockRate()
{	return m_imf_clock_rate;	}

void COPLEmulator::setIMFClockrate(const unsigned int clock_rate)
{	m_imf_clock_rate = clock_rate;	}

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
    Chip__WriteReg( alEffects, 0);
    ALStopSound();
    AlSetFXInst(m_alZeroInst);
}

void COPLEmulator::shutdown()
{
    Chip__WriteReg( alEffects,0 );
    for (int i = 1; i < 0xf5; i++)
    	Chip__WriteReg( i, 0);
}

void COPLEmulator::clear()
{
    m_opl_chip.clear();
}


void dumpData(const std::string filename, const void *data, const int size)
{
    std::ofstream file(filename.c_str());

    unsigned char byte;    
    unsigned char *ptr = (unsigned char *)(data);
    for( int i=0 ; i<size ; i++ )
    {	
	memcpy(&byte, ptr, 1);
	file << byte << "\n";
	ptr++;
    }    
}

void COPLEmulator::dump()
{
  dumpData("chipDump.dat", &m_opl_chip, sizeof(Chip));
}
