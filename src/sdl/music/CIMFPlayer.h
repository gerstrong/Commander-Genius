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
#include "hardware/dbopl.h"
#include <SDL.h>
#include <string>

class CIMFPlayer : public CMusicPlayer {
public:
	CIMFPlayer(const std::string& filename, const SDL_AudioSpec& AudioSpec);
	CIMFPlayer(const CExeFile& ExeFile, const int level, const SDL_AudioSpec &AudioSpec);
	virtual ~CIMFPlayer();

	bool open();
	void readBuffer(Uint8* buffer, Uint32 length);
	void close();

private:
    byte *mp_imfdata_start;
    byte *mp_imfdata_ptr;
    byte *mp_imfdata_end;
    word m_data_size;
    const SDL_AudioSpec& m_AudioSpec;
    COPLEmulator &m_opl_emulator;

	Uint32 m_numreadysamples;
	Uint32 m_soundTimeCounter;
	Uint32 m_samplesPerMusicTick;
	word m_Delay;
	Chip m_opl_chip;
	Bit32s *m_mix_buffer;
};

#endif /* CIMFPLAYER_H_ */
