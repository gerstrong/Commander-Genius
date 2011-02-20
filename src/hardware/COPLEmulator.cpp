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

void COPLEmulator::init()
{
    DBOPL_InitTables();
    Chip__Chip(&m_opl_chip);
    Chip__Setup(&m_opl_chip, m_AudioDevSpec.freq);
}

unsigned int COPLEmulator::getIMFClockRate()
{	return IMF_CLOCK_RATE;	}


void COPLEmulator::shutdown()
{

}
