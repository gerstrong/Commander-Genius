/*
 * hq_sound.h
 *
 *  Created on: 05.01.2009
 *      Author: gerstrong
 */

#ifndef __CG_HQ_SOUND_H__
#define __CG_HQ_SOUND_H__

#include <SDL/SDL.h>
#include <string>

struct stHQSound
{
	Uint8 *sound_buffer;
	Uint32 sound_len;
	int sound_pos;
	bool enabled;
};

short HQSndDrv_Load(SDL_AudioSpec *AudioSpec, stHQSound *psound, const std::string& soundfile);
void HQSndDrv_Unload(stHQSound *psound);

#endif
