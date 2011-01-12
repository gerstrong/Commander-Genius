/*
 * hq_sound.h
 *
 *  Created on: 05.01.2009
 *      Author: gerstrong
 */

#ifndef __CG_HQ_SOUND_H__
#define __CG_HQ_SOUND_H__

#include <SDL.h>
#include <string>
#include "sdl/sound/Mixer.h"

struct stHQSound
{
	Uint8 *sound_buffer;
	Uint32 sound_len;
	bool enabled;

	stHQSound() : sound_buffer(NULL), sound_len(0),
				  enabled(false) {}
};

short HQSndDrv_Load(SDL_AudioSpec *AudioSpec, stHQSound *psound, const std::string& gamepath, const std::string& soundname);
void HQSndDrv_Unload(stHQSound *psound);

#endif
