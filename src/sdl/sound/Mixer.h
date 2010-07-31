/*
 * Mixer.h
 *
 *  Created on: 31.07.2010
 *      Author: gerstrong
 */

#ifndef MIXER_H_
#define MIXER_H_

#include <SDL.h>

void mixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, Uint8 volume, Uint16 format);

#endif /* MIXER_H_ */
