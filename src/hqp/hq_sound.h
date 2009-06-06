/*
 * hq_sound.h
 *
 *  Created on: 05.01.2009
 *      Author: gerstrong
 */

#include <SDL.h>

typedef struct stHQSound
{
	Uint8 *sound_buffer;
	Uint32 sound_len;
	int sound_pos;
	bool enabled;
} stHQSound;

short HQSndDrv_Load(SDL_AudioSpec *AudioSpec, stHQSound *psound, const char *soundfile);
void HQSndDrv_Unload(stHQSound *psound);
