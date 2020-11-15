/*
 * Sampling.h
 *
 *  Created on: 05.08.2010
 *      Author: gerstrong
 */

#ifndef SAMPLING_H_
#define SAMPLING_H_

#include <SDL.h>

void resample(const Uint8* output_buffer, const Uint8 *input_buffer,
		const unsigned long output_len, const unsigned long input_len,
		const Uint16 format, const size_t channels );

#endif /* SAMPLING_H_ */
